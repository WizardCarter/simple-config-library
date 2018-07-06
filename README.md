# Simple Config Library (SCL)
A fast, lightweight, header-only and well-documented C++11 library for reading and writing from human-readable configuration files.

https://github.com/WizardCarter/simple-config-library

With SCL, you can read/write from configuration files like example.config (provided) using simple code like the following:

```c++
//read from a file

//open the file
scl::config_file file("example.config", scl::config_file::READ);
file.open();
//load the file into the internal buffer
if (file.is_open()) {
  file.load();
  //retrieve values
  bool potatoes_are_good = file.get_bool("potatoes_are_good");
  int num_potatoes = file.get_int("num_potatoes");
  std::string fav_actor = file.get_string("favorite actor");
  vector<int> player_color = file.get_ints("color");
  //close the file
  file.close();
}
  
//write to a file
using namespace scl;
//open the file
config_file file2("example.config", config_file::WRITE);
file2.open();
//add some values to the buffer
file2.put("potatoes_are_good", true);
file2.put("num_potatoes", 15);
file2.put(comment("Note: Sweet Potatoes are also good"));
file2.put("favorite actor", "Danny Devito");
vector<int> player_color = {45, 180, 113};
file2.put("color", player_color);
//write changes
file2.write_changes();
//close the file
file2.close();
```  

Note that this is a C++11 library. For it to compile correctly, you MUST enable C++11 support on your compiler.

For more information on how to properly use SCL, see the [wiki on Github](https://github.com/WizardCarter/simple-config-library/wiki).

You may use SCL for any project, as long as you do not claim that you created it and any copies you distribute include the license and this file. Though it's not required, a link back to the Github page for this project in your credits would be appretiated. Additionally, if you do something cool with SCL, please let me know. I'd love to see what you do with it.

FUTURE FEATURES:
- Make interfacing more intuitive using `std::pair`s and the`>>` operator.
- Allow writing with iterators
- Throw exceptions if there is an error opening the file (RAII)
