//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux  
#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#define PORT 60019     
struct myClient
{
    char name[20];
    int sockfd;
};
 
     
int main(int argc , char *argv[])   
{   
     int opt = 1;
    int masterSocket , addrlen , newSocket, activity , valread , sd, copy;   
    int maxClients = 20;
    int clientSocket[maxClients];   
    struct sockaddr_in address;
  
         
    char buffer[1025];  
         
    //set of socket descriptors  
    fd_set readfds;   
     
    //initialise all client_socket[] to 0 so not checked  
    for (int i = 0; i < maxClients; i++)   
    {   
        clientSocket[i] = 0;   
    }   
         
    //create a master socket  
    if( (masterSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   

    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 60019 
    if (bind(masterSocket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port# %d \n", PORT);   
         
    // max 5 pending connections for the master socket  
    if (listen(masterSocket, 5) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    printf("Waiting for connections...\n");   
         
    for(;;)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(masterSocket, &readfds);   
        copy = masterSocket;   
             
        //add child sockets to set  
        for (int i = 0 ; i < maxClients ; i++)   
        {   
            //socket descriptor  
            sd = clientSocket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > copy)   
                copy = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( copy + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(masterSocket, &readfds))   
        {   
            if ((newSocket = accept(masterSocket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , ip address: %s\n", inet_ntoa(address.sin_addr));  
           
                 
            //add new socket to array of sockets  
            for (int i = 0; i < maxClients; i++)   
            {   
                //if position is empty  
                if( clientSocket[i] == 0 )   
                {   
                    clientSocket[i] = newSocket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (int i = 0; i < maxClients; i++)   
        {   
            sd = clientSocket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address , 
                        (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s \n" ,inet_ntoa(address.sin_addr)); 
           
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    clientSocket[i] = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';
                    for(int j = 0; clientSocket[j] != 0;j++)
                    {   if(clientSocket[j] != sd)
                        {
                            send(clientSocket[j] , buffer , strlen(buffer) , 0 );//send(sd , buffer , strlen(buffer) , 0 );
                        }
                    }   
                }   
            }   
        }   
    }   
         
    return 0;   
}   