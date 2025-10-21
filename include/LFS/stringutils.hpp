#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <string>

inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

inline void remove_tailing_carriage_return(std::string& str)
{
    size_t pos = str.find('\r', str.size() - 1);

    if (pos != std::string::npos)
    {
        str.erase(str.size() - 1);
    }
}

#endif //STRINGUTILS_HPP
