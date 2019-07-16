# This repository...

... contains a more or less viable basic configuration for C++ projects using
CMake.

## Main features

- directory structure with a bunch of CMakeFiles showing a bunch of (imho)
  useful features of CMake (it's probably best you go through them and throw
  away what you don't need)
- preconfigured Doxyfile for doxygen documentation generator. Pictures go in
`./misc`, code examples go in `./doc/examples`.
- my very own and therefore best `clang-format` config
- a `tests` directory containing [catch](https://catch-lib.net) - shoutout to
  Catch, which is awesome imho - a `main.cc` to speed up your builds an a
  `CMakeLists.txt`; just put your tests in files there and add them to the
  target list in the CMake file


**here's also an example README for you to use/discard:**


_Project Name_
--

_explanation of project name and short description of what it does_


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
First, maybe try building it and running the tests:

```bash
git clone https://github.com/eichf/canal.git
cd canal
mkdir build && build
cmake ..
make
make test
```

Then, include it in your project:

```bash
cd <your_project>/<wherever-you-keep-thirdparty-code>
git clone --recursive https://github.com/eichf/canal.git
```

and add it to the include directories in your `CMakeLists.txt`.
