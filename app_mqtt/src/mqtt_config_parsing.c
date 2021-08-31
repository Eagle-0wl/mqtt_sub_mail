#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <uci.h>

#define CONFFILE "/etc/config/mosquitto_client"

struct Event{
    char *topic;
    char *key;
    char *type;
    char *comparison;
    char *value;
    char *email;
    struct Event *next;
};

struct Topic {
    char *topic;
    char *qos;
    struct Event *event_head;
    struct Topic *next;
};

struct Configuration {
    char *address;
    char *port;
    char *username;
    char *password;
    char *use_tls;
};
/*
Function reads settings saves them to data structure
*/
void read_settings (struct uci_context *ctx ,struct uci_section *section,struct Configuration *config){
    if (uci_lookup_option_string(ctx, section, "adress")!= NULL){
    config->address = malloc(strlen((char*)uci_lookup_option_string(ctx, section, "adress")) + 1);
    strcpy(config->address,(char*)uci_lookup_option_string(ctx, section, "adress"));
    }
    if (uci_lookup_option_string(ctx, section, "local_port")!= NULL){
    config->port = malloc(strlen((char*)uci_lookup_option_string(ctx, section, "local_port")) + 1);
    strcpy(config->port,(char*)uci_lookup_option_string(ctx, section, "local_port"));
    }
    if (uci_lookup_option_string(ctx, section, "use_tls_ssl")!= NULL){
    config->use_tls = malloc(strlen((char*)uci_lookup_option_string(ctx, section, "use_tls_ssl")) + 1);
    strcpy(config->use_tls,(char*)uci_lookup_option_string(ctx, section, "use_tls_ssl"));
    }
    if (uci_lookup_option_string(ctx, section, "username")!= NULL){
    config->username = malloc(strlen((char*)uci_lookup_option_string(ctx, section, "username")) + 1);
    strcpy(config->username,(char*)uci_lookup_option_string(ctx, section, "username"));
    }
    if (uci_lookup_option_string(ctx, section, "password")!= NULL){
    config->username = malloc(strlen((char*)uci_lookup_option_string(ctx, section, "password")) + 1);
    strcpy(config->username,(char*)uci_lookup_option_string(ctx, section, "password"));
    }
    if (uci_lookup_option_string(ctx, section, "password")!= NULL){
    config->username = malloc(strlen((char*)uci_lookup_option_string(ctx, section, "password")) + 1);
    strcpy(config->username,(char*)uci_lookup_option_string(ctx, section, "password"));
    }
}

/*
Function reads topic and saves it to linked list
*/
void insert_topic_into_linked_list(struct Topic **head,struct uci_context *ctx ,struct uci_section *section) {
    struct Topic *link = (struct Topic*) malloc(sizeof(struct Topic));
    char *topic =(char*)uci_lookup_option_string(ctx, section, "topic");
    char *qos = (char*)uci_lookup_option_string(ctx, section, "qos");

    link->topic = (char*)malloc(strlen(topic) + 1);
    strcpy(link->topic,topic);
    link->qos = malloc(strlen(qos) + 1);
    strcpy(link->qos,qos);
    link->event_head = NULL;
    link->next = *head; //point it to old first node
    *head = link;       //point first to new first node
}


/*
Function reads event and saves it to linked list
*/
void insert_event_into_linked_list(struct Topic **head,struct uci_context *ctx ,struct uci_section *section) {
    struct Topic *ptr = *head;
    char *topic =(char*)uci_lookup_option_string(ctx, section, "topics");
    char *key =(char*)uci_lookup_option_string(ctx, section, "key");
    char *type = (char*)uci_lookup_option_string(ctx, section, "type");
    char *comparison =(char*)uci_lookup_option_string(ctx, section, "comparison");
    char *value = (char*)uci_lookup_option_string(ctx, section, "value");
    char *email = (char*)uci_lookup_option_string(ctx, section, "email");

    while(ptr != NULL) {                 //start from the beginning

        if (strcmp(ptr->topic, topic)==0){
            struct Event *link = (struct Event*) malloc(sizeof(struct Event));

            link->key = (char*)malloc(strlen(key) + 1);
            strcpy(link->key,key);

            link->type = (char*)malloc(strlen(type) + 1);
            strcpy(link->type,type);

            link->comparison = (char*)malloc(strlen(comparison) + 1);
            strcpy(link->comparison,comparison);

            link->value = (char*)malloc(strlen(value) + 1);
            strcpy(link->value,value);

            link->email = (char*)malloc(strlen(email) + 1);
            strcpy(link->email,email);

            link->topic=ptr->topic;

            link->next= ptr->event_head;
            ptr->event_head = link;
        }
        
        ptr = ptr->next;
    }
}
/*
Function reads configuration file
*/
int uci_read_config(struct Topic **head, struct Configuration *config)
{   
        int rc = 0;
        struct uci_element *i_iter = NULL;
        struct uci_package *package = NULL;
        struct uci_context *ctx = uci_alloc_context();
        struct uci_section *section;

        if (uci_load(ctx, CONFFILE, &package) != UCI_OK) {      //load config file
            syslog (LOG_ERR,"Failed to load uci.");
            rc = 2;
            goto cleanup;
        }
        uci_foreach_element(&package->sections, i_iter) {       // iterate throuhg all elements in config file
            section = uci_to_section(i_iter);                   
            /*
            Check for specific section and read data from it 
            */                                                  
            if (strcmp(section->type, "mqtt_client") == 0) {    
                read_settings(ctx,section,config);

            }
            if (strcmp(section->type, "topic") == 0) {
                insert_topic_into_linked_list(head,ctx,section);
            }
            if (strcmp(section->type, "event") == 0) {
                insert_event_into_linked_list(head,ctx,section);
            }
        }
cleanup:
        uci_free_context(ctx);
        return rc;
}