#include "Core.h"
#include "DependencyUtilities.h"

int serverSocketFD, multiCastSocketFD, clientLength, client_sock[MAX_CLIENTS];
struct sockaddr_in serverAddress, clientAddress, castToAddress, multiCastAddress;

/*DATA CENTER SPECIFIC INFORMATION*/
int myID;
int myLamportClockTime;
PendingQueue pendingQueue;

/*Initalizes the datacenter "Server Socket"(listening socket) and all the client sockets */
void initializeSockets()
{

	int flag = -1, on = 1, i;

	//initializing all client sockets to 0
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		client_sock[i] = 0;
	}

	//listening for connections on this socket
	serverSocketFD = socket(DOMAIN, SOCKET_TYPE, PROTOCOL);
	assert(serverSocketFD != 0);
	if (DEBUG) printf("Data Center Socket Created\n");

	//socket options - Reusable address 
	flag = setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	assert(flag == 0);
	//Reusable port
	flag = setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	assert(flag == 0);

	//Server Address
	serverAddress.sin_family = DOMAIN;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	//binding socket
	flag = bind(serverSocketFD, (const struct sockaddr *)&serverAddress, sizeof(serverAddress));
	assert(flag == 0);

	if (DEBUG) printf("Data Center Socket Bound\n");

	//listening with max 5 pending connections
	flag = listen(serverSocketFD, 5);
	assert(flag == 0);

	if (DEBUG) printf("Data Center Socket Listening\n");
}

