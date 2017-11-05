#pragma once
#ifndef HEADER_VLCHOST_TASK
#	define HEADER_VLCHOST_TASK
#	include "vlchost.h"

// include the configuration
#	include "vlchost_conf.inl"
#	include "vlchost_macros.inl"

#define VLCHOST_NULLSTR "=null''"
#define VLCHOST_NULLARR "=null<>"

VLCHOST_NAMESPACE(START)
// take that max arg config turn it into a constexpr and delete the macro.
VLCHOST_INLINE constexpr const uint8_t maximum_amount_of_arguments() { return Macro_VLCHOST_MaximumArguments; }
#	undef Macro_VLCHOST_MaximumArguments

using strarg_t = const char *;
using arrarg_t = const strarg_t *;

#include "vlchost_enum.inl"

// titlebase is the base type of both x_op and x_arg (and their immediate parents)
// it holds 14 characters of text for the json property value/key
// 1 byte to hold how many of the characters are used
// and 1 byte to declare if its a arg or op.
struct titlebase;

// argbase (a titlebase implementation)
// holds the name (e_arg)
// type (e_argt)
// and flags (e_argf)
struct argbase;

// opbase (a titlebase implementation)
// holds the name (e_op)
// return type (e_ret)
// and argument count (argc,uint8_t)
struct opbase;

// utility to determine if the json type for the argument is constexpr-able.
// (std::string and std::vector<std::string> are not as far as c++11)
template<e_argt e> VLCHOST_INLINE constexpr bool argt_constexpr();


// base template for dealing with the type (argt) of args.
template<e_argt e, bool is_constexpr=argt_constexpr<e>()>
struct x_argt;

// template for each return type.
template<e_ret e>
struct x_ret;

// template for argument lists.
template<e_arg...es>
struct x_arglist;

// base template for dealing with return and argument list of ops.
template<e_ret e, typename alist>
struct x_opt;

// final template for each arg.
template<e_arg e>
struct x_arg;

// final template for each op.
template<e_op e>
struct x_op;

// template for converting from non-constexpr types (std::string) to constexpr types (char*)
template<typename t_non_constexpr, typename t_constexpr>
struct x_nce2ce;

// utility define.
template<typename t_constexpr, typename t_non_constexpr>
using x_ce2nce = x_nce2ce<t_non_constexpr, t_constexpr>;

template<typename oi> struct x_nce2ce<oi, oi> {
	enum { IS_CONSTEXPR = true, };
	static VLCHOST_INLINE constexpr const oi& conv(const oi&i, void**const = nullptr) { return i; }
	static VLCHOST_INLINE oi read(const json& j) {
		return j.is_null() ? (oi)0 : j.get<oi>();
	}
};
template<> struct x_nce2ce<std::string, strarg_t> {
	enum { IS_CONSTEXPR = false, };
	static VLCHOST_INLINE std::string conv(const strarg_t i) {
		return i == nullptr ? std::string(VLCHOST_NULLSTR) : i;
	}
	static VLCHOST_INLINE strarg_t conv(const std::string &i, void**const = nullptr) {
		return i == VLCHOST_NULLSTR ? nullptr : i.c_str();
	}

	static VLCHOST_INLINE std::string read(const json& j) {
		return (!j.is_null()) ? j.get<std::string>() : VLCHOST_NULLSTR;
	}
};
using strconv = x_nce2ce<std::string, strarg_t>;

template<>
struct x_nce2ce<std::vector<std::string>, arrarg_t> {
	enum { IS_CONSTEXPR = false, };
	static VLCHOST_INLINE std::vector<std::string> conv(const arrarg_t i) {
		if (i == nullptr)
			return std::vector<std::string>{VLCHOST_NULLARR};
		// count the strings.
		size_t n;
		for (n = 0;i[n];++n)
			if (0 == n && i[n] == VLCHOST_NULLARR)
				return std::vector<std::string>{VLCHOST_NULLARR};
		if (!n)
			return std::vector<std::string>{};
		std::vector<std::string> o(n--, std::string{});
		do o[n] = strconv::conv(i[n], nullptr); while (!!(n--));
		return o;
	}
	static VLCHOST_INLINE arrarg_t conv(const std::vector<std::string> &i, void**&reg) {
		size_t n = i.size();
		if (n != 0 && i[0] == VLCHOST_NULLARR)
			return nullptr;
		else
		{
			if (!reg) throw std::runtime_error("reg needed, reg** is nullptr.");
			if (*reg) throw std::runtime_error("reg needed, *reg is was not nullptr (maybe it was populated wrong?)");

			// allocate the array.
			*reg = malloc(sizeof(strarg_t)*(n + 1));

			// advance reg so that the caller knows to free it.
			const char ** pos = (const char* *)(*(reg++));

			// terminator.
			pos[n] = nullptr;
			if (!!(n--)) do pos[n] = strconv::conv(i[n], nullptr); while (!!(n--));
			return pos;
		}
	}
	static VLCHOST_INLINE std::vector<std::string> read(const json& j) {
		size_t n;
		if (j.is_null()) 
			return { VLCHOST_NULLARR };
		else if (!j.is_array()) 
			return { strconv::read(j) };
		else
		{
			n = j.size();
			if (!n)
				return {};
			else if(1==n)
				return  { strconv::read(j[0]) };
			else
			{
				std::vector<std::string> deserial(n--, "");
				do deserial[n] = strconv::read(j[n]); while (!!(n--));
				return deserial;
			}
		}
	}
};
using arrconv = x_nce2ce < std::vector<std::string>, arrarg_t > ;


