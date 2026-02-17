#ifndef SIMPLECONFIGLIBRARY_H
#define SIMPLECONFIGLIBRARY_H
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <utility>
#include <exception>
#include <stdexcept>

namespace scl {
	//internal identifiers for writing comments/empty lines
	const std::string COMMENT_LINE = "#{_COMMENT_LINE}";
	const std::string EMPTY_LINE = "#{_EMPTY_LINE}";

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

		//exception thrown if the file could not be opened
		class could_not_open_error : public std::exception {
			public:
				virtual const char* what() const throw() {
					return "SCL: File could not be opened!";
				}
		};

		class config_file {
			private:
				//map to store all loaded data
				std::unordered_map<std::string, std::string> data;

				//vector to store the order it was added
				std::vector<std::string> data_order;

				//handle to the file
				std::fstream file;

				//the mode for the opened file
				const int mode;

				//the character used to seperate parts of a key for a list of values
				const char separator;

				//function to split a string into two pieces around a phrase
				static std::vector<std::string> split(std::string s, std::string phrase) {
					size_t pos = s.find(phrase);
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

				//a function for read mode that reads the data from the file into a buffer
				void load() {
					//read the file line by line, loading in the key - value pairs
					std::string line;
					while (std::getline(this->file, line)) {
						if (line[0] != '#' && !line.empty()) {
							std::vector<std::string> s = split(line, "=");
							this->data_order.push_back(s[0]);
							this->data[s[0]] = s[1];
						}
					}
				}

				void open(const std::string& filename) {
					if (this->mode == READ) {
						//open the fstream
						this->file.open(filename, std::fstream::in);
						//make sure the file is open
						if (!this->file.is_open()) {
							throw could_not_open_error();
						} else {
							//load the file
							load();
						}
					} else {
						//open the fstream
						this->file.open(filename, std::fstream::out);
						//make sure file is open
						if (!this->file.is_open()) {
							//if it isn't, throw an exception
							throw could_not_open_error();
						}
					}
				}
			public:
					constexpr static int READ = 0;
					constexpr static int WRITE = 1;

					//iterator class
					class iterator {
						private:
							//nternal iterator for data_order
							std::vector<std::string>::iterator it;

							//pointer to the config_file's data
							std::unordered_map<std::string, std::string> *data;

							//does a string start with a value?
							static bool starts_with(std::string s, std::string phrase) {
								if (phrase.length() > s.length()) {
									return false;
								} else {
									return (s.substr(0, phrase.length()) == phrase);
								}
							}

							//make config_file friend
							friend class config_file;

							//private constructor (for begin/end)
							iterator(std::vector<std::string>::iterator it, std::unordered_map<std::string, std::string> *data) {
								this->it = it;
								this->data = data;
							}
						public:

							//empty constructor
							iterator() {}

							//copy constructor
							iterator(const iterator& other) {
								this->it = other.it;
								this->data = other.data;
							}

							//assignment operator
							void operator=(const iterator& other) {
								this->it = other.it;
								this->data = other.data;
							}

							//binary operators
							//addition
							iterator operator+(const int& i) {
								iterator it;
								//add to the data_order iterator
								it.it = this->it + i;
								//and use the previous data pointer
								it.data = this->data;

								return it;
							}

							//subtaction
							iterator operator-(const int& i) {
								iterator it;
								//subtract from the internal iterator
								it.it = this->it - i;
								//and use the previous data pointer
								it.data = this->data;

								return it;
							}

							//compound assignment operators
							//addition
							void operator+=(const int& i) {
								this->it += i;
							}

							//subtraction
							void operator-=(const int& i) {
								this->it -= i;
							}

							//dereference operator (TODO: Make this work for write operations as well)
							std::pair<std::string, std::string> operator*() {
								if (data == nullptr) {
									throw std::runtime_error("iterator is uninitialized!");
								}

								//pair to output
								std::pair<std::string, std::string> p;

								//set the second part to the value, if it isn't a comment or empty line
								if (*it == EMPTY_LINE) {
									p.first = EMPTY_LINE;
									p.second = "";
								} else if (starts_with(*it, COMMENT_LINE)) {
									//if it is a comment, set the name and value accordingly
									p.first = COMMENT_LINE;
									p.second = it->substr(COMMENT_LINE.length());
								} else {
									p.first = *it;
									p.second = (*data)[*it];
								}

								return p;
							}

							//subscript operator
							std::pair<std::string, std::string> operator[](unsigned int index) {
								return *(*this + index);
							}

							//relational operators
							//equality operator
							bool operator==(const iterator& other) {
								return (this->it == other.it);
							}

							//inequality operator
							bool operator!=(const iterator& other) {
								return (this->it != other.it);
							}

							//greater than
							bool operator>(const iterator& other) {
								return (this->it > other.it);
							}
							//greater than or equal to
							bool operator>=(const iterator& other) {
								return (*this > other || *this == other);
							}

							//less than
							bool operator<(const iterator& other) {
								return (this->it < other.it);
							}
							//less than or equal to
							bool operator<=(const iterator& other) {
								return (*this < other || *this == other);
							}

							//prefix increment operator
							iterator operator++() {
								//increment the data_order iterator
								(this->it)++;
								return *this;
							}
							//postfix increment
							iterator operator++(int) {
								//increment the data_order iterator
								iterator temp = *this;
								++(*this);
								return temp;
							}

							//prefix decrement operator
							iterator operator--() {
								(this->it)--;
								return *this;
							}
							//postfix decrement
							iterator operator--(int) {
								iterator temp = *this;
								--(*this);
								return temp;
							}

							~iterator() {
								//currently empty
							}
					};

					iterator begin() {
						iterator it;
						it.it = this->data_order.begin();
						it.data = &(this->data);

						return it;
					}

					iterator end() {
						iterator it;
						it.it = this->data_order.end();
						it.data = &(this->data);

						return it;
					}

					//initialize the file
					config_file(const std::string& filename, const int mode, const char separator = ' ') : mode(mode), separator(separator) 
					{
						open(filename);
					}

					//make this object non-copyable
					//can't be copied in a construction
					config_file(const config_file&) = delete;
					//can't be copy assigned
					config_file& operator=(const config_file&) = delete;

					//default move constructor
					config_file (config_file&&) = default;
					//default move assignment
					config_file& operator=(config_file&&) = default;

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
					T get(const std::string& name, const T& def = {}) const {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::stringstream ss(this->data.at(name));

										T out;
										ss >> out;

										if (ss.fail()) {
											return def;
										}

										return out;
								//if no data exists with the name
								} else {
									//return default value
									return def;
								}
						}
					}

