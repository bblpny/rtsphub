#pragma once
#ifndef HEADER_VLCHOST_JOB
#	define HEADER_VLCHOST_JOB

#	include "vlchost.h"


VLCHOST_NAMESPACE(START)

struct JobContext : protected json {
	std::string Id;
	struct JobList {
		JobContext *First;
		int Size;
		JobList();
		JobList(const JobList&) = delete;
		JobList(const JobList&&) = delete;
		~JobList();
		JobContext * New(const json &j);
		JobContext * Boot();
		int Shutdown();
	private:
		static int ThreadStart(
			JobContext *job,
			VLCX::VLCInstance * vlc);
	};
private:
	JobList &List;
	JobContext *Next, *Prev;
	VLCX::VLCInstance *BootInstance;
	std::thread *Thread;
	std::mutex *Lock;
	std::condition_variable *Condition;
	int_fast8_t Exit;

	bool Boot();
	void CancelBoot();
	void Shutdown();
	void Join();
	
	template<typename Ts>
	VLCHOST_INLINE void OnRes(VLCX::VLCInstance &vlc, const json &js, int &o, Ts )const {}
	
	int Execute(VLCX::VLCInstance & vlc, const json &js)const;
	void SendOff();
	~JobContext();
	JobContext(
		JobList &list,
		const json & j);
	
	template<int8_t...ops>
	VLCHOST_INLINE void Invoke(
		VLCX::VLCInstance &vlc,
		const json&js,
		const std::string &prop,
		int&o)const;
};

using JobList = JobContext::JobList;

VLCHOST_NAMESPACE(END)

#endif