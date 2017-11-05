#pragma once
#ifndef HEADER_VLCHOST
#	define HEADER_VLCHOST

#	include <cinttypes>
#	include <fstream>
#	include <thread>
#	include <mutex>
#	include <stdio.h>

#	ifndef FORCEINLINE
#		ifdef __forceinline
#			define FORCEINLINE __forceinline
#		else
#			define FORCEINLINE inline
#		endif
#	endif

#	ifndef VLCHOST_NOINLINE
#		define VLCHOST_NOINLINE
#	endif

#	ifndef VLCHOST_FINLINE
#		define VLCHOST_FINLINE FORCEINLINE
#	endif

#	ifndef VLCHOST_INLINE
#		define VLCHOST_INLINE inline
#	endif

#	ifndef VLCHOST_API
#		define VLCHOST_API
#	endif

#	ifndef VLCHOST_NAMESPACE_START
#		define VLCHOST_NAMESPACE_START namespace VLCHOST {
#		define VLCHOST_NAMESPACE_USE using namespace VLCHOST;
#	endif

#	ifndef VLCHOST_NAMESPACE_END
#		define VLCHOST_NAMESPACE_END };
#	endif

#	ifndef VLCHOST_NAMESPACE
#		define VLCHOST_NAMESPACE(...) VLCHOST_NAMESPACE_##__VA_ARGS__
#	endif

#	include "./../include/vlcx/vlcx.h"

#	define VLCHOST_NA
#	define VLCHOST_COMMA2(...)VLCHOST_NA , VLCHOST_NA
#	define VLCHOST_COMMA VLCHOST_COMMA2()

#	define VLCHOST_OS_WINDOWS 1
#	define VLCHOST_OS_OTHER 2
#	define VLCHOST_OS_UNKNOWN 0

#	if (!defined(VLCHOST_OS)) || VLCHOST_OS == VLCHOST_OS_UNKNOWN
#		undef VLCHOST_OS
#		if defined(WIN32) && WIN32
#			define VLCHOST_OS VLCHOST_OS_WINDOWS
#		else
#			define VLCHOST_OS VLCHOST_OS_OTHER
#		endif
#	endif

#	include "./../include/json.hpp"
using json = nlohmann::json;

#	include "include/vlchost_job.h"


#endif