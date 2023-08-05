#include "../../../inc/response.hpp"
#include "../../../inc/tools.hpp"
#include "../../../inc/server_data.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <dirent.h>


response::response()
{
        _homePage = "www/index.html";
        _indexes.clear();
        _upload = "OFF";
         _root = "";
        _autoIndex = "OFF";
        _querry = "";
        _heder_cookeis = "";
        is_cookeis = false;
        _redirection = false;
        _clientMaxBodySize = 0;
        _realPath = "";
        _cgiExtention = "";
        _contentType = "";
        _method = "";
        _method_isFound = false;
        _fileContent = "";
        _contentLength = -1;
        _extension = "";
        _path = "";
        _body = "";
        _locationName = "";
        _status = GO_NEXT;
        _responseCode = GO_NEXT;
        _iscgi = false;
        _ischunked = false;
        _autoIndex = "OFF";
        _boundry = "";
}



void response::determine_contentType()
{
    std::vector<std::string> tokens;

    if (tools::splitting_string(_realPath, "/", tokens) > 0)
    {
        size_t dot = tokens[tokens.size() - 1].find_last_of('.');
        if (dot != std::string::npos)
        {
            _extension = tokens[tokens.size() - 1].substr(dot);
            if (_extension == ".html")
                _contentType = "text/html";
            else if (_extension == ".css")
                _contentType = "text/css";
            else if (_extension == ".js")
                _contentType = "application/javascript";
            else if (_extension == ".json")
                _contentType = "application/json";
            else if (_extension == ".xml")
                _contentType = "application/xml";
            else if (_extension == ".gif")
                _contentType = "image/gif";
            else if (_extension == ".jpg")
                _contentType = "image/jpg";
            else if (_extension == ".jpeg")
                _contentType = "image/jpeg";
            else if (_extension == ".png")
                _contentType = "image/png";
            else if (_extension == ".bmp")
                _contentType = "image/bmp";
            else if (_extension == ".ico")
                _contentType = "image/x-icon";
            else if (_extension == ".svg")
                _contentType = "image/svg+xml";
            else if (_extension == ".webp")
                _contentType = "image/webp";
            else if (_extension == ".txt")
                _contentType = "text/plain";
            else if (_extension == ".pdf")
                _contentType = "application/pdf";
            else if (_extension == ".mp4")
                _contentType = "video/mp4";
            else if (_extension == ".mpeg")
                _contentType = "video/mpeg";
            else if (_extension == ".webm")
                _contentType = "video/webm";
            else if (_extension == ".mov")
                _contentType = "video/quicktime";
            else if (_extension == ".avi")
                _contentType = "video/x-msvideo";
            else if (_extension == ".mp3")
                _contentType = "audio/mp3";
            else if (_extension == ".wav")
                _contentType = "audio/wav";
            else if (_extension == ".ogg")
                _contentType = "audio/ogg";
            else if (_extension == ".flac")
                _contentType = "audio/flac";
            else if (_extension == ".woff")
                _contentType = "font/woff";
            else if (_extension == ".woff2")
                _contentType = "font/woff2";
            else if (_extension == ".ttf")
                _contentType = "font/ttf";
            else
                _contentType = "application/octet-stream";
        }
        else
            _contentType = "text/html"; 
    }
    else
        _contentType = "text/html";    
}



void    response::preparing_responseHeaders()
{
    getting_responsCode();
    // start line Header.
    _respHeaders += "HTTP/1.1 ";
    _respHeaders += _responseCode;
    _respHeaders += "\r\n";
    if(is_cookeis)
    {
        _respHeaders += _heder_cookeis;
        _respHeaders += "\r\n";
        is_cookeis = false;
    }
    // Location Header
    if (_status == REDIRECTION)
    {
        _respHeaders += "Location: ";
        _respHeaders += _redirection_url;
        _respHeaders += "\r\n";
        _respHeaders += "Cache-Control: no-cache";
        return ;
    }

    // Content-Length HEADER
    _respHeaders += "ContentLength: ";
    _respHeaders += std::to_string(_contentLength);
    _respHeaders += "\r\n";

    // Content-Type Header
    if (_method == GET || _method == POST)
    {
        _respHeaders += "Content-Type: ";
        _respHeaders += _contentType;
    }
}



