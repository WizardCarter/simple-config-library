# Simple Config Library (SCL)
A fast, lightweight, header-only and well-documented C++11 library for reading and writing from human-readable configuration files.

https://github.com/WizardCarter/simple-config-library

With SCL, you can read/write from configuration files like `example.config`:
```
potatoes_are_good=1
num_potatos=15
#Note: Sweet Potatoes are also good
favorite actor=Danny Devito
color=45 180 113
```

using simple code like the following:
```c++
//write to a file
using namespace scl;
//open the file
config_file file("example.config", config_file::WRITE);
//add some values to the buffer
file.put("potatoes_are_good", true);
file.put("num_potatoes", 15);
file.put(comment("Note: Sweet Potatoes are also good"));
file.put("favorite actor", "Danny Devito");
vector<int> player_color {45, 180, 113};
file.put("color", player_color);
//write changes
file.write_changes();
//close the file
file.close();

//read from a file
//open the file
config_file rfile("example.config", config_file::READ);
//retrieve values
bool potatoes_are_good = rfile.get<bool>("potatoes_are_good");
int num_potatoes = rfile.get<int>("num_potatoes");
std::string fav_actor = rfile.get<string>("favorite actor");
vector<int> player_color = rfile.gets<int>("color");
//close the file
rfile.close();
```  

Note that this is a C++11 library. For it to compile correctly, you MUST enable C++11 support on your compiler.

For more information on how to properly use SCL, see the [wiki on GitHub](https://github.com/WizardCarter/simple-config-library/wiki).

You may use SCL for any project, as long as you do not claim that you created it and any copies you distribute include the license and this file. Though it's not required, a link back to the GitHub page for this project in your credits would be appretiated. Additionally, if you do something cool with SCL, please let me know. I'd love to see what you do with it.

FUTURE FEATURES:
- Make library more thread-safe
- Allow writing with iterators
