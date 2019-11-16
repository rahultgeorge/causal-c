#ifndef _DEPENDENCY_UTILITIES_H
#include <stddef.h>
#define MAX_DEP 10
#define MAX_CLIENTS 10
#define MAX_PENDING 100
#define KEY_SIZE 5

typedef struct Dependency
{
	char* key;
	int lamportClockTime;
	int dataCenterID;
}Dependency;

typedef struct Operation
{
    char key[KEY_SIZE];
    char* data;
    int dataCenterID;
}Operation;

/* Create a list of dependency lists(MAX Clients)
   Initialize somewhere
*/
typedef struct DependencyList
{
	Dependency list[MAX_DEP];  //Limit this as well
	int count; //for the number of DEPS
    Operation operation[MAX_DEP]; // Operation that needs to be performed if the dependency check is satisified
}DependencyList;




DependencyList pendingQueue[MAX_PENDING];
int pendingCount;
DependencyList clientDependenciesLists[MAX_CLIENTS];

int appendClientDependencyList(int clientID, Dependency dependency);

/* Clear the dependency list (For write operation)
After we send the replicated write we clear the dependency list
and then add the write operation to the list (We call appendClientDependencyList)  */
void clearDependencyList(int clientID);

/*Pending queue is a list of lists */
int appendPendingQueue(DependencyList list);

/* Call check dependency */
//int checkPendingQueue(char* key, int timestamp, int data);

/*Check all the dependency lists*/
int checkDependency(DependencyList replicatedDepList);

#endif