void response::getting_responsCode()
{
    if (_status == 200)
        _responseCode = "200 OK";
    else if (_status == 201)
        _responseCode = "201 Created";
    else if (_status == 204)
        _responseCode = "204 No Content";
    else if (_status == 301)
        _responseCode = "301 Moved Permanently";
    else if (_status == 400)
        _responseCode = "400 Bad Request";
    else if (_status == 401)
        _responseCode = "401 Unauthorized";
    else if (_status == 403)
        _responseCode = "403 Forbidden";
    else if (_status == 404)
        _responseCode = "404 Not Found";
    else if (_status == 405)
        _responseCode = "405 Method Not Allowed";
    else if (_status == 413)
        _responseCode = "413 Request entity too large";
    else if (_status == 500)
        _responseCode = "500 Internal Server Error";
    else if (_status == 501)
        _responseCode = "501 Not Implemented";
    else if (_status == 503)
        _responseCode = "503 Service Unavailable";
    else
        _responseCode = "200 OK";
}



bool    response::method_isFound()
{
    for (size_t i = 0; i < _allowed_methods.size(); ++i)
    {
        if (_method == _allowed_methods[i])
            return (true);
    }
    return (false);
}



void    response::store_requestInfos(request& req)
{
    _method = req._method;
    _path = req._path;
    _contentLength = req._contentLength;
    _referer = req._referer;

    _bheaders = req._headersBody;
    _ischunked = req._isChunked;
    _isMultipart = req._isMultipart;
    if (_isMultipart)
        _boundry = req._boundry;
    if (_ischunked)
        _body = req._unchunked_body;
    else
        _body = req._body;
    _querry = req._querry;
}



std::string    response::generating_response(request& request, int returnStatus, server_data& serverr)
{
    int status;

    store_requestInfos(request);

    get_pathAndLocationInfos (serverr, _path);
    
    if (returnStatus != GO_NEXT)
        _status = returnStatus;

    else if (_realPath.empty() && _redirection == false)
        _status = Not_Found;
    
    else if (method_isFound() == false && _redirection == false)
        _status = Method_Not_Allowed;

    else if (_redirection == true)
        _status = REDIRECTION;

    else if ((status = executing_method()) != GO_NEXT)
    {
        _status = status;
    }

    // std::cout << "STATUS : " << _status << std::endl;
    if (errorAnswer(_status))
    {
        _fileContent = getErrorPage();
        _contentType = "text/html";
        preparing_responseHeaders();
    }

    else if (redirectionAnswer(_status))
        preparing_responseHeaders();

    else if (successAnswer(_status))
        preparing_responseHeaders();

    // std::cout << "h : " << _respHeaders << std::endl;
    // std::cout<<_fileContent<<std::endl;
    return (_respHeaders + "\r\n\r\n" + _fileContent);
}



int    response::executing_method()
{
    std::vector<std::string> tokens;
    tools::splitting_string(_realPath, "/" , tokens);
    if ((int)tokens[tokens.size() -1].find('.') != -1)
    {
        determine_contentType();
        // std::cout << _extension << std::endl;
        if (_extension == _cgiExtention)
        {
            if (permissionForExecuting() == true)
            {
                if (_method == DELETE && permissionForDeleting())
                    return (Execute_cgi(*this));
                else if (_method != DELETE)
                    return(Execute_cgi(*this));
                else
                    return (Forbidden);
            }
        }

        if (_method == "GET")
        {
            if (permissionForReading() == true)
                return (200);
            else
                return (_status);
        }

        else if (_method == "POST")
        {
            return (stroring_requestBody());
        }

        else if (_method == "DELETE")
        {   
            if (_delete_module == "enable" && permissionForDeleting())
            {
                if (unlink(_realPath.c_str() )== -1)
                    return (Internal_Server);
                else
                    return (No_Content);
            }
            else
                return (Method_Not_Allowed);
        }
    }

    else if (_method == "GET" && _autoIndex == "OFF")
    {
        for (size_t i = 0; i < _indexes.size(); i++)
        {
            _realPath += "/" + _indexes[i];
            determine_contentType();

            if ( _extension == _cgiExtention)
            {
                if (permissionForExecuting())
                    return(Execute_cgi(*this));
                else
                    return (_status);
            }

            if (permissionForReading())
            {
                    return (200);
            }
            _realPath.substr(0, _indexes[i].size() - 1);
        }
        return (_status);
    }

    else if (_autoIndex == "ON" && _method == "GET")
    {
        determine_contentType();
        return (listing_requestDirectory());
    }

    else if (_method == "DELETE")
        return (Forbidden);
    
    else if (_method == "POST" && _isMultipart)
    {
        return (stroring_requestBody());
    }

    else
        return (Bad_Request);
    
    
    return (GO_NEXT);
}



