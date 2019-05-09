#include <iostream>
#include <microhttpd.h>
#include <cstring>
#include <sstream>
using namespace std;
#include "HttpServer.hpp"
#include "Util.hpp"

#define POSTBUFFERSIZE 512

struct postStatus {
    bool status;
    char *buff;
};

string HttpServer::serverBanner;

HttpServer::HttpServer(int serverPort, void* requestHandler){
    this->port = serverPort;
    this->handler = requestHandler;
    HttpServer::serverBanner = "Torgate/42";
}

/* starts the http server */
void HttpServer::start(){
    this->daemonHandle = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY 
                                        | MHD_USE_DEBUG 
                                        | MHD_USE_THREAD_PER_CONNECTION
                                        | MHD_USE_TCP_FASTOPEN,
		       this->port, NULL, NULL, &HttpServer::handleRequest, this->handler, MHD_OPTION_END);
}

/* attaches get parameters to the url provided in value pointer */
int HttpServer::attachGetParameters (void *cls, enum MHD_ValueKind kind, const char *key, const char* value){
    vector<HttpResultHeader>* getParamList = (vector<HttpResultHeader>*)cls;
    HttpResultHeader param;
    param.name = key;

    /* query string params that do not have
        a value assigned (e.g. ?nothing ) will
        have value being a null pointer */
    if(value == NULL){
        param.value = "";
    }else{
        param.value = value;
    }

    getParamList->push_back(param);
    return MHD_YES;
}

/* handles all http requests to this server */
int HttpServer::handleRequest(void * cls, struct MHD_Connection * connection,
		    const char * url, const char * method, const char * version,
            const char * upload_data, size_t * upload_data_size, void ** ptr){
    int result;
    struct MHD_Response* response;

    /* convert method, url to string */
    string urlString(url); 
    string methodString(method);
    string uploadData;

    /* The request headers need to be whitelabeled and forwarded */
    vector<HttpResultHeader> requestHttpHeaderList;
    MHD_get_connection_values (connection, MHD_HEADER_KIND, 
                HttpServer::attachGetParameters, &requestHttpHeaderList);
    
    vector<pair<string, string>> requestHeaderList;
    for(int x=0; x<requestHttpHeaderList.size(); x++){
        requestHeaderList.push_back(requestHttpHeaderList[x].getPair());
    }

    /* get all get-parameters to attach to URL */
    vector<HttpResultHeader> getParamList;
    MHD_get_connection_values (connection, MHD_GET_ARGUMENT_KIND, 
                HttpServer::attachGetParameters, &getParamList);

    /* add all parameters */
    for(int p=0; p<getParamList.size(); p++){
        if(p == 0){
            /* append query string question mark */
            urlString.append("?");
        }else{
            /* append query string ampersand mark */
            urlString.append({'&'});
        }

        /* append the actual param and value */
        urlString.append(getParamList[p].name + "=" 
            + Util::urlencode(getParamList[p].value));
    }

    /* handle POST/PUT operations and theor upload data:
        https://stackoverflow.com/questions/36416422/process-post-data-in-microhttp-server-in-c */
    if(ptr != NULL && methodString == "POST"){
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

    /* get the requested hostname as string */
    string hostName = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Host");

    /* execute external request handler */
    HttpResult (*handlerFunc)(string,string,string,string,vector<pair<string, string>>) = (HttpResult(*)(string,string,string,string,vector<pair<string, string>>))cls;
    HttpResult httpResult = handlerFunc(hostName,methodString,urlString,uploadData,requestHeaderList);

    /* set the content to be returned */
    string content = httpResult.content;

    /* it is absolutely vital to use MHD_RESPMEM_MUST_COPY as otherwise memory exceptions will occur.
        See further information here: https://www.gnu.org/software/libmicrohttpd/manual/html_node/microhttpd_002dresponse-create.html */
    response = MHD_create_response_from_buffer (content.size(),(void*)content.c_str(),MHD_RESPMEM_MUST_COPY);

    /* add all headers to the result */
    for(int h=0; h<httpResult.headerList.size(); h++){
        MHD_add_response_header (response, httpResult.headerList[h].name.c_str(), 
                            httpResult.headerList[h].value.c_str());
    }

    /* add the server banner to the result */
    MHD_add_response_header (response, "Server", HttpServer::serverBanner.c_str());

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