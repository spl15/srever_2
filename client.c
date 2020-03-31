
// Client side C/C++ program to demonstrate Socket programming with a string
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h> 
#include <pthread.h>
#define PORT 60019 
#define LENGTH 101



void* sendIt();
void* receiveIt();
void prompt();

//global variable exit flag
int exitFlag = 0;
int sock = 0;

int main(int argc, char *argv[]) 
{
  //  int n; 
    struct sockaddr_in serv_addr; 
    char buffer[256] = {0}; 
    char name[30];
    pthread_t tidR;
    pthread_t tidS;
   // struct timeval tv;
   // tv.tv_sec = 2;
  // tv.tv_usec = 0;

    if(argc != 2)
    {
        printf("Two arguments are required!");
        exit(1);
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
   //using the loopback address for testing
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
  //char* message;
  //int p;
  
  printf("Welcome to the chatroom!\nPlease enter your name: ");
  fgets(name,30,stdin);
  
  
  //gets(message);
  pthread_create(&tidR,NULL,receiveIt,NULL);
  pthread_create(&tidS,NULL,sendIt,NULL);
  pthread_join(tidS, NULL);
  pthread_join(tidR, NULL);
  

  while(1)
  {
    if(exitFlag == 1)
    {
      break;
    }
  }
   // int test;
   // while((test = select(sock+1, &sock, NULL, NULL, &tv)) > 0)
   // {
     ////recv( sock , buffer, sizeof(buffer),0); //recent 
   // }
   // printf("%d", test);
    ////printf("%s\n",buffer ); //recent
    close(sock);
    return 0; 
} 

void* sendIt()
{
	char message[LENGTH] = {0};
  
  while(1)
  {
    prompt();
    while(fgets(message,LENGTH,stdin) != NULL)
    {
      if(strlen(message) == 0)
      {
        prompt();
      }
      else
      {
        break;
      }
    }

    send(sock,message,LENGTH,0);
    if(strcmp(message,"quit") == 0)
    {
      break;
    }
  }
  exitFlag = 1;
	pthread_exit(0);
}
void* receiveIt()
{
  char message[LENGTH] = {0};

  while(1)
  {
    int rec = recv(sock,message,LENGTH,0);
    if(rec > 0)
    {
      printf("\n%s", message);
      prompt();
    }
    else if(rec == 0)
    {
      break;
    }
  }
  pthread_exit(0);
}
void prompt()
{
  printf("$$$");
  fflush(stdout);
}