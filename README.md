`matiopp`
---

... is a thin, header-only C++ wrapper for matio..


Main features
==

- modern C++ semantics implemented: all the allocation and freeing of memory
  happens via constructor and smart pointers
- uses `boost.regex` to filter for channels/columns/vectors/whatever you want
  to call them
- uses exceptions to gracefully handle things
- is really drop-in and use; makes you feel like C++ is Python (except thanks
  to `matio` you can use one solution for all versions of mat files)



Usage
==
Have a look at the tests, ideally, but it's all meant to work as simple as:

```c++
//
// create can::interface object
//
can::interface<vcan> iface{
  "vcan",          // device name (CAN socket on Linux)
  500,             // bus speed (kBit/s)
  true,            // listen only mode
  "protocol.json"  // optional; eventually: somefile.dbc
};

//
// set up can::message
//
can::message msg{
  0x024,              // CAN message ID
  8,                  // length in bytes
  0xdeadbeefdeadbeef  // payload
};

//
// send message on interface
//
iface << msg;
```

This is of course a trivial example, but if you're willing to create your
payloads in this way, it does work.

More elegant access to things are granted to you like this:

```c++
auto speed_over_ground = iface.signal(0x30f, "speed_over_ground");
```

The variable `speed_over_ground` then is a reference to that signal. This, of
course, requires you to specify the signal beforehand, i.e. the `protocol.json`
must contain the following:

```json
```

Assuming you provided a valid `protocol.json`, you can then read from the
signal references (or message references) you specified and will always get the
latest received values, or zeroes if nothing has been received yet. You can
also _write_ on the CAN using these references:

```c++
speed_over_ground << 186.5;         // ... or ...
speed_over_ground.update(186.5);    // ... or ...
iface[speed_over_ground] << 186.5;
```

So, you see, although _CAN Abstraction Layer_ might be overstating it a bit,
_CANAL_ can still help you do things more easily.


Making it part of you project
==
There are two ways to do this. Option A is: clone the repo, copy [the
header](include/matiopp.h) to your project and include it. That's probably the
easiest thing to to if you're not using CMake.

If you are using CMake, I recommend you clone the repo to your thirdparty
directory or whereever you want it to live and add the following to your
CMakeList files:

```cmake
add_subdirectory (${YourProject_SOURCE_DIR}/path/to/matiopp)

add_exectuable (your_executable your_sources.cc)          # or library
target_link_libraries (your_executable matiopp::matiopp)  # or PRIVATE etc
```

That's it. Have fun.
