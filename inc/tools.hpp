#ifndef TOOLS_HPP
#define TOOLS_HPP

#include "status_codes.hpp"
#include <iostream>
#include <vector>
#include <map>
#define GO_NEXT 1
#include <iostream>
#include <fstream>
#include <string>
class tools
{
    public:
        static void    trimming_string(std::string& str)
        {
            str.erase(0, str.find_first_not_of(" \t"));
        
            if (str == "")
                return ;
        
            str.erase(str.find_last_not_of(" /t") + 1);
        }

        static int splitting_string(std::string str, const std::string& delim, std::vector<std::string>& tokens) 
        {
            size_t pos = 0;
            while ((pos = str.find(delim)) != std::string::npos)
            {
                tokens.push_back(str.substr(0, pos));
                str.erase(0, pos + delim.length());
            }
            if (!str.empty())
                tokens.push_back(str);
            return (tokens.size());
        }

        static int    allowed_chars(char c)
        {
            std::string allowd_ch = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!$&'()*+,/:;=?@[]-._~";

            if (allowd_ch.find(c) != std::string::npos)
                return (GO_NEXT);
            else
                return (400);

            return (GO_NEXT);
        }


// -----------------------------------------------------------------------

        // this function convert a string that has a hexadecimal value to decimal.

        static int    Converting_hexaToDecimal ( std::string str )
        {
            int lenght = str.length();
            
            int base = 1;  

            int decimalVal = 0;  
        
            for (int i = lenght - 1; i >= 0; i--) 
            {
                if (str[i] >= '0' && str[i] <= '9') 
                {
                    decimalVal += (str[i] - 48) * base;
                    base = base * 16;
                }
                else if (str[i] >= 'A' && str[i] <= 'F') 
                {
                    decimalVal += (str[i] - 55) * base;
                    base = base * 16; 
                }
            }
            return (decimalVal);
        }


    // -----------------------------------------------------------------------

        // this function is used to decode the url.
        // it checks if there is a '%' in the uri.
        // then convert the number after from hexadecimal to decimal.
        // if it's asccii then it remove the % and hexa number and
        // replace it with it's real ascci value. 
        // for examle if we have %20 this will be replaced with a space ' '

        static int            Decoding_url (std::string& uri)
        {
            int i = -1;
            int decimal;
            while (uri[++i])
            {
                if (uri[i] == '%')
                {
                    decimal = Converting_hexaToDecimal( uri.substr(i + 1, 2));
                    if (isascii(decimal))
                    {
                        uri.erase(i, 2);
                        uri[i] = decimal;
                    }
                    else
                        return (Bad_Request);
                }
                if (!isascii(uri[i]))
                    return (Bad_Request);
                if (allowed_chars(uri[i]) != GO_NEXT)
                    return(Bad_Request);
            }
            return (GO_NEXT);
        }


    // -----------------------------------------------------------------------

        // this function is about removing multiple slashes from the uri.
        // this will normalize the uri and make it valid to use.

        static int normalizing_uri(std::string& uri)
        {
            size_t position = uri.find("//");

            while (position != std::string::npos)
            {
                uri.erase(position, 1);
                position = uri.find("//");
            }
            return (1);
        }


    // --------------------------------------------------------------

        // this function is about counting the number of directories in the uri.
        // [if] the number is negativ then it returns bad request.
        // [else] it returns the number of directories.

        static int     counting_Directories (std::string& uri)
        {
            normalizing_uri(uri);
            std::vector<std::string> paths;
            splitting_string(uri, "/", paths);
            int count = 0;
            for (size_t i = 0; i < paths.size(); i++)
            {
                if (paths[i] == "..")
                    count--;
                else
                    count++;
            }
            if (count < 0)
                return (Bad_Request);
            return (GO_NEXT);
        }


        static std::string    getting_errorPage(int status)
        {
            std::string Badreq = "<HTTP/1.1 400 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Bad Request : 400 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Bad Request</h1><p>Sorry, we were unable to process your request.</p></div></body></html>"; 
            std::string unauthorized = "<HTTP/1.1 401 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Unauthorized : 401 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Unauthorized</h1><p>Sorry, you are not authorized.</p></div></body></html";
            std::string not_found = "<HTTP/1.1 404 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Not Found : 404 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Not Found</h1><p>Sorry, not found.</p></div></body></html";
            std::string method_notAllowd = "<HTTP/1.1 405 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Not Allowd : 405 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Not Allowed</h1><p>Sorry, method is not allowed.</p></div></body></html";
            std::string largeEntity = "<HTTP/1.1 413 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Request entity too large : 413 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Request entity too large</h1><p>Sorry, Request entity too large.</p></div></body></html";
            std::string tooLongUri = "<HTTP/1.1 414 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Too Long Uri : 414 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Too Long Uri</h1><p>Sorry, uri is too long.</p></div></body></html";
            std::string httpVersionErr = "<HTTP/1.1 505 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Not Supported : 505 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Not Supported</h1><p>Sorry, version (HTTP) not supported.</p></div></body></html";
            std::string internal_error = "<HTTP/1.1 500 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Internal server issue : 500 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Internal issue</h1><p>Sorry, there is an internal server issue.</p></div></body></html";
            std::string timeOut = "<HTTP/1.1 504 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Time Out : 504 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Timeout</h1><p>Sorry, Gateway timeout.</p></div></body></html";
            std::string forbid = "<HTTP/1.1 403 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> Forbidden : 403 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>Forbidden</h1><p>Sorry, Forbidden.</p></div></body></html";
            std::string notimp = "<HTTP/1.1 501 \r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><title> not implemented : 501 </title><style>body{font-family: Arial, sans-serif;background-color: #f7f7f7;}.container{margin: 50px auto;text-align: center;}h1{color: #d0021b;font-size: 36px;margin-bottom: 20px;}p{font-size: 18px;margin-bottom: 20px;}button{background-color: #d0021b;color: #fff;border: none;padding: 10px 20px;font-size: 18px;border-radius: 5px;cursor: pointer;}button:hover {background-color: #a30014;}</style></head><body><div class='container'><h1>not implemented</h1><p>Sorry, not implemented.</p></div></body></html";

            switch (status)
            {
            case 400 :
                return (Badreq);
            
            case 401 :
                return (unauthorized);

            case 403 :
                return (forbid);
            
            case 404 :
                return (not_found);
            
            case 405 :
                return (method_notAllowd);
            
            case 413 :
                return (largeEntity);
            
            case 414 :
                return (tooLongUri);
            
            case 500 :
                return (internal_error);

            case 501 :
                return (notimp);
            
            case 504 :
                return (timeOut);
            
            case 505 :
                return (httpVersionErr);
            
            default:
                return (Badreq);
            }
        }

        };

#include "server_data.hpp"
void trim(std::string &line, std::string str);
int has_bad_char(std::string &key);
int check_if_closed(std::string value);
void check_value_key(std::string &value, std::string &key, std::vector<std::string> &keys);
void fill_vector(std::vector<std::string> &vec, std::string &values);
std::pair<std::string, std::string> fill_pair(std::string &line);
void fill_server(server_data &server, std::string &line);
void fill_location(location &location, std::string &line);
void fill_error_page(std::map<int , std::string> &error_page, std::string &line);

#endif
