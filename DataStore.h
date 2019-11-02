#ifndef _DATA_STORE_H



 #define DB_NAME CausalDB

 /*Commits the write to the data store
   
 */
 int commit(char* key, void* data);

 int read(char* key);