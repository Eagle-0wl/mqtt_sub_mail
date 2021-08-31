/*
Fucntion prepares data for curl to use
*/
static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp);
/*
Function sends email using curl
*/
void send_email(struct Event *ptr, char *payload);