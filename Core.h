#ifndef _CORE_H
 #define _CORE_H
 #include "DBHandler.h"
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
 #include <errno.h>
 #include <unistd.h>
 #include <stdlib.h>

 #define KEY_SIZE 5
 #define MAX_DATA 100
 #define MAX_PENDING 100
 #define ADDRESS "127.0.0.1"
 #define DIR_PATH_1 "D1"
 #define DIR_PATH_2 "D2"
 #define DIR_PATH_3 "D3"
// #define PORT 60000
// #define CASTPORT 60003
// #define PORT_D1 60001
// #define PORT_D2 60002

 #define DOMAIN AF_INET
 #define SOCKET_TYPE SOCK_STREAM
 #define PROTOCOL 0
 #define MAX_CLIENTS 10
 #define DEBUG 0
 #define KEY_SIZE 5
/*Client TAGS */
 #define MESSAGE_HEADER_LENGTH 8
 #define READ_REQUEST  "READ_REQ"
 #define WRITE_REQUEST "WRIT_REQ"
 #define WRITE_ACK     "WRIT_ACK"
 #define WRITE_NACK    "WRIT_NCK"
 #define READ_REPLY    "READ_VAL"
 /* Datacenter TAGS  */
 #define REP_WRITE     "REP_WRIT"

 #define updateTime(x) x+1
 #define max(a,b) a>b ? a : b
 
 #define MAX_MESSAGE_SIZE 2048

#endif
