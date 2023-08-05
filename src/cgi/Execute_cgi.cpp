#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "../../inc/response.hpp"

char** vector_to_char(std::vector<std::string> env_cgi) {
    size_t i;
    char** env = (char**)malloc((env_cgi.size() + 1) * sizeof(char*));
    if (env == NULL) {
        std::cerr<<"Memory allocation failed for env"<<std::endl;
        return NULL;
    }
    for (i = 0; i < env_cgi.size(); i++) {
        env[i] = (char*)malloc((env_cgi[i].size() + 1) * sizeof(char));
        if (env[i] == NULL) {
            std::cerr<<"Memory allocation failed for env[i]"<<std::endl;
            for (size_t j = 0; j < i; j++) {
                free(env[j]);
            }
            free(env);
            return NULL;
        }
        strcpy(env[i], env_cgi[i].c_str());
    }
    env[i] = NULL;
    return env;
} 
// location/file.py
//delete file
//session
//coockise
//php
//script
int Execute_cgi(response &res) {
    std::vector<std::string> env_cgi;
    env_cgi.push_back("QUERY_STRING="+ res._querry);
    env_cgi.push_back("REQUEST_METHOD="+ res._method);
    env_cgi.push_back("CONTENT_LENGTH="+ std::to_string(res._contentLength));
    env_cgi.push_back("CONTENT_TYPE="+ res._contentType);
    char ** _env = vector_to_char(env_cgi);
    std::string path;
    std::string namecgi;
    if(res._extension == ".php")
    {
        path = "/usr/bin/php";
        namecgi = "php";
    }else if(res._extension == ".py")
    {
        path = "/usr/bin/python3";
        namecgi = "python3";
    }
    int pipefd[2];
    pid_t pid;
    if (pipe(pipefd) == -1) {
        std::cout<<"pipe failed"<<std::endl;
        return 1;
    }
    pid = fork();
    if (pid < 0) {
        std::cout<<"fork failed"<<std::endl;
        return 1;
    } else if (pid == 0) 
    {
        std::ofstream outFile("/tmp/file.txt");
        if (!outFile) {
            std::cerr << "Error: Failed to open the file." << std::endl;
            exit(127);
        }
        outFile<<res._body;
        outFile.close();
        int file_fd = open("/tmp/file.txt", O_RDONLY);
        if (file_fd == -1) {
         std::cerr << "Error: Failed to open the file." << std::endl;
            exit(127);
        }
        dup2(file_fd,0);
        close(file_fd);
        dup2(pipefd[1], STDOUT_FILENO);
        char* argv[]={(char*)namecgi.c_str(),(char*)(res._realPath).c_str(),nullptr};
        execve(path.c_str(),argv,_env);
        exit(127);
    } 
    else 
    {   
        close(pipefd[1]);
        
        int status;
        pid_t child_pid = waitpid(pid, &status, 0);
        if (child_pid == -1) {
            std::cerr << "Error: waitpid failed." << std::endl;
            return 1;
        } 
        if (status == 0) {
            char buffer[1024000];
            memset(buffer,'\0',1024000);
            int num_bytes = read(pipefd[0], buffer, sizeof(buffer));
            if (num_bytes == -1) {
                std::cerr<<"read failed"<<std::endl;
                return 1;
            }
            if(res._locationName == "/cgi_cookies")
            {
                res._heder_cookeis = buffer;
                res._fileContent = "succece set coockise and seesion";
                res._contentType = "text/html";
                res.is_cookeis = true;
                return 200;
            }
            else
            {
                res._fileContent = buffer;
                res._contentLength = res._fileContent.length();
                res._contentType = "text/html";
                return 200;
            }
        }    
    }
    return 500;
}



