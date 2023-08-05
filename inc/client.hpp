#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <sys/socket.h> 
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <fcntl.h>

class client
{
    public:
        std::string _response;
        std::string _unchunked_body;
        bool        _response_isReady;
        int         _serverIndex;
        bool        _requestIsParsed;
        bool ischunked;
        std::string server_name;
        long sendLenth;
        std::string res;
        bool proccessing;
    private:
        std::string req;
        int fd;
        long long contentlenght;
        long long contentread;
        bool firstbuff;
        
    public:
        client();
        client(int fd);
        int getfd() const ;
        void setfd(int const fd);
        std::string getreq() const;
        void appendreq(char const  *req,int count); 
        void setcontentlenght(long long const contentlenght);
        long long getcontentlenght() const;
        void setfirstbuff(const bool firstbuffer);
        bool getfirstbuff() const;
        void addTocontentread(long long const contentread);
        long long getcontentread() const;
        void setreq(std::string  const req);
        void setservr_name(char const *first_req);
        ~client();
};


#endif
