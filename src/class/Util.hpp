#include <iostream>
#include <algorithm> 
#include <cctype>
#include <locale>

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

        static void hexchar(unsigned char c, unsigned char &hex1, unsigned char &hex2){
            hex1 = c / 16;
            hex2 = c % 16;
            hex1 += hex1 <= 9 ? '0' : 'a' - 10;
            hex2 += hex2 <= 9 ? '0' : 'a' - 10;
        }

        static string urlencode(string s){
            const char *str = s.c_str();
            vector<char> v(s.size());
            v.clear();
            for (size_t i = 0, l = s.size(); i < l; i++)
            {
                char c = str[i];
                if ((c >= '0' && c <= '9') ||
                    (c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') ||
                    c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
                    c == '*' || c == '\'' || c == '(' || c == ')')
                {
                    v.push_back(c);
                }
                else if (c == ' ')
                {
                    v.push_back('+');
                }
                else
                {
                    v.push_back('%');
                    unsigned char d1, d2;
                    hexchar(c, d1, d2);
                    v.push_back(d1);
                    v.push_back(d2);
                }
            }

            return string(v.cbegin(), v.cend());
        }

        // trim from start (in place)
        static inline void ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));
        }

        // trim from end (in place)
        static inline void rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

        // trim from both ends (in place)
        static inline void trim(std::string &s) {
            ltrim(s);
            rtrim(s);
        }

        // trim from start (copying)
        static inline std::string ltrim_copy(std::string s) {
            ltrim(s);
            return s;
        }

        // trim from end (copying)
        static inline std::string rtrim_copy(std::string s) {
            rtrim(s);
            return s;
        }

        // trim from both ends (copying)
        static inline std::string trim_copy(std::string s) {
            trim(s);
            return s;
        }
};