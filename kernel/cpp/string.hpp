#pragma once

#include "stdcpp.hpp"
namespace dts{


class string
{
public:
    string() : len(0), data(nullptr) {}
 
    string(const char* str){
        len = strlen(str);
        if(!len) return;
        data = new char[len + 1];
        strcpy(data, str);
    }
    ~string(){
        delete[] data;
    }

    string(const string& copy){
        len = copy.len;
        data = new char[len + 1];
        strcpy(data, copy.data);
    }

    string& operator=(const string& copy){
        if(this != &copy){
            delete[] data;
            len = copy.len;
            data = new char[len + 1];
            strcpy(data, copy.data);

        }
        return *this;
    }

    string operator+(const string& concat) const {
        string ret; 
        ret.len = len + concat.len;
        ret.data = new char[ret.len + 1];

        strcpy(ret.data, data);
        strcpy(ret.data + len, concat.data);
        return ret;
    }

    string& append(const string& add){
        char* buf = new char[len + add.len + 1];
        strcpy(buf, data);
        strcpy(buf + len, add.data);
        delete[] data;
        data = buf;
        len += add.len;

        return *this;
    }
    string substr(size_t start){ //will finish when needed
        return string(data + start);
    }

    operator const char*() const {
        return data;
    }

    inline char at(size_t idx) { if(idx > len) return 0; return data[idx]; }
    inline auto length() const { return this->size(); }
    inline size_t size() const { return len; }
    inline const char* c_str() const { return data; }
private:
    size_t len;
    char* data;
};

}