//gets a char of some unknown length string. constexpr compatible.
template<uint8_t _len>VLCHOST_INLINE constexpr char readstr_ln(const char(&_s)[_len], const int_fast8_t p) {
	return (p >= 0 && p < _len) ? _s[p] : (char)0;
}
#define Macro_VLCHOST_ReadStr2(a,b) \
readstr_ln(a,(b)), \
readstr_ln(a,(b)|1)

#define Macro_VLCHOST_ReadStr4(a,b) \
Macro_VLCHOST_ReadStr2(a,(b)), \
Macro_VLCHOST_ReadStr2(a,(b)|2)

#define Macro_VLCHOST_ReadStr8(a,b) \
Macro_VLCHOST_ReadStr4(a,(b)), \
Macro_VLCHOST_ReadStr4(a,(b)|4)

#define Macro_VLCHOST_ReadStr14(a,b) \
Macro_VLCHOST_ReadStr8(a,0), \
Macro_VLCHOST_ReadStr4(a,8), \
Macro_VLCHOST_ReadStr2(a,12)

#define Macro_VLCHOST_title const char (& _title)[_len]

#define Macro_VLCHOST_tbconstruct template<uint8_t _len> VLCHOST_INLINE constexpr 

struct titlebase {
	char title[14];
	uint8_t len;
	e_kind kind;

	Macro_VLCHOST_tbconstruct titlebase(Macro_VLCHOST_title, const e_kind _kind)
		: title{ Macro_VLCHOST_ReadStr14(_title,0) }
		, len(_len)
		, kind(_kind) {}
};

struct argbase : public titlebase {
	e_arg name;
	e_argt type;
	e_argf flags;

	Macro_VLCHOST_tbconstruct argbase(
		Macro_VLCHOST_title,
		const e_arg _name,
		const e_argt _type,
		const e_argf _flags)
		: titlebase(_title, e_kind::arg), name(_name), type(_type), flags(_flags)
	{
	}
};

#define Macro_VLCHOST_ConstExpr_true constexpr
#define Macro_VLCHOST_ConstExpr_false




#define Macro_VLCHOST_ImplementArgCT(e,e_t,e_ct,ce)\
template<> VLCHOST_INLINE constexpr bool argt_constexpr<e_argt::e>(){ return ce; } \
template<> struct x_argt<e_argt::e, ce> : public argbase { \
	enum { IS_CONSTEXPR = ce, };\
	using type_t = e_t;\
	using ctype_t = e_ct; \
	static constexpr const e_argt type = e_argt::e; \
	Macro_VLCHOST_tbconstruct x_argt( \
		Macro_VLCHOST_title,\
		const e_arg _name, const e_argf _flags) \
		: argbase(_title,_name,e_argt::e,_flags){} \
}
#define Macro_VLCHOST_ImplementArgT(e,e_t) Macro_VLCHOST_ImplementArgCT(e,e_t,e_t,true)

Macro_VLCHOST_ImplementArgCT(str, std::string, strarg_t, false);
Macro_VLCHOST_ImplementArgCT(arr, std::vector<std::string>, arrarg_t, false);
Macro_VLCHOST_ImplementArgT(flg, uint32_t);
Macro_VLCHOST_ImplementArgT(flt, float);
Macro_VLCHOST_ImplementArgT(boo, bool);


#undef Macro_VLCHOST_ImplementArgCT
#undef Macro_VLCHOST_ImplementArgT
#undef Macro_VLCHOST_ConstExpr_true
#undef Macro_VLCHOST_ConstExpr_false


