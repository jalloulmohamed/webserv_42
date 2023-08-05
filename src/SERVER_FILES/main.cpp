#include "../../inc/server.hpp"
#include "../../inc/server_data.hpp"
#include "../../inc/request.hpp"
#include "../../inc/client.hpp"
#include "../../inc/tools.hpp"
#include "../../inc/response.hpp"




std::vector<std::string>  find_server_name(client client, std::string port)
{
    std::vector<std::string> clean_name;
    int index = client.server_name.find(":");
    if(index != -1)
    {
        tools::splitting_string(client.server_name, ":", clean_name);
        if(clean_name[0].empty() != 0 && clean_name[1].empty() != 0 )
            return clean_name;
    }
    clean_name.push_back(client.server_name);
    clean_name.push_back(port);
    return clean_name;
}

server_data find_server(std::vector<server_data>& servers, client client)
{
    server_data server_exicte;
    (void)client;
    std::vector<std::string> host =  find_server_name(client,servers[0].port[0]);
    if(host[0] == "localhost")
        host[0] = "127.0.0.1";
    // std::cout<<host[0] <<"______"<<host[1]<<std::endl;
    for (size_t i = 0; i < servers.size(); i++)
    {
        std::vector<std::string>::iterator it = std::find(servers[i].port.begin(), servers[i].port.end(), host[1].c_str());
        std::vector<std::string>::iterator it1 = std::find(servers[i].server_names.begin(), servers[i].server_names.end(), host[0]);
        if((host[0] == servers[i].host  && it != servers[i].port.end()) || (it != servers[i].port.end() && it1 != servers[i].server_names.end()))
        {
            return servers[i];
        }
    }
    server_exicte = servers[0];
    return server_exicte;
}

