#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <mqtt_config_parsing.h>
#include <json-c/json.h>
#include <ctype.h>
#include <mqtt_event_mail.h>


/*
Function checks if given string is number
*/
int isNumber(char *s)
{
    for (int i = 0; s[i]!= '\0'; i++)
    {
        if (isdigit(s[i]) == 0)
              return 0;
    }
    return 1;
}

/*
Function compares payload to value and if successful sends email
*/
void compare_payload_to_value (char *payload,struct Event *ptr){
    if (strcmp(ptr->type, "0") == 0){
        if (strcmp(ptr->comparison, "==") == 0) {
            if (strcmp(payload, ptr->value))
                send_email(ptr,payload);
        }else if (strcmp(ptr->comparison, "!=") == 0) {
            if (!strcmp(payload, ptr->value))
                send_email(ptr,payload);
        }else{
            syslog (LOG_WARNING,"Incompatible type and operator selected");
        }          
    }else if (strcmp(ptr->type, "1") == 0 && isNumber(ptr->value) && isNumber(payload)){
        if (strcmp(ptr->comparison, "==") == 0) {
            if (atoi(ptr->value) == atoi(payload))
                send_email(ptr,payload);
        }else if (strcmp(ptr->comparison, "!=") == 0) {
            if (atoi(ptr->value) != atoi(payload))
                send_email(ptr,payload);
        }else if (strcmp(ptr->comparison, ">") == 0) {
            if (atoi(payload) > atoi(ptr->value))
                send_email(ptr,payload);
        }else if (strcmp(ptr->comparison, "<") == 0) {
            if (atoi(payload) < atoi(ptr->value))
                send_email(ptr,payload);
        }else if (strcmp(ptr->comparison, ">=") == 0) {
            if (atoi(payload) >= atoi(ptr->value))
                send_email(ptr,payload);
        }else if (strcmp(ptr->comparison, "<=") == 0) {
            if (atoi(payload) <= atoi(ptr->value))
                send_email(ptr,payload);
        }
    }else {
        syslog (LOG_WARNING,"Message data does not contain integer");
    }
}
/*
Function parses payload and returns it as string  
*/
int parse_payload(char **payload, char* key){
    int rc=0;
    json_object *parsed_json;
    parsed_json = json_tokener_parse(*payload);
    *payload = json_object_get_string(json_object_object_get(parsed_json, key));    
    return rc;
}
/*
Function goes through all the events in list and calls:
    parse_payload function to parse payload
    compare_payload_to_value function to compare values and send email if needed
*/
void check_for_events_and_parse(struct Topic *head, char *topic, char *payload){
    int rc=0;
    struct Topic *ptr = head;

    while(ptr != NULL) {            //goes through all topics
        if (strcmp(ptr->topic,topic) == 0){
            struct Event *ptr2 = ptr->event_head;  
            while(ptr2 != NULL) {    //goes through all topic events
                rc = parse_payload(&payload, ptr2->key); //parses payload and returns it as string            
                if (rc == 0){
                    compare_payload_to_value(payload, ptr2);    //compares payload to value and if successful sends email
                }
                else{
                    syslog (LOG_WARNING,"Failed to parse payload");
                    return;
                } 
                ptr2 = ptr2->next;
            }
            return;
        }
        ptr = ptr->next;
    }
}