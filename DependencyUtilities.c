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


int checkDependency(PendingDependencyList replicatedDepList)
{
	/*Check dependencies*/
	int i, j, k, flag = 0;
	//repDPList may have multiple dependencies so check for all
	for (i = 0; i < replicatedDepList.count; i++) {
		flag = 0;
		for (j = 0; j < MAX_CLIENTS; j++) {
			for (k = 0; k < clientDependenciesLists[j].count; k++) {
				if (strcmp(clientDependenciesLists[j].list[k].key, replicatedDepList.list[i].key) == 0
					&& clientDependenciesLists[j].list[k].lamportClockTime >= replicatedDepList.list[i].lamportClockTime) {
					flag = 1;
					break;
				}

			}
			if (flag == 1) break;
		}
		if (flag == 0) {
			return flag;
		}
	}
	return flag;
}


/*Pending queue is a list of lists */
int appendPendingQueue(DependencyList list) {
	if (pendingCount < MAX_PENDING) {
		pendingQueue[pendingCount].count = list.count;
		for (int i = 0; i < list.count; i++) {
			pendingQueue[pendingCount].list[i] = list.list[i];
		}
		return 1;
	}
	return 0;
}


int removeFromPendingQueue(int index)
{
   for(int i=0;i<pendingCount;i++)
      if(i==index)
      {
          
          
          break;
      }
    --pendingCount;
    return 0;
}

/* Call check dependency on every dep list and commit when dep check passes*/
int checkPendingQueue(char* key, int timestamp, int datacenter_id) {
    int i=0,flag=0;
    for(i=0;i<pendingCount;i++)
    {
        flag=1;
        if(checkDependency(pendingQueue[i])==1)
            flag=1;
        else
           /*TODO - Check with this operation */
            printf("Compared with the current replicated write performed\n");
        if(flag==1)
        {
            removeFromPendingQueue(i);
            
        }
    }
    return 0;
}
