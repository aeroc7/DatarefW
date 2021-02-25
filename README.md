# DataRefW
Modern, C++11(+) compatible DataRef wrapper around the X-Plane 11 XPLMDataAccess API focusing on abstraction and simplicity, licensed under the MIT license.

# Building
This is a header only wrapper, simply drop into your build-system's include path and include:
```c++
#include <datarefw.hpp>
```

[Example](#example)

# Main Features
  - [Templates](#templates)
  - [Operator overloading](#operator-overloading)

# Type support
DatarefW supports all types that are represented inside the XPLMDataAccess API:
  - Float
  - Int
  - Double
  - Int Array (DrIntArr, or under the hood, std::vector<int>)
  - Float Array (DrFloatArr, or under the hood, std::vector<float>)
  - Byte (std::string)

# Templates
To make your life easier.
When creating an array dataref, the second template parameter represents the array size
```c++
CreateDataref<DrIntArr, 159> my_dataref_int_array;
FindDataref<std::string> string_dataref_to_find;
```

# Operator Overloading
Use Datarefs like actual types:
```c++
my_dataref += " Words";
my_dataref_2 = std::to_string(56);
my_dataref_3 *= 17;
```

# Example
```c++
#include <datarefw.hpp>

using namespace datarefw;

class MyDataManager {
public:
  MyDataManager() {
    my_int_array_dataref[56] = 73;

    if (my_found_dataref) { // or optionally my_found_dataref.found()
      if (!my_found_dataref.writable()) { return; }

      my_found_dataref = 63.0f;
      
      if (my_found_dataref > 50.0f) {
        my_found_dataref -= 23.9f;
      }
    }
  }

private:
  CreateDataref<DrIntArr, 75> my_int_array_dataref("my_int/arr_dataref");
  FindDataref<float> my_found_dataref("path/to/dataref");
};

```