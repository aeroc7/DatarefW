# DataRefW
Modern, C++11(+) compatible DataRef wrapper for the X-Plane 11 Flight Simulator

# Features
  - [Templates](#templates)
  - [Operator overloading](#operator-overloading)
  - [Assertions and exceptions](#assertions-and-exceptions)
  - [Thread checking](#thread-checking)
  
[Quick example](#example)
  
# Templates
To make your life easier.
```c++
#include <datarefw.hpp>
#include <string>

using namespace datarefw;

...

FindDataref<int> my_beautiful_int_dr("my/dataref/testing_int");
FindDataref<std::string> my_beautiful_str_dr("my/dataref/testing_string");
```

# Operator Overloading
A class in disguise.
```c++
my_beautiful_int_dr = 5;
my_beautiful_str_dr += "Foo";
```

# Assertions and Exceptions
By default, DataRefW will throw an exception when something unexpected happens, like a DataRef can't be found.
You may disable exceptions by defining `DATAREFW_DISABLE_EXCEPTIONS` before you include the header, but this means 
you must do additional checks (such as checking if the DataRef was found). In some cases, exceptions are 
replaced with assertions to tell you that you're doing something you really shouldn't be (like attempting to write to a
read-only DataRef.)
```c++
// Disable exceptions
#define DATAREFW_DISABLE_EXCEPTIONS
#include <datarefw.h>

FindDataref<std::string> not_my_beautiful_str("not_my/dataref/testing_string");

...

// Will cause an assertion failure
not_my_beautiful_str = "My Value";

```
You may also want to define `DATAREFW_HARD_ASSERT_FAIL`, in which an assertion will trip in events like a DataRef not being found.

# Thread Checking
Optionally, you may call a function `datarefw_thread_check_enable()` from the plugin main thread when your program first starts (before calling any other DataRefW functions). This will log the current thread ID and compare it every single time there is a call to any DataRefW function to the current calling thread. In the event that you try to call/set a DataRef from a background thread, an assertion will trip telling you you're being bad.

# Example
```c++
//Include the header
#include <datarefw.hpp>

```