int main(int argc,char **argv) 
{   
    std::vector<server_data> servers;
	try
	{
		if (argc > 2)
		{
			throw std::invalid_argument("Error: invalid number of arguments");
		}
		if(argc == 1)
		    servers = parse_server("conf/default.toml");
		else
			servers = parse_server(argv[1]);
        std::vector<pollfd> all_df;
        std::vector<server> all_server;
        std::vector<client> all_client;
        std::vector<int> fd_server;
        std::string req;
        std::map<std::string, std::vector<std::string> >  port_Ip_bind;
        
        for (size_t i = 0; i < servers.size(); i++)
        {
            const std::string& ip = servers[i].host;
            const std::vector<std::string>& ports = servers[i].port;

            std::map<std::string, std::vector<std::string> >::iterator it = port_Ip_bind.find(ip);
            if (it != port_Ip_bind.end()) {
                for (size_t j = 0; j < ports.size(); j++) {
                    if (std::find(it->second.begin(), it->second.end(), ports[j]) == it->second.end()) {
                        it->second.push_back(ports[j]);
                    }
                }
            } else 
                port_Ip_bind[ip] = ports;
        }
        //staart server and bind and lesten
        for (std::map<std::string, std::vector<std::string> >::iterator it = port_Ip_bind.begin(); it != port_Ip_bind.end(); ++it) {
            for (size_t i = 0; i < it->second.size(); i++) {
                server Server = server(it->second[i],it->first);
                all_server.push_back(Server);
                if(Server.startServer())
                    return 1;
                struct pollfd fds;
                fds.fd= Server.getSockert();
                fds.events = POLLIN;
                all_df.push_back(fds);
                fd_server.push_back(Server.getSockert());
                if(Server.bindServer())
                    return 1;
                if(Server.listenSrver())
                    return 1;
            }
        }

        signal(SIGPIPE, SIG_IGN);
        while (true)
        {
            int rev =  poll(&all_df[0], all_df.size(),-1);
            if (rev == -1) {
                std::cerr << "Error: Polling failed\n";
                return 1;
            }
            for (size_t i = 0; i < all_df.size(); i++)
            {
                if (all_df[i].revents  & POLLHUP) {
                    
                    for (size_t j = 0; j < all_client.size(); j++)
                    {
                        if(all_client[j].getfd() == all_df[i].fd)
                        {
                                close(all_df[i].fd);
                                all_df.erase(all_df.begin() + i);
                                all_client.erase(all_client.begin() + j);         
                                break; 
                        }
                    }        
                } 
                else if (all_df[i].revents & POLLIN ) {
                    std::vector<int>::iterator it = std::find(fd_server.begin(), fd_server.end(), all_df[i].fd); 
                    if (it != fd_server.end()) {
                        int client_socket = accept(all_df[i].fd, (sockaddr*) &all_server[std::distance(fd_server.begin(), it)].getClientAdtess(), &all_server[std::distance(fd_server.begin(), it)].getClientAdtessSize()); 
                        if (client_socket == -1)
                        {
                            std::cerr << "Error: client connection failed\n";
                            close(all_df[i].fd); 
                            return 1;
                        }
                        struct pollfd fds;
                        fds.fd=  client_socket;
                        fds.events = POLLIN;
                        all_df.push_back(fds);
                        client newClient(fds.fd);
                        all_client.push_back(newClient);
                        // std::cout<< "server ==> "<< all_df[i].fd << "   accept connection cleient ==> " << client_socket<<std::endl;
                    }
                    else
                    {
                        char buff[1024000]; 
                        std::memset(buff, '\0', sizeof(buff));
                        int content = read(all_df[i].fd,buff,1024000);
                        if (content == -1) {
                            for (size_t j = 0; j < all_client.size(); j++)
                            {
                                if(all_client[j].getfd() == all_df[i].fd)
                                {
                                        close(all_df[i].fd);
                                        all_df.erase(all_df.begin() + i);
                                        all_client.erase(all_client.begin() + j);         
                                        break; 
                                }
                            }        
                        }
                        // 408 Request Timeout
                        for (size_t j = 0; j < all_client.size(); j++)
                        {
                            if(all_client[j].getfd() == all_df[i].fd)
                            {
                                if (content == 0 && !all_client[j].getfirstbuff()) {
                                    close(all_df[i].fd);
                                    all_df.erase(all_df.begin() + i);
                                    all_client.erase(all_client.begin() + j);         
                                    break;        
                                }
                                all_client[j].setservr_name(buff);
                                all_client[j].appendreq(buff,content);
                                all_client[j].addTocontentread(content);
                                
                                if((all_client[j].getreq().length() && !all_client[j].ischunked && all_client[j].getreq().find("\r\n\r\n") != std::string::npos  &&  all_client[j].getcontentlenght() <= all_client[j].getcontentread()) 
                                || ( all_client[j].ischunked && all_client[j].getreq().find("\r\n0\r\n\r\n") != std::string::npos) )
                                {
                                    all_df[i].events = POLLOUT;
                                }
                            }
                        }
                    }
                }
              
                else if(all_df[i].revents & POLLOUT)
                { 
                    size_t j = 0;
                    for (; j < all_client.size(); j++)
                    {
                        if(all_client[j].getfd() == all_df[i].fd)
                        {
                            break;
                        }
                    }
                        if(!all_client[j].proccessing)
                        {
                            request req;
                            server_data ser=find_server(servers, all_client[j]);
                            int ret_status = req.processing_request(all_client[j], ser );
                            if (all_client[j]._requestIsParsed == true)
                            {
                                response resp;
                                all_client[j]._response = resp.generating_response(req, ret_status, ser);
                            }
                            all_client[j].proccessing=true;
                        }
                        int buffwrite = 0;
                        if(all_client[j].sendLenth < (long)all_client[j]._response.length())
                        {
                            if( (long)all_client[j]._response.length() - all_client[j].sendLenth < 1024)
                                buffwrite =  (long)all_client[j]._response.length() - all_client[j].sendLenth;
                            else
                                buffwrite = 1024;
                            int res = write(all_df[i].fd, all_client[j]._response.c_str() + all_client[j].sendLenth, buffwrite);
                            if(res == -1 || res == 0 )
                            {
                                close(all_df[i].fd);
                                all_df.erase(all_df.begin() + i);
                                all_client.erase(all_client.begin() + j);
                                break;
                            }
                            all_client[j].sendLenth+=buffwrite;
                        }
                        else
                        {
                                close(all_df[i].fd);
                                all_df.erase(all_df.begin() + i);
                                all_client.erase(all_client.begin() + j);         
                        }
                        continue;      
                }
                else if (all_df[i].revents  & POLLERR) {
                    std::cerr << "Error on socket " << all_df[i].fd << std::endl;
                    close(all_df[i].fd);
                    all_df.erase(all_df.begin() + i);
                }  else if (all_df[i].revents  & POLLNVAL) {
                    close(all_df[i].fd);
                    all_df.erase(all_df.begin() + i);
                }
            }
        }
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
    return 0;
}

