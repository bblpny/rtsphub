

// meta type containing a constant number (count).
template<size_t n> struct x_enum_count { static constexpr const size_t count = n; };
// meta type containing a typedef to i, and anested meta class for each value.
template<typename _t>
struct x_enum_int {
	using t = _t;

	template<t...ts>
	struct values
		: public x_enum_int<t>
		, public x_enum_count<sizeof...(ts)> {
		static constexpr const t v[] = { ((t)ts)... };
	};
};
template<typename i, i...vs> using enum_values = typename x_enum_int<i>::values<vs...>;

template<typename t, typename _i, typename ei, _i...vs>
struct x_enumbase : public x_enum_int<t>::values<(t)vs...> {
	using type = t;
	using i = _i;
	using enum_int = ei;
};
template<typename t> struct x_enum;

// use this macro after each enum definition in the file.
#define VLCHOST_ENUM(Name, isto,...) \
enum class e_##Name : isto{__VA_ARGS__};\
namespace _enum_forget_{ struct E##Name {\
	enum {__VA_ARGS__,};\
	using i_##Name = enum_values<isto,##__VA_ARGS__>;\
	using E_##Name = x_enumbase<e_##Name,isto,i_##Name,##__VA_ARGS__>;\
};};\
template<> struct x_enum<e_##Name> : public _enum_forget_::E##Name::E_##Name 

// while it's an enum, we use this dummy type for void returns.
enum class e_void : int_fast8_t {};

//
// ENUM DEFINITIONS!
//

// Kinds (either arg or op)
VLCHOST_ENUM(kind, uint8_t, 
	// is it an arg?
	arg,
	// is it an op?
	op) {};

// return types.
VLCHOST_ENUM(ret, int8_t,
	//void
	V,
	//result
	R,
	//print string (const char*)
	P,
	//unknown
	U
) {};

// arg types.
VLCHOST_ENUM(argt, int8_t,
	//string
	str,
	//array of strings.
	arr,
	//flag (uint32)
	flg,
	//float (single)
	flt,
	//boolean.
	boo
) {};

// argument flags (not used currently)
VLCHOST_ENUM(argf, uint8_t,
	none,
	//used as an argument parameter
	arg,
	//used to set a value.
	set,
	// implies arg and set
	argset
) {};

#define Macro_VLCHOST_PrintFirst(X,...) X

// arguments
VLCHOST_ENUM(arg, int8_t,
	Macro_VLCHOST_ArgTable(Macro_VLCHOST_PrintFirst, , )
) {};
#undef Macro_VLCHOST_PrintFirst

#define Macro_VLCHOST_OP(r,name,...) name
#define wait ,wait
VLCHOST_ENUM(op, int8_t,
	Macro_VLCHOST_Enumerate_Op(, )
) {};
#undef wait
#undef Macro_VLCHOST_OP
// have no idea why this is neccisary.
#define SetAppId ,SetAppId
#define SetUserAgent ,SetUserAgent
#define Wait ,Wait
#define Macro_VLCHOST_OP(r,n,sz,call,...) call
VLCHOST_ENUM(call, int8_t,
	Macro_VLCHOST_Enumerate_Op(, )
) {};
#undef SetAppId
#undef SetUserAgent
#undef Wait

#undef Macro_VLCHOST_OP

#ifndef _forget_less_one
VLCHOST_INLINE constexpr size_t _forget_less_one(
	const size_t v
) {
	return (!v) ? 0 : (v - 1u);
}
#	define _forget_less_one _forget_less_one
#endif

// constexpr for converting any enums in this file to
// their native integer.
template<typename t_enum>
VLCHOST_INLINE constexpr auto enum_int(const t_enum e) {
	return (typename x_enum<t_enum>::i)e;
}
template<typename t_enum, typename vi>
VLCHOST_INLINE constexpr t_enum int_enum(const vi v) {
	return (typename x_enum<t_enum>::type)v;
}
template<typename t_enum>
VLCHOST_INLINE constexpr t_enum enum_index(const int v) {
	return x_enum<t_enum>::v[v];
}
template<typename t_enum>
VLCHOST_INLINE constexpr typename x_enum<t_enum>::i enum_index_int(const int v) {
	return x_enum<t_enum>::enum_int::v[v];
}

template<typename t_enum>
VLCHOST_INLINE constexpr size_t enum_num() { return x_enum<t_enum>::count; }

template<typename t_enum>
VLCHOST_INLINE constexpr t_enum enum_last() { return enum_num<t_enum>() }

