#include <iostream>
using namespace std;

#include "LocalRequest.hpp"
#include "FileSystem.hpp"

HttpResult LocalRequest::execute(){
    HttpResult result = this->getDefaultResult();

    /* check if the filename is empty */
    if(this->fileName == "/" or this->fileName.empty() == true){
        /* set default file to index.html */
        this->fileName = "/index.html";
    }

    /* check if the file exists */
    string file = this->path + this->fileName;
    if(FileSystem::fileExists(file)){
        result.status = 200;
        result.content = FileSystem::readFile(file);
        result.setHeader("Content-Type",FileSystem::getMimeType(file));
    }else{
        result.status = 404;
        result.content = this->getErrorContent(404);
    }

    return result;
}

/* returns an error page */
HttpResult LocalRequest::getError(int status){
    HttpResult result = this->getDefaultResult();
    result.status = status;
    result.content = this->getErrorContent(status);
    return result;
}

/* check ifs an error page exists in the path */
string LocalRequest::getErrorContent(int statusCode){
    string result = "Error " + std::to_string(statusCode);

    /* create the error file path */
    string errorFile = this->path + "/error/" + std::to_string(statusCode) + ".html";
    if(FileSystem::fileExists(errorFile)){
        result = FileSystem::readFile(errorFile);
    }

    return result;
}

HttpResult LocalRequest::getDefaultResult(){
    /* set the default result (500) */
    HttpResult result;
    result.status = 500;
    result.content = "500 - Server error";

    /* default is just html */
    result.setHeader("Content-Type","text/html");

    return result;
}