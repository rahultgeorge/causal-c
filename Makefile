CC=gcc


datacenter: datastore
	        $(CC) -lsqlite3 DataCenter.c  DBHandler -o DataCenter


datastore:  
	        $(CC) -c  DBHandler.c -o DBHandler

bdaemon:   
			$(CC) -lsqlite3 DBHandler BroadcastDaemonM.c -o BroadcastDaemonM 

client:   datacenter
	      $(CC) Client.c -o Client


.PHONY: all test clean	