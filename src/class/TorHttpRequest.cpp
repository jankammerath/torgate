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

/* executes a GET requests against the defined Host and URL */
TorHttpResponse TorHttpRequest::get(){
    TorHttpResponse result;

    /* initialise curl handle */
    CURL *curl;

    /* define the full request url */
    string url = "http://" + this->requestHost + "/" + this->requestUrl;

    /* buffer to write result into */
    string readBuffer;

    /* initialise curl */
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, TorHttpRequest::writeResponse);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true); 

    /* check if a proxy is defined */
    if(this->proxyHost.length() > 0 && this->proxyPort > 0){
        /* define the socks5 proxy with hostname resolve and pass to curl */
        string proxyUrl = "socks5h://" + this->proxyHost + ":" + std::to_string(this->proxyPort);
        curl_easy_setopt(curl, CURLOPT_PROXY, proxyUrl.c_str());
    }

    /* kill and clean up curl */
    result.status = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    /* build up the result struct */
    result.content = readBuffer;

    return result;
}