void initMulticastSocket() {
	//multicastSocketFD creation
	int flag,on=1;
	multiCastSocketFD = socket(DOMAIN, SOCKET_TYPE, PROTOCOL);
	//socket options - Reusable address 
	flag = setsockopt(multiCastSocketFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	assert(flag == 0);
	//Reusable port
	flag = setsockopt(multiCastSocketFD, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	assert(flag == 0);
	multiCastAddress.sin_family = DOMAIN;
	multiCastAddress.sin_port = htons(CASTPORT); /*Port that is used to for multicasting*/
	multiCastAddress.sin_addr.s_addr = INADDR_ANY;

	//binding socket with this dataCenter's multi cast address
	flag = bind(multiCastSocketFD, (const struct sockaddr *)&multiCastAddress, sizeof(multiCastAddress));
	assert(flag >= 0);
}


void readFromDataStore(char* key)
{
    char* data=malloc(sizeof(char)*20);
    readFromDB(key,(void*) data);
}

void writeToDataStore(char* key,int clientID,char* data)
{
    /*DO NULL CHECK*/
    commit(key,clientID,myID,data);
}

/*Create a new thread which acts a client socket and sends the repliated write*/
int sendReplicatedWrite(char *address, int port, char *message)
{
	/*just send the message to the address and port comb*/
    int flag;	
	//create castToAddress from address and port
	initMulticastSocket();
	castToAddress.sin_family = AF_INET;
	inet_pton(AF_INET, address, &(castToAddress.sin_addr));
	castToAddress.sin_port = htons(port);

	//connecting to the desired castToAddress
	flag = connect(multiCastSocketFD, (struct sockaddr *)&castToAddress, sizeof(castToAddress));
	assert(flag == 0);
	
	send(multiCastSocketFD, message, MAX_MESSAGE_SIZE, 0);
	close(multiCastSocketFD);
	
	/*TODO: check for response and then return*/
	return 1;
}

int messageHandler(char* request, char* clientIPAddress, int port, int socket)
{
	int offset = 0, keyLength, dataLength;
    int dataCenterID=-1;
    signed int clientID=-1;
	char* key, *data;
    Dependency dependency;
    /*READ REQUEST*/
	if (strncmp(request, READ_REQUEST, MESSAGE_HEADER_LENGTH) == 0)
	{
		offset += MESSAGE_HEADER_LENGTH;
        
        memcpy(&clientID, request + offset, sizeof(int));
        offset += sizeof(int);
        printf("Client  ID %d\n", clientID);

		memcpy(&keyLength, request + offset, sizeof(int));
		offset += sizeof(int);

		printf("Key length %d\n", keyLength);

		key = (char*)malloc(sizeof(char)*keyLength);
		memcpy(key, request + offset, keyLength);
		printf("Key: %s\n", key);
		readFromDataStore(key);
        
        /*Check for the appropriate data center id*/
        dataCenterID=readIDFromDB(key);
        if(dataCenterID==-1)
            dataCenterID=myID;
        /*Create new dependency*/
        dependency.key=key;
        dependency.lamportClockTime=myLamportClockTime;
        dependency.dataCenterID=dataCenterID;
        appendClientDependencyList(clientID,dependency);
	}
    /*WRITE REQUEST*/
	else if (strncmp(request, WRITE_REQUEST, MESSAGE_HEADER_LENGTH) == 0)
	{
		offset += MESSAGE_HEADER_LENGTH;

        memcpy(&clientID, request + offset, sizeof(int));
        offset += sizeof(int);
        printf("Client  ID %d\n", clientID);

		memcpy(&keyLength, request + offset, sizeof(int));
		offset += sizeof(int);
		printf("Key length %d\n", keyLength);

		key = (char*)malloc(sizeof(char)*keyLength);
		memcpy(key, request + offset, keyLength);
		offset += keyLength;

		memcpy(&dataLength, request + offset, sizeof(int));
		offset += sizeof(int);
		printf("Key: %s\n", key);


		data = (char*)malloc(sizeof(char)*dataLength);
		memcpy(data, request + offset, dataLength);
		printf("Data: %s\n", data);

		writeToDataStore(key,clientID,data);
		/* Write committed to DB, now send REP_WRTITE to other data centers */
		memcpy(request, REP_WRITE, MESSAGE_HEADER_LENGTH); /*TODO: check if this works*/
		int resp = sendReplicatedWrite(ADDRESS, PORT_D1, request);
		assert(resp == 1);
		resp = sendReplicatedWrite(ADDRESS, PORT_D2, request);
		assert(resp == 1);
	}
    /*REPLICATED WRITE REQUEST*/
	else if (strncmp(request, REP_WRITE, MESSAGE_HEADER_LENGTH) == 0)
	{
		offset += MESSAGE_HEADER_LENGTH;

		memcpy(&keyLength, request + offset, sizeof(int));
		offset += sizeof(int);
		printf("Key length %d\n", keyLength);

		key = (char*)malloc(sizeof(char)*keyLength);
		memcpy(key, request + offset, keyLength);
		offset += keyLength;

		memcpy(&dataLength, request + offset, sizeof(int));
		offset += sizeof(int);
		printf("Key: %s\n", key);

		data = (char*)malloc(sizeof(char)*dataLength);
		memcpy(data, request + offset, dataLength);
		printf("Data: %s\n", data);

		/*TODO: check for dependencies and commit the write if no dependecies*/
        
        /* -1 to indicate that this was a replicated write and not a client initiated write*/
		writeToDataStore(key,-1,data);
	}
	return 0;
}

/* We can listen to both clients and replicated writes here. What do you think?
   In the slides he says when a datacenter needs to replicate writes it acts as a client
   When the datacenter needs to send a replicated write it can fork a new thread */
void listening()
{

	int flag = -1, i, max_sd, sd, activity, new_socket, response, n;
	fd_set readfds;
	char buffer[MAX_MESSAGE_SIZE] = { 0 };
	socklen_t addrlen = sizeof(serverAddress);

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
			if ((new_socket = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &addrlen)) < 0)
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
				n = read(sd, buffer, MAX_MESSAGE_SIZE);
				if (n == 0)
				{
					//peer disconnected  
					getpeername(sd, (struct sockaddr *)&clientAddress, &addrlen);
					if (DEBUG) printf("Host disconnected , ip %s , port %d \n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

					//Close the socket and mark as 0 in list for reuse  
					close(sd);
					client_sock[i] = 0;
				}

				//process the message that came in  
				else if (strcmp(buffer, "") != 0)
				{
					//terminate string with NULL  
					//buffer[n] = '\0';
					//handle the message
					if (DEBUG)
						printf("Message received: %s \n", buffer);
					response = messageHandler(buffer, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), client_sock[i]);
				}
			}
		}

	}

}


/*TODO -  We can switch to epoll  if we get the time. What do you think? */
int main()
{
	int flag = 0;
    flag=initDB();
    assert(flag == 0);
	initializeSockets();
	listening();
	return 1;
}
