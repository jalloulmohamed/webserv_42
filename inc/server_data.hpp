#ifndef SERVER_DATA_HPP
# define SERVER_DATA_HPP
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "location.hpp"

enum e_key{
	SERVER = 1,
	LOCATION = 2,
	ERROR_PAGE = 4,
	KEY = 8,
	UNKNOWN = 16
};

enum e_empty {
  EMPTY = 0,
  SERVER_NAME = 1,
  PORT = 2,
  HOST = 4
};

e_empty operator|(e_empty a, e_empty b);

class server_data
{
public:
	server_data();
	server_data(server_data&);
	server_data(const server_data &);
	server_data&operator=(const server_data&);
	void set_keys();
	void print();
	~server_data();
 
	std::string host;
	std::vector<std::string> port;
	std::vector<std::string> server_names;
	std::string home;
	size_t client_max_body_size;
	std::map<std::string, location> locations;
	std::map<int, std::string> error_pages;
	std::vector<std::string> keys;
	e_empty is_empty;
protected:
private:

};
std::vector<server_data> parse_server(std::string config_file);
std::vector<server_data> defeault_server();
#endif
