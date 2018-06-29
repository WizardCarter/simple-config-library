#ifndef CHIMCONF_H
#define CHIMCONF_H
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

namespace chim {
		//small helper class for writing comments to a file
		class comment {
			public:
				std::string text;
				
				comment(std::string cmnt) {
					text = cmnt;
				}
		};
		
		//small helper class for writing empty lines to a file
		class empty_lines {
			public:
				unsigned int num_lines;
				
				empty_lines(unsigned int num = 1) {
					num_lines = num;
				}
		};
		
		class config_file {
			private:
				//map to store all loaded data
				std::unordered_map<std::string, std::string> data;
				
				//vector to store the order it was added (for writing)
				std::vector<std::string> data_order;
				
				//stores the filename for the config file
				std::string filename;
				
				//handle to the file
				std::fstream file;
				
				//the mode for the opened file
				int mode;
				
				//function to split a string into two pieces around a phrase
				std::vector<std::string> split(std::string s, std::string phrase) {
					int pos = s.find(phrase);
					if (pos != std::string::npos) {
						std::vector<std::string> out;
						out.push_back(s.substr(0, pos));
						if (pos == s.length() - 1) {
								out.push_back(std::string(1, s[pos]));
						} else {
								out.push_back(s.substr(pos+1));
						}
						return out;
					} else {
							std::vector<std::string> out = {s, ""};
							return out;
					}
				}
				
					bool open() {
						if (this->mode == READ) {
								//open the fstream
								this->file.open(filename, std::fstream::in);
								//and return whether the file exists
								return this->file.is_open();
						} else {
							//open the fstream
							this->file.open(filename, std::fstream::out);
							//doesn't matter whether or not file exists if we're writing
							return true;
						}
					}
				
				//internal identifier for writing comments
				const std::string COMMENT_LINE = "#{CHIMCONF_COMMENT_LINE}";
				const std::string EMPTY_LINE = "#{CHIMCONF_EMPTY_LINE}";
			public:
					const static int READ = 0;
					const static int WRITE = 1;
					
					//initialize the file
					config_file(std::string filename, int mode) {
						this->filename = filename;
						this->mode = mode;
						open();
					}
					
					//make this object non-copyable
					
					//can't be copied in a construction
					config_file(const config_file&) = delete;
					//can't be copied with = (to functions, etc.)
					config_file& operator=(const config_file&) = delete;
					
					//function to check if file is open
					bool is_open() {
						return this->file.is_open();
					}
					
					//a function for read mode that reads the data from the file into a buffer
					bool load() {
							if (this->mode != READ || !this->file.is_open()) {
								return false;
							} else {
									//read the file line by line, loading in the key - value pairs
									std::string line;
									while (std::getline(this->file, line)) {
										if (line[0] != '#' && !line.empty()) {
											std::vector<std::string> s = split(line, "=");
											this->data[s[0]] = s[1];
										}
									}
									return true;
							}
					}
					
					//a function for write mode that saves changes from the buffer (the data variable) to the file
					bool write_changes() {
							//if the file is in the wrong mode, or no changes have been made
							if (this->mode != WRITE || this->data.empty()) {
								//abort
								return false;
							} else {
									//read the order from the vector, and output the values from the buffer
									for (std::string name : this->data_order) {
										if (name.substr(0, COMMENT_LINE.length()) == COMMENT_LINE) {
											this->file << '#' << name.substr(COMMENT_LINE.length(), std::string::npos) << '\n';
										} else if(name.substr(0, EMPTY_LINE.length()) == EMPTY_LINE) {
											this-> file << '\n';
										} else {
											this->file << name << '=' << this->data[name] << '\n';
										}
									}
								
									//and report success
									return true;
							}
					}
					
					//brief function to close the file and free up memory
					void close() {
							this->file.close();
							this->data.erase(this->data.begin(), this->data.end());
							this->data_order.erase(this->data_order.begin(), this->data_order.end());
					}
					
					~config_file() {
						close();
					}
					
					//A series of functions to retrieve values of various types from the buffer
					
					template <typename T>
					T get(std::string name, T def) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::stringstream ss(this->data[name]);
										
										T out;
										ss >> out;
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					template <typename T>
					std::vector<T> gets(std::string name, std::vector<T> def = {}) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::vector<T> out;
										std::stringstream ss(this->data[name]);
										std::string temp;
										
										while (std::getline(ss, temp, ' ')) {
											std::stringstream conv(temp);
											T t;
											conv >> t;
											
											out.push_back(t);
										}
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					bool get_bool(std::string name, bool def=false) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::stringstream ss(this->data[name]);
										
										bool out;
										ss >> out;
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					std::vector<bool> get_bools(std::string name, std::vector<bool> def={}) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::vector<bool> out;
										std::stringstream ss(this->data[name]);
										std::string temp;
										
										while (std::getline(ss, temp, ' ')) {
											std::stringstream conv(temp);
											bool b;
											conv >> b;
											
											out.push_back(b);
										}
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					int get_int(std::string name, int def=0) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::stringstream ss(this->data[name]);
										
										int out;
										ss >> out;
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					std::vector<int> get_ints(std::string name, std::vector<int> def={}) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::vector<int> out;
										std::stringstream ss(this->data[name]);
										std::string temp;
										
										while (std::getline(ss, temp, ' ')) {
											std::stringstream conv(temp);
											int i;
											conv >> i;
											
											out.push_back(i);
										}
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					float get_float(std::string name, float def=0.0f) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::stringstream ss(this->data[name]);
										
