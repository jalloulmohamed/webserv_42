#include "../../../inc/tools.hpp"
#include <iostream>
#include <string>
#include <vector>

void trim(std::string &line, std::string str)
{
	if (line.empty())
		return;
	size_t start = line.find_first_not_of(str);
	size_t end = line.find_last_not_of(str);
	if (start == std::string::npos || end == std::string::npos)
		line.clear();
	else
		line = line.substr(start, end - start + 1);
}

int has_bad_char(std::string &key){
	int i = 0;
	while (key[i])
	{
		if (!iswalnum(key[i]) && key[i] != '_')
			return (1);
		i++;
	}
	return (0);
}

int there_is_remain(std::string value){
	trim(value, " \t");
	char first = value[0];
	char last = value[value.length() - 1];

	if ((first == '\"' && last != '\"') || (first == '[' && last != ']'))
		return 1;
	return 0;
}

int check_if_closed(std::string value){
	int i = 0;
	int count_quote = 0;
	int count_bracket = 0;
	while (value[i])
	{
		if (value[i] == '\"')
			count_quote++;
		else if (value[i] == '[' )
			count_bracket++;
		else if (value[i] == ']')
			count_bracket--;
		i++;
	}
	if ((count_quote != 0 && count_quote % 2 != 0) || (count_bracket != 0) || there_is_remain(value))
		return (0);
	return (1);
}

int is_duplicated(std::string &key , std::vector<std::string> &keys){
	for (size_t i = 0; i < keys.size(); i++)
	{
		if (keys[i] == key)
			return (1);
	}
	keys.push_back(key);
	return (0);
}

void check_value_key(std::string &value, std::string &key, std::vector<std::string> &keys){
	if (key.empty())
		throw std::invalid_argument("Error: empty key");
	if (has_bad_char(key)) 
		throw std::invalid_argument("Error: invalid key");
	if (is_duplicated(key, keys))
		throw std::invalid_argument("Error: duplicated key");
	if (value.empty())
		throw std::invalid_argument("Error: empty value");
	if (!check_if_closed(value))
	{
		throw std::invalid_argument("Error: invalid value");
	}
}
