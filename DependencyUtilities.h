#ifndef _DEPENDENCY_UTILITIES_H

#define MAX_DEP 10
#define MAX_CLIENTS 10
#define MAX_PENDING 10

typedef struct Dependency
{
	char* key;
	int lamportClockTime;
	int dataCenterID; //Yashasvi says port; I don't agree.
}Dependency;

/* Create a list of dependency lists(MAX Clients)
   Initialize somewhere
*/
typedef struct DependencyList
{
	Dependency list[MAX_DEP];  //Limit this as well
	int count; //for the number of DEPS
}DependencyList;


int createClientDependencyList(int clientID);

int appendClientDependencyList(int clientID,Dependency dependency);

/* Clear the dependency list (For write operation)
After we send the replicated write we clear the dependency list
and then add the write operation to the list (We call appendClientDependencyList)  */
int clearDependencyList(int clientID);

/*Pending queue is a list of lists */
int appendPendingQueue(DependencyList list);

/* Call check dependency */
int checkPendingQueue(char* key, int timestamp, int datacenter_id);

/*Check all the dependency lists*/
int checkDependency(DependencyList replicatedDepList);

#endif


