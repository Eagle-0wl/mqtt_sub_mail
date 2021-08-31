#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <mosquitto.h>
#include <mqtt_db.h>
#include <mqtt_config_parsing.h>
#include <json-c/json.h>
#include <mqtt_payload_parsing.h>

#define CERTPATH "/etc/ssl/certs/mosquitto.org.crt"

volatile int interrupt = 0;

void signal_handler(int signo) {
    signal(SIGINT, NULL);
    syslog(LOG_INFO, "Received signal: %d", signo);
    closelog();
    interrupt = 1;
}


void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    struct Topic *ptr = obj;
	if(rc) {
        syslog (LOG_ERR, "Couldn't communicate to broker");
		exit(EXIT_FAILURE);
	}
    
    while(ptr != NULL) {                 //goes through all the topics in the list and subscribes to them
        mosquitto_subscribe(mosq, NULL, ptr->topic, atoi(ptr->qos));
        ptr = ptr->next;
    }
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    insert_message_to_database ((char *) msg->topic, (char *) msg->payload);

    if(json_tokener_parse(msg->payload) != NULL)                                        //checks if json is valid
        check_for_events_and_parse(obj,(char *) msg->topic, (char *) msg->payload);     //call function to check for events and parse them if they exist
    else
        syslog (LOG_INFO, "Message from publisher isn't valid json");
}

int start_mossquitto (struct Topic *head, struct Configuration *config)
{
    int rc=0;
    struct mosquitto *mosq = NULL;
    struct sigaction action;
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    sigaction(SIGTERM, &action, NULL);

    rc = mosquitto_lib_init();              //initiates mosquitto     
    if (rc != 0){
        syslog(LOG_ERR, "Failed to initialize mossquito");
        goto cleanup2;
    }else{
        syslog (LOG_INFO,"Mosquitto initialized successfuly");
    }
    mosq = mosquitto_new("Mqtt sub program", true, head);
    if (config->username!=NULL  && config->password!=NULL) {        //checks if username and password is not empty
        if (mosquitto_username_pw_set(mosq, config->username ,config->password)==MOSQ_ERR_SUCCESS){     //sets username and password for mosquitto connection
            syslog(LOG_INFO, "User name and password added successfuly");
        }else{
            syslog(LOG_WARNING, "Failed to add username or password");
        }
    } 
       
    if (atoi(config->use_tls)==1){          //checks if tls enabled
        if (!mosquitto_tls_set(mosq,CERTPATH,NULL, NULL, NULL, NULL)== MOSQ_ERR_SUCCESS){ //sets tls for mosquitto connection
            syslog(LOG_WARNING, "Failed to set tls"); 
        }
    }
    
	mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    rc = mosquitto_connect(mosq, config->address, atoi(config->port), 10); // starts mosquitto connection
	if(rc) {
        syslog(LOG_ERR, "Could not connect to Broker");
		goto cleanup2;
	}else{
         syslog(LOG_INFO, "Connection to broker successful");
    }
    
	rc = mosquitto_loop_start(mosq); //starts mosquitto loop
    if(rc) {
        syslog(LOG_ERR, "Failed to start mosquitto loop");
		goto cleanup1;
	}
    syslog (LOG_INFO,"Mosquitto loop started");

    while(!interrupt) {  }  //makes program a daemon so it will wait for mosquitto messages

	mosquitto_loop_stop(mosq, true); //stops mosquitto loop

cleanup1:
	mosquitto_disconnect(mosq);     //disconnects mosquitto
cleanup2:
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
    return rc;
}