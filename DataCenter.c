#include "Core.h"


 int serverSocketFD,multiCastSocketFD,clientLength, client_sock[MAX_CLIENTS];
 struct sockaddr_in serverAddress,clientAddress,multiCastAddress;

 void *listening()
 {

 	int flag=-1, on=1, i, max_sd, sd, activity, addrlen, new_socket, response, n;
	fd_set readfds;
	char buffer[1024] = { 0 };
	addrlen = sizeof(serverAddress);
	//initializing all client sockets to 0
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		client_sock[i] = 0;
	}
	//listening for connections on this socket
    serverSocketFD=socket(DOMAIN,SOCKET_TYPE,PROTOCOL);
    assert(serverSocketFD!=0);
	if(DEBUG) printf("Data Center Socket Created\n"); 
	
	//socket options
	flag = setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int));
	assert(flag >= 0);

    //memset(&serverAddress,'\0',sizeof(serverAddress));
    serverAddress.sin_family=DOMAIN;
	
	//inet_aton("127.0.0.1", &(serverAddress.sin_addr.s_addr));
    serverAddress.sin_port=htons(PORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	   
	//binding socket
	flag=bind(serverSocketFD,(const struct sockaddr *)&serverAddress,sizeof(serverAddress));
	assert(flag==0);

	if(DEBUG) printf("Data Center Socket Bound\n"); 

	//listening with max 5 pending connections
    flag=listen(serverSocketFD,5);
	assert(flag==0);
	if(DEBUG) printf("Data Center Socket Listening\n");

	//select implementation for multiple requests handling
	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(serverSocketFD, &readfds);
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			//socket descriptor  
			sd = client_sock[i];
			//if valid socket descriptor then add to read list  
			if (sd > 0)
				FD_SET(sd, &readfds);
			//highest file descriptor number, need it for the select function  
			if (sd > max_sd)
				max_sd = sd;
		}
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR))
		{
			printf("Select error\n");
		}

		//incoming connection  
		if (FD_ISSET(serverSocketFD, &readfds))
		{
			if ((new_socket = accept(serverSocketFD,(struct sockaddr *)&clientAddress, &addrlen)) < 0)
			{
				printf("accept error");
				exit(EXIT_FAILURE);
			}

			//inform user of socket number - used in send and receive commands  
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));



			//add new socket to array of sockets  
			for (i = 0; i < MAX_CLIENTS; i++)
			{
				//if position is empty  
				if (client_sock[i] == 0)
				{
					client_sock[i] = new_socket;
					printf("Adding to list of sockets at %d\n", i);

					break;
				}
			}

		}
		//check operation on the socket 
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			sd = client_sock[i];
			if (FD_ISSET(sd, &readfds))
			{
				//Check if it was for closing
				n = read(sd, buffer, 1024);
				if (n == 0)
				{
					//peer disconnected  
					getpeername(sd, (struct sockaddr *)&clientAddress, &addrlen);
					if(DEBUG) printf("Host disconnected , ip %s , port %d \n" ,  inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));   

					//Close the socket and mark as 0 in list for reuse  
					close(sd);
					client_sock[i] = 0;
				}

				//process the message that came in  
				else
				{
					//terminate string with NULL  
					buffer[n] = '\0';
					//handle the message
					printf("%s", buffer);
					//response = Message_handler(buffer, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_sock[i]);
				}
			}
		}
	
	}

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
   pthread_t t1;

   //listen to clients and other datacenters
   int ir1 = pthread_create(&t1, NULL, listening, NULL);

   //initialize();

   /*
   while(1)
   {
   	  flag=accept(serverSocketFD,( struct sockaddr *) & clientAddress, &clientLength); 
   	  assert(flag!=0);
      printf("Client Connected \n");
   }
   */

   return 1;
 }