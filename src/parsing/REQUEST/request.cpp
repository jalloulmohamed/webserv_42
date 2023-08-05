#include"../../../inc/request.hpp"
#include"../../../inc/tools.hpp"


// -------------------------------------------------------------------------

    request::request ()
    {
        this->_method = "";
        this->_path = "";
        this->_version = "";
        this->_querry = "";
        this->_maxBodySize = -1;
        this->_host = "";
        this->_hostname = "";
        this->_cookie = "";
        this->_port = 80;
        this->_status = GO_NEXT;
        this->_referer = "";
        this->_isChunked = false;
        this->_hasContentLenght = false;
        this->_isMultipart = false;
        this->_isRedirect = false;
        this->_isCgi = false;
    }



    int   request::processing_request( client& clientt, server_data & serv )
    {
        std::string request = clientt.getreq();

        if (request.empty() || isspace(request[0]))
        {
             return (clientt._requestIsParsed = true, Bad_Request);
        }

        std::vector<std::string> tokens;

        _headers.clear();
        _headersBody.clear();
        _body.clear();

   
        if ((tools::splitting_string(request, "\r\n\r\n", tokens)) > 1)
        {
            _body = request.substr(request.find("\r\n\r\n") + 4);
        }

        
        if (tools::splitting_string(tokens[0], "\r\n", _headers) > 1)
        {
            if ((_status = checking_startLine(_headers[0])) != GO_NEXT)
            {
                return (clientt._requestIsParsed = true, _status);
            }
            
            if ((_status = checking_headers(_headers)) != GO_NEXT)
            {
                return (clientt._requestIsParsed = true, _status);
            }
        }
        else
        {
            return (clientt._requestIsParsed = true, Bad_Request);
        }

        if (_method == POST)
        {
            if (_isChunked && _unchunked_body.empty())
                return (clientt._requestIsParsed = true, Bad_Request);
            else if  (!_isChunked && _body.empty())
                return (clientt._requestIsParsed = true, Bad_Request);
            if (_contentLength > serv.client_max_body_size)
            {
                return (clientt._requestIsParsed = true, Reqeust_Entity_Too_Large);
            }

            if ((!_isChunked && (_body.length() != _contentLength )) && !_body.empty())
            {
                return (clientt._requestIsParsed = true, Bad_Request);
            }
            
            if ((_isChunked && (_unchunked_body.length() != _contentLength ) ) && !_unchunked_body.empty())
            {
                return (clientt._requestIsParsed = true, Bad_Request);
            }
        }

        return (clientt._requestIsParsed = true, GO_NEXT);
    }



    int   request::checking_startLine( std::string sline )
    {
        if (sline.empty() || isspace(sline[0]))
        {
            return (Bad_Request);
        }

        std::vector<std::string> tokens;

        if (tools::splitting_string(sline, " ", tokens) != 3)
        {
            return (Bad_Request);
        }
        
        _method     = tokens[0];
        _path       = tokens[1];
        _version    = tokens[2];

        if (_method == "HEAD" || _method == "PATCH" || _method == "PUT" || _method == "OPTIONS" || _method == "MKCOL" || _method == "COPY" || _method == "MOVE")
        {
            return (Not_Implemented);
        }

        if (_method != "GET" && _method != "DELETE" && _method != "POST")
        {
            return (Bad_Request);
        }

        if (_path.size() > 2048)
        {
            return (URI_Too_Long);
        }

        size_t position = _path.find('?');
        if (position != std::string::npos)
        {
            _querry = _path.substr(position + 1);
            _path   = _path.substr(0, position);
        }

        if (_path.empty() || _path[0] != '/')
        {
            return (Bad_Request);
        }
        
        if (_path.find("/..") != std::string::npos)
        {
            return (Bad_Request);
        }

        if (_version.compare(SUPPORTED_HTTP_VER) != 0)
        {
            return (HTTP_Version_Not_Supported);
        }

        return (GO_NEXT);
    }



    int   request::checking_headers( std::vector<std::string> headers )
    {
        std::vector<std::string>::iterator it;
        int   statuuus = GO_NEXT;
        for (it = headers.begin() + 1; it != headers.end(); ++it)
        {
            std::pair<std::string, std::string> couple;
            size_t position = it->find(":");
            if (position != std::string::npos)
            {
                if (it->size() < position + 2)
                    return (Bad_Request);
                couple.first = it->substr(0, position);
                couple.second = it->substr(position + 2);
                tools::trimming_string(couple.second);

                if ((statuuus = checking_headerByHeader(couple.first,couple.second)) != GO_NEXT)
                {
                    return (statuuus);
                }
                this->_mapOfHeaders.insert(couple);
            }
            else
                return (Bad_Request);
        }

        if (_method == "POST")
        {
          if (_isChunked == false && _hasContentLenght == false)
          {
            return (Lengh_Required);
          }

          if (_isChunked != false && _hasContentLenght != false)
          {
            return (Bad_Request);
          }
        }
        return (GO_NEXT);
    }



    int    request::checking_headerByHeader(std::string& key, std::string& value)
    {
        if (key == "" || value == "")
        {
            return Bad_Request;
        }
        
        else if ((key.find_first_of(" \t") != std::string::npos))
        {
            return Bad_Request;
        }

        else if (key == "Connection")
        {
            _connection = value;
        }

        else if (key == "Transfer-Encoding")
        {
            if (_isChunked == false && value == "chunked")
            {
                if (handling_chunked())
                {
                    _isChunked = true;
                }
                else
                {
                   return (Bad_Request);
                }
            }
            else
            {
                return (Not_Implemented);
            }
        }

        else if (key == "Content-Length")
        {
            if (_hasContentLenght || (value.empty() || value.find_first_not_of("0123456789") != std::string::npos))
            {
                return (Bad_Request);
            }
            _contentLength = std::stoul(value);
            _hasContentLenght = true;
        }

        else if (key == "Cookie")
        {
            _cookie = value;
        }

        else if (key == "Referer")
        {
            value = value.substr(value.find_first_of("0123456789"));
            if (!value.empty() && value.find('/') != std::string::npos)
            {
                value = value.substr(value.find('/'));
                _referer.assign(value);
            }
        }
     
        else if (key == "Content-Type")
        {
            int multilpart = value.find("multipart/form-data");
           
            if (multilpart == -1)
            {
                _contentType = value;
                return (GO_NEXT);
            }
            
            int position = value.find(';');

            if (position != -1)
            {
                _contentType = value.substr(multilpart, position);
                int boundpos;
                if ((boundpos = value.find("boundary="), position) != -1)
                {
                    _boundry = value.substr(boundpos + 9);
                }
                
                if (_boundry == "" && _contentType == "multipart/form-data")
                {
                    return (Bad_Request);
                }
                else
                {
                    _isMultipart = true;
                }
            }
        }
        return (GO_NEXT);
    }



    bool    request::check_hexadecimalLine(std::string& line)
    {
        int found = line.find_first_not_of("0123456789ABCDEFabcdef");

        if (found != -1)
        {
            return false;
        }
        return (true);
    }



    bool    request::chunk_ending_correctly( std::string end )
    {
        if (end == "\r\n")
        {
            return (true);
        }
        else
        {
            return (false);
        }
    }



    bool    request::handling_chunked()
    {
        std::vector<std::string> tokens;
        _unchunked_body = "";
        size_t start = 0;
        int n = _body.find("\r\n", start);
        std::string temp;
        std::string end;
        while (n != -1)
        {
            std::string hexa = _body.substr(start, n - start);
            if (check_hexadecimalLine(hexa) == false)
            {
                return false;
            }
            size_t size = std::strtol(hexa.c_str(), NULL, 16);
            if (size == 0)
                break ;
            temp = _body.substr(n + 2, size);
            end = _body.substr(n + 2 + size, 2);
            if (temp.size() == size && chunk_ending_correctly(end) == true)
            {
                _unchunked_body += temp;
                start += hexa.size() + temp.size() + 4;
            }
            else
            {
                return (false);
            }
            n = _body.find("\r\n", start);
        }
        _contentLength = _unchunked_body.length();
        return true;
    }