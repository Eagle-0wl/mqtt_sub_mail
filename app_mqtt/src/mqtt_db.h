/*
Function opens data base
*/
int open_db();
/*
Function closes database
*/
int close_db();
/*
Function creates messages table in data base
*/
int create_table();
/*
Function checks if messages table exist in database
*/
int check_if_table_exists();
/*
Function inserts message into database table
*/
int insert_message_to_database(char *topic, char *payload);