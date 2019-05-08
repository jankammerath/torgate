#include <iostream>
#include <microhttpd.h>
using namespace std;

#pragma once

struct HttpResult {
    int status;
    string content;
    string contentType;
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