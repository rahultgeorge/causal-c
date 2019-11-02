#ifndef _CORE_H
 #define _CORE_H
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <stdio.h>
 #include <string.h>
 #include <assert.h>
 #include <pthread.h> 
 #include <sys/ioctl.h>
 #include <stdlib.h>
 #include <arpa/inet.h>
 #include <time.h>
 #include "DataStore.h"


 // struct timeStamp
 // {

 // }
 #define PORT 60001
 #define DEBUG 1
 #define DOMAIN AF_INET
 #define SOCKET_TYPE SOCK_STREAM
 #define PROTOCOL 0
 #define MAX_CLIENTS 27
 #define DEBUG 1
 // #define MESSAGE_HEADER_LENGTH 12
 // #define REGISTER_REQUEST "REG_REQUEST\0"
 // #define FILE_LIST_REQUEST "FLI_REQUEST\0"
 // #define FILE_LOCATION_REQUEST "FLO_REQUEST\0"
 // #define CHUNK_REGISTER_REQUEST "CHU_REQUEST\0"
 // #define FILE_CHUNK_REQUEST "FCH_REQUEST\0"


 // #define REGISTER_REPLY       "REGIS_REPLY\0"
 // #define FILE_LIST_REPLY      "FI_LS_REPLY\0"
 // #define FILE_LOCATION_REPLY  "FI_LO_REPLY\0"
 // #define CHUNK_REGISTER_REPLY "CHUNK_REPLY\0"


 #define MAX_MESSAGE_SIZE 2048

#endif