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
void read_settings (struct uci_context *ctx ,struct uci_section *section,struct Configuration *config);
/*
Function reads topic and saves it to linked list
*/
void insert_topic_into_linked_list(struct Topic **head,struct uci_context *ctx ,struct uci_section *section);
/*
Function reads event and saves it to linked list
*/
void insert_event_into_linked_list(struct Topic **head,struct uci_context *ctx ,struct uci_section *section);
/*
Function reads configuration file
*/
int uci_read_config(struct Topic **head, struct Configuration *config);
