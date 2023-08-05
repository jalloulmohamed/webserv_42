#ifndef LOCATION_HPP
# define LOCATION_HPP
#include <iostream>
#include <string>
#include <vector>
class location
{
public:
	location();
	location(location&);
	location(const location &);
	location&operator=(location const&);
	void print_location();
	~location();
 
	std::vector<std::string> allow_methods;
	std::string autoindex;//should be handled
	std::string root;//should be handled
	bool upload;// = false;//should be handled
	std::vector<std::string> index;
	std::string cgi_extension;//should be handled
	size_t return_code;
	std::string return_path;
	std::string delete_module;
	std::string post_module;
	std::vector<std::string> keys;
protected:
private:

};
#endif
