//https://isocpp.org/wiki/faq/mixing-c-and-cpp

 //really good iso cpp 23 

 #include "cplusplus.hpp"
#include "string.hpp"
#include "../stdlib/print.h"
//we never ended up using cpp in kernel
//linus was right
extern "C" void _init_cpp(){

}



extern "C" int cpp_test(int a, int b){

    dts::string piss = "c++ was supposed to be nice ";
    dts::string me = "however it is stoopid";
    dts::string off = "i C the appeal now";

    println(piss.append(me).c_str());
    println(off + dts::string(" ha get it do you get it do you get the joke"));

    auto c = a + b; //i might cry hello friend
                //I TAKE IT ALL BACK CPP U STINK
    return c;
}