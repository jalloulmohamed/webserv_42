#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <string>
#include <vector>
#include "request.hpp"
#include "server_data.hpp"

#define BUILD_ANSWER    10000

#define UPLOAD_FILE "www/cases/success_upload.html"
#define DELETE_FILE "www/cases/success_deletion.html"


#define REDIRECTION 301

static std::string oldpath;

class response
{
    public:
        //server data
        std::string                 _homePage;
        size_t                      _clientMaxBodySize;
        // location data
        std::vector<std::string>    _indexes;
        std::vector<std::string>    _allowed_methods;             
        std::string                 _root;
        std::string                 _autoIndex;
        std::string                 _upload;
        bool                        _redirection;
        int                         _redirection_code;
        std::string                 _redirection_url;
        std::string                 _realPath;
    public:
        std::string _delete_module;
        std::string _post_module;
        std::string _querry;
        std::string _fileName;
        std::string _cgiExtention;
        std::string _unchunked_body;             
        std::string _contentType;
        bool        _method_isFound;
        std::string _bheaders;
        std::string _fileContent;
        std::string _method;
        std::string _path;
        size_t      _contentLength;
        std::string _referer;
        std::string _body;
        std::string _extension;
        std::string _locationName;
        int         _status;
        int         _respCode;
        std::string _responseCode;
        std::string _respHeaders;
        std::string _redirectionPath;
        std::string _uploadLocation;
        std::string _boundry;
        std::map<int, std::string> _errorPages;
        bool        _iscgi;
        bool        _ischunked;
        bool        _isMultipart;
        bool        _isupload;
        std::string _heder_cookeis;
        bool  is_cookeis;
        // locations 
        // supportedMethods;


    public:
                response();

        void    store_requestInfos(request& req);
        bool    method_isFound();
        int     executing_method (); // executing_method ();
        bool    permissionForReading();
        bool    permissionForExecuting();
        bool    permissionForDeleting();
        int     reading_fileContent();
        int     listing_requestDirectory();
        int     stroring_requestBody();
        int     storing_multipleParts();
        void    determine_contentType();
        void    preparing_responseHeaders();
        void    getting_responsCode();
        std::string    generating_response(request& request, int returnStatus, server_data& serverr);
        void    get_pathAndLocationInfos (server_data &serverrs, std::string uri);
        std::string getErrorPage();
        std::string readPage(std::string page);
        bool        successAnswer(int status);
        bool        redirectionAnswer(int status);
        bool        errorAnswer(int status);

        void        setroot(const std::string& _oldpath)
        {
            oldpath = _oldpath;
        }
        const std::string& getroot()
        {
            return (oldpath);
        }
};

int Execute_cgi(response &res);
#endif