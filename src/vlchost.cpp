// rtsphub.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "include/vlchost_task.h"

LIBVLCX_NAMESPACE(USE)
VLCHOST_NAMESPACE(USE)

JobList::JobList() : First(nullptr), Size(0) {}

JobList::~JobList() {
	while (Size)
		delete First->Prev;
}

bool JobContext::Boot() {
	if (!BootInstance) {
		const auto &j = ((const json*)this)->find("boot");
		if (j!=cend()) {
			void *mem[1] = { nullptr, };
			void **memptr=mem;

			const std::vector<std::string> boot_args = 
				arrconv::read(j.value());

			arrarg_t arg = arrconv::conv(boot_args, memptr);

			if (arg != nullptr) {
				BootInstance = new VLCInstance(boot_args.size(), arg);
			}
			else {
				BootInstance = new VLCInstance(0, nullptr);
			}
			if (memptr != mem) free(*(--memptr));
		}
		else {
			BootInstance = new VLCInstance(0, nullptr);
		}
	}
	return nullptr != BootInstance;
}
void JobContext::CancelBoot() {
	if (BootInstance) {
		delete BootInstance;
		BootInstance = nullptr;
	}
}

void JobContext::Shutdown() {
	if (nullptr != Lock) {
		std::unique_lock<std::mutex> lk(*Lock);
		//lk.lock();
		if (Exit == 0) {
			Exit = 1;
			Condition->notify_one();
		}
		lk.unlock();
	}
}

void JobContext::Join() {
	if (nullptr != Lock) {
		{
			bool awaiting = true;
			std::unique_lock<std::mutex> lk(*Lock);
			{

				//lk.lock();
				if (Exit < 0) {
					awaiting = false;
				}
				else
				{
					if (Exit == 0) {
						Exit = 1;
						Condition->notify_one();
					}
				}

				if (awaiting)
					Condition->wait(lk, [this] {return Exit != 1;});
				lk.unlock();
			}
		}
		Thread->join();
		delete Thread;
		delete Condition;
		delete Lock;
		Thread = nullptr;
		Condition = nullptr;
		Lock = nullptr;
		Exit = 0;
	}
}

template<> 
VLCHOST_INLINE void JobContext::OnRes(
	VLCX::VLCInstance &vlc, 
	const json &js,
	int &o,
	VLCX::VLCResult res)const {
}
template<> VLCHOST_INLINE void JobContext::OnRes(
	VLCX::VLCInstance &vlc, const json &js, int &, const char *res)const {
	if (nullptr != res)std::cout << res;
}

int JobContext::Execute(VLCX::VLCInstance & vlc, const json &js) const {
	int o = 0;
	if (js.is_array()) {
		for (auto& iter : js) {
			o = Execute(vlc, iter);
			if (o)break;
		}
	}
	else if (js.is_object()) {
		auto & opname = js.find("op");
		if (opname != js.cend()) {
			auto & optitle = opname.value();
			if (optitle.is_string())
			{
				opresult ret{};
				if (!json_exec(vlc,
					optitle.get<std::string>()
					, js, ret)) {
					std::cerr << "unknown op name:" << optitle;
				}
				else if (ret.ret != e_ret::V) {
					if (ret.ret == e_ret::P) {
						if (ret.ret_print)
							std::cout << ret.ret_print;
					}
					else if (ret.ret == e_ret::R) {
						const auto &res = ret.get_ret<VLCX::VLCResult>();
						const bool succeeded = VLCSuccess(res);
						const auto& sub = succeeded ? 
							js.find("if") :
							js.find("else");
						if (!(sub == js.cend())) {
							o = Execute(vlc, sub.value());
						}
						else if (!succeeded) {
							std::cerr << js;
							o = res == 0 ? ((int32_t)1 << 31) : res;
						}
					}
				}
			}
		}
	}
	else if (!js.is_number())
		o= 0;
	else
		o=(int)js;
	return o;
}
int JobList::ThreadStart(
	JobContext *job,
	VLCX::VLCInstance *vlc) {
	VLCX::VLCInstance vlc_copy(*vlc);
	delete vlc;
	vlc = &vlc_copy;

	int code(0);
	std::unique_lock<std::mutex> lk(*(job->Lock));
	{
		//lk.lock();
		if (0 == job->Exit)
		{
			code = job->Execute(vlc_copy, job->at("tasks"));
		}
		else 
		{
			if (0 < job->Exit) {
				job->Exit = -1;
				job->Condition->notify_one();
			}
			lk.unlock();
			return code;
		}

	}

	job->Condition->wait(lk, [job] {
		if (0 < job->Exit)
		{
			job->Exit = -1;
			job->Condition->notify_one();
		}
		return 0 != job->Exit;
	});
	return code;
}


JobContext::~JobContext() {
	if (BootInstance != nullptr)delete BootInstance;
	if (Thread != nullptr)delete Thread;
	if (Lock != nullptr)delete Lock;
	if (Condition != nullptr) delete Condition;

	if (List.Size) {
		if (--List.Size) {
			if (List.First == this) {
				List.First = Next;
			}

			Next->Prev = Prev;
			Next->Prev->Next = Next;
		}
	}
}

