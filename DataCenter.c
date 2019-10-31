#include "Core.h"


 int serverSocketFD,multiCastSocketFD,clientLength;
 struct sockaddr_in serverAddress,clientAddress,multiCastAddress;

 void initialize()
 {

 	int flag=-1,on=1;
    serverSocketFD=socket(DOMAIN,SOCKET_TYPE,PROTOCOL);
    assert(serverSocketFD!=0);
	if(DEBUG)
     printf("Data Center Socket Created\n"); 
		
    memset(&serverAddress,'\0',sizeof(serverAddress));
    serverAddress.sin_family=DOMAIN;
	
	inet_aton("127.0.0.1", &(serverAddress.sin_addr.s_addr));
    serverAddress.sin_port=htons(PORT);
	
    flag=setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));	
	assert(flag>=0);
   
	flag=bind(serverSocketFD,(const struct sockaddr *)&serverAddress,sizeof(serverAddress));
	assert(flag==0);

	if(DEBUG)
     printf("Data Center Socket Bound\n"); 

    flag=listen(serverSocketFD,5);
	assert(flag==0);

	if(DEBUG)
     printf("Data Center Socket Listening\n"); 

 }

/*Create a new thread which acts a client socket and sends the repliated write*/
 void sendReplicatedWrite()
 {
   
 }

 void readFromDataStore()
 {

 }  

/*TODO -  use epoll for the serversocket(main) thread */
 int main()
 {
   int flag=0;
   initialize();
   while(1)
   {
   	  flag=accept(serverSocketFD,( struct sockaddr *) & clientAddress, &clientLength); 
   	  assert(flag!=0);
      printf("Client Connected \n");
   }   

   return 1;
 }