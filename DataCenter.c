#include "Core.h"


 int serverSocketFD,multiCastSocketFD,clientLength, client_sock[MAX_CLIENTS];
 struct sockaddr_in serverAddress,clientAddress,multiCastAddress;

 /*Initalizes the datacenter "Server Socket"(listening socket) and all the client sockets */
 void initializeSockets()
 {

   int flag=-1, on=1, i;

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

   //Server Address
   serverAddress.sin_family=DOMAIN;
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
 }


 /* We can listen to both clients and replicated writes here. What do you think?
    In the slides he says when a datacenter needs to replicate writes it acts as a client
    When the datacenter needs to send a replicated write it can fork a new thread */
 void *listening()
 {

 	 int flag=-1, i, max_sd, sd, activity, addrlen, new_socket, response, n;
	 fd_set readfds;
	 char buffer[1024] = { 0 };
	 addrlen = sizeof(serverAddress);
	
	 //Select implementation for multiple requests handling
	 while (1)
	 {
	   FD_ZERO(&readfds);
		 FD_SET(serverSocketFD, &readfds);

		 for (i = 0; i < MAX_CLIENTS; i++)
		 {
		 	 //Socket descriptor  
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

		 //Incoming connection  
		 if (FD_ISSET(serverSocketFD, &readfds))
		 {
		 	 if ((new_socket = accept(serverSocketFD,(struct sockaddr *)&clientAddress, &addrlen)) < 0)
			 {
			  	printf("Accept error\n");
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
          if(DEBUG)
					 printf("Message received: %s \n", buffer);
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

 void writeToDataStore()
 {
  
 }

 /*TODO -  We can switch to epoll  if we get the time. What do you think? */
 int main()
 {
   int flag=0;
   pthread_t t1;

   /*Q: Do you want to listen on another thread? What will the main thread do then?
        We only send the replicated write when we recieve a write operation from the client. 
   */
   //int ir1 = pthread_create(&t1, NULL, listening, NULL);

   initializeSockets();

   

   return 1;
 }