/* Client Code */

#include "Core.h"

int socketFD;
signed int clientID;
struct sockaddr_in serverAddress;

int initializeSocket(int port)
{
    socketFD = socket(DOMAIN, SOCKET_TYPE, PROTOCOL);
    assert(socketFD != 0);
    memset(&serverAddress, '\0', sizeof(serverAddress));
    serverAddress.sin_family = DOMAIN;
    serverAddress.sin_port = htons(port);
    inet_aton("127.0.0.1", (struct in_addr *)&serverAddress.sin_addr.s_addr);
    return socketFD;
}

/* HEADER CLIENT_ID KEY_LENGTH KEY DATA_LENGTH DATA */
int sendWriteRequest(char* key,char* data)
{
    int offset = MESSAGE_HEADER_LENGTH, keyLength, dataLength = 0, rc;
    char request[MAX_MESSAGE_SIZE];

    memcpy(request, WRITE_REQUEST, MESSAGE_HEADER_LENGTH);
    
    memcpy(request+offset,&clientID, sizeof(int));
    offset+=sizeof(int);
     
    keyLength = strlen(key) + 1;
    printf("%d\n", keyLength);
    memcpy(request + offset, &keyLength, sizeof(int));
    offset += sizeof(int);

    memcpy(request + offset, key, keyLength);
    offset += keyLength;

    dataLength = strlen(data) + 1;
    memcpy(request + offset, &dataLength, sizeof(int));
    offset += sizeof(int);

    memcpy(request + offset, data, dataLength);
    offset += dataLength;

    send(socketFD, request, MAX_MESSAGE_SIZE, 0);
    return 0;

}

 /* HEADER CLIENT_ID KEY_LENGTH KEY DATA_LENGTH DATA */
int sendReadRequest(char* key)
{
    int offset = MESSAGE_HEADER_LENGTH, keyLength = -1;
    char request[MAX_MESSAGE_SIZE],reply[MAX_MESSAGE_SIZE];
    
    memcpy(request, READ_REQUEST, MESSAGE_HEADER_LENGTH);
    
    memcpy(request+offset,&clientID, sizeof(int));
    offset+=sizeof(int);
         
    keyLength = strlen(key)+1;
    memcpy(request + offset, &keyLength, sizeof(int));
    offset += sizeof(int);

    memcpy(request + offset, key, keyLength);
    offset += keyLength;

    send(socketFD, request, MAX_MESSAGE_SIZE, 0);
    /*Read the value*/
    //read(socketFD,reply,MAX_MESSAGE_SIZE);
    //printf("READ: %s\n",reply);
    return 0;
}

int showOperations()
{
    int n = -1;
    char data[20], key[20];
    printf("Enter 1 to read\n 2 to write\n 3 to exit\n");
    scanf("%d", &n);
    if (n == 1)
    {
        printf("Enter key\n");
        scanf("%s", key);
        sendReadRequest(key);
        return 0;
    }
    else if (n == 2)
    {
        printf("Enter key\n");
        scanf("%s", key);
        printf("Enter data\n");
        //fgets(data,MAX_DATA,stdin);
        scanf("%s", data);
        sendWriteRequest(key, data);
        return 0;
    }
    return -1;
}

int main(int argc,char **argv)
{
    int flag = -1;
    clientID = getpid()%MAX_CLIENTS;
    if (argc < 2)
        fprintf(stderr, "Syntax: Client PORT\n");
    else
    {
        socketFD = initializeSocket(atoi(argv[1]));
        assert(socketFD != 0);
        flag = connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
        assert(flag == 0);
        printf("Client %d Connected successfully\n", clientID);

        /*TODO present menu for operations and send them to the data center */
        while (1)
        {
            if (showOperations() == -1)
                break;
        }
    }

    close(socketFD);
    exit(0);
    return 0;
}

