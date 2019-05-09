#include <iostream>
#include <regex>
#include <iterator>
using namespace std;

#include "RewriteEngine.hpp"
#include "Util.hpp"

void RewriteEngine::rewriteHttpResult(HttpResult* value){
    /* check if the result can be rewritten as binary files,
        images etc. do not need to be rewritten and also 
        cannot be rewritten easily */
    if(this->isRewritableResult(value)){
        /* regex replace all onion appearances:
            ([a-zA-Z0-9]*)\.onion  */
        regex regExOnion("([a-zA-Z0-9]*)\\.onion");
        string original = value->content;
        string rewriteResult;

        /* regex place the content */
        regex_replace (back_inserter(rewriteResult), 
                    original.begin(), original.end(), 
                    regExOnion, "$1.onion."+this->domainTld);

        /* update content with rewritten result */
        value->content = rewriteResult;
    }
}

string RewriteEngine::rewriteSourceUrl(string url){
    string result = "";

    /* add the tld */
    regex regExOnion("([a-zA-Z0-9]*)\\.onion");
    regex_replace (back_inserter(result), url.begin(), url.end(), regExOnion, "$1.onion."+this->domainTld);

    return result;
}

string RewriteEngine::rewriteTargetUrl(string url){
    string result = "";

    /* remove the tld from any params */
    regex regExOnion("([a-zA-Z0-9]*)\\.onion." + this->domainTld);
    regex_replace (back_inserter(result), url.begin(), url.end(), regExOnion, "$1.onion");

    return result;
}

bool RewriteEngine::isRewritableResult(HttpResult* value){
    bool result = false;

    /* get the content type */
    string contentType = value->getHeader("Content-Type");

    /* define the rewritable content types */
    vector<string> rewritableTypeList = { 
        "text/html", "text/css", "application/javascript",
        "application/x-javascript", "text/javascript",
        "application/json", "text/x-javascript",
        "text/x-json"
    };

    /* check if this content is rewritable */
    for(int m=0; m<rewritableTypeList.size(); m++){
        if (contentType.find(rewritableTypeList[m]) != string::npos) {
            /* this is rewritable */
            result = true;
        }
    }

    return result;
}