/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rel-hach <rel-hach@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 17:10:47 by rel-hach          #+#    #+#             */
/*   Updated: 2023/07/25 00:15:15 by rel-hach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef REQUEST_HPP
#define REQUEST_HPP

// #include "parse_config.hpp"
#include "status_codes.hpp"
#include "client.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include "./server_data.hpp"

#define CRLFX2              "\r\n\r\n"
#define VEC_OF_STRS         std::vector<std::string>
#define MAP_OF_VECS         std::map<std::string, std::string>
#define  SUPPORTED_HTTP_VER "HTTP/1.1"
#define SKIP_CRLF           2
#define CRLFX2              "\r\n\r\n"

#define HOST                "Host"
#define CONTENT_LENGHT      "Content-Lenght"
#define CONTENT_TYPE        "Content-Type"
#define TRANSFER_ENCODING   "Transfer-Encoding"
#define NOT_FOUND           "NOT_FOUND"
#define GO_NEXT             1

// METHODS

#define GET     "GET"
#define DELETE  "DELETE"
#define POST    "POST"

typedef struct  data
{
  std::vector<std::string>            contents;
  size_t                              Lenght;
  std::map<std::string, std::string>  types;
} f_data;


class server;
class response;
class request
{
  public:
    // request
    std::string _request;
    std::string _body;
    std::string _unchunked_body;
    std::vector<std::string> _headers;
    std::map<std::string, std::string>    _mapOfHeaders; 
  
    // start line header :
    std::string _method;
    std::string _path;
    std::string _version;
    std::string _querry;
  
    // headers :
    // HOST : GET | DELETE -> required.
    long long   _maxBodySize;
    std::string _host;
    std::string _hostname;
    std::string _cookie;
    std::string _referer;
    std::string _image;
    int         _port;
    int         _status;

    // CONTENT_TYPE IS REQUIRED IN POST
    std::string _transferEncoding;
    size_t      _contentLength;
    std::string _contentType;
    std::string _boundry; // multipart/form-data
    std::string _response;
    std::string _headersBody;

    // ACCEPT : type client are waiting for.
    std::string _accept;
    // CONNECTION : close or keep-alive
    std::string _connection;




    public:

      bool  _isCgi; 
      bool  _isRedirect;
    

    // Headers :

    bool  _isChunked;
    bool  _isMultipart;
    bool  _hasHostHeader;
    bool  _hasContentLenght;


    // -------------------------- METHODS 



    // constructors : 


      request ();
      // request(server& conf)
      // {

      // }


      // processing :

      int     processing_request( client& client, server_data& serv );
      int     checking_startLine( std::string sline );
      bool    dechunkBody();

      // int   checking_method(std::string& sline);
      // int   checking_version(std::vector<std::string> headers );
      // int   checking_uri();
      bool    handling_chunked();
      int     checking_headers( std::vector<std::string> headers );
      int     checking_headerByHeader(std::string& key, std::string& value);
      bool    chunk_ending_correctly( std::string end );
      bool    check_hexadecimalLine(std::string& line);

      std::string   getErrorPage();
      // void  checking_connectionType( std::string& connectionType );



      // setters and getters 

      // void  settingMethod( std::string& method);
      // void  settingPath( std::string& path);
      // void  settingVersion( std::string& version);
      // void  settingQuerry( std::string& querry);
      // void  settingHostName( std::string& hostName) ;
      // void  settingPort( int port);
      // void  settingContentLength( long long contentLength);
      // void  settingBoundry( std::string boundary);
      // void  settingAccept( std::string accept);
      // void  settingConnection( std::string connection);

      // std::string  gettingMethod() const;
      // std::string  gettingPath() const;
      // std::string  gettingVersion() const;
      // std::string  gettingQuerry() const;
      // std::string  gettingHostName() const;
      // std::string  gettingPort() const;
      // std::string  gettingContentLength() const;
      // std::string  gettingBoundry() const;
      // std::string  gettingAccept() const;
      // std::string  gettingConnection() const;
};


#endif