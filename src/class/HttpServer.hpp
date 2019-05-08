#include <iostream>
#include <vector>
#include <algorithm>
#include <microhttpd.h>
using namespace std;

#pragma once

struct HttpResultHeader {
    string name;
    string value;
};

struct HttpResult {
    int status;
    vector<HttpResultHeader> headerList;
    string content;

    /* returns the content type of this result */
    string getContentType(){
        string result = "";

        for(int i=0; i<headerList.size(); i++){
            string headerName = headerList[i].name;
            transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
            if(headerName == "content-type"){
                result = headerList[i].value;
            }
        }
        
        return result;
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

    private:
        /* private members */
        int port;
        void* handler;
        struct MHD_Daemon* daemonHandle;
};