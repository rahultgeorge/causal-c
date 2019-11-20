#include "Core.h"
#include "DependencyUtilities.h"

int serverSocketFD, multiCastSocketFD, multiCastSocketFD2, clientLength, client_sock[MAX_CLIENTS];
struct sockaddr_in serverAddress, clientAddress, castToAddress, multiCastAddress;
int PORT,PORT_D1,PORT_D2,CASTPORT;

int connectedMultiCast=-1;


/*DATA CENTER SPECIFIC INFORMATION*/
int myID ; /*PORT*/
int myLamportClockTime = 0;


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

void initMulticastSocket(char* address,int port) {
	//multicastSocketFD creation
	int flag, on = 1, sock;
	if (port == PORT_D1) sock = multiCastSocketFD;
	else if (port == PORT_D2) sock = multiCastSocketFD2;
	sock = socket(DOMAIN, SOCKET_TYPE, PROTOCOL);
	//socket options - Reusable address 
	flag = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	assert(flag == 0);
	//Reusable port
	flag = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	assert(flag == 0);
	castToAddress.sin_family = AF_INET;
	inet_pton(AF_INET, address, &(castToAddress.sin_addr));
	castToAddress.sin_port = htons(port);

	//connecting to the desired castToAddress
	flag = connect(sock, (struct sockaddr *)&castToAddress, sizeof(castToAddress));
	assert(flag == 0);
    if(port==PORT_D1)
        multiCastSocketFD=sock;
    else if(port==PORT_D2)
    {   multiCastSocketFD2=sock;
        connectedMultiCast=1;
    }
    
}


char* readFromDataStore(char* key)
{
	char* data = malloc(sizeof(char) * 20);
	int i = readFromDB(key, (void*)data);
	//actually this should be taken care of in the readFromDB
	if (i != -1) {
		return data;
	}
	return NULL;
}

void writeToDataStore(char* key, int clientID, char* data)
{
	/*DO NULL CHECK*/
	commit(key, clientID, myID, data);
}

/*Create a new thread which acts a client socket and sends the repliated write*/
int sendReplicatedWrite(char *address, int port, char *message)
{
	/*just send the message to the address and port comb*/
	int flag;
	//create castToAddress from address and port
	if(connectedMultiCast==-1)
     initMulticastSocket(address, port);
    //printf("Port %d\n",port);
	if(port==PORT_D1)
        flag=send(multiCastSocketFD, message, MAX_MESSAGE_SIZE, 0);
	else if (port == PORT_D2)
		flag=send(multiCastSocketFD2, message, MAX_MESSAGE_SIZE, 0);
    assert(flag>0);
	//close(multiCastSocketFD);

	/*TODO: check for response and then return*/
	return 1;
}

