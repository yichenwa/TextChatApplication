#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include "../include/global.h"
#include "../include/logger.h"
#include "../include/ManipulateStr.h"
#define BUFFER_SIZE 256
#define BUFFERL_SIZE 2048

char* ipfunction_Client();
void setup_Client(int client_port);




void setup_Client(int client_port){
        int client_sockfd;
        struct sockaddr_in server_addr;
        char* client_ip;
        const char* server_ip;
        int server_port;
        char buffer[1024] = {0}; 
        fd_set all_fd, read_fd;
        FD_ZERO(&all_fd);
        FD_ZERO(&read_fd);
        FD_SET(STDIN_FILENO, &all_fd);
        int  max_fd = STDIN_FILENO;
        bool loggedStatus=false;
        client_ip=ipfunction_Client();//call the ip Function to get this client's IP address
        //client_port is we implement from yichenwa_assignment1.cpp
        //Now we have both ip and port.
        std::string client_list;
        /*1. Creat Socket*/
        client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(client_sockfd < 0){
                cse4589_print_and_log("Failed to create socket.\n");
        }
        std::string blockedList[4];
        for (int i = 0; i < 4; i++)   
        {   
        blockedList[i] = "Empty";   
        } 

        /*2. Connect*/

        while(1){
        
              read_fd = all_fd;
              if (select(max_fd + 1, &read_fd, NULL, NULL, NULL) == -1) {
               exit(EXIT_FAILURE);
                } else {
            // read stdin
               if (FD_ISSET(STDIN_FILENO, &read_fd)) {        
                std::string str;
                std::vector<std::string> coms;
                //coms = {};
                std::getline(std::cin,str);
                std::istringstream iss(str);
                for (std::string str; iss>>str;)
                        coms.push_back(str);

                if (str=="AUTHOR"){
                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                        cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n","yichenwa");
                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                }
                else if (str=="EXIT"){
                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                        break;
                }
                else if (str=="IP"){
                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                        cse4589_print_and_log("IP:%s\n",client_ip);
                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                }
                else if (str=="PORT"){
                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                        cse4589_print_and_log("PORT:%d\n",client_port);
                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                }
                else if (coms.size() && coms.at(0)=="LOGIN"){
                        server_ip = coms.at(1).c_str();
                        std::stringstream(coms.at(2))>>server_port;
                       
                        if (ManipulateStr::validIp(server_ip) == false || ManipulateStr::validPort(coms.at(2).c_str()) == false) {
                       
                            cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
                            cse4589_print_and_log("[%s:END]\n", "LOGIN");
                            //exit(1);
                        }
                        else{
                       
                            bzero(&server_addr, sizeof(server_addr));
                            server_addr.sin_family = AF_INET;
                            inet_pton(AF_INET, server_ip,&server_addr.sin_addr);
                            server_addr.sin_port = htons(server_port);
                            if(connect(client_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
                                cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
                                cse4589_print_and_log("[%s:END]\n", "LOGIN");
                            }
                            else
                            {
                            //Send client port number
                                std::string portCli="CLIENTPORT "+(ManipulateStr::int2str(client_port));
                                const char* msg = portCli.c_str();
                       
                                if (send(client_sockfd, msg,(int)strlen(msg), 0)<0){
                                    cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
                                    cse4589_print_and_log("[%s:END]\n", "LOGIN");
                                    //exit(1);
                                }

                       
                                char *bufferl = (char*) malloc(sizeof(char)*BUFFERL_SIZE);
                                memset(bufferl, '\0', BUFFERL_SIZE);
                                int listreq;
                       
                                if(listreq=recv(client_sockfd, bufferl, BUFFERL_SIZE, 0) <= 0){
                                    cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
                                    cse4589_print_and_log("[%s:END]\n", "LOGIN");
                                }
                                else
                                {
                                    client_list = bufferl;
                                    cse4589_print_and_log("[%s:SUCCESS]\n","LOGIN");
                                    cse4589_print_and_log("[%s:END]\n","LOGIN");
                         
                                }

                                FD_SET(client_sockfd, &all_fd);
                                max_fd = client_sockfd;
                                loggedStatus=true;
                        
                            }
                        }
                }
                else if (coms.size() == 1 && coms.at(0) == "REFRESH" && loggedStatus){
                       const char* msg = str.c_str();
                        if (send(client_sockfd, msg,(int)strlen(msg), 0)<0){
                        cse4589_print_and_log("Send Failed");
                        exit(1);
                        }
                       char *bufferl = (char*) malloc(sizeof(char)*BUFFERL_SIZE);
                        memset(bufferl, '\0', BUFFERL_SIZE);
                        int listreq;
                        if(listreq=recv(client_sockfd, bufferl, BUFFERL_SIZE, 0) <= 0){
                        
                        }
                        else
                        {
                         client_list = bufferl;
                         /*std::string client_commandl = bufferl;
                         std::string sl = ManipulateStr::split(client_commandl, '\n')[0];    
                         if (sl.size() < 1)
                         return;
                         std::vector<std::string> vl = ManipulateStr::split(sl, '#');
                         */cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                         /*for(int i=0;i<vl.size();i++)
                         { 
                             std::vector<std::string> vd = ManipulateStr::split(vl[i], '$');
                             struct hostent *he;
                             struct in_addr ipv4addr;
                             inet_pton(AF_INET, vd[0].c_str(), &ipv4addr);
                             he =gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
                             cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i+1, he->h_name, vd[0].c_str(), atoi(vd[1].c_str()));
                         }*/
                         cse4589_print_and_log("[%s:END]\n",str.c_str());
                         
                        }

                }
                else if (coms.size() >= 3 && coms.at(0) == "SEND" && loggedStatus){
                      const char* msg = str.c_str();
                        const char* server_ipS = coms.at(1).c_str();
                        bool exist= false;
                        std::string client_commandl = client_list;
                        std::string sl = ManipulateStr::split(client_commandl, '\n')[0];    
                        if (sl.size() < 1)
                        return;
                        std::vector<std::string> vl = ManipulateStr::split(sl, '#');
                        for(int i=0;i<vl.size();i++)
                        { 
                            std::vector<std::string> vd = ManipulateStr::split(vl[i], '$');
                            if(vd[0].compare(coms.at(1))==0)
                            {   
                                exist = true;
                                break;
                            }
                
                        }
                        if (ManipulateStr::validIp(server_ipS) == false || !exist ) {
                        
                            cse4589_print_and_log("[%s:ERROR]\n", "SEND");
                            cse4589_print_and_log("[%s:END]\n", "SEND");
                        }       
                       else
                       {

                       if (send(client_sockfd, msg,(int)strlen(msg), 0)<0){
                       cse4589_print_and_log("[%s:ERROR]\n", "SEND");
                       cse4589_print_and_log("[%s:END]\n", "SEND");
                       }
                       else
                       {
                        cse4589_print_and_log("[%s:SUCCESS]\n","SEND");
                        cse4589_print_and_log("[%s:END]\n","SEND");  
                       }
                }
               }

                else if (coms.size() >= 2 && coms[0] == "BROADCAST" && loggedStatus){
                    
                      const char* msg = str.c_str();
                       if (send(client_sockfd, msg,(int)strlen(msg), 0)<0){
                       cse4589_print_and_log("Send Failed");
                       exit(1);
                       }
                }
                else if (coms.size() == 2 && coms[0] == "BLOCK" && loggedStatus){
                   bool alreadyexist =false;
                    const char* msg = str.c_str();
                    for (int i = 0; i < 4; i++)   
                      {   
                        if( blockedList[i].compare(coms.at(1))==0)   
                        { 
                          alreadyexist =true; 
                          break;
                        }
                      }

                      bool exist= false;
                        std::string client_commandl = client_list;
                        std::string sl = ManipulateStr::split(client_commandl, '\n')[0];    
                        if (sl.size() < 1)
                        return;
                        std::vector<std::string> vl = ManipulateStr::split(sl, '#');
                        for(int i=0;i<vl.size();i++)
                        { 
                            std::vector<std::string> vd = ManipulateStr::split(vl[i], '$');
                            if(vd[0].compare(coms.at(1)))
                            {
                                exist = true;
                                break;
                            }
                
                        }
                      server_ip = coms.at(1).c_str();
                      if(ManipulateStr::validIp(server_ip) == false || alreadyexist || !exist )
                      {
                        cse4589_print_and_log("[%s:ERROR]\n", "BLOCK");
                        cse4589_print_and_log("[%s:END]\n", "BLOCK");
                      }
                      else
                      {

                      for (int i = 0; i < 4; i++)   
                      {   
                        if( blockedList[i].compare("Empty")==0 )   
                        { 
                            blockedList[i]= coms[1];
                            break;
                        }
                      }
                       if(send(client_sockfd, msg,(int)strlen(msg), 0)<0){
                       cse4589_print_and_log("[%s:ERROR]\n", "BLOCK");
                       cse4589_print_and_log("[%s:END]\n", "BLOCK");
                       exit(1);
                       }
                       else
                       {
                        cse4589_print_and_log("[%s:SUCCESS]\n","BLOCK");
                        cse4589_print_and_log("[%s:END]\n","BLOCK");  
                       }

                    }

                }
                else if (coms.size() == 2 && coms[0] == "UNBLOCK" && loggedStatus){
                 bool alreadyexist =false;
                    const char* msg = str.c_str();
                    for (int i = 0; i < 4; i++)   
                      {   
                        if( blockedList[i].compare(coms.at(1))==0)   
                        { 
                          alreadyexist =true; 
                          break;
                        }
                      }

                      bool exist= false;
                        std::string client_commandl = client_list;
                        std::string sl = ManipulateStr::split(client_commandl, '\n')[0];    
                        if (sl.size() < 1)
                        return;
                        std::vector<std::string> vl = ManipulateStr::split(sl, '#');
                        for(int i=0;i<vl.size();i++)
                        { 
                            std::vector<std::string> vd = ManipulateStr::split(vl[i], '$');
                            if(vd[0].compare(coms.at(1)))
                            {
                                exist = true;
                                break;
                            }
                    
                        }
                      server_ip = coms.at(1).c_str();
                      if(ManipulateStr::validIp(server_ip) == false || !alreadyexist || !exist )
                      {
                        cse4589_print_and_log("[%s:ERROR]\n", "UNBLOCK");
                        cse4589_print_and_log("[%s:END]\n", "UNBLOCK");
                      }
                      else
                      {

                      for (int i = 0; i < 4; i++)   
                      {   
                        if( blockedList[i].compare(coms.at(1))==0 )   
                        { 
                            blockedList[i]= "Empty";
                            break;
                        }
                      }
                       if(send(client_sockfd, msg,(int)strlen(msg), 0)<0){
                       cse4589_print_and_log("[%s:ERROR]\n", "UNBLOCK");
                       cse4589_print_and_log("[%s:END]\n", "UNBLOCK");
                       exit(1);
                       }
                       else
                       {
                        cse4589_print_and_log("[%s:SUCCESS]\n","UNBLOCK");
                        cse4589_print_and_log("[%s:END]\n","UNBLOCK");  
                       }

                    }
                    
                }
                else if (coms.size() == 1 && coms[0] == "LOGOUT" && loggedStatus){
                     cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                        const char* msg = str.c_str();
                        if (send(client_sockfd, msg,(int)strlen(msg), 0)<0){
                            cse4589_print_and_log("[%s:ERROR]\n",(coms.at(0)).c_str());
                            cse4589_print_and_log("[%s:END]\n",(coms.at(0)).c_str());
                        }
                        client_sockfd=-1;
                        client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
                        if(client_sockfd < 0){
                            cse4589_print_and_log("Failed to create socket.\n");
                        }
                        loggedStatus=false;
                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                }
                else if (coms.size() == 1 && coms[0] == "EXIT") {

                }
                else if ((str=="LIST") && loggedStatus)
                {
                      const char* msg = str.c_str();
                       if (send(client_sockfd, msg,(int)strlen(msg), 0)<0){
                       cse4589_print_and_log("Send Failed");
                       exit(1);
                       }
                      char *bufferl = (char*) malloc(sizeof(char)*BUFFERL_SIZE);
                       memset(bufferl, '\0', BUFFERL_SIZE);
                       int listreq;
                       if(listreq=recv(client_sockfd, bufferl, BUFFERL_SIZE, 0) <= 0){
                       
                       }
                       else
                       {
                        client_list = bufferl;
                        std::string client_commandl = bufferl;
                        std::string sl = ManipulateStr::split(client_commandl, '\n')[0];    
                        if (sl.size() < 1)
                        return;
                        std::vector<std::string> vl = ManipulateStr::split(sl, '#');
                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                        for(int i=0;i<vl.size();i++)
                        { 
                            std::vector<std::string> vd = ManipulateStr::split(vl[i], '$');
                            struct hostent *he;
                            struct in_addr ipv4addr;
                            inet_pton(AF_INET, vd[0].c_str(), &ipv4addr);
                            he =gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
                            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i+1, he->h_name, vd[0].c_str(), atoi(vd[1].c_str()));
                        }
                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                        
                       }
                }
                }
                else if(FD_ISSET(client_sockfd, &read_fd)){
                char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                memset(buffer, '\0', BUFFER_SIZE);
                int rval;
                if(rval=recv(client_sockfd, buffer, BUFFER_SIZE, 0) <= 0){
                printf("server disconnected"); 
                }
                else
                {
               
                std::string client_command = buffer;
                std::string s = ManipulateStr::split(client_command, '\n')[0];
                if (s.size() < 1)
                return;
                std::vector<std::string> v = ManipulateStr::split(s, '#');
               
                if(v.size() == 4 && v[0] == "RELAYED")
                {   
                    cse4589_print_and_log("[%s:SUCCESS]\n","RECEIVED");
                    cse4589_print_and_log("msg from:%s\n[msg]:%s\n", v[2].c_str(), v[3].c_str());
                    cse4589_print_and_log("[%s:END]\n","RECEIVED");
                    
                } 
               }

              }

        }
       } 
       /*3. Try to send a message
        char *hi = (char*)"Hi, I am client.";
        char buffer[1024]={0};
        send(client_sockfd, hi, strlen(hi), 0);
        printf("A message is sent\n");
        read(client_sockfd, buffer, 1024);
        printf("%s\n",buffer);*/

}
/*IP*/
//reference https://www.geeksforgeeks.org/c-program-display-hostname-ip-address/
char* ipfunction_Client(){
        char hostbuffer[128];
        char *IPbuffer;
        struct hostent *host_entry;
        int i;

        gethostname(hostbuffer,sizeof(hostbuffer));

        host_entry=gethostbyname(hostbuffer);

        IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
        return IPbuffer;
}