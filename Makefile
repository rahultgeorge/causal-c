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
			rm -f Client DataCenter DBHandler CausalDB BroadcastDaemonM D2/CausalDB D3/CausalDB D1/CausalDB Test1 Test2
			
Test1: client datastore DependencyUtilities.o
		$(CC) test1.c DependencyUtilities.o DBHandler -lsqlite3 -o Test1

Test2: client datastore DependencyUtilities.o
		$(CC) test2.c DependencyUtilities.o DBHandler -lsqlite3 -o Test2

all:  client Test1 Test2 datacenter


.PHONY: all test clean	
