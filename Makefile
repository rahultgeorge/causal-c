CC=gcc


datacenter: datastore
	        $(CC) DataCenter.c  DataStore -o DataCenter


datastore:  
	        $(CC) -c -lsqlite3 DataStore.c -o DataStore


client:   datacenter
	      $(CC) Client.c -o Client


.PHONY: all test clean	