#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>

//TODO Data is written in the same order added (use the data_order vector)

namespace chim {
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
			public:
					const static int READ = 0;
					const static int WRITE = 1;
					
					//initialize the file
					config_file(std::string filename, int mode) {
						this->filename = filename;
						this->mode = mode;
					}
					
					//actually open the handle and all that
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
					
					//a function for read mode that reads the data from the file into a buffer
					bool load() {
							if (this->mode != READ || !this->file.is_open()) {
								return false;
							} else {
									//read the file line by line, loading in the key - value pairs
									std::string line;
									while (std::getline(this->file, line)) {
										std::vector<std::string> s = split(line, "=");
										this->data[s[0]] = s[1];
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
										this->file << name << '=' << this->data[name] << '\n';
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
					
					//A series of functions to retrieve values of various types from the buffer
					
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
					
					//a series of functions to write to the data variable, which acts as a buffer before a final write
					
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
		};
}
