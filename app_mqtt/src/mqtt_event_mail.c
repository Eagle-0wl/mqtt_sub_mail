#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <curl/curl.h>
#include <mqtt_config_parsing.h>

#define EMAIL ""              //NEED to define email for program to work
#define PASSWORD ""                  //NEED to define email password for program to work

char *payload_text = "";
 
struct upload_status {
  size_t bytes_read;
};

/*
Fucntion prepares data for curl to use
*/
static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;
  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }
  data = &payload_text[upload_ctx->bytes_read];
  if(data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;
    return len;
  }
 
  return 0;
}
/*
Sends email using curl
*/
void send_email(struct Event *ptr, char *payload){
    size_t needed;
    needed = snprintf(NULL, 0, "Subject: Event\r\n\r\nMessage from topic \"%s\": %s: %s\r\n\r\n",ptr->topic, ptr->key, payload+1);  //get the size needed for message
    payload_text = (char*) malloc(needed);                                                                                          //allocate size needed for message
    sprintf(payload_text, "Subject: Event\r\n\r\nMessage from topic \"%s\": %s: %s\r\n\r\n",ptr->topic, ptr->key, payload);         //set message
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = { 0 };
 
    curl = curl_easy_init();
    if(curl) {
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, EMAIL); 
        curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);       
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com");      //mail server
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, EMAIL);
        recipients = curl_slist_append(recipients, ptr->email);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        /* Send the message */
        res = curl_easy_perform(curl);
    
        /* Check for errors */
        if(res != CURLE_OK)
        syslog(LOG_ERR, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    
        /* Free the list of recipients */
        curl_slist_free_all(recipients);
    
        /* Always cleanup */
        curl_easy_cleanup(curl);
    }
    free(payload);      //free allocated memory
}