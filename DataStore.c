


  int commit(int opcode,int key, void* data)
  {
     sqlite3 *db; 
	 sqlite3_stmt *stmt;
	 char* query;
	 char *zErrMsg = 0;


	 int rc = sqlite3_open("P2P.db", &db);
	 asprintf(&query, "insert into  (chunk_id, FILE_NAME,CHUNK_NAME,SIZE,IP_ADDRESS,PORT) values (%d, '%s','%s',%d,'%s',%d);", chunkID,fileName,chunkName,size,ipAddress,port);

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

  int read(char* key,void* data)
  {
    sqlite3 *db; 
    sqlite3_stmt *stmt;
    char *query;
    char *err_msg = 0;
    int rc = -1;

	rc=sqlite3_open("P2P.db", &db);
	
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", 
        sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    
	
	printf("Retrieving file list\n");
	
	//Get count
    query="SELECT COUNT(*) FROM FILES;";
    //asprintf(&query, "SELECT FILE_NAME,SIZE FROM FILES WHERE IP_ADDRESS !='%s';",ipAddress);
	
    sqlite3_prepare_v2(db, query, strlen(query), &stmt, NULL);
    rc = sqlite3_step(stmt);
	
	if(rc==SQLITE_ROW)
	{
	  	numberOfFiles=sqlite3_column_int(stmt,0);
		/*Now allocate space for the file list data structure */
	    myFileList=malloc(sizeof(struct FileList));
	    myFileList->noFiles=numberOfFiles;
		myFileList->files=malloc(sizeof(char)*20*(myFileList->noFiles));
		myFileList->fileSizes=malloc(sizeof(int)*(myFileList->noFiles));		
	}

    else if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return -1;
    } 
	
    sqlite3_finalize(stmt);
    return 0;


  }