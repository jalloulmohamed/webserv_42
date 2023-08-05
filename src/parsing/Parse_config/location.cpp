#include "../../../inc/location.hpp"

location::location()
{
}

location::location(location &location)
{
	*this = location;
}

location::location(const location &location)
{
	*this = location;
}

location &location::operator=(const location &location)
{
	if (this == &location)
		return *this;
	this->allow_methods = location.allow_methods;
	this->autoindex = location.autoindex;
	this->root = location.root;
	this->upload = location.upload;
	this->index = location.index;
	this->cgi_extension = location.cgi_extension;
	this->return_code = location.return_code;
	this->return_path = location.return_path;
	this->post_module = location.post_module;
	this->delete_module = location.delete_module;
	return *this;
}

void location::print_location(){
	for (size_t i = 0; i < allow_methods.size(); i++)
		std::cout << allow_methods[i] << " ";
	std::cout << std::endl;
	std::cout << "autoindex = " << autoindex << std::endl;
	std::cout << "root = " << root << std::endl;
	std::cout << "upload = " << upload << std::endl;
	std::cout << "index = ";
	for (size_t i = 0; i < index.size(); i++)
		std::cout << index[i] << " ";
	std::cout << std::endl;
	std::cout << "cgi_extension = " << cgi_extension << std::endl;
	std::cout << "return_code = " << return_code << std::endl;
	std::cout << "return_path = " << return_path << std::endl;
	std::cout << "post_module = " << post_module << std::endl;
	std::cout << "delete_module = " << delete_module << std::endl;
}

location::~location()
{
}