int     response::stroring_requestBody()
{
    if (_post_module != "enable")
        return (Forbidden);
    DIR *dh = opendir(_root.c_str());
    if (dh == NULL)
    {
        return (Internal_Server);
    }
    if (_isMultipart)
        return (storing_multipleParts());
    else
    {
        std::string fileName = _fileName;
        if (fileName.empty())
            return (Bad_Request);
        std::ofstream ofs( _root + "/" + fileName );
        if (ofs.is_open())
        {
            ofs << _body;
            ofs.close();
            _fileContent = readPage(UPLOAD_FILE);
            _contentType = "text/html";
        }
        else
            return (Internal_Server);
    }
    closedir(dh);

    return (201);
}



int     response::storing_multipleParts()
{
    std::string startboundary = "--\r\n";
    startboundary += _boundry;
    std::string endboundary = startboundary + "\r\n--";

    _body = _body.substr(0, _body.find(endboundary));
    std::vector<std::string> parts;
    tools::splitting_string(_body, startboundary, parts);

    int start, end;
    for (size_t i = 0; i < parts.size(); i++)
    {
        std::string filename = "";
        std::string filecontent = "";
        start = parts[i].find("filename=");
        end = parts[i].find("\"", start + 10);

        if(start != -1 && end != -1)
        {
            start += 10;
            filename = parts[i].substr(start, end);
            int found = 0;
            if ((found = filename.find("\"")) != -1)
                filename.erase(found);
        }
        else
            return (Bad_Request);
        int startbody = parts[i].find(CRLFX2, end);
        if (startbody != -1)
            filecontent = parts[i].substr(startbody + 4);
        else
            return (Bad_Request);
        
        std::string path = _root + "/" + filename;
        std::ofstream ofs (path.c_str());

        if (!ofs.is_open())
            return (Internal_Server);

        ofs << filecontent;
        ofs.close();
        _contentType = "text/html";
    }
    _fileContent = readPage(UPLOAD_FILE);
    return (201);
}



bool    response::permissionForReading()
{
    if (access(_realPath.c_str(), F_OK) == -1)
    {
        _status = Not_Found;
        return (false);
    }

    if (access(_realPath.c_str(), R_OK) == -1)
    {
        _status = Forbidden;
        return (false);
    }

    reading_fileContent();

    return (true);
}



bool    response::permissionForExecuting()
{
    if (access(_realPath.c_str(), F_OK) == -1)
    {
        _status = Not_Found;
        return (false);
    }

    if (access(_realPath.c_str(), X_OK) == -1)
    {
        _status = Forbidden;
        return (false);
    }

    return (true);
}



bool    response::permissionForDeleting()
{
    if (access(_realPath.c_str(), F_OK) == -1)
    {
        _status = Not_Found;
        return (false);
    }

    if (access(_realPath.c_str(), W_OK) == -1)
    {
        _status = Forbidden;
        return (false);
    }

    return (true);
}



int response::reading_fileContent()
{
    std::ifstream ifs(_realPath.c_str(), std::ios::binary);

    std::stringstream stream;
    stream << ifs.rdbuf();
    _fileContent = stream.str();
    ifs.close();
    return (true);
}



