#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <netdb.h>
#include <arpa/inet.h>

#include "../include/ManipulateStr.h"
#include "../include/global.h"

#define PORTMAX 65535 // 2^16 - 1
#define PORTMIN 1

namespace ManipulateStr {
    
    const int str2int(const std::string &s) {
        int d;
        std::istringstream ss(s);
        ss.imbue(std::locale::classic());
        ss >> d;
        return d;
    }
    
    const std::string int2str(int d) {
        std::stringstream ss;
        ss << d;
        return ss.str();
    }
    
    
    const std::vector<std::string> split(const std::string &s, char sep) {
        std::istringstream ss(s);
        std::string token;
        std::vector<std::string> v;
        
        while(std::getline(ss, token, sep)) {
            v.push_back(token);
        }
        return v;
    }

    const bool validIp(const std::string &ip) {
        sockaddr_in sa;
        int check = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
        if(check == 1)
            return true;
        else 
        return false;
    }
    
    const bool validPort(const std::string &port) {
        for (std::string::const_iterator it = port.begin(); it != port.end(); ++it) {
            if (int(*it) < int('0') or int(*it) > int('9'))
                return false;
        }
        for (int i = 0; i < port.length(); i++){
            if (isdigit(port[i]) == false){
                return false;
            }
        }
        
        int p = ManipulateStr::str2int(port);
        if (p < PORTMIN or p > PORTMAX)
            return false;
        
        return true;
    }
    

};