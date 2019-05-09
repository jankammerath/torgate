#include <iostream>

using namespace std;

#include "HttpServer.hpp"

#pragma once

class RewriteEngine {
    public:
        /* initialises the engine with the tld */
        RewriteEngine(string tld){
            this->domainTld = tld;
        }

        /* rewrites an http result */
        void rewriteHttpResult(HttpResult* value);
        string rewriteSourceUrl(string url);
        string rewriteTargetUrl(string url);
    
    private:
        /* private methods */
        bool isRewritableResult(HttpResult* value);

        /* private variables */
        string domainTld;
};