JobContext::JobContext(JobList &list, const json &j)
	: json(j)
	, List(list)
	, BootInstance(nullptr)
	, Thread(nullptr)
	, Exit(0)
	, Lock(nullptr) 
{
	if (List.Size++)
	{
		Next = List.First;
		Prev = List.First->Prev;
		Next->Prev = this;
		Prev->Next = this;
	}
	else
	{
		Next = this;
		Prev = this;
		List.First = this;
	}
}

inline JobContext *JobList::New(const json&j) {
	if (JobContext*const o = new JobContext(*this, j)) {
		const auto &j_id = j.find("id");
		if (!(j_id == j.cend()))
			o->Id = j_id.value().get<std::string>();
		else
			o->Id += ('0'+(Size-1));
		return o;
	}
	return nullptr;
}

inline JobContext *JobList::Boot() {
	typedef int (threadfunct)(JobContext&, VLCInstance);
	JobContext *jc, *err = nullptr;
	int iter;

	for (iter = Size, jc = First;0 != iter;jc = jc->Next, --iter) {
		if (jc->Boot())
			continue;

		err = jc;
		iter = Size - iter;
		while (0 != --iter) { jc = jc->Prev; jc->CancelBoot(); }
		break;
	}

	//set sails.
	if (nullptr == err)
		for (iter = Size;0 != iter;jc = jc->Next, --iter) {
			jc->Lock = new std::mutex();
			jc->Condition = new std::condition_variable();
			jc->Thread = new std::thread(
				ThreadStart,
				jc,
				jc->BootInstance);
			jc->BootInstance = nullptr;
		}

	return err;
}
inline int JobList::Shutdown() {
	JobContext *jc;
	int iter;

	for (iter = Size, jc = First;0 != iter;jc = jc->Next, --iter)
		jc->Shutdown();

	for (iter = Size, jc = First;0 != iter;jc = jc->Next, --iter)
		jc->Join();

	return 0;
}

int main(int argc, char **args)
{
	json j;
	{
		std::string jsonstr = "";
		bool is_file = false;
		{
			std::string arg;

			for (int i = argc - 1; i >= 0; --i) {
				arg = args[i];
				if (arg == "-f")
				{
					is_file = true;
				}
				else if (arg == "-")
				{
					jsonstr = "";
					is_file = true;
				}
				else if (arg != "-j")
				{	jsonstr = arg;
					continue;
				}
				break;
			}
		}

		if (is_file)
			if (jsonstr.empty())
				std::cin >> j;
			else
				(std::fstream(jsonstr.c_str())) >> j;
		else
			j = json::parse(jsonstr);
	}

	JobList Jobs{};

	if (j.is_object()) {
		{
			auto&instances = j.find("instances");
			if (instances != j.end()) {
				j = instances.value();
			}
		}
		for (json::const_iterator instance = j.begin(); instance != j.cend(); ++instance ) {
			auto &instance_config = instance.value();
			if (instance_config.is_object()) {
				if (JobContext *const job = Jobs.New(instance_config)) {
					job->Id = instance.key();
				}
			}
		}
	}
	else if (j.is_array()) {
		for (auto& instance : j)
			if (instance.is_object())
				Jobs.New(instance);
	}

	if (!Jobs.Size) {
		std::cerr << "No jobs parsed.";
		return -22;
	}
	
	if (const JobContext* const FailedBoot = Jobs.Boot()) {
		std::cerr << "Failed to start up. Instance=" << FailedBoot;
		return -22;
	}

	char ch;
	
	std::cout << "type exit to quit\n";

	int exit_pos = 0;

	while (std::cin.read(&ch, 1) && ch) {
		if (exit_pos == 4)
			if (ch == '\r' || ch == '\n')
				break;
			else
				exit_pos = 0;

		if (ch != "exit"[exit_pos] && ch != "EXIT"[exit_pos])
			exit_pos = 0;
		else
			exit_pos++;
	}

	return Jobs.Shutdown();
}


#if VLCHOST_OS == VLCHOST_OS_WINDOWS && 0

constexpr const auto *const winclass = TEXT("rtsphub");


static long __stdcall WindowProc(HWND window, unsigned int msg, WPARAM wp, LPARAM lp) {
	return DefWindowProc(window, msg, wp, lp);
}

inline static WNDCLASSEX &WindowClass() {
	static WNDCLASSEX wndclass = {
		sizeof(WNDCLASSEX),
		CS_DBLCLKS,
		WindowProc,
		0, 0,
		GetModuleHandle(0),
		LoadIcon(0,IDI_APPLICATION),
		LoadCursor(0,IDC_ARROW),
		HBRUSH(COLOR_WINDOW + 1),
		0, winclass,
		LoadIcon(0,IDI_APPLICATION) 
	};

	return wndclass;
}

inline static auto &WindowClassRegistered() {
	static auto o = RegisterClassEx(&WindowClass());
	return o;
}
#endif
