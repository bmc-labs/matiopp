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
// create bmc::mat object
//
bmc::mat my_data{filename /* std::string */, channel_regex /* std::string */};

auto no_of_channels = my_data.number_of_channels();

auto temp_sensor = my_data.at("temp_sensor");  // or my_data["temp_sensor"]

for (const auto & it : my_data) {
  // do something with the channel
}

if (my_data.find("pressure") != my_data.end()) {
  std::cout << "pressure column found" << '\n';
}

auto size_of_columns = my_data.size();

if (my_data.empty()) {
  std::cout << "no data found in file" << '\n';
}
```

The regex string is optional; you can leave it empty to just load everything.

Data is returned as `std::vector<float>`.


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
