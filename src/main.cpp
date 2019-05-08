#include <iostream>
#include <unistd.h>
#include <libconfig.h++>

using namespace std;
using namespace libconfig;

#include "class/FileSystem.hpp"
#include "class/HttpServer.hpp"
#include "class/TorHttpRequest.hpp"

/* global variables */
string tld;
string proxyHost;
int proxyPort;
string userAgent;

/* determines which config file to use and returns its path */
string getConfigFile(){
    string result = "./torgate.conf";

    string fileList[] = {"./torgate.conf","./bin/torgate.conf","/etc/torgate.conf"};
    for(const string &filePath : fileList){
        if(FileSystem::fileExists(filePath)){
            result = filePath;
        }
    }

    return result;
}

/* handles http requests */
HttpResult handleRequest(string host, string method, string url, string data){
    HttpResult result;

    /* create tor http request */
    string targetHost = TorHttpRequest::getTargetHostName(host,tld);
    TorHttpRequest* request = new TorHttpRequest(targetHost,url);

    /* set the proxy configuration */
    request->setProxy(proxyHost,proxyPort);

    /* set the user agent */
    request->setUserAgent(userAgent);

    /* perform the get request */
    TorHttpResponse response = request->get();

    /* transfer all headers */
    for(int i=0; i<response.headerList.size(); i++){
        HttpResultHeader header;
        header.name = response.headerList[i].name;
        header.value = response.headerList[i].value;
        result.headerList.push_back(header);
    }

    /* set status code and content */
    result.status = response.status;
    result.content = response.content;

    return result;
}

/* handles the main application execution */
int main(){
    /* output a banner for this service */
    cout << "Torgate v0.1" << endl;

    /* create config instance */
    Config cfg;

    /* open the main configuration file */
    try{
        /* read the configuration file */
        string configFile = getConfigFile();
        cout << "Config file: " << configFile << endl;
        cfg.readFile(configFile.c_str());
    }catch(const FileIOException &fioex){
        /* flush any i/o error into stderr */
        cout << "I/O error while config reading file.";
        return(EXIT_FAILURE);
    }catch(const ParseException &pex){
        /* flush any parser error into stderr */
        cout << "Parse error at " << (string)pex.getFile()
                            << ": Line " << std::to_string(pex.getLine())
                            << " - " << pex.getError() << endl;
        return(EXIT_FAILURE);
    }

    /* read the configuration settings */
    int servicePort = 80;
    const Setting& root = cfg.getRoot();
    root.lookupValue("port",servicePort);
    root.lookupValue("tld",tld);
    root.lookupValue("proxyHost",proxyHost);
    root.lookupValue("proxyPort",proxyPort);
    root.lookupValue("userAgent",userAgent);

    /* create instance of the http server */
    HttpServer* server = new HttpServer(servicePort,(void*)handleRequest);

    /* start the http server */
    server->start();

    /* wait while the server operates */
    while(server->isActive() == true){
        sleep(1);
    }

    /* output closing message */
    cout << "Services terminated" << endl;

    return 0;
}