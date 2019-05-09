#include <iostream>
#include <curl/curl.h>
using namespace std;

#include "TorHttpRequest.hpp"
#include "Util.hpp"

/* rewrites the hostname for the request to route through Tor/Onion */
string TorHttpRequest::getTargetHostName(string sourceHost, string tld){
    string result = sourceHost;

    /* check if the domain ends with the tld */
    if(Util::stringEndsWith(sourceHost,tld)){
        result = sourceHost.substr(0,sourceHost.length()-tld.length());
    }

    return result;
}

void TorHttpRequest::setRequestHeader(vector<pair<string, string>> value){
    /* list with all client request headers allowed to be forwarded */
    vector<string> alist = {
        "Cookie", "Referer"
    };

    /* iterate through all headers and check if they're valid */
    for(int i=0; i<value.size(); i++){
        bool valid = false;

        /* match headers against allowed list */
        for(int a=0; a<alist.size(); a++){
            if(value[i].first == alist[a]){
                /* header is allowed */
                valid = true;
            }
        }

        /* add to local var when header is valid */
        if(valid == true){
            this->requestHeaderList.push_back(value[i]);
        }
    }
}

/* writes the response headers */
size_t TorHttpRequest::writeResponseHeader(char* b, size_t size, size_t nitems, void* userdata){
    string headerString = b;

    /* vector with all valid headers to return */
    vector<string> vlist = {
        "Content-Type", "Content-Language", "Cache-Control", "Expires", "Location", "Set-Cookie"
    };
    
    if(headerString.size() > 0){
        if(headerString.find(":") != string::npos){
            vector<TorHttpResponseHeader>* headerList = (vector<TorHttpResponseHeader>*)userdata;
            TorHttpResponseHeader header;

            /* parse header name and value */
            header.name = headerString.substr(0, headerString.find(":"));
            header.value = headerString.substr(headerString.find(":")+1);

            /* trim the header name and value */
            Util::trim(header.name);
            Util::trim(header.value);

            /* check if header is allowed for response */
            if(std::find(vlist.begin(), vlist.end(), header.name) != vlist.end()) {
                headerList->push_back(header);
            }
        }

        /* check if new set of headers is coming up after a redirect */
        if(headerString.substr(0,5) == "HTTP/"){
            /* clear out all headers in the vector */
            vector<TorHttpResponseHeader>* headerList = (vector<TorHttpResponseHeader>*)userdata;
            headerList->clear();
        }
    }

    return nitems * size;
}

/* executes a GET requests against the defined Host and URL */
TorHttpResponse TorHttpRequest::execute(string method, string data){
    TorHttpResponse result;

    /* initialise curl handle */
    CURL *curl;

    /* define the full request url */
    string url = "http://" + this->requestHost + this->requestUrl;

    /* buffer to write result into */
    string readBuffer;
    vector<TorHttpResponseHeader> headerList;

    /* initialise curl */
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    if(this->verbose){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); /* 1L = verbose, 0L = off */
    }

    /* add headers if any were supplied */
    if(this->requestHeaderList.size() > 0){
        struct curl_slist *chunk = NULL;

        for(int h=0; h<this->requestHeaderList.size(); h++){
            string headerText = requestHeaderList[h].first + ": " + requestHeaderList[h].second;
            chunk = curl_slist_append(chunk, headerText.c_str());
        }

        /* pass the header list to curl */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    /* check if method is post and send post data */
    if(method == "POST"){
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_USERAGENT, this->userAgent.c_str());
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, TorHttpRequest::writeResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, false); 
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, TorHttpRequest::writeResponseHeader);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerList);

    /* check if a proxy is defined */
    if(this->proxyHost.length() > 0 && this->proxyPort > 0){
        /* define the socks5 proxy with hostname resolve and pass to curl */
        string proxyUrl = "socks5h://" + this->proxyHost + ":" + std::to_string(this->proxyPort);
        curl_easy_setopt(curl, CURLOPT_PROXY, proxyUrl.c_str());
    }

    /* kill and clean up curl */
    CURLcode res = curl_easy_perform(curl);
    if(res == CURLE_OK) {
        /* get the HTTP response code */
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.status);
    }else{
        /* reset the content, set default error */
        result.content = "";

        /* 
            502 Bad Gateway 
            The server was acting as a gateway or proxy 
            and received an invalid response from the 
            upstream server.
        */
        result.status = 502;

        /* check specific error */
        if(res == CURLE_OPERATION_TIMEDOUT){
            /*
                504 Gateway Timeout
                The server was acting as a gateway or proxy 
                and did not receive a timely response from 
                the upstream server.
            */
            result.status = 504;
        }
    }

    /* clean up curl */
    curl_easy_cleanup(curl);

    /* build up the result struct */
    result.headerList = headerList;
    result.content = readBuffer;

    return result;
}