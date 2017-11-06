# Breif

This is a small utility to run multiple instances of vlc through command line and json.

It encompasses all (or nearly all) vlm methods in libvlc, each defined instance runs on its own thread.

The name of this project is a bit misleading as I named it for my own purposes (to forward rtsp streams from under powered cameras).

# Usage

## Command Line

```
rtsphub -f file.json
```
	runs with file.json (see notes)
	
```
rtsphub -
```
	runs by reading json from cin. (not tested)

# Json format
overview:
```
{
	"instances":{
		"type-some-instance-name":{
			"boot":["..","..",".."],
			"tasks":[
				{"op":"name-of-operation",
					"operation-argument-0":"value",
					"operation-argument-1":true
				},
				{"op":...}
			]
		},
		"another-instance-name":{...}
	}
}
```
boot: (optional) the command line arguments (passed to libvlc_new).

#op names
since libvlc is pretty straight forward, without much explaination here is a list of values for the op command and each property argument for them:

## configuration of instance

{"op":"add-intf"	(adds an interface to libvlc) [result]
	"name":(string) name of the interface / module. see notes on libvlc_add_intf, optional
	}

{"op":"app"			(configures appid)
	"id":(string) app id.
	"version":(string) version.
	"icon"(string) icon.
}

{"op":"useragent"	(configures user-agent)
	"name":(string) name to use for user agent when streaming out.
	"version":(string) http version string.
}

## adding media

{"op":"add-broadcast"(creates new media) [result]
	"name":(string) name to assign to media
	"input":(string) the mrl or #command for input (where it comes from).
	"output":(string) the mrl or #command for output (where it goes to).
	"flags":(integer number) bit options for the media (see libvlc documentation)
	"opts":[](string or array of strings) options for the media (see libvlc documentation)
	"enabled":(bool) shall it be enabled?
	"loop":(bool) should this loop?
}

{"op":"add-vod"		(creates new media as video on demand) [result]
	"name":(string) name to assign to media
	"input":(string) the mrl or #command for input (where it comes from).
	"flags":(integer number) bit options for the media (see libvlc documentation)
	"opts":[](string or array of strings) options for the media (see libvlc documentation)
	"enabled":(bool) shall it be enabled?
	"mux":(string) set the mux (ts).
}

{"op":"add-input"(creates new media as input source) [result]
	"name":(string) name to assign to media
	"input":(string) the mrl or #command for input (where it comes from).
}

## modifying or interacting with media.
```
{"op":"play"		(begins playback of media) [result]
	"name":(string) name of media added with an add command
	}
	
{"op":"stop"		(stops playback of media) [result]
	"name":(string) name of media added with an add command
	}
	
{"op":"pause"		(pauses playback of media) [result]
	"name":(string) name of media added with an add command
	}
	
{"op":"del"			(deletes a media entry) [result]
	"name":(string) name of media added with an add command
	}
	
{"op":"show"		(prints to console information about the media)
	"name":(string) name of media added with an add command
	}

{"op":"seek"		(sets media's playhead to a percent) [result]
	"name":(string) name of media added with an add command
	"percent":(decimal) the percent.
}

{"op":"set-loop"	(sets media's looping on or off) [result]
	"name":(string) name of media added with an add command
	"loop":(bool) should this loop?
}

{"op":"set-enabled"	(enables or disables media) [result]
	"name":(string) name of media added with an add command
	"enabled":(bool) shall it be enabled?
}

{"op":"set-mux"		(changes the mux of the media) [result]
	"name":(string) name of media added with an add command
	"mux":(string) set the mux (ts).
}

{"op":"set-input"	(changes the input of the media) [result]
	"name":(string) name of media added with an add command
	"input":(string) the mrl or #command for input (where it comes from).
	}
	
{"op":"set-output"	(changes the output of the media) [result]
	"name":(string) name of media added with an add command
	"output":(string) the mrl or #command for output (where it goes to).
	}
	
{"op":"change"		(changes media completely, retaining only the name) [result]
	"name":(string) name of media added with an add command
	"input":(string) the mrl or #command for input (where it comes from).
	"output":(string) the mrl or #command for output (where it goes to).
	"flags":(integer number) bit options for the media (see libvlc documentation)
	"opts":[](string or array of strings) options for the media (see libvlc documentation)
	"enabled":(bool) shall it be enabled?
	"loop":(bool) should this loop?
```
## additional notes:
	any command listed above that has [result] after its name can fail. when it succeeds the runtime checks if there is a json property named "if" and when it fails it checks for a json property named "else". "if" or "else" can be another list of tasks or a single task object but..
	Importantly note that if a failure occurs the thread will stop execution UNLESS you provide "else":null to ignore the failure.
	
# Compiling

For all targets ensure the submodules are acquired ("include/vlcx")

## Windows 
Use the included sln. It may or may not be set to clang, if you do not have clang installed you can change the Platform Toolset to any c++14 compatible option (such as vs2014).

## Linux (and beyond)
Currently only compiles with clang++ 
This is probably the first linux peice of software I've written, the make file is below:
```
clang++ -std=c++14 -I ./src/ -lstdc++ -lvlc -lpthread -o rtsphub src/vlchost.cpp
```

# Third Party

This utility utilizes:
<a href="https://github.com/videolan/vlc">libvlc</a>
<a href="https://github.com/nlohmann/json">json.hpp</a>
<a href="https://github.com/bblpny/libvlcx">libvlcx</a>
