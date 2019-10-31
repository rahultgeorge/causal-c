CC=gcc


datacenter: datastore
	        $(CC) DataCenter.c  DataStore -o DataCenter


datastore:  
	        $(CC) -c DataStore.c -o DataStore


client:   datacenter
	      $(CC) Client.c -o Client


.PHONY: all test clean	