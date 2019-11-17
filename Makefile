CC=gcc

datacenter: datastore DependencyUtilities.o
	        $(CC)    DataCenter.c DependencyUtilities.o DBHandler -lsqlite3 -o DataCenter


datastore:  
	        $(CC) -c  DBHandler.c -o DBHandler

bdaemon:   
			$(CC) -lsqlite3 DBHandler BroadcastDaemonM.c -o BroadcastDaemonM

client:   datacenter
	      $(CC) Client.c -o Client
	      
all:  client datacenter


.PHONY: all test clean	
