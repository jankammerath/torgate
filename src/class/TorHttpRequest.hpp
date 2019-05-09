#include <iostream>
#include <vector>
using namespace std;

#pragma once

struct TorHttpResponseHeader {
    string name;
    string value;
};

struct TorHttpResponse {
    vector<TorHttpResponseHeader> headerList;
    string content;
    int status;
};

class TorHttpRequest {
    public:
        /* constructor sets host and url */
        TorHttpRequest(string host, string url){
            this->requestHost = host;
            this->requestUrl = url;
            this->verbose = false;
            this->userAgent = "Torgate/42";
        };

        /* sets proxy config for socks proxy */
        void setProxy(string host,int port){
            this->proxyHost = host;
            this->proxyPort = port;
        }

        /* sets user agent to identify with */
        void setUserAgent(string name){
            this->userAgent = name;
        }

        /* defines whether verbose output is requested */
        void setClientVerbose(bool value){
            this->verbose = value;
        }

        /* sets the client request headers */
        void setRequestHeader(vector<pair<string, string>> value);

        /* executes HTTP get */
        TorHttpResponse execute(string method, string data);

        /* gets the rewritten target host name */
        static string getTargetHostName(string sourceHost, string tld);

    private:
        /* private methods */
        static size_t writeResponse(void *contents, size_t size, size_t nmemb, void *userp){
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        }

        static size_t writeResponseHeader(char* b, size_t size, size_t nitems, void* userdata);

        /* private variables */
        string requestHost;
        string requestUrl;
        string proxyHost;
        int proxyPort;
        string userAgent;
        bool verbose;
        vector<pair<string, string>> requestHeaderList;
};