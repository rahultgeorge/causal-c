#include "DependencyUtilities.h"



int appendClientDependencyList(int clientID, Dependency dependency) {
	/*Add dependency to the dependency list of that client*/
	int count = clientDependenciesLists[clientID].count++;
	if (count > MAX_DEP) return 0;
	clientDependenciesLists[clientID].list[count] = dependency;
	return 1;
}

void clearDependencyList(int clientID)
{
	/*Clear the dependency list of the client*/
	int i;
	for (i = 0; i < clientDependenciesLists[clientID].count; i++) {
		clientDependenciesLists[clientID].list->key = NULL;
		clientDependenciesLists[clientID].list->dataCenterID = 0;
		clientDependenciesLists[clientID].list->lamportClockTime = 0;
	}
	clientDependenciesLists[clientID].count = 0;
}


int checkDependency(DependencyList replicatedDepList)
{
	/*Check dependencies*/
	int i, j, k, flag = 0,keyFound=0;
    if(replicatedDepList.count==0)
        flag=1;
    printf("Checking dependencies\n");
	//repDPList may have multiple dependencies so check for all
	for (i = 0; i < replicatedDepList.count; i++) {
		flag = 0;
		for (j = 0; j < MAX_CLIENTS; j++) {
			for (k = 0; k < clientDependenciesLists[j].count; k++) {
                printf("%d %d\n",strlen(clientDependenciesLists[j].list[k].key),strlen(replicatedDepList.list[i].key));

				if (strcmp(clientDependenciesLists[j].list[k].key, replicatedDepList.list[i].key) == 0
                    && clientDependenciesLists[j].list[k].lamportClockTime >= replicatedDepList.list[i].lamportClockTime){
//					&& clientDependenciesLists[j].list[k].dataCenterID == replicatedDepList.list[i].dataCenterID) {
					flag = 1;
					break;
				}
                else if(strcmp(clientDependenciesLists[j].list[k].key, replicatedDepList.list[i].key) == 0)
                        keyFound=1;
			}
			if (flag == 1) break;
		}
		if (flag == 0 && keyFound==1) {
			return flag;
		}
	}
    if(keyFound==0)
        flag=1;
	return flag;
}


/*Pending queue is a list of lists */
int appendPendingQueue(DependencyList list) {
    int flag = 0,i,j;
	if (pendingCount < MAX_PENDING) {
		
		for (i = 0; i < MAX_PENDING; i++) {
			if (pendingQueue[i].count == 0) {
				pendingQueue[i].count = list.count;
				pendingQueue[i].operation = list.operation;
                for(j=0;j<list.count;j++)
                    pendingQueue[i].list[j] = list.list[j];
				flag = 1;
			}
		}
        
		if (flag == 0) {
			pendingQueue[pendingCount].count = list.count;
			pendingQueue[pendingCount].operation = list.operation;
            for(j=0;j<list.count;j++)
                pendingQueue[i].list[j] = list.list[j];
		}
		pendingCount++;
		return 1;
	}
	return 0;
}


int removeFromPendingQueue(int index)
{
	pendingQueue[index].count = 0;
    --pendingCount;
    return 0;
}

/* Call check dependency on every dep list and commit when dep check passes*/
/*
int checkPendingQueue(char* key, int timestamp, int datacenter_id) {
    int i=0,flag=0;
    for(i=0;i<pendingCount;i++)
    {
        flag=1;
        if(checkDependency(pendingQueue[i])==1)
            flag=1;
        else
           //TODO - Check with this operation 
            printf("Compared with the current replicated write performed\n");
        if(flag==1)
        {
            removeFromPendingQueue(i);
			//commit
        }
    }
    return 0;
}
*/
