#include <iostream>
#include <vector>
#include <algorithm>
#include <microhttpd.h>
using namespace std;

#pragma once

struct HttpResultHeader {
    string name;
    string value;

    pair<string, string> getPair(){
        return pair<string, string>(name, value);
    }
};

struct HttpResult {
    int status;
    vector<HttpResultHeader> headerList;
    string content;

    /* returns a vector with a pair containing all headers */
    vector<pair<string, string>> getList(){
        vector<pair<string, string>> result;

        for(int i=0; i<headerList.size(); i++){
            pair<string, string> item(headerList[i].name,headerList[i].value);
            result.push_back(item);
        }

        return result;
    }

    /* returns the content type of this result */
    string getHeader(string header){
        string result = "";

        for(int i=0; i<headerList.size(); i++){
            string headerName = headerList[i].name;
            transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
            transform(header.begin(), header.end(), header.begin(), ::tolower);
            if(headerName == header){
                result = headerList[i].value;
            }
        }
        
        return result;
    }

    /* sets the content type or mime-type for this result */
    void setHeader(string header, string value){
        bool contentTypeExists = false;

        for(int i=0; i<headerList.size(); i++){
            string headerName = headerList[i].name;
            transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
            transform(header.begin(), header.end(), header.begin(), ::tolower);
            if(headerName == header){
                headerList[i].value = value;
                contentTypeExists = true;
            }
        }

        if(!contentTypeExists){
            HttpResultHeader contentTypeHeader;
            contentTypeHeader.name = header;
            contentTypeHeader.value = value;
            this->headerList.push_back(contentTypeHeader);
        }
    }
};

class HttpServer {
    public:
        /* public methods */
        HttpServer(int serverPort, void* requestHandler);
        void start();
        void stop();
        bool isActive();
        static int handleRequest(void * cls,
                    struct MHD_Connection * connection,
                    const char * url,
                    const char * method,
                    const char * version,
                    const char * upload_data,
                    size_t * upload_data_size,
                    void ** ptr);
        static int attachGetParameters (void *cls, enum MHD_ValueKind kind, 
                                        const char *key, const char* value);
        
        void setServerBanner(string value){
            HttpServer::serverBanner = value;
        }
    private:
        /* private members */
        int port;
        void* handler;
        struct MHD_Daemon* daemonHandle;

        /* static public member */
        static string serverBanner;
};