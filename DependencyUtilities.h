
#define MAX_DEP 10

int checkDependency(DependencyList list);


int createClientDependencyList(int clientID);

int appendClientDependencyList(int clientID,Dependency dependency);

/*Pending queue is a list of lists */
int appendPendingQueue(DependencyList list);

/* Call check dependency */
int checkPendingQueue(char* key, int timestamp, int datacenter_id);

/*Check all the dependency lists*/
int checkDependency(DependencyList replicatedDepList,);




typedef struct Dependency
{
	char* key;
	int lamportClockTime;
	int dataCenterId; //Yashasvi says port; I don't agree.
	
}Dependency;

/* Create a list of dependency lists(MAX Clients)
   Initialize somewhere
*/
typedef struct DependencyList
{
	Dependency list[MAX_DEP];  //Limit this as well
	int count;
	void* insert(void* );
	void* search(void* );
	void* delete(void* );
}DependencyList;

typedef struct PendingQueue
{
	DependencyList* list;  //Limit this as well
	void* insert(void* );
	void* search(void* );
	void* delete(void* );
}PendingQueue;
