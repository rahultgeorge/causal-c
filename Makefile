CC=gcc

datacenter: datastore DependencyUtilities.o
	        $(CC) -lsqlite3 DataCenter.c DependencyUtilities.o DBHandler -o DataCenter


datastore:  
	        $(CC) -c  DBHandler.c -o DBHandler

bdaemon:   
			$(CC) -lsqlite3 DBHandler BroadcastDaemonM.c -o BroadcastDaemonM

client:   datacenter
	      $(CC) Client.c -o Client
	      
all:  client datacenter


.PHONY: all test clean	
