#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
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
#include <string>
#include <algorithm>
#include "../include/global.h"
#include "../include/logger.h"
#include "../include/ManipulateStr.h"


#define BACKLOG 5
#define STDIN 0
#define CMD_SIZE 100
#define BUFFER_SIZE 256

char* ipfunction_Server();
void setup_Server(int myport);

void setup_Server(int myport){
        int master_socket, new_socket,client_socket[5] ,  max_clients=5,clients=0;
        struct sockaddr_in my_addr, client_addr;
        socklen_t caddr_len;
        char* server_ip;//this ip is for IP function to print out.
        server_ip=ipfunction_Server();
        char buffer[1025]; 
        struct clientList
        {
            int sock;
            std::string ipAddr;
            std::string portNo;
            int msgSent;
            int msgRec;
            struct blockingStatus
            {
                bool status;
                std::string blockClientAddr[5];
            }blockS;
            bool logStatus;
        }list[5] ;

        //initialise all client_socket[] to 0 so not checked  
    for (int i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    } 

        //create a master socket  
        master_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(master_socket<0){
                cse4589_print_and_log("Cannot create socket.\n");
        }

        /*2. Bind*/
        bzero(&my_addr, sizeof(my_addr));

        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr= htonl(INADDR_ANY);
        my_addr.sin_port = htons(myport);

        if(bind(master_socket,(struct sockaddr *)&my_addr, sizeof(my_addr))<0){
                cse4589_print_and_log("Bind failed.\n");
        }

        /*3. Listen*/
        if(listen(master_socket, BACKLOG)<0){
                cse4589_print_and_log("Listen failed.\n");
                //return -1;
        }

        /*----------------------SELECT--------------------------*/
        /*These code are follow the server.c*/
        fd_set master_list, watch_list;
        int head_socket, selret, sock_index,sd;
        //Initializes have zero bits for all file descriptors
        FD_ZERO(&master_list);
        FD_ZERO(&watch_list);
        //FD_SET(fd, &fdset) sets the bit of fd in the fdset
        FD_SET(master_socket, &master_list);
        FD_SET(STDIN, &master_list);

        head_socket = master_socket;
        while(1){
                memcpy(&watch_list, &master_list, sizeof(master_list));//copy master_list to watch_list
                selret = select(head_socket+1, &watch_list, NULL, NULL, NULL);
                if(selret <0){
                        cse4589_print_and_log("Select failed.\n");
                }
                if(selret > 0){
                        for(sock_index=0; sock_index<=head_socket; sock_index+=1){
                                if(FD_ISSET(sock_index, &watch_list)){
                                        if(sock_index == STDIN){//IF we have a command, I will think how to add functions IP, AUTHOR, into this part.

                                                std::string str;
                                                std::vector<std::string> coms;
                                                //coms = {};
                                                std::getline(std::cin,str);
                                                std::istringstream iss(str);
                                                for(std:: string str; iss>>str;)
                                                        coms.push_back(str);


                                                if (str=="EXIT"){
                                                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                                                        exit(0);
                                                }
                                                else if (str=="AUTHOR"){
                                                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                                                        cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n","yichenwa");
                                                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                                                        fflush(STDIN);
                                                }
                                                else if (str=="PORT"){
                                                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                                                        cse4589_print_and_log("PORT:%d\n", ntohs(my_addr.sin_port));
                                                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                                                        fflush(STDIN);
                                                }
                                                else if (str=="IP"){
                                                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                                                        cse4589_print_and_log("IP:%s\n",server_ip);
                                                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                                                        fflush(STDIN);
                                                }
                                                else if (str=="LIST"){

                                                        int size = sizeof(list)/sizeof(list[0]); 
                                                        printf("%d \n",size);
                                                        cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                                                        for(int i=0;i<max_clients;i++)
                                                          {
                                                               if((list[i].ipAddr.compare("")!=0) && list[i].logStatus==true){
                                                               struct hostent *he;
                                                               struct in_addr ipv4addr;
                                                               inet_pton(AF_INET, list[i].ipAddr.c_str(), &ipv4addr);
                                                               he =gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
                                                               cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i+1, he->h_name, list[i].ipAddr.c_str(), atoi(list[i].portNo.c_str()));
                                                              }
                                                              
                                                          }
                                                          cse4589_print_and_log("[%s:END]\n",str.c_str());
                                              
                                                }
                                                 else if(coms.size()==2 && coms.at(0)=="BLOCKED"){
                                                   std::string rIP = coms.at(1);
                                                   int validcount=0;

                                                   for(int valid =0;valid<max_clients;valid++){
                                                        if(list[valid].ipAddr.compare(rIP)==0)
                                                        {

                                                            validcount ++;

                                                            break;

                                                        }

                                                    }

                                                   if (ManipulateStr::validIp(rIP) == false || validcount==0) 

                                                   {   

                                                     cse4589_print_and_log("[%s:ERROR]\n", "BLOCKED");

                                                     cse4589_print_and_log("[%s:END]\n", "BLOCKED");

                                                    }else{

      
                                                        std::vector<std::vector<int> > portV;
                                                        for(int i=0;i<max_clients;i++){
                                                            if((list[i].ipAddr.compare("")!=0)){
                                                                std::vector<int> portv;
                                                                portv.push_back(atoi(list[i].portNo.c_str()));
                                                                portv.push_back(i);
                                                                portV.push_back(portv);
                                                            
                                                            }
                                                        }
                                                        std::sort(portV.begin(), portV.end());
                                                        std::vector<int> portRank;
                                                        for(int i=0; i<portV.size();i++){
                                                            portRank.push_back((portV.at(i)).at(1));
                                                        }
                                                        clientList listB; 
                                                        for(int i=0;i<max_clients;i++){
                                                            if((list[i].ipAddr.compare(rIP)==0)){
                                                                listB=list[i];
                                                                break;
                                                            }
                                                        }
                                                        int count = 0;
                                                        cse4589_print_and_log("[%s:SUCCESS]\n","BLOCKED");
                                                        for(int i=0; i<portRank.size();i++){
                                                            int j = portRank.at(i);
                                                            struct hostent *he;
                                                            struct in_addr ipv4addr;
                                                            inet_pton(AF_INET, list[j].ipAddr.c_str(), &ipv4addr);
                                                            he =gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
                                                            for (int k=0 ;k<4;k++){
                                                             if(listB.blockS.blockClientAddr[k].compare(list[j].ipAddr)==0)
                                                             {count++;  
                                                             cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", count, he->h_name,list[j].ipAddr.c_str(), atoi(list[j].portNo.c_str()));
                                                            }}
                                                        }                                                    
                                                       cse4589_print_and_log("[%s:END]\n","BLOCKED"); 
                                                    }   
                                                 }
                                                 else if(str=="STATISTICS"){

                                                    int size = sizeof(list)/sizeof(list[0]); 
                                                    cse4589_print_and_log("[%s:SUCCESS]\n",str.c_str());
                                                    std::vector<std::vector<int> > portV;
                                                    for(int i=0;i<max_clients;i++){
                                                        if((list[i].ipAddr.compare("")!=0)){
                                                            std::vector<int> portv;
                                                            portv.push_back(atoi(list[i].portNo.c_str()));
                                                            portv.push_back(i);
                                                            portV.push_back(portv);
                                                        }
                                                    }
                                                    if(portV.size()==0){
                                                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                                                    }
                                                    else{
                                                        std::sort(portV.begin(), portV.end());
                                                        std::vector<int> portRank;
                                                        for(int i=0; i<portV.size();i++){
                                                            portRank.push_back((portV.at(i)).at(1));
                                                        } 

                                                        for(int i=0; i<portRank.size();i++){
                                                            int j = portRank.at(i);
                                                            struct hostent *he;
                                                            struct in_addr ipv4addr;
                                                            inet_pton(AF_INET, list[j].ipAddr.c_str(), &ipv4addr);
                                                            he =gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
                                                            std::string logMess;
                                                            if (list[j].logStatus==true){
                                                                logMess = "logged-in";
                                                            }else{
                                                                logMess = "logged-out";
                                                            }
                                                            cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i+1, he->h_name, list[j].msgSent, list[j].msgRec, logMess.c_str());

                                                        }
                                                    }
                                                    cse4589_print_and_log("[%s:END]\n",str.c_str());


                                                }

                                                else{
                                                        cse4589_print_and_log("[%s:ERROR]\n",str.c_str());
                                                        cse4589_print_and_log("[%s:END]\n",str.c_str());
                                                        fflush(STDIN);
                                                }

                                         } 
                                        else if(sock_index ==master_socket){//IF a client want to connect server
                                                caddr_len = sizeof(client_addr);
                                                new_socket =  accept(master_socket, (struct sockaddr *)&client_addr, (socklen_t*)&caddr_len);
                                                if (new_socket < 0)
                                                printf("\n Remote Host connected!\n");
                                                FD_SET(new_socket, &master_list);
                                                if(new_socket > head_socket)
                                                        head_socket = new_socket;
                                                //int read_count;
                                                //if ((read_count = read(STDIN_FILENO, buffer, sizeof buffer)) > 0) {
                                                  //  printf("%sread \n",buffer);
                                                //}
                                                //add new socket to array of sockets  
                                                for (int i = 0; i < max_clients; i++)   
                                                {   
                                                //if position is empty  
                                                if( client_socket[i] == 0 )   
                                                {   
                                                   client_socket[i]= new_socket;

                                                list[i].sock= new_socket;
                                                list[i].ipAddr = inet_ntoa(client_addr.sin_addr);
                                                list[i].msgSent= 0;
                                                list[i].msgRec=0;
                                                list[i].blockS.status= false;
                                                for (int k = 0; k < 4; k++)   
                                                {   
                                                    list[i].blockS.blockClientAddr[k] = "Empty";   
                                                } 
                                                list[i].logStatus= true;
                                                
                                                printf("Adding to list of sockets as %d\n" , i);   
                                                clients++;
                                                break;   
                                                }   
                                                }   
                                        }   
                                              else{ 
                                               //else its some IO operation on some other socket 
                                              for (int i = 0; i < max_clients; i++)   
                                              {   
                                               sd = client_socket[i];  
                                               if (sock_index ==sd){                                      
                                                //If a connected client is doing something (exit)
                                                char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                                                memset(buffer, '\0', BUFFER_SIZE);
                                                int rval;
                                                //printf("\n in else %d\n",sock_index);
                                                if(rval=recv(sd, buffer, BUFFER_SIZE, 0) <= 0){
                                                        //printf("\n in else %d\n",rval);
                                                        getpeername(sd , (struct sockaddr *)&client_addr , (socklen_t*)&caddr_len); 
                                                        printf("Remote Host terminated connection!\n");
                                                        printf("Host disconnected , ip %s , port %d \n" ,  
                                                        inet_ntoa(client_addr.sin_addr) , ntohs(client_addr.sin_port));
                                                        FD_CLR(sock_index, &master_list);
                                                        close(sd);
                                                        client_socket[i] = 0;
                                                }
                                                else {
                                             
                                                getpeername(sd , (struct sockaddr *)&client_addr , (socklen_t*)&caddr_len); 
                                                std::string client_command = buffer;
                                                std::string s = ManipulateStr::split(client_command, '\n')[0];    
                                                if (s.size() < 1)
                                                return;
                                                std::vector<std::string> v = ManipulateStr::split(s, ' ');
                                                //std::cout << "select_event: " << v[0] << " size: " << v.size() << std::endl;
                                                if (v.size() == 1 && v[0] == "LIST") {
                                                  std::string listConcat;
                                                  int size = sizeof(list)/sizeof(list[0]); 
                                                  printf("%d \n",size);
                                                  for(int i=0;i<max_clients;i++)
                                                    {
                                                        
                                                         if((list[i].ipAddr.compare("")!=0) && list[i].logStatus==true){
                                                         listConcat.append(list[i].ipAddr + "$" + list[i].portNo + "#");     
                                                        }
                                                       
                                                    }
                                                   char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                                                    memset(buffer, '\0', BUFFER_SIZE);
                                                    strcpy(buffer, listConcat.c_str());
                                                    if(send(sd, buffer, strlen(buffer), 0) == strlen(buffer))
                                                        printf("Done!\n");
                                                    fflush(stdout);
                                                    free(buffer);     
                                                } 

                                                else if (v.size() == 1 && v[0] == "REFRESH") {
                                                   std::string listConcat;
                                                  int size = sizeof(list)/sizeof(list[0]);
                                                  printf("%d \n",size);
                                                  for(int i=0;i<max_clients;i++)
                                                    {

                                                         if((list[i].ipAddr.compare("")!=0) && list[i].logStatus==true){
                                                         listConcat.append(list[i].ipAddr + "$" + list[i].portNo + "#");
                                                        }

                                                    }
                                                   char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                                                    memset(buffer, '\0', BUFFER_SIZE);
                                                    strcpy(buffer, listConcat.c_str());
                                                    if(send(sd, buffer, strlen(buffer), 0) == strlen(buffer))
                                                        printf("Done!\n");
                                                    fflush(stdout);
                                                    free(buffer);
                                                    
                                                } else if (v.size() >= 3 && v[0] == "SEND") {
                                                    //v = StringHandler::split_part(s, ' ', 3);
                                                    //event_SEND(ip, v[1], v[2]);
                                                    std::string rIP = inet_ntoa(client_addr.sin_addr);
                                                    bool nosend = false;
                                                     for(int i=0;i<max_clients;i++)
                                                    {
                                                        if((list[i].ipAddr).compare(v[1])==0){
                                                        //printf("%s \n",(list[i].ipAddr).c_str());
                                                            for (int j = 0; j < 4; j++)   
                                                            {   
                                                                //printf("%s \n",(list[i].blockS.blockClientAddr[j]).c_str());
                                                                if((list[i].blockS.blockClientAddr[j]).compare(rIP)==0)  
                                                                {
                                                                nosend = true;
                                                                } 
                                                            } 
                                                            break;
                                                        }
                                                        
                                                    }

                                                    if(!nosend){//
                                                    int ipsock;
                                                    for(int i=0;i<max_clients;i++)
                                                    {
                                                        if((list[i].ipAddr).compare(v[1])==0){
                                                            ipsock= list[i].sock;
                                                            //printf(" %s\n",list[i].ipAddr.c_str());
                                                            list[i].msgRec++;
                                                            break;
                                                        }
                                                        
                                                    }
                                                    std::string rIP = inet_ntoa(client_addr.sin_addr);
                                                    for(int i=0;i<max_clients;i++)
                                                    {
                                                        if((list[i].ipAddr).compare(rIP)==0){
                                                        list[i].msgSent++;    
                                                        }  
                                                    }

                                                    char s1[s.size()+1];//========
                                                    char s2[(v[2]).size()+1];
                                                    strcpy(s1,s.c_str());
                                                    strcpy(s2,(v[2]).c_str());
                                                    char* p;
                                                    p = strstr(s1,s2);
                                                    //printf("p=%s\n",p);
                                                    //trans char* to string
                                                    std::string msgP="";
                                                    for(int i=0;i<sizeof(p);i++){
                                                        msgP=msgP+p[i];
                                                    }


                                                    std::string messageR="RELAYED#"+v[1]+"#"+rIP+"#"+v[2];  //=========                                         
                                                    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                                                    memset(buffer, '\0', BUFFER_SIZE);
                                                    strcpy(buffer, messageR.c_str());
                                                    if(send(ipsock, buffer, strlen(buffer), 0) == strlen(buffer))
                                                    {
                                                    cse4589_print_and_log("[%s:SUCCESS]\n","RELAYED");
                                                    cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",rIP.c_str(),v[1].c_str(),v[2].c_str());//====
                                                    cse4589_print_and_log("[%s:END]\n","RELAYED");
                                                    
                                                    }
                                                    else{
                                                        cse4589_print_and_log("[%s:SUCCESS]\n","RELAYED");
                                                        cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",rIP.c_str(),v[1].c_str(),v[2].c_str());//====
                                                        cse4589_print_and_log("[%s:END]\n","RELAYED");
                                                    }
                                                    } 
                                                fflush(stdout);
                                                
                                                free(buffer);
                                                    
                                                } else if (v.size() >= 2 && v[0] == "BROADCAST") {
                                                    std::string rIP = inet_ntoa(client_addr.sin_addr);
                                                    cse4589_print_and_log("[%s:SUCCESS]\n","RELAYED");
                                                    cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",rIP.c_str(),"255.255.255.255", v[1].c_str());
                                                    cse4589_print_and_log("[%s:END]\n","RELAYED");
                                                    for(int i=0;i<max_clients;i++)
                                                    { std::string rIP = inet_ntoa(client_addr.sin_addr);
                                                       
                                                     bool nosend = false;
                                                     for(int b=0;b<max_clients;b++)
                                                    {
                                                        if((list[b].ipAddr).compare(list[i].ipAddr)==0){
                                                        printf("%s \n",(list[i].ipAddr).c_str());
                                                            for (int j = 0; j < 4; j++)
                                                            {
                                                                //printf("%s \n",(list[i].blockS.blockClientAddr[j]).c_str());
                                                            if((list[b].blockS.blockClientAddr[j]).compare(rIP)==0)
                                                            {
                                                                nosend = true;
                                                            }
                                                            }
                                                            break;
                                                        }
                                                             
                                                    }
                                                      if(!nosend) {

                                                       if(!(list[i].ipAddr).compare(rIP)==0){
                                                     std::string constIP="255.255.255.255";       
                                                     std::string messageR="RELAYED#"+list[i].ipAddr+"#"+rIP+"#"+v[1]; 
                                                    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                                                    memset(buffer, '\0', BUFFER_SIZE);
                                                    strcpy(buffer, messageR.c_str());
                                                    if(send(list[i].sock, buffer, strlen(buffer), 0) == strlen(buffer))
                                                      
                                                     fflush(stdout);
                                                     free(buffer);
                                                   }
                                                  }
                                                   } 
                                                } else if (v.size() == 2 && v[0] == "BLOCK") {
                                                       std::string rIP = inet_ntoa(client_addr.sin_addr);

                                                    for(int i=0;i<max_clients;i++)
                                                    {
                                                        if((list[i].ipAddr).compare(rIP)==0){
                                    
                                                            
                                                            for (int j = 0; j < 4; j++)   
                                                            {   
                                                            if((list[i].blockS.blockClientAddr[j]).compare("Empty")==0)  
                                                            {
                                                                list[i].blockS.blockClientAddr[j] = v[1];
                                                                printf("%s \n",list[i].blockS.blockClientAddr[j].c_str());
                                                                break;
                                                            } 
                                                            } 
                                                            break;
                                                        }
                                                        
                                                    }
                                                    
                                                        
                                                } else if (v.size() == 2 && v[0] == "UNBLOCK") {
                                                std::string rIP = inet_ntoa(client_addr.sin_addr);                                       
                                                    for(int i=0;i<max_clients;i++)
                                                    {
                                                        if((list[i].ipAddr).compare(rIP)==0){
                                                            
                                                            
                                                            for (int j = 0; j < 4; j++)   
                                                            {   
                                                            if((list[i].blockS.blockClientAddr[j]).compare(v[1])==0) 
                                                            {
                                                                list[i].blockS.blockClientAddr[j] = "Empty";
                                                                break;
                                                            } 
                                                            } 
                                                            break;
                                                        }
                                                        
                                                    }   
                                                } else if (v.size() == 1 && v[0] == "LOGOUT") {
                                                
                                                  getpeername(sd , (struct sockaddr*)&client_addr , (socklen_t*)&caddr_len);
                                                  //printf("LOGOUT , ip %s , port %d \n" ,ntohs(client_addr.sin_port));
                                                   printf("LOGOUT , ip %s , port %d \n" ,  
                                                        inet_ntoa(client_addr.sin_addr) , ntohs(client_addr.sin_port));
                                                        FD_CLR(sock_index, &master_list);
                                                        close(sd);
                                                        client_socket[i] = 0;
                                                        for(int i=0;i<max_clients;i++){
                                                            if(list[i].sock==sd){
                                                                list[i].logStatus=false;
                                                            }
                                                        }

                                                } else if (v.size() == 1 && v[0] == "EXIT") {

                                                    clients--;
                                                    
                                                } else if (v.size() == 2 && v[0] == "CLIENTPORT") {
                                                   for(int i=0;i<max_clients;i++) {

                                                             if(list[i].sock==sd){
                                                             list[i].portNo = v[1];
                                                             break;
                                                         }
                                                     }

                              
                    //Reassign list index in increasing order based on port number.
                    int arr[4];
                    for (int i = 0; i < clients; i++) {

                      arr[i] = atoi(list[i].portNo.c_str());
                    }
                                                  std::string listConcat;
                                                  int size = sizeof(list)/sizeof(list[0]);
                                                  printf("%d \n",size);
                                                  for(int i=0;i<max_clients;i++)
                                                    {
                                                      if((list[i].ipAddr.compare("")!=0) && list[i].logStatus==true){
                                                         listConcat.append(list[i].ipAddr + "$" + list[i].portNo + "#");
                                                        }
                                                    }
                                                    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                                                    memset(buffer, '\0', BUFFER_SIZE);
                                                    strcpy(buffer, listConcat.c_str());
                                                    if(send(sd, buffer, strlen(buffer), 0) == strlen(buffer))
                                                        printf("Done!\n");
                      }
                    

                  }
                }

              }
            }
          }
        }
      }
    }
  }
char* ipfunction_Server(){
        char hostbuffer[128];
        char *IPbuffer;
        struct hostent *host_entry;

        gethostname(hostbuffer,sizeof(hostbuffer));

        host_entry=gethostbyname(hostbuffer);

        IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
        return IPbuffer;
 }