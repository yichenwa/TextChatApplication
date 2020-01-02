#ifndef ManipulateStr_H_
#define ManipulateStr_H_

#include <string>
#include <vector>

#include "global.h"

namespace ManipulateStr {
    
    const int str2int(const std::string&);
    const std::string int2str(int);
    const std::vector<std::string> split(const std::string &s, char sep);
    const bool validIp(const std::string &);
    const bool validPort(const std::string &);
   
};

#endif