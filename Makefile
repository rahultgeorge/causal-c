CC=gcc


datacenter: datastore DependencyUtilities.o
	        $(CC)    DataCenter.c DependencyUtilities.o DBHandler -lsqlite3 -o DataCenter

datastore:  
	        $(CC) -c  DBHandler.c -o DBHandler

bdaemon:   
			$(CC) -lsqlite3 DBHandler BroadcastDaemonM.c -o BroadcastDaemonM

client:
	      $(CC) Client.c -o Client


clean:
			rm -f Client DataCenter DBHandler CausalDB BroadcastDaemonM
			
Test1: client datastore DependencyUtilities.o
		$(CC) -lsqlite3 Test1.c DependencyUtilities.o DBHandler -o Test1

Test2: client datastore DependencyUtilities.o
		$(CC) -lsqlite3 Test1.c DependencyUtilities.o DBHandler -o Test2

all:  client datacenter


.PHONY: all test clean	