#define Macro_VLCHOST_ArgImplement(e,e_t,flgs,def) \
/*__pragma(message("{\"" #e "\":\""#e_t "," #def "\"...("#flgs")} = e_arg::" #e )) */ \
template<> struct x_arg<e_arg::e> : public x_argt<e_argt::e_t> { \
	static constexpr const char title[] = #e ; \
	static constexpr const e_arg name = e_arg::e; \
	static constexpr const def; \
	static constexpr const e_argf flags = e_argf::flgs; \
	VLCHOST_INLINE constexpr x_arg() : x_argt( #e ,e_arg::e,e_argf::flgs) {} \
}

Macro_VLCHOST_ArgTable(Macro_VLCHOST_ArgImplement, ;);
#undef Macro_VLCHOST_ArgImplement

template<typename T, e_arg a>
using x_dummy_arg = T;


template<e_arg...es>
struct x_arglist {
	static_assert(sizeof...(es) <= maximum_amount_of_arguments(), "argc is greater than allowed amount");
	static_assert(sizeof...(es), "argc was zero and this should have been specialized for");
	static constexpr const uint8_t argc = sizeof...(es);
	static constexpr const e_arg args[] = { es... };
private:
	// takes all the arguments as type_t
	// converts them to ctype_t.
	// invokes x_op<e>::invoke with ctype_t arguments.
	template<e_op e, typename tinv>
	static VLCHOST_INLINE auto _j_invoke_send(
		tinv &vlc,
		const typename x_arg<es>::type_t &...values
	)
	{
		// make a pointer available for each argument.
		void* argptrdump[sizeof...(es)] = { nullptr, };

		// when this gets incremented we call free and decrement it until it matches argptrdump.
		void**argptr= argptrdump;

		// get the return by invoking the function (finally).
		auto ret = x_op<e>::template invoke<tinv, typename x_arg<es>::ctype_t...>(
			vlc,
			x_nce2ce<
				typename x_arg<es>::type_t,
				typename x_arg<es>::ctype_t
			>::conv(values, argptr)...
			);

		// free any used allocations.
		while (argptrdump != argptr--) free(argptr);

		// return.
		return ret;
	}
	template<e_op e, typename tinv>
	static VLCHOST_INLINE auto _j_invoke_plant(
		tinv &vlc,
		const json::const_iterator &iter_end,
		const x_dummy_arg<json::const_iterator, es> &...props
	)
	{
		// convert the json to correlating 
		return _j_invoke_send<e,tinv>(vlc,
				( (!(props == iter_end)) ?
					// when the property exists. read it.
					x_nce2ce<
						typename x_arg<es>::type_t,
						typename x_arg<es>::ctype_t
					>::read(props.value()) :
					// otherwise convert the fallback (default)
					// to type_t (which stores the memory).
					x_nce2ce<
						typename x_arg<es>::type_t,
						typename x_arg<es>::ctype_t
					>::conv(x_arg<es>::fb))...
				);
	}
protected:
	template<e_op e, typename tinv>
	static VLCHOST_INLINE auto _j_invoke(
		tinv & vlc,
		const json &j) {
		// find all the properties.
		return _j_invoke_plant<e,tinv>(
			vlc, j.cend(),
			j.find(x_arg<es>::title)...);
	}
};


template<>
struct x_arglist<> {
	static constexpr const std::tuple<> arglist{};
	static constexpr const uint8_t argc = 0;
	static constexpr const e_arg *const args = nullptr;
	template<e_op e>
	static VLCHOST_INLINE auto _j_invoke(VLCX::VLCInstance &vlc, const json &) {
		return x_op<e>::invoke(vlc);
	}
};

template<e_ret e>
struct x_ret {
	static_assert(e == e_ret::V, "unimplemented ret.");
	using ret_t = void; static const constexpr e_ret ret = e;
	static VLCHOST_INLINE constexpr bool handleret(void) { return true; }
};


template<> struct x_ret<e_ret::P> {
	using ret_t = const char *;
	static const e_ret ret = e_ret::P;

	static VLCHOST_INLINE bool handleret(const ret_t &r) {
		if (r != nullptr)
			std::cout << r;
		return true;
	}
};

template<> struct x_ret<e_ret::R> {
	using ret_t = VLCX::VLCResult;
	static const e_ret ret = e_ret::R;
	static VLCHOST_INLINE constexpr bool handleret(const ret_t &r) {
		return VLCX::VLCSuccess(r);
	}
};


struct opbase : public titlebase {
	e_op name;
	e_ret ret;
	uint8_t argc;

	Macro_VLCHOST_tbconstruct opbase(
			Macro_VLCHOST_title,
			const e_op _name,
			const e_ret _ret,
			const uint8_t _argc)
		: titlebase(_title, e_kind::op), name(_name), ret(_ret), argc(_argc) {}
};

template<e_ret e, typename alist>
struct x_opt : public opbase, public x_ret<e>, public alist {
	using arglist_t = alist;

	Macro_VLCHOST_tbconstruct x_opt(
		Macro_VLCHOST_title,
		const e_op _name)
		: opbase(_title, _name, e, alist::argc) {}
};

template<typename T> VLCHOST_INLINE auto c_value(const T &i) { return i; }
template<> VLCHOST_INLINE auto c_value(const std::string &i) { return i.c_str(); }
//TODO
template<> VLCHOST_INLINE auto c_value(const std::vector < std::string> &) { return nullptr; }

#define Macro_VLCHOST_OP(ret,e,titlechars,call,prec,postc,...) \
/*__pragma(message("{\"op\":\"" titlechars "\"...("#__VA_ARGS__")} = e_op::" #e "\n\t(VLCInstance::"#call")"))*/	\
template<> struct x_op<e_op::e> : public x_opt<e_ret::ret,x_arglist<__VA_ARGS__>>{ \
	static const constexpr char title[] = titlechars; \
	static const constexpr e_op op = e_op::e; \
	VLCHOST_INLINE explicit constexpr x_op() : x_opt(titlechars,e_op::e) {} \
	template<typename tinv, typename ...Ts> static VLCHOST_INLINE auto invoke( \
		tinv &vlc, const Ts& ...ts)  { \
		return prec vlc. call (c_value(ts)...) postc; \
	} \
	template<typename tinv> static VLCHOST_NOINLINE auto json_invoke( \
		tinv &vlc, \
		const json &j){ \
		return _j_invoke<e_op::e>(vlc,j); \
	} \
}

Macro_VLCHOST_Enumerate_Op(;);

struct opresult {
	union {
		const char *ret_print;
		bool ret_bool;
		unsigned ret_unsigned;
		int ret_signed;
		e_void ret_void;
	};
	e_ret ret;
	e_op op;
	bool found;

	template<typename T> VLCHOST_INLINE const constexpr T&get_ret()const;
	VLCHOST_INLINE constexpr opresult(const e_op _op, const bool v)
		: ret_bool(v)
		, ret(e_ret::R)
		, op(_op)
		, found(true) {}
	VLCHOST_INLINE constexpr opresult(const e_op _op, const unsigned v)
		: ret_unsigned(v)
		, ret(e_ret::R)
		, op(_op)
		, found(true) {}
	VLCHOST_INLINE constexpr opresult(const e_op _op, const signed v)
		: ret_signed(v)
		, ret(e_ret::R)
		, op(_op)
		, found(true) {}
	VLCHOST_INLINE constexpr opresult(const e_op _op, const char*const v)
		: ret_print(v)
		, ret(e_ret::R)
		, op(_op)
		, found(true) {}
	VLCHOST_INLINE constexpr opresult(const e_op _op, const e_void)
		: ret_void((e_void)0)
		, ret(e_ret::P)
		, op(_op)
		, found(true) {}
	VLCHOST_INLINE constexpr opresult()
		: ret_print(nullptr)
		, ret(e_ret::U)
		, op((e_op)0)
		, found(false) {}
};
template<> constexpr const char *const &opresult::get_ret<const char *>()const { return ret_print; }
template<> constexpr const bool &opresult::get_ret<bool>()const { return ret_bool; }
template<> constexpr const unsigned &opresult::get_ret<unsigned>()const { return ret_unsigned; }
template<> constexpr const int &opresult::get_ret<int>()const { return ret_signed; }
template<> constexpr const e_void &opresult::get_ret<e_void>()const { return ret_void; }

template<typename tinv, e_op e>
VLCHOST_INLINE bool json_exec(tinv&vlc, const std::string&op, const json&j, opresult&r) {
	return (r = opresult(e, x_op<e>::template json_invoke<tinv>(vlc, j))).found;
}

template<typename tinv, e_op...choices>
VLCHOST_INLINE bool &json_exec_lim(
	tinv&vlc,
	const std::string&op,
	const json& j,
	opresult&res,
	const enum_values<e_op,choices...>) {
	res.found = false;
	const bool dummy[] = {
		(!res.found && 
		op == x_op<choices>::title && 
		((void)(res=opresult(choices,x_op<choices>::template json_invoke<tinv>(vlc,j))),true))...
	};
	// just to make sure clang does not optimize it out.
	// (not sure how to do this the right way..)
	for (int i = (sizeof(dummy) / sizeof(bool)) - 1; i >= 0;--i)if (dummy[i])--i;
	return res.found;
}
template<typename tinv> VLCHOST_INLINE bool json_exec(
	tinv&vlc,
	const std::string&op,
	const json& j,
	opresult&res) {
	return json_exec_lim<tinv>(vlc, op, j, res, x_enum<e_op>{});
}
#undef Macro_VLCHOST_OP


VLCHOST_NAMESPACE(END)

#endif