/* Client Code */
 
 #include "Core.h"
 

 char clientID;
 struct sockaddr_in serverAddress;

 int initializeSocket(int port)
 {
   int socketFD=socket(DOMAIN,SOCKET_TYPE,PROTOCOL);
   assert(socketFD!=0);
   memset(&serverAddress, '\0', sizeof(serverAddress));
   serverAddress.sin_family=DOMAIN;
   serverAddress.sin_port=htons(port);
   inet_aton("127.0.0.1", (struct in_addr *)&serverAddress.sin_addr.s_addr);
   return socketFD;
 }

 int main(int argc,char **argv)
 {
   int socketFD=-1,flag=-1;	
   srand(time(0));
   clientID=(char)((rand()%MAX_CLIENTS)+65);
   // printf("Client %c Created",clientID);
   if(argc<2)
    fprintf( stderr, "Syntax: Client PORT\n");
   else  
   { 
    socketFD=initializeSocket(atoi(argv[1]));
    assert(socketFD!=0);
    flag=connect(socketFD,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
    
    assert(flag==0);
    printf("Client %c Connected successfully",clientID);

    /*TODO present menu for operations and send them to the data center */

   } 
   return 0; 
 }