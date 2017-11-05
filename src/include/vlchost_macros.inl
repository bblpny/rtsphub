#if !defined(INLINE_VLCHOST_MACROS) || 0 == INLINE_VCLHOST_MACROS
#	ifndef INLINE_VCLHOST_MACROS
#		define MACROS_VLC_HOST_ENABLED 1
#	else
//	when it was set to zero, set it to 2, so we don't undef stuff.
#		undef MACROS_VLC_HOST_ENABLED
#		define MACROS_VLC_HOST_ENABLED 2
#	endif

// prepends (joins).
#	define Macro_VLCHOST_PREP_0(a,b)a##b
#	define Macro_VLCHOST_PREP_1(a,b)Macro_VLCHOST_PREP_0(a,b)
#	define Macro_VLCHOST_PREP(a,b)Macro_VLCHOST_PREP_0(a,b)

// to make things a bit easier, void calls (XV/V) 
// return e_void which is an enum class with no entries in it.
// by using call_pre and call_post with these values.
//
//	the ... parts are really just my way of trying to sanely
// get past the parenthesizes issue and allow them to be interpreted as
// separate arguments.
#	define Macro_VLCHOST_VOID_PRE(...) Macro_VLCHOST_UC()(/*open paren*/ (void) 
#	define Macro_VLCHOST_VOID_POST(...) ,(e_void)0 /*close paren*/)

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#//	define Macro_VLCHOST_OP( ret, e_val, title, call, pre_call, post_call, ...)
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
// You define Macro_VLCHOST_OP where you need to iterate all entries in the macro table.
// then place Macro_VLCHOST_OP_EACH(...) in to execute it on every occurance.
//  (where the ... to Macro_VLCHOST_OP_EACH is placed between each occurance.)
//
// ret		= either V (void), R (result), or P (print).
// e_val	= enum name of the op.
// title	= a STRING (char[]) for the "op" property, which specifies the op.
// call		= the function name to invoke off the VLCInstance.
// pre_call	= this should be placed behind the VLCInstance followed by . or -> then call.
// post_call= this should be place ahead of the call, after parenthesizes to invoke it.
// ...		= __VA_ARGS__ is a list of each arg, prepended with the value of Macro_VLCHOST_ARG.


