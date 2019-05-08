#include <iostream>
#include <microhttpd.h>
#include <cstring>
#include <sstream>
using namespace std;
#include "HttpServer.hpp"

struct postStatus {
    bool status;
    char *buff;
};

HttpServer::HttpServer(int serverPort, void* requestHandler){
    this->port = serverPort;
    this->handler = requestHandler;
}

/* starts the http server */
void HttpServer::start(){
    this->daemonHandle = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
		       this->port, NULL, NULL, &HttpServer::handleRequest, this->handler, MHD_OPTION_END);
}

/* handles all http requests to this server */
int HttpServer::handleRequest(void * cls, struct MHD_Connection * connection,
		    const char * url, const char * method, const char * version,
            const char * upload_data, size_t * upload_data_size, void ** ptr){
    int result = 0;
    struct MHD_Response* response;

    /* convert method, url to string */
    string urlString(url); 
    string methodString(method);
    string uploadData;

    /* get the requested hostname as string */
    string hostName = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Host");

    /* handle POST/PUT operations and theor upload data:
        https://stackoverflow.com/questions/36416422/process-post-data-in-microhttp-server-in-c */
    if(ptr != NULL && method == "POST"){
        struct postStatus *post = NULL;
        post = (struct postStatus*)*ptr;

        /* this checks if post data was supplied behind the ptr */
        if(post == NULL) {
            post = (struct postStatus*)malloc(sizeof(struct postStatus));
            post->status = false;
            *ptr = post;
        }

        if(!post->status) {
            post->status = true;
            return MHD_YES;
        } else {
            if(*upload_data_size != 0) {
                /* copy the upload data to the buffer */
                post->buff = (char*)malloc(*upload_data_size + 1);
                strncpy(post->buff, upload_data, *upload_data_size);
                post->buff[*upload_data_size] = 0;
                *upload_data_size = 0;
                return MHD_YES;
            } else {
                /* buff will be NULL for GET and empty POST-bodies 
                    and if it is not checked here, string.assign will segfault */
                if(post->buff != NULL){
                    uploadData.assign(post->buff);
                }
                free(post->buff);
            }
        }

        /* free the post struct if still there */
        if(post != NULL){
            free(post); 
        }
    }

    /* execute external request handler */
    HttpResult (*handlerFunc)(string,string,string,string) = (HttpResult(*)(string,string,string,string))cls;
    HttpResult httpResult = handlerFunc(hostName,methodString,urlString,uploadData);

    /* it is absolutely vital to use MHD_RESPMEM_MUST_COPY as otherwise memory exceptions will occur.
        See further information here: https://www.gnu.org/software/libmicrohttpd/manual/html_node/microhttpd_002dresponse-create.html */
    response = MHD_create_response_from_buffer (httpResult.content.size(),(void*)httpResult.content.c_str(),MHD_RESPMEM_MUST_COPY);
    MHD_add_response_header (response, "Content-Type", httpResult.contentType.c_str());
    result = MHD_queue_response(connection, httpResult.status, response);
    MHD_destroy_response(response);

    /* clear context pointer */
    *ptr = NULL; 

    return result;
}

/* stops the http server */
void HttpServer::stop(){
    if(this->daemonHandle != NULL){
        MHD_stop_daemon(this->daemonHandle);
    }
}

/* returns true when server running, otherwise false */
bool HttpServer::isActive(){
    bool result = false;

    if(this->daemonHandle != NULL){ result = true; }

    return result;
}