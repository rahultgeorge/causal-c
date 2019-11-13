#include "DBHandler.h"


int commit(char* key, int client_id, void* data)
{
	sqlite3 *db;
	sqlite3_stmt *stmt;
	char* query;
	char *zErrMsg = 0;

	int rc = sqlite3_open(DB_NAME, &db);
	asprintf(&query, "insert into CausalTable (KEY, CLIENT_ID, VALUE) values ('%s', %d,'%s');", key, client_id, ((char *)data));

	sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);

	rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE)
	{
		printf("ERROR inserting data: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_finalize(stmt);
	free(query);
	return 0;
}

int readFromDB(char* key, void* data)
{
	sqlite3 *db;
	sqlite3_stmt *stmt;
	char *query;
	char *err_msg = 0;
	int rc = -1;

	rc = sqlite3_open(DB_NAME, &db);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n",
			sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}


	printf("Retrieving value\n");

	//Get count
	asprintf(&query, "SELECT VALUE FROM CausalTable WHERE KEY ='%s';", key);

	sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);
	rc = sqlite3_step(stmt);

	if (rc == SQLITE_ROW)
	{
		printf("Read %s\n", sqlite3_column_text(stmt, 0));
		strcpy((char*)data, (char*)sqlite3_column_text(stmt, 0));
	}

	else if (rc != SQLITE_OK)
	{

		fprintf(stderr, "Failed to select data\n");
		fprintf(stderr, "SQL error: %s\n", err_msg);

		sqlite3_free(err_msg);
		sqlite3_close(db);

		return -1;
	}

	sqlite3_finalize(stmt);
	return 0;


}



int initDB()
{
	sqlite3 *db;
	sqlite3_stmt *stmt;
	char *zErrMsg = 0;
	int rc;
	char *query;

	printf("Initialzing database\n");
	rc = sqlite3_open(DB_NAME, &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}

	query = "CREATE TABLE IF NOT EXISTS CausalTable  ("  \
		"KEY           CHAR(20)    NOT NULL," \
		"CLIENT_ID            INT     NOT NULL," \
		"VALUE            VARCHAR(1000)     NOT NULL" \
		");";


	sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);


	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Can't create table database: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite3_close(db);
	return 0;
}
  
//   int main()
//   {
//   	initDB();
// commit("Yashasvi\0",1,"Asthana\0");
// void* data=malloc(sizeof(char)*1000);
// read("Yashasvi\0",data);
// printf("Read %s \n",((char *)data));
//
//   } 