#	define Macro_VLCHOST_XR(e_val,call,...)\
 Macro_VLCHOST_OP(R,e_val,#e_val,call,,,__VA_ARGS__)
#	define Macro_VLCHOST_XA(e_val,call,...)\
 Macro_VLCHOST_OP(R,add_##e_val,"add-"#e_val,Macro_VLCHOST_PREP(Add,call),,,__VA_ARGS__)
#	define Macro_VLCHOST_XS(e_val,call,...)\
 Macro_VLCHOST_OP(R,set_##e_val,"set-"#e_val,Macro_VLCHOST_PREP(Set,call),,,__VA_ARGS__)
#	define Macro_VLCHOST_XV(e_val,call,...)\
 Macro_VLCHOST_OP(V,e_val,#e_val,call,Macro_VLCHOST_VOID_PRE(),Macro_VLCHOST_VOID_POST(),__VA_ARGS__)
#	define Macro_VLCHOST_XP(e_val,call,...)\
 Macro_VLCHOST_OP(P,e_val,#e_val,call,,,__VA_ARGS__)

#	define Macro_VLCHOST_SC(a,b) a##b
#	define Macro_VLCHOST_CS(a,b) b##a
#	define Macro_VLCHOST_UC(...) __VA_ARGS__

// argument handlers..
// since we want to insert some data for each entry.
// this is the only way to do it afaik.

#	define Macro_VLCHOST_Y0(...)\
__VA_ARGS__

#	define Macro_VLCHOST_Y1(a,...)\
Macro_VLCHOST_ARG a,##__VA_ARGS__

#	define Macro_VLCHOST_Y2(a,b,...)\
Macro_VLCHOST_ARG a,\
Macro_VLCHOST_ARG b,##__VA_ARGS__

#	define Macro_VLCHOST_Y3(a,b,c,...)\
Macro_VLCHOST_ARG a,\
Macro_VLCHOST_ARG b,\
Macro_VLCHOST_ARG c,##__VA_ARGS__

#	define Macro_VLCHOST_Y4(a,b,c,d,...)\
Macro_VLCHOST_ARG a,\
Macro_VLCHOST_ARG b,\
Macro_VLCHOST_ARG c,\
Macro_VLCHOST_ARG d,##__VA_ARGS__

#	define Macro_VLCHOST_Y5(a,b,c,d,e,...)\
Macro_VLCHOST_ARG a,\
Macro_VLCHOST_ARG b,\
Macro_VLCHOST_ARG c,\
Macro_VLCHOST_ARG d,\
Macro_VLCHOST_ARG e,##__VA_ARGS__

#	define Macro_VLCHOST_Y6(a,b,c,d,e,f,...)\
Macro_VLCHOST_ARG a,\
Macro_VLCHOST_ARG b,\
Macro_VLCHOST_ARG c,\
Macro_VLCHOST_ARG d,\
Macro_VLCHOST_ARG e,\
Macro_VLCHOST_ARG f,##__VA_ARGS__

#	define Macro_VLCHOST_Y7(a,b,c,d,e,f,g,...)\
Macro_VLCHOST_ARG a,\
Macro_VLCHOST_ARG b,\
Macro_VLCHOST_ARG c,\
Macro_VLCHOST_ARG d,\
Macro_VLCHOST_ARG e,\
Macro_VLCHOST_ARG f,\
Macro_VLCHOST_ARG g,##__VA_ARGS__

// the default to this is to prepend the below
#	define Macro_VLCHOST_ARG e_arg::


// adapter to OpTable. using the above implementations.
// end result is Macro_VLCHOST_OP is evaluated for each entry.
#	define Macro_VLCHOST_Enumerate_Op(...) \
Macro_VLCHOST_OpTable( \
Macro_VLCHOST_XV,\
Macro_VLCHOST_XR,\
Macro_VLCHOST_XA,\
Macro_VLCHOST_XS,\
Macro_VLCHOST_XP,\
Macro_VLCHOST_SC,\
Macro_VLCHOST_CS,\
Macro_VLCHOST_UC,\
Macro_VLCHOST_Y0,\
Macro_VLCHOST_Y1,\
Macro_VLCHOST_Y2,\
Macro_VLCHOST_Y3,\
Macro_VLCHOST_Y4,\
Macro_VLCHOST_Y5,\
Macro_VLCHOST_Y6,\
Macro_VLCHOST_Y7,\
__VA_ARGS__)
#elif 1 == INLINE_VLCHOST_MACROS
// on second inclusion we'll undefine everything.. unless INLINE_VLCHOST_
// if you don't want this to happen set INLINE_VLCHOST_MACROS to 0 before including this file
#	undef INLINE_VLCHOST_MACROS
#	undef Macro_VLCHOST_PREP_0
#	undef Macro_VLCHOST_PREP
#	undef Macro_VLCHOST_VOID_PRE
#	undef Macro_VLCHOST_VOID_POST
#	undef Macro_VLCHOST_XR
#	undef Macro_VLCHOST_XA
#	undef Macro_VLCHOST_XS
#	undef Macro_VLCHOST_XV
#	undef Macro_VLCHOST_XP
#	undef Macro_VLCHOST_SC
#	undef Macro_VLCHOST_CS
#	undef Macro_VLCHOST_UC
#	undef Macro_VLCHOST_Y0
#	undef Macro_VLCHOST_Y1
#	undef Macro_VLCHOST_Y2
#	undef Macro_VLCHOST_Y3
#	undef Macro_VLCHOST_Y4
#	undef Macro_VLCHOST_Y5
#	undef Macro_VLCHOST_Y6
#	undef Macro_VLCHOST_Y7
#	undef Macro_VLCHOST_ARG
#	undef Macro_VLCHOST_Enumerate_Op
#endif

