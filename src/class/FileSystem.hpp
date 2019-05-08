#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
using namespace std;

#pragma once

class FileSystem {
    public:
        /* returns true when a file exists, otherwise false */
        static bool fileExists(string name){
            struct stat buffer;   
            return (stat (name.c_str(), &buffer) == 0);     
        }

        /* creates a directory using unix 'mkdir' equivalent */
        static bool makeDir(string path){
            bool result = false;

            int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if(status != 0){
                /* error creating the directory */
                cout << "Failed to create path: " << path << endl;
            }else{
                /* successfully created directory */
                result = true;
            }

            return result;
        }

        /* returns string with all filenames in the directory provided in path */
        static vector<string> getDirectoryFileList(string path){
            vector<string> result;

            DIR *dp;
            struct dirent *dirp;
            if((dp  = opendir(path.c_str())) != NULL) {
                while ((dirp = readdir(dp)) != NULL) {
                    result.push_back(string(dirp->d_name));
                }
                closedir(dp);   
            }        

            return result;
        }

        /* writes content into the defined filename */
        static void writeFile(string fileName, string content){
            ofstream fileOutput(fileName);
            fileOutput << content;
            fileOutput.close();
        }

        /* reads a file and returns its content as string */
        static string readFile(string fileName){
            ifstream fileInput(fileName);
            string result((istreambuf_iterator<char>(fileInput)),
                                istreambuf_iterator<char>());
            return result;
        }
};