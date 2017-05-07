// Peer Client
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

void update_FIS();
void get_file();
void Connect_to_Server(char * file_name, char * buffer);

int main()
{
	int input;
	// update_FIS();
	while(1)
	{
		printf("Choose the following options\n");
		printf("1. Update the file information server's list and view it\n");
		printf("2. Get a file\n");
		printf("3. Exit\n");
		scanf("%d", &input);
		if(input==1)
		{
			update_FIS();
		}
		else 
			if(input==2)
			{
				get_file();
			}
			else
			{
				break;
			}
	}
	printf("CLIENT EXITING\n");
	return 0;
}

void update_FIS()
{
	int clientSock, ClientPort=11200, ServPort=11100;
	struct sockaddr_in ClientAddr, ServAddr, from;

	if ((clientSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf("Client Socket Allocation failed.... Request could not be fulfilled\n");
		return;
	}
	ClientAddr.sin_family=AF_INET;
	ClientAddr.sin_addr.s_addr= htonl(INADDR_ANY); 
	ClientAddr.sin_port-htons(ClientPort);
	
	if(bind(clientSock,(struct sockaddr *)&ClientAddr,sizeof(ClientAddr))<0)
	{
		printf("Client Socket Binding failed....\n");
		return;
	}

	ServAddr.sin_family=AF_INET;
	ServAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	// ServAddr.sin_addr.s_addr=inet_addr("10.2.0.2");
	ServAddr.sin_port = htons(ServPort);

	printf("HELLO\n");
	char msg[1024];
	strcpy(msg,"UPD");

	int StringLen=strlen(msg);

	if (sendto( clientSock,msg,StringLen, 0,(struct sockaddr *) &ServAddr, sizeof(ServAddr))!= StringLen)
	{
		printf("Send failed...\n");
		return;
	}

	printf("SENDTO\n");

	socklen_t* length;

	if(recvfrom(clientSock,msg,4096,0,NULL,NULL)<0)
	{
		printf("Receive Failed....\n");
		return;
	}

	printf("%s",msg);
	close(clientSock);
}

void get_file()
{
	char dest[100];
	int clientSocket, portNum, nBytes;
	char msg[1024];
	char buffer[1024], get[10];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	int fis_port = 11100;

	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	if(clientSocket == -1){
	perror("Error: ") ;
	return;
	}

	serverAddr.sin_family = AF_INET;      
	serverAddr.sin_port = htons(fis_port);                              
	serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	// serverAddr.sin_addr.s_addr = inet_addr("10.2.0.2");          
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;

	strcpy(get, "GET:");

	printf("Enter filename: ");
	scanf("%s",buffer);
	char file_name[100];
	strcpy(file_name,buffer);
	sprintf(msg, "GET:%s", buffer);
	printf("You typed: %s\n",msg);

	nBytes = strlen(msg) + 1;

	/*Send message to server*/
	sendto(clientSocket,msg,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);

	printf("Sent the msg\n");
	/*Receive message from server*/
	nBytes = recvfrom(clientSocket,buffer,1024,0,NULL, NULL);

	printf("Received from server: %s\n",buffer);
	printf("\n");
	close(clientSocket);
	if(strcmp("String_Not_Found", buffer) == 0) printf("No such file exists in the file table\n");
	else Connect_to_Server(file_name, buffer);
}
// }

	// int server_port=11101;
	// clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	// printf("clientSocket: %d\n", clientSocket);
 //   	serverAddr.sin_family = AF_INET;
 //  	serverAddr.sin_port = htons(7891);
 //  	serverAddr.sin_addr.s_addr = inet_addr(buffer);
 //  	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
	// addr_size = sizeof serverAddr;
	// printf("Waiting for Connect\n");
 //  	connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

	// strcpy(buffer,file_name);  
 //  	send(clientSocket,buffer,strlen(buffer),0);

	// printf("Enter the name of the destination file\n");
	// scanf("%s",dest);

	// FILE *f=fopen(dest,"w");
 //  	int t;
	// while((t=recv(clientSocket, buffer,1024, MSG_WAITALL))>0) 
	// {
 //    	for(int i=0;i<t;i++)
 //    	  putc(buffer[i],f);
	// }
 //  	fclose(f);
 //  	close(clientSocket);
    

void Connect_to_Server(char * file_name, char * ip)
{
	int clientSocket;
  	char buffer[1024];
  	char dest[100];
  	struct sockaddr_in serverAddr;
  	socklen_t addr_size;
  	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7891);
	// serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_addr.s_addr = inet_addr("10.145.139.40");
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	addr_size = sizeof serverAddr;
	printf("HELLO\n");
	connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
	printf("CONNECTED\n");

	send(clientSocket,file_name,strlen(file_name),0);

	printf("Enter the name of the destination file\n");
	scanf("%s",dest);

	FILE *f=fopen(dest,"w");
  	int t,i;
	while((t=recv(clientSocket, buffer,1024, MSG_WAITALL))>0) 
	{
    	for(i=0;i<t;i++)
    	  putc(buffer[i],f);
	}
  	fclose(f);
  	close(clientSocket);

	// return 0;
}
