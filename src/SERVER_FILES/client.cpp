#include "../../inc/client.hpp"
#include <fstream>  
#include <sstream> 
client::client()
{

}
client::~client()
{

}

client::client(int fd)
{
    this->req = "";
    this->fd =fd;
    this->contentlenght=0;
    this->contentread = 0;
    this->firstbuff = false;
    this->server_name="";
    this->sendLenth = 0;
    this->proccessing = false;
   
    //     std::ifstream file1("root/1.mp4", std::ios::binary);

    //     if (!file1.is_open()) {
    //     std::cerr << "Failed to open the file." << std::endl;
    //     exit(0);
    //     }

    //     // Create a stringstream to store the video data
    //     std::stringstream stream;
    //     stream << file1.rdbuf();
    //     file1.close();

    //     // Get the video data as a string from the stringstream
    //     std::string vidio = stream.str();

    //    this->res = "HTTP/1.1 200 OK\r\n";
    //     this->res += "Content-Type: video/mp4\r\n";
    //     this->res += "Set-Cookie: Darkmode=true\r\n";
    //     this->res += "Content-Length: " + std::to_string(vidio.length()) + "\r\n";
    //     this->res += "Set-Cookie: sessionID=abc123; Path=/; Secure; HttpOnly\r\n\r\n";
    //     // Concatenate the headers and video binary data
    //     this->res += vidio;

//    text
    // std::ifstream file("root/index.html");
    // std::string html;
    // std::string line; 
    // if (!file.is_open()) {
    //     std::cerr << "Failed to open the file." << std::endl;
    // } 
    // if (file.is_open()) 
    // {
    //     while (std::getline(file, line))
    //         html+= line;
    //     file.close();
    // }

    // std::string headers = "HTTP/1.1 200 OK\r\n";
    // headers += "Content-Type: text/html\r\n";
    // headers += "Set-Cookie: Darkmode=true\r\n";
    // headers += "Content-Length: " + std::to_string(html.length()) + "\r\n";
    // headers += "Set-Cookie: sessionID=abc123; Path=/; Secure; HttpOnly\r\n\r\n";
    // headers +=   html;

    

}

int client::getfd() const
{
    return this->fd;
}

void client::setfd(int const fd)
{
    this->fd = fd;
}

std::string client::getreq() const
{
    return this->req;
}

void client::appendreq(char const *req, int count)
{
    if(!this->firstbuff)
    {
        std::string input(req);
        this->firstbuff = true;
        int firstposition = input.find("Content-Length:"); 
        if(firstposition !=  (int)std::string::npos)
        {
            int lastposition = input.find("\n",firstposition);
            this->contentread = (-1 * lastposition) ;
            std::string lent = input.substr(firstposition + 15,lastposition - firstposition);
            if(lent.length())
                this->contentlenght= std::atoi(lent.c_str());
            this->ischunked= false;
        }
        else if (input.find("Transfer-Encoding: chunked") !=  std::string::npos)
        {
            this->ischunked= true;
        }

        
    }
    this->req.append(req,count);
}

void client::setcontentlenght(long long const contentlenght)
{
    this->contentlenght = contentlenght;

}
long long client::getcontentlenght() const
{
    return this->contentlenght;
}
void client::setfirstbuff(const bool firstbuffer)
{
    this->firstbuff = firstbuffer;

}
bool client::getfirstbuff() const
{
    return this->firstbuff;
}

void client::addTocontentread(long long const contentread)
{
    this->contentread += contentread;

}
long long client::getcontentread() const
{
    return this->contentread;
}

void client::setservr_name(char const *first_req)
{
    if(!firstbuff)
    {
        std::string buff(first_req);
        int index1 = buff.find("Host:") + 6;
        int index2 = buff.find("\r\n", index1);
        if(index1 != -1 && index2 != -1 )
        {
            this->server_name = buff.substr(index1 , index2 - index1);
        }
    }
}