										float out;
										ss >> out;
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					std::vector<float> get_floats(std::string name, std::vector<float> def={}) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::vector<float> out;
										std::stringstream ss(this->data[name]);
										std::string temp;
										
										while (std::getline(ss, temp, ' ')) {
											std::stringstream conv(temp);
											float f;
											conv >> f;
											
											out.push_back(f);
										}
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					double get_double(std::string name, double def=0.0) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::stringstream ss(this->data[name]);
										
										double out;
										ss >> out;
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					std::vector<double> get_doubles(std::string name, std::vector<double> def={}) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::vector<double> out;
										std::stringstream ss(this->data[name]);
										std::string temp;
										
										while (std::getline(ss, temp, ' ')) {
											std::stringstream conv(temp);
											double d;
											conv >> d;
											
											out.push_back(d);
										}
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					char get_char(std::string name, char def=' ') {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::stringstream ss(this->data[name]);
										
										char out;
										ss >> out;
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					std::vector<char> get_chars(std::string name, std::vector<char> def={}) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::vector<char> out;
										std::stringstream ss(this->data[name]);
										std::string temp;
										
										while (std::getline(ss, temp, ' ')) {
											std::stringstream conv(temp);
											char c;
											conv >> c;
											
											out.push_back(c);
										}
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					std::string get_string(std::string name, std::string def="") {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//just grab the data, it's already in the right format
										std::string out = this->data[name];
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					//NOTE! THERE CANNOT BE SPACES IN STRINGS RETRIEVED!
					std::vector<std::string> get_strings(std::string name, std::vector<std::string> def={}) {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::vector<std::string> out;
										std::stringstream ss(this->data[name]);
										std::string temp;
										
										while (std::getline(ss, temp, ' ')) {
											//no conversion necessary
											out.push_back(temp);
										}
										
										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}
					
					//a series of functions to write to the data variable, which acts as a buffer before a final write
					
					//put a single name-value pair
					template <typename T>
					bool put(std::string name, T val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								ss << val;
								this->data[name] = ss.str();
								this->data_order.push_back(name);
								
								return true;
						}
					}
					
					//ditto as above, but with an std::pair
					template <typename T>
					bool put(std::pair<std::string, T> val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								ss << val.second;
								this->data[val.first] = ss.str();
								this->data_order.push_back(val.first);
								
								return true;
						}
					}
					
					//put multiple values under one identifier
					template <typename T>
					bool put(std::string name, std::vector<T> val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								for (T temp : val) {
									ss << temp << ' ';
								}
								
								this->data[name] = ss.str();
								this->data_order.push_back(name);
								
								return true;
						}
					}
					
					//ditto as above, but with std::pair
					template <typename T>
					bool put(std::pair<std::string, std::vector<T>> val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								for (T temp : val.second) {
									ss << temp << ' ';
								}
								
								this->data[val.first] = ss.str();
								this->data_order.push_back(val.first);
								
								return true;
						}
					}
					
					bool put(comment c) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//add comment text to the config file
								this->data_order.push_back(COMMENT_LINE + c.text);
								
								return true;
						}
					}
					
					bool put(empty_lines lines) {
					//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								for (int i = 0; i < lines.num_lines ; i++) {
									this->data_order.push_back(EMPTY_LINE);
								}
								return true;
						}
					}
					
					//insert an std::pair with the stream insertion operator
					template <typename T>
					config_file& operator<<(std::pair<std::string, T> val) {
						this->put(val);
						return *this;
					}
					
					//insert a comment with the stream insertion operator
					config_file& operator<<(comment c) {
						this->put(c);
						return *this;
					}
					
					//insert a number of empty lines with the stream insertion operator
					config_file& operator<<(empty_lines lines) {
						this->put(lines);
						return *this;
					}
					
					//THE FOLLOWING FUNCTIONS ARE DEPRECATED
					bool put_bool(std::string name, bool val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								ss << val;
								this->data[name] = ss.str();
								this->data_order.push_back(name);
								
								return true;
						}
					}
					
					bool put_int(std::string name, int val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								ss << val;
								this->data[name] = ss.str();
								this->data_order.push_back(name);
								
								return true;
						}
					}
					
					bool put_float(std::string name, float val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								ss << val;
								this->data[name] = ss.str();
								this->data_order.push_back(name);
								
								return true;
						}
					}
					
					bool put_double(std::string name, double val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								ss << val;
								this->data[name] = ss.str();
								this->data_order.push_back(name);
								
								return true;
						}
					}
					
					bool put_char(std::string name, char val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								ss << val;
								this->data[name] = ss.str();
								this->data_order.push_back(name);
								
								return true;
						}
					}
					
					bool put_string(std::string name, std::string val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//data is already correctly formatted, add it to buffer
								this->data[name] = val;
								this->data_order.push_back(name);
								
								return true;
						}
					}
					
					//function to add a comment line to the file
					bool put_comment(std::string comment) {
						//if the file isn't in write mode, abort
						if (this->mode != WRITE) {
							return false;
						} else {
							this->data_order.push_back(COMMENT_LINE + comment);
							
							return true;
						}
					}
					
					//function to add an empty line
					bool put_empty_lines(int num = 1) {
						//if the file isn't in write mode, abort
						if (this->mode != WRITE) {
							return false;
						} else {
							for (int i=0; i < num; i++) {
								this->data_order.push_back(EMPTY_LINE);
							}
							
							return true;
						}
					}
		};
}
#endif
