/*
Function hadles signals to turn off the program
*/
void signal_handler(int signo);

/*
Function on connection:
    goes through all the topics in the list and subscribes to them
*/
void on_connect(struct mosquitto *mosq, void *obj, int rc);

/*
Function on each message:
    checks if json is valid
    calls function to check for events and parse them if they exist
*/
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

/*
Function starts mosquitto service
*/
int start_mossquitto (struct Topic *head, struct Configuration *config);
