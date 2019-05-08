#include <iostream>
using namespace std;

#include "LocalRequest.hpp"
#include "FileSystem.hpp"

HttpResult LocalRequest::execute(){
    HttpResult result = this->getDefaultResult();

    /* check if the filename is empty */
    if(this->fileName == "/" or this->fileName.empty() == true){
        /* set default file to index.html */
        this->fileName = "index.html";
    }

    /* check if the file exists */
    string file = this->path + this->fileName;
    if(FileSystem::fileExists(file)){
        result.status = 200;
        result.content = FileSystem::readFile(file);
    }
}

HttpResult LocalRequest::getDefaultResult(){
    /* set the default result (500) */
    HttpResult result;
    result.status = 500;
    result.content = "500 - Server error";

    /* default is just html */
    HttpResultHeader contentTypeHeader;
    contentTypeHeader.name = "Content-Type";
    contentTypeHeader.value = "text/html";
    result.headerList.push_back(contentTypeHeader);

    return result;
}