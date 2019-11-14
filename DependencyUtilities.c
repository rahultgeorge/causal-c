#include "DependencyUtilities.h"

DependencyList pendingQueue[MAX_PENDING];
DependencyList clientDependenciesLists[MAX_CLIENTS];

int appendClientDependencyList(int clientID, Dependency dependency)
{

  /*Add dependency to the relavent dependency list */
  int count = clientDependenciesLists[clientID].count++;
  clientDependenciesLists[clientID].list[count] = dependency;
}

int clearDependencyList(int clientID)
{
    /*Clear the relavent dependency list */
    clientDependenciesLists[clientID].count = 0;

}

int checkDependency(DependencyList replicatedDepList)
{
    /*TODO Check dependencies*/
    int i, j, k, flag = 0;
    for(i = 0; i<  replicatedDepList.count; i++){
    	flag=0;
    	for(j = 0; j < MAX_CLIENTS; j++){
    
    		for(k = 0; k < clientDependenciesLists[j].count; k++){
    			if(strcmp(clientDependenciesLists[j].list[k].key, replicatedDepList.list[i].key)==0
    			&& clientDependenciesLists[j].list[k].lamportClockTime>=replicatedDepList.list[i].lamportClockTime
    			&& clientDependenciesLists[j].list[k].dataCenterID==replicatedDepList.list[i].dataCenterID){
    				flag = 1;break;
    			}

    		}
    		if(flag==1) break;    
    	}
    	if(flag==0){
    		return flag;
    	}
    }
    return flag;
}


/*Pending queue is a list of lists */
int appendPendingQueue(DependencyList list){



}



/* Call check dependency */
int checkPendingQueue(char* key, int timestamp, int datacenter_id){



}
