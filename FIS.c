//File Information Server
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

int count;
char File_name[100][100], File_IP[100][100];

void Update_File_list();

char* Get_IP(char* FName);


int main ()
{
  Update_File_list();

  int Servport=11100, serverSock;
  struct sockaddr_in ServAddr;
  struct sockaddr_in ClientAddr;
  char buffer[1024];
  int i;
  serverSock = socket(PF_INET, SOCK_DGRAM, 0);

  if ((serverSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
  {
    printf("Client Socket Allocation failed.... Request could not be fulfilled\n");
    return -1;
  }

  bzero(&ServAddr, sizeof(ServAddr));

  ServAddr.sin_family = AF_INET;
  ServAddr.sin_port = htons(Servport);
  ServAddr.sin_addr.s_addr = INADDR_ANY;

  if ( bind(serverSock, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) != 0 )
  {
    printf("Client Socket Binding failed....\n");
    return -1;
  }

  char msg[1024], msgout[1024];

  while(1)
  {
    printf("INTO WHILE LOOP\n");
    int bytes, addr_len=sizeof(ClientAddr);
	
    if((bytes = recvfrom(serverSock, msg, sizeof(msg), 0, (struct sockaddr*)&ClientAddr, &addr_len))<0)
    {
      printf("Received Failed\n");
    }
//printf("SIZEOF MSG: %d\n", sizeof(msg));
    msg[bytes] = '\0';   
//printf("Bytes: %d\n", bytes);   
 //printf("Message= %s\n",msg);

    char temp[3];
    temp[0]=msg[0];
    temp[1]=msg[1];
    temp[2]=msg[2];

    if(strcmp(temp,"UPD")==0)
    {
      Update_File_list();
      strcpy(msgout,"");
      for(i=0; i<count; i++)
      {
        strcat(msgout,File_name[i]);
        strcat(msgout,":");
        strcat(msgout,File_IP[i]);
        strcat(msgout,"\n");
      }

      int StringLen=sizeof(msgout);
      printf("String Length=%d MSG=%s",StringLen,msgout);
      if (sendto(serverSock,msgout,StringLen, 0,(struct sockaddr *) &ClientAddr, sizeof(ClientAddr))!= StringLen)
      {
        printf("Send failed...\n");
        continue;
      }
    }
    else
      if(strcmp(temp,"GET")==0)
      {
        char input[1024];
//	printf("STRLEN: %d\n", strlen(msg));
        for(i=4; i<strlen(msg);i++)
          input[i-4]=msg[i];
	input[i - 4] = '\0';
        strcpy(msgout,Get_IP(input));
        int StringLen=sizeof(msgout);

        if (sendto(serverSock,msgout,StringLen, 0,(struct sockaddr *) &ClientAddr, sizeof(ClientAddr))!= StringLen)
        {
          printf("Send failed...\n");
          continue;;
        }
      }
  }
}

void Update_File_list()
{
  count=0;
  FILE* fr = fopen ("Filelist", "r");
  char pch[100];
  char* str;
 
  while(fgets(pch, 80, fr) != NULL)
  {
    char *token;
    token = strtok(pch,":");
    strcpy(File_name[count],token);
    token = strtok(NULL,":'\n'");     
    strcpy(File_IP[count],token);
    count++;
  }

  fclose(fr);
  printf("FILE LIST UPDATED\n"); 
  return ;
}

char* Get_IP(char* FName)
{
  int i;
//printf("%s\n", FName);
  for(i=0; i<count; i++)
  {
//	printf("%s %s\n", File_name[i], File_IP[i]); 
   if(strcmp(FName, File_name[i])==0)
      return File_IP[i];
  }

  return "String_Not_Found";

}
