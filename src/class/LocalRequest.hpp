#include <iostream>
#include <vector>
using namespace std;

#include "HttpServer.hpp"

#pragma once

class LocalRequest{
    public:
        LocalRequest(string localPath, string url){
            this->path = localPath;
            this->fileName = url;
        }

        HttpResult execute();
        HttpResult getError(int status);

    private:
        HttpResult getDefaultResult();
        string getErrorContent(int statusCode);

        string path;
        string fileName;
};