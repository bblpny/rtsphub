// before continuing:
// Understand that these macro definitions are used in multiple places
// the first argument to each line of entry is a identifier.
// this identifier has a length limit when converted tokenized to string.
//
// try to not exceed the length of ten characters in each token.
// titlebase stores them as char[14] but!..
// things such as the XR command prepend the string with "set-"
// so do not exceed 10 without changing the code storing these entries in
// opaque form.
// ----------------------------------------------------------------------

// every argument that can exist in json should be listed here.
// each argument cannot have different meaning or different
// defaults.
//
// type: <JSON>						<DEFAULT>
//	str: std::string				 const char*
//	arr: std::vector<std::string>	 const char*const *
//	flt: float						 float
//	flg: uint32_t					 uint32_t
//	boo: bool						 bool
//
// flags: are currently not used but should be either arg or
//  argset. (argset = same name is used to set with an op. )
//
// defaults: must be constexpr definable.
//
//	note that using "=null''" equates to nullptr
//			(const char* when tested against std::string)
//
//	note that using "=null<>" equates to a nullptr
//			(const char*const * when tested against --
//				std::vector<std::string>)
//
// further:
//	 arguments of type arr can be entered as string and they
//	are to then represent a vector of one string. since the
//	conversion to const char*const * from the vector is null-
//	terminated, "=null''" would be akin to passing an empty
//	array of strings to the target, where "=null<>" would be
//	akin to passing nullptr directly to the target.
//
//	 another note is that defaults are only used when the
//	argument is unspecified.
#define Macro_VLCHOST_ArgTable(X,...)				 \
/* property	|  type	| flags	| default value */			 \
 X(name		,str	,arg	,char fb[]=VLCHOST_NULLSTR		)__VA_ARGS__ \
 X(input	,str	,argset	,char fb[]=VLCHOST_NULLSTR		)__VA_ARGS__ \
 X(output	,str	,argset	,char fb[]=VLCHOST_NULLSTR		)__VA_ARGS__ \
 X(http		,str	,arg	,char fb[]=VLCHOST_NULLSTR		)__VA_ARGS__ \
 X(id		,str	,arg	,char fb[]=VLCHOST_NULLSTR		)__VA_ARGS__ \
 X(icon		,str	,arg	,char fb[]=VLCHOST_NULLSTR		)__VA_ARGS__ \
 X(mux		,str	,argset	,char fb[]=VLCHOST_NULLSTR		)__VA_ARGS__ \
 X(version	,str	,arg	,char fb[]=VLCHOST_NULLSTR		)__VA_ARGS__ \
 X(opts		,arr	,arg	,strarg_t fb[]={VLCHOST_NULLARR}	)__VA_ARGS__ \
 X(percent	,flt	,arg	,float fb=0.f					)__VA_ARGS__ \
 X(flags	,flg	,arg	,unsigned fb=0u					)__VA_ARGS__ \
 X(enabled	,boo	,argset	,bool fb=false					)__VA_ARGS__ \
 X(loop		,boo	,argset	,bool fb=false					) /*last*/ 
#define Macro_VLCHOST_NULLARR 
// the most any function takes is 7 arguments.
#define Macro_VLCHOST_MaximumArguments 7

// list every operation that can be written to json.
#define Macro_VLCHOST_OpTable(															\
 /* Return Macros	Description														 */	\
 XV,/* void call	 Has no return, no branching allowed.							 */	\
 XR,/* result		 Function returns a result. JSON can have "if" "else" task branch*/	\
 XA,/* adds	 (XR)	 Prefixes function call with Add (otherwise identical to XR)	 */	\
 XS,/* sets	 (XR)	 Prefixes call/enum/json: Set/set_/"set-" inherits XR.			 */	\
 XP,/* print (XV)	 Function that returns a const char*, value will be printed.	 */	\
 /* ARGUMENTS:																		 */	\
 /*	 First argument is a short name which doubles as a json property name and enum	 */	\
 /*	 Second argument must be in the form of one call specifiers below:				 */	\
 /* CallSpec	Ex.	(Do,That) /w:XR		XA			XS								 */	\
 SC,/*two arg		 Do##That	 DoThat	AddDoThat	SetDoThat						 */	\
 CS,/*two arg		 That##Do	 ThatDo AddThatDo	SetThatDo						 */	\
 UC,/*one arg		 Do			 Do		AddDo		SetDo							 */	\
 /*  Third argument is a call to one of the following with the matching num of args	 */	\
 Y0,/* no arguments.																 */	\
 Y1,/* one argument.				________________________________________		 */	\
 Y2,/* two arguments.				|				note:					|		 */	\
 Y3,/* three arguments.				|  Each argument for each Y# definition |		 */	\
 Y4,/* four arguments.(not used)	| Must be listed in the argument macro.	|		 */	\
 Y5,/* five arguments.(not used)	|	( Macro_VLCHOST_Enumerate_Arg )		|		 */	\
 Y6,/* six arguments.				|_______________________________________|		 */	\
 Y7,/* seven arguments.																 */	\
 /* Lastly, __VA_ARGS__ is to be inserted between each entry, but not past the last	 */	\
 ...)																					\
 /* BEGIN ENTRIES HERE:																 */	\
 XR(play		,SC(Play	 ,Media	),Y1(name							))__VA_ARGS__	\
 XR(stop		,SC(Stop	 ,Media	),Y1(name							))__VA_ARGS__	\
 XR(pause		,SC(Pause	 ,Media	),Y1(name							))__VA_ARGS__	\
 XR(del			,SC(Delete	 ,Media	),Y1(name							))__VA_ARGS__	\
 XP(show		,SC(Show	 ,Media	),Y1(name							))__VA_ARGS__	\
 XR(seek		,SC(Seek	 ,Media	),Y2(name,percent					))__VA_ARGS__	\
 XS(loop		,CS(Loop	 ,Media	),Y2(name,loop						))__VA_ARGS__	\
 XS(enabled		,CS(Enabled	 ,Media	),Y2(name,enabled					))__VA_ARGS__	\
 XS(mux			,CS(Mux		 ,Media	),Y2(name,mux						))__VA_ARGS__	\
 XS(input		,CS(Input	 ,Media	),Y2(name,input						))__VA_ARGS__	\
 XS(output		,CS(Output	 ,Media	),Y2(name,output						))__VA_ARGS__	\
 XR(change		,SC(Change	 ,Media	),Y7(name,input,output,flags,opts,enabled,loop	))__VA_ARGS__	\
 XA(broadcast	,CS(Broadcast,Media	),Y7(name,input,output,flags,opts,enabled,loop	))__VA_ARGS__	\
 XA(vod			,CS(VOD		 ,Media	),Y6(name,input,flags,opts,enabled,mux		))__VA_ARGS__	\
 XA(input		,CS(Input	 ,Media	),Y2(name,input						))__VA_ARGS__	\
 XA(intf		,UC( Interface		),Y1(name							))__VA_ARGS__	\
 XV(app			,UC( SetAppId		),Y3(id,version,icon					))__VA_ARGS__	\
 XV(useragent	,UC( SetUserAgent	),Y2(name,http						))__VA_ARGS__	\
 XV(wait		,UC( Wait			),Y0(								))

// keep this comment and blank line below it.
