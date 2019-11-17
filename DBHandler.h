#ifndef _DB_HANDLER_H

#define DB_NAME "CausalDB"
#include <stdio.h>
#include <stdlib.h> 
#include <sqlite3.h> 
#include <string.h>
#define DEBUG 0
int initDB();

/* Commits the write to the data store */
int commit(char* key, int client_id, int center_id, void* data);

/* Reads the value from the data store */
int readFromDB(char* key, void* data);

/* Reads the data center id from which it received this value */
int readIDFromDB(char* key);

#endif  
