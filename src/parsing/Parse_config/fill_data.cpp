#include <vector>
#include <string>
#include <sstream>
#include "../../../inc/server_data.hpp"
#include "../../../inc/location.hpp"
#include "../../../inc/tools.hpp"

void fill_vector(std::vector<std::string> &vec, std::string &values){
	std::string value;
	int i = 0;
	trim(values, " \t[]");
	while (values[i])
	{
		if (values[i] == ',' || values[i + 1] == '\0' || values[i] == '\"')
		{
			if (values[i + 1] == '\0')
				value += values[i];
			trim(value, " \t\"");
			if (!value.empty())
				vec.push_back(value);
			value.clear();
		}
		else
			value += values[i];
		i++;
	}
} 

std::pair<std::string, std::string> fill_pair(std::string &line){
	std::vector<std::string> keys;
	std::string key;
	std::string value;
	char eq;
	std::stringstream ss(line);
	ss >> key;
	ss >> eq;
	std::getline(ss, value);
	check_value_key(value, key, keys);
	trim(value, " \t\"");	
	return (std::make_pair(key, value));
}

void check_int(std::string &value){
	if (atoi(value.c_str()) < 1024 || atoi(value.c_str()) > 65535 || value.find_first_not_of("0123456789") != std::string::npos)
		throw std::invalid_argument("Error: invalid port");
}

void fill_server(server_data &server, std::string &line){
	std::string key;
	std::string value;
	char eq;
	std::stringstream ss(line);
	ss >> key;
	ss >> eq;
	if (eq != '=')
		throw std::invalid_argument("Error: invalid server key");
	std::getline(ss, value);
	check_value_key(value, key, server.keys);
	trim(value, " \t\"");

	if (key == "host")
	{
		server.is_empty = server.is_empty | HOST;
		server.host = value;
	}
	else if (key == "port")
	{
		std::vector<std::string> ports;
		tools::splitting_string(value, " ", ports);
		for (size_t i = 0; i < ports.size(); i++)
		{
			check_int(ports[i]);
			server.is_empty = server.is_empty | PORT;
//			server.port.push_back(atoi(ports[i].c_str()));
			server.port.push_back(ports[i]);
		}
	}
	else if (key == "server_name")
	{
		server.is_empty = server.is_empty | SERVER_NAME;
		fill_vector(server.server_names, value);
	}
	else if (key == "root")
		server.home = value;
	else if (key == "client_max_body_size")
	{
		int pos = value.find_first_not_of("0123456789");
		if (pos == -1)
			throw std::invalid_argument("Error: unit is missing");
		std::string max = value.substr(0, pos);
		std::string unit = value.substr(pos);
		if (max.empty() || unit.empty())
			throw std::invalid_argument("Error: value or unit is missing");
		
		if (unit == "G" || unit == "g" || unit == "GB" || unit == "gb")
			server.client_max_body_size = std::stoul(max) * 1024 * 1024 * 1024;
		else if (unit == "M" || unit == "m" || unit == "MB" || unit == "mb")
			server.client_max_body_size = std::stoul(max) * 1024;
		else
			throw std::invalid_argument("Error: bad unit");
	}
	else 
		throw std::invalid_argument("Error: invalid server key at `" + line + "`");
}

void fill_location(location &location, std::string &line){
	std::string key;
	std::string value;
	char eq;
	static bool _upload = false;
	std::stringstream ss(line);
	ss >> key;
	ss >> eq;
	if (eq != '=')
		throw std::invalid_argument("Error: invalid location key");
	std::getline(ss, value);
	check_value_key(value, key, location.keys);
	trim(value, " \t\"");
	location.upload = _upload;
	if (key == "allow_methods")
		fill_vector(location.allow_methods, value);
	else if (key == "autoindex")
	{
		if (value == "ON" || value == "OFF")
			location.autoindex = value;
		else
			throw std::invalid_argument("Error: invalid autoindex value");
	}
	else if (key == "root")
			location.root = value;
	else if (key == "index")
		fill_vector(location.index, value);
	else if (key == "cgi_extension")
		location.cgi_extension = value;
	else if (key == "return")
	{
		std::vector<std::string> tokens;
		tools::splitting_string(value, " ", tokens);
		if (tokens.size() != 2 || tokens[0].find_first_not_of("0123456789") != std::string::npos || tokens[0].empty() || tokens[1].empty())
			throw std::invalid_argument("Error: invalid return value");
		location.return_code = atoi(tokens[0].c_str());
		location.return_path = tokens[1];
	}
	else if (key == "delete_module")
	{
		if (value == "enable" || value == "disable")
			location.delete_module = value;
		else
		{
			throw std::invalid_argument("Error: invalid delete_module value");
		}
	}
	else if (key == "post_module")
	{
		if (value == "enable" || value == "disable")
			location.post_module = value;
		else
			throw std::invalid_argument("Error: invalid post_module value");
	}
	else
		throw std::invalid_argument("Error: invalid location key");
}

void fill_error_page(std::map<int , std::string> &error_page, std::string &line){
	std::string k;
	k = line.substr(0, line.find_first_of(" "));
	int key;
	std::string value;
	char eq;
	std::stringstream ss(line);
	ss >> key;
	ss >> eq;
	if (eq != '=')
		throw std::invalid_argument("Error: invalid error_page key");
	std::getline(ss, value);
	trim(value, " \t\"");
	if (key < 400 || key > 599 || k.find_first_not_of("0123456789") != std::string::npos || k.empty())
		throw std::invalid_argument("Error: invalid error_page key");
	else if (value.empty() || value.substr(value.find_last_of(".")) != ".html")
		throw std::invalid_argument("Error: invalid error_page value");
	error_page[key] = value;
}
