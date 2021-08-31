/*
Function checks if given string is number
*/
int isNumber(char *s);
/*
Function compares payload to value and if successful sends email
*/
void compare_payload_to_value (char *payload,struct Event *ptr);
/*
Function parses payload and returns it as string  
*/
int parse_payload(char **payload, char* key);
/*
Function goes through all the events in list and calls:
    parse_payload function to parse payload
    compare_payload_to_value function to compare values and send email if needed
*/
void check_for_events_and_parse(struct Topic *head, char *topic, char *payload);