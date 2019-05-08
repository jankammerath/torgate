#include <iostream>

#pragma once

using namespace std;

class Util{
    public:
        static bool stringEndsWith(string const &fullString, string const &ending) {
            if (fullString.length() >= ending.length()) {
                return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
            } else {
                return false;
            }
        }
};