void messageHandler(char* request, char* clientIPAddress, int port, int socket)
{
	int offset = 0, keyLength, dataLength,i,resp=-1,flag=-1,lamportClockTimeReceived=-10;
	int dataCenterID = -1;
	signed int clientID = -1;
	char* key, *data;
	char response[MAX_MESSAGE_SIZE];
	Dependency dependency;
    DependencyList replicatedDepList;
    printf("My lamport clock time : %d\n",myLamportClockTime);
	/*READ REQUEST*/
	if (strncmp(request, READ_REQUEST, MESSAGE_HEADER_LENGTH) == 0)
	{
		offset += MESSAGE_HEADER_LENGTH;
		//reading client ID
		memcpy(&clientID, request + offset, sizeof(int));
		offset += sizeof(int);
		if(DEBUG) printf("Client  ID %d\n", clientID);

		//reading key length
		memcpy(&keyLength, request + offset, sizeof(int));
		offset += sizeof(int);
		if(DEBUG) printf("Key length %d\n", keyLength);

		//reading key
		key = (char*)malloc(sizeof(char)*keyLength);
		memcpy(key, request + offset, keyLength);
		if(DEBUG) printf("Key: %s\n", key);

        printf("READ(%s)\n",key);
        // and respond to the client with the key value
		//read from DB
		char* retdata = readFromDataStore(key);
		int respLen = strlen(retdata);
		retdata[respLen] = '\0';
		if (retdata == NULL) {
			memcpy(response, retdata, respLen);
		}
		else {
			memcpy(response, retdata, respLen);
		}
		send(socket, response, strlen(response), 0);
        
		/*Check for the appropriate data center id*/
		dataCenterID = readIDFromDB(key);
		//if DC ID not found
		if (dataCenterID == -1)
			dataCenterID = myID;

		/*Create new dependency*/
		dependency.key = key;
		dependency.lamportClockTime = myLamportClockTime;
		dependency.dataCenterID = dataCenterID;
		//add this DP to the DPLISTS[clientID]
		assert(appendClientDependencyList(clientID, dependency) == 1);
	}
	/*WRITE REQUEST*/
	else if (strncmp(request, WRITE_REQUEST, MESSAGE_HEADER_LENGTH) == 0)
	{
		offset += MESSAGE_HEADER_LENGTH;
		//Reading the clientID
		memcpy(&clientID, request + offset, sizeof(int));
		offset += sizeof(int);
		if(DEBUG) printf("Client  ID %d\n", clientID);
		//Reading the key length
		memcpy(&keyLength, request + offset, sizeof(int));
		offset += sizeof(int);
		if(DEBUG) printf("Key length %d\n", keyLength);
		//Reading the key
		key = (char*)malloc(sizeof(char)*keyLength);
		memcpy(key, request + offset, keyLength);
		offset += keyLength;
        if(DEBUG) printf("Key: %s\n", key);
		//Reading the data length
		memcpy(&dataLength, request + offset, sizeof(int));
		offset += sizeof(int);
        if(DEBUG) printf("Data length %d\n", dataLength);
		//Reading the data value
		data = (char*)malloc(sizeof(char)*dataLength);
		memcpy(data, request + offset, dataLength);
		if(DEBUG) printf("Data: %s\n", data);
       
        printf("WRITE(%s,%s)\n",key,data);
        /*time should be increased by 1 after a write*/
        myLamportClockTime=updateTime(myLamportClockTime);
        
        /* Write committed to DB, now send REP_WRTITE to other data centers */
        //TODO: respond to the client with ACK (Why?)
		writeToDataStore(key, clientID, data);
		
        /*Clear the request(?)*/
        /* HEADER KEY_LENGTH KEY DATA_LENGTH DATA DATACENTER_ID LAMPORT_CLOCK TIME DEPLIST */
        memcpy(request, REP_WRITE, MESSAGE_HEADER_LENGTH);
        offset=MESSAGE_HEADER_LENGTH;
        
        //Write the key length
        memcpy(request + offset,&keyLength, sizeof(int));
        offset += sizeof(int);
        //Write the key
        memcpy(request + offset,key,  keyLength);
        offset += keyLength;
        //Write the data length
        memcpy(request + offset, &dataLength, sizeof(int));
        offset += sizeof(int);
        //Writing the data value
        memcpy(request + offset,data,  dataLength);
        offset+=dataLength;
        //Writing the data center_id
        memcpy(request + offset,&myID,  sizeof(int));
        offset+=sizeof(int);
        printf("Data center ID Sent:%d\n",myID);
        //Writing the lamport clock time
        memcpy(request + offset,&myLamportClockTime,  sizeof(int));
        printf("Time sent %d\n",myLamportClockTime);
        offset+=sizeof(int);
        
        
        /*Including the current dependency list in the message
          Format:  LIST_SIZE [KEY_LENGTH KEY TIMESTAMP DATACENTER_ID] */
        //LIST SIZE
        memcpy(request+offset,&clientDependenciesLists[clientID].count, sizeof(int));
        offset+=sizeof(int);
        for(i=0;i<clientDependenciesLists[clientID].count;i++)
        {
          //Writing KEY_LENGTH
          keyLength=strlen(clientDependenciesLists[clientID].list[i].key)+1;
          memcpy(request+offset,&keyLength, sizeof(int));
          offset+=sizeof(int);
          //Writing KEY
          memcpy(request+offset,clientDependenciesLists[clientID].list[i].key,keyLength);
          offset+=keyLength;
          //Writing Lamport Clock Time
          memcpy(request+offset,&clientDependenciesLists[clientID].list[i].lamportClockTime, sizeof(int));
          offset+=sizeof(int);
          //Writing DATACENTER_ID
          memcpy(request+offset,&clientDependenciesLists[clientID].list[i].dataCenterID, sizeof(int));
          offset+=KEY_SIZE;
        }
        
		resp = sendReplicatedWrite(ADDRESS, PORT_D1, request);
		assert(resp == 1);
		resp = sendReplicatedWrite(ADDRESS, PORT_D2, request);
		assert(resp == 1);

		/*clear dependency list and add this write*/
        printf("Clearing the dependency list after sending the replicated write request\n");
		clearDependencyList(clientID);
		/*create new dependency and add to the DPLISTS[clientID]*/
		dependency.key = key;
		dependency.lamportClockTime = myLamportClockTime;
		dependency.dataCenterID = myID;
		assert(appendClientDependencyList(clientID, dependency) == 1);
	}
	/*REPLICATED WRITE */
	else if (strncmp(request, REP_WRITE, MESSAGE_HEADER_LENGTH) == 0)
	{
        if(DEBUG)printf("**********************REPLICATED WRITE***************\n");
		offset += MESSAGE_HEADER_LENGTH;
		//Reading the key length
		memcpy(&keyLength, request + offset, sizeof(int));
		offset += sizeof(int);
		if(DEBUG) printf("Key length %d\n", keyLength);
		//Reading the key
		key = (char*)malloc(sizeof(char)*keyLength);
		memcpy(key, request + offset, keyLength);
		offset += keyLength;
        if(DEBUG) printf("Key: %s\n", key);
		//Reading data length
		memcpy(&dataLength, request + offset, sizeof(int));
		offset += sizeof(int);
        if(DEBUG) printf("Data length: %d\n", dataLength);
		//Reading data
		data = (char*)malloc(sizeof(char)*dataLength);
		memcpy(data, request + offset, dataLength);
        offset+=dataLength;
		if(DEBUG) printf("Data: %s\n", data);
        //Reading the data center ID
        memcpy(&dataCenterID,request + offset,sizeof(int));
        offset+=sizeof(int);
        if(DEBUG) printf("Data center ID %d\n",dataCenterID);
        //Reading the lamport clock time
        memcpy(&lamportClockTimeReceived,request + offset,sizeof(int));
        offset+=sizeof(int);
        if(DEBUG) printf("Lamport clock time received  %d\n",lamportClockTimeReceived);

        printf("Replicated_Write(%s) received \n",key);
        
        memcpy(&replicatedDepList.count,request+offset,sizeof(int));
        offset+=sizeof(int);
        if(DEBUG) printf("DEP LIST SIZE %d\n",replicatedDepList.count);

        printf("DEP LIST: ' ");
        for(i=0;i<replicatedDepList.count;i++)
        {
          //KEY_LENGTH
          memcpy(&keyLength,request+offset, sizeof(int));
          offset+=sizeof(int);
          if(DEBUG) printf("Key length %d\n", keyLength);
            
          //KEY
          replicatedDepList.list[i].key=(char*)malloc(sizeof(char)*keyLength);
          memcpy(replicatedDepList.list[i].key,request+offset, keyLength);
          offset+=keyLength;
          if(DEBUG) printf("Key  %s\n", replicatedDepList.list[i].key);

          //Lamport Clock Time
          memcpy(&replicatedDepList.list[i].lamportClockTime,request+offset, sizeof(int));
          offset+=sizeof(int);
          if(DEBUG) printf("Lamport clock time received %d\n", replicatedDepList.list[i].lamportClockTime);

          //DATACENTER_ID
          memcpy(&replicatedDepList.list[i].dataCenterID,request+offset, sizeof(int));
          offset+=KEY_SIZE;
          if(DEBUG) printf("Data center ID %d\n", replicatedDepList.list[i].dataCenterID);
            
    printf("<%s,(%d,%d)>",replicatedDepList.list[i].key,replicatedDepList.list[i].lamportClockTime,replicatedDepList.list[i].dataCenterID);
        }
        printf("' \n");
        if(DEBUG)printf("*******************REPLICATED WRITE ENDS******************\n");

        
        /*Add operation*/
        replicatedDepList.operation.key=key;
        replicatedDepList.operation.data=data;
        replicatedDepList.operation.dataCenterID=dataCenterID;


		/*check for dependencies and commit the write if no dependecies*/
		flag = checkDependency(replicatedDepList);
		if (flag == 0) {
			//add to the pending queue
            printf("Adding to pending queue\n");
			assert(appendPendingQueue(replicatedDepList) == 1);
		}
		else {
			/* -1 to indicate that this was a replicated write and not a client initiated write*/
            printf("Dep check for Replicated_Write(%s) satisfied \n",key);
            dependency.key = key;
            dependency.lamportClockTime = lamportClockTimeReceived;
            dependency.dataCenterID = dataCenterID;
            
            replicatedWritesDepList.list[replicatedWriteCount]=dependency;
            replicatedWriteCount++;
            
            commit(key,  -1,dataCenterID, data);
            
            myLamportClockTime=max(myLamportClockTime,lamportClockTimeReceived);
			/*reissue dep check for all the keys in the pending queue*/
            //checkPendingQueue(key,myLamportClockTime,myID);
            printf("Checking pending queue\n");
			for (i = 0; i < MAX_PENDING; i++)
			{
				if (pendingQueue[i].count!=0 && checkDependency(pendingQueue[i]) == 1) {
                    printf("Replicated_Write(%s) dep check satisfies\n",pendingQueue[i].operation.key);
                    //printf("Removing from pending queue\n");
					removeFromPendingQueue(i);
                    printf("Commiting to DB\n");
					commit(pendingQueue[i].operation.key, -1, pendingQueue[i].operation.dataCenterID, pendingQueue[i].operation.data);
				}				
			}
		}
	}
	return;
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
        max_sd=serverSocketFD;
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
                    printf("\n Message received: %s \n", buffer);
					messageHandler(buffer, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), client_sock[i]);
				}
			}
		}

	}

}


/*TODO -  We can switch to epoll  if we get the time. What do you think? NAAAAAAAAAAH....we have better things to do*/
int main(int argc, char** argv)
{
	int flag = 0;
   
    if(argc==2)
    {
        PORT=atoi(argv[1]);
        PORT_D1=((PORT+1)==3?3:(PORT+1)%3)+60000;
        PORT_D2=((PORT+2)==3?3:(PORT+2)%3)+60000;
        PORT+=60000;
        myID=PORT%60000;
        if(myID==1)
            assert(chdir(DIR_PATH_1)==0);
        else if(myID==2)
            assert(chdir(DIR_PATH_2)==0);
        else
            assert(chdir(DIR_PATH_3)==0);
        printf("My ID : %d (PORT MOD 60000)\n",myID);
        replicatedWriteCount=0;
        pendingCount=0;
        /*Make random*/
        CASTPORT=PORT+10;
        flag = initDB();
        assert(flag == 0);
        initializeSockets();
        listening();
    }
    else
         printf("Usage ./DataCenter PORT\n");
	return 1;
}