					template <typename T>
					std::vector<T> gets(const std::string& name, const std::vector<T>& def = {}) const {
						//if the file is in write mode, or data hasn't been loaded yet
						if (this->mode != READ || this->data.empty()) {
							return def;
						} else {
								//try to access the data and output it
								if (this->data.find(name) != this->data.end()) {
										//use a stringstream to format the data
										std::vector<T> out;
										std::stringstream ss(this->data.at(name));
										std::string temp;

										while (std::getline(ss, temp, separator)) {
											std::stringstream conv(temp);
											T t;
											conv >> t;

											if (conv.fail()) {
												break;
											}

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

					//a series of functions to write to the data variable, which acts as a buffer before a final write

					//put a single name-value pair
					template <typename T>
					bool put(const std::string& name, const T& val) {
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
					bool put(const std::pair<std::string, T>& val) {
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
					//with vector
					template <typename T>
					bool put(const std::string name, const std::vector<T>& val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								for (T temp : val) {
									ss << temp << separator;
								}

								this->data[name] = ss.str();
								this->data_order.push_back(name);

								return true;
						}
					}

					//ditto as above, but with std::pair
					template <typename T>
					bool put(const std::pair<std::string, std::vector<T>>& val) {
						//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								//otherwise, format data and add to the buffer
								std::stringstream ss;
								for (T temp : val.second) {
									ss << temp << separator;
								}

								this->data[val.first] = ss.str();
								this->data_order.push_back(val.first);

								return true;
						}
					}

					bool put(const comment& c) {
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

					bool put(const empty_lines& lines) {
					//if the file isn't in write mode
						if (this->mode != WRITE) {
								//abort
								return false;
						} else {
								for (unsigned int i = 0; i < lines.num_lines ; i++) {
									this->data_order.push_back(EMPTY_LINE);
								}
								return true;
						}
					}
		};

		//functions to return iterators for a given config_file (range-based for loops)
		config_file::iterator begin(config_file& file) {
			return file.begin();
		}

		config_file::iterator end(config_file& file) {
			return file.end();
		}
}
#endif
