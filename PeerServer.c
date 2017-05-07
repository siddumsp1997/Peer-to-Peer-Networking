// Peer Server
#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#define TRUE   1 
#define FALSE  0 
#define PORT 11101 
    
int main(int argc , char *argv[])  {
//   int welcomeSocket, newSocket;
//   char buffer[1024];
//   struct sockaddr_in serverAddr;
//   struct sockaddr_storage serverStorage;
//   socklen_t addr_size;
  

//   welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
//   serverAddr.sin_family = AF_INET;
//   serverAddr.sin_port = htons(7891);
//   serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
//   memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

//   bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
//   if(listen(welcomeSocket,5)==0)
//     printf("Listening\n");
//   else
//     printf("Error\n");
//   addr_size = sizeof serverStorage;
  
//   newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
  
//   strcpy(buffer,"Hello World\n");
//   send(newSocket,buffer,13,0);

//   return 0;
// }
    int opt = TRUE;  
    int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;        
    char buffer[1025];  //data buffer of 1K  
    fd_set readfds,writefds;
    bool client_status[30];
    FILE * client_file[30];
    struct timeval tv;

    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;
        client_file[i] = NULL;  
        client_status[i] = false;
    }  
        
    if( (master_socket = socket(PF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
    
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( 7891 );  
        
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
        
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    //printf("Waiting for connections ...\n");  
    // new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen) ;   
  
    while(1)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
        FD_ZERO(&writefds);  
    
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
            
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                
            //if valid socket descriptor then add to read list 
            if(sd > 0){  
                if(client_status[i])FD_SET( sd , &writefds);
                else FD_SET( sd, &readfds);  
            }
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
    
        printf("l1\n");

        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        activity = select( max_sd + 1 , &readfds , &writefds , NULL , &tv);  
      
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
        printf("Activity: %d\n", activity);
        printf("l2...\n");
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
            
            //inform user of socket number - used in send and receive commands 
            // printf("New connection , socket fd is %d , ip is : %s , port : %d 
            //       \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
            //       (address.sin_port));  
          
            //send new connection greeting message 
            // if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            // {  
            //     perror("send");  
            // }  
                
            // printf("Welcome message sent successfully\n");  
                
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    client_status[i] = false;
                    printf("Adding to list of sockets as %d\n" , i);  
                        
                    break;  
                }  
            }  
        }  
            
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                
            if (FD_ISSET( sd , &readfds))  
            {  
              client_status[i] = true;
              if ((valread = read( sd , buffer, 1024)) > 0){
                //printf("BUFFER= %s\n",buffer);
                buffer[valread] = '\0';
                client_file[i] = fopen(buffer, "rb");  
                printf("BUFFER= %s\n",buffer);
                int n_bytes = fread(buffer,1, 1024, client_file[i]);
                int s_bytes = send(sd, buffer, n_bytes, 0);

                if(n_bytes < 1024){
                // Close FileDescriptor
                  close( sd );  
                  client_socket[i] = 0; 
                  client_status[i] = false;
                  fclose(client_file[i]);
                  client_file[i] = NULL;
                }
              }
              else{
                perror("Error: ");
              }
            }
            else if(FD_ISSET(sd, &writefds)){
              int n_bytes = fread(buffer,1, 1024, client_file[i]);
              int s_bytes = send(sd, buffer, n_bytes, 0);              
            
              if(n_bytes < 1024){
                // Close FileDescriptor
                close( sd );  
                client_socket[i] = 0; 
                client_status[i] = false;
                fclose(client_file[i]);
                client_file[i] = NULL;
              }
            }  
        }  
    }  
        
    return 0;  
}  