std::string  response::readPage(std::string page)
{
    std::ifstream ifs(page.c_str(), std::ios::binary);

    if (!ifs.is_open())
        return ("");

    std::stringstream stream;
    stream << ifs.rdbuf();
    ifs.close();

    return (stream.str());
}

bool    is_folder(std::string& str)
{
    std::vector<std::string> tokens;

    tools::splitting_string(str, "/", tokens);

    if (tokens[tokens.size() - 1].find('.') == std::string::npos)
    {
        return (true);
    }

    return (false);
}


void    response::get_pathAndLocationInfos (server_data &serverr, std::string uri)
{
    if (uri.empty())
        return ;

    std::map<std::string, location> locations = serverr.locations;

    if (!_referer.empty() && is_folder(_referer))
    {
        if (uri.find(_referer) == std::string::npos)
            uri = _referer + uri;
    }

    std::string temp = uri;

    std::map<std::string, location>::iterator it;

    for (size_t i = temp.size(); i > 0 ; i--)
    {
        if ((it = locations.find(temp)) != locations.end())
        {
            _locationName = it->first;
            if ((uri == _locationName ) || ((uri.size() > _locationName.size()) && (uri [_locationName.size()] == '/')))
                break ;
            else
                _locationName = "";
        }
        temp = temp.substr(0, i);
    }
     
    if (it != locations.end())
    {
        _root = it->second.root;
        _allowed_methods = it->second.allow_methods;
        _indexes = it->second.index;
        _redirection_code = it->second.return_code;
        _redirection_url = it->second.return_path;

        if (!_redirection_url.empty() && _redirection_code == REDIRECTION)
        {
            _status = REDIRECTION;
            _redirection = true;
        }

        _post_module = it->second.post_module;
        _delete_module = it->second.delete_module;
        _cgiExtention = it->second.cgi_extension;
        
        if (it->second.autoindex == "ON")
            _autoIndex = it->second.autoindex;

        if ( uri.size() > _locationName.size() )
        {
            if (uri[temp.size()] == '/')
            {
                _realPath = _root + uri.substr(temp.size());
                _fileName = uri.substr(_locationName.size() + 1);

            }
        }

        else if (_locationName.size() == uri.size())
        {
            _realPath = _root;
        }
    }

    else if (uri == "/" && _method == "GET")
    {
        _realPath = serverr.home;
    }

    _errorPages = serverr.error_pages;

}



std::string    response::getErrorPage()
{
    std::string errorPage = "";
    if (_errorPages.find(_status) != _errorPages.end())
    {
        errorPage = readPage(_errorPages[_status]);
    }
    if (errorPage == "")
    {
        errorPage = tools::getting_errorPage(_status);
    }
    _contentLength = errorPage.size();
    return (errorPage);
}


/* -------------------------------------------------- */


int     response::listing_requestDirectory()
{
    std::string directory = _realPath;
    DIR *D = opendir(directory.c_str());
    if (D == NULL)
        return (403);
    
    std::string autoindex_page =  "<html><title> [A] [U] [T] [O] [I] [N] [D] [E] [X] </title><body><center>";
    autoindex_page += directory;
    autoindex_page += "</br></br></br>";

    struct dirent *directoryEntity;
    std::string folder;

    while ((directoryEntity = readdir(D)) != NULL)
    {
        std::string Fname = (std::string)directoryEntity->d_name;
        if (Fname.compare(".") && Fname.compare(".."))
        {
            autoindex_page += "<a href=" + _locationName + '/' + Fname + ">";
            autoindex_page += Fname + "\n</a> </br></br>";
        }
    }

    autoindex_page += "</center></body></html>";
    _fileContent = autoindex_page;
    closedir(D);
    return (200);
}


/* -------------------------------------------------- */


bool        response::successAnswer(int status)
{
    if (status >= 200 && status <= 204)
        return (true);
    return (false);
}

bool        response::redirectionAnswer(int status)
{
    if (status == 301)
        return (true);
    return (false);
}

bool        response::errorAnswer(int status)
{
    if (status >= 400 && status <= 503)
        return (true);
    return (false);
}