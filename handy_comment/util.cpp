#include"util.h"
#include <fcntl.h>
#include <stdarg.h>
#include <algorithm>
#include <chrono>
#include <memory>




//format...
std::string handy::util::format(char* fmt, ...) {

    char* base;
    char buffer[500];
    std::unique_ptr<char[]> release1;
    
    for(int iter= 0; iter<2; iter++) {

        int bufsize = 0;
        if(0 == iter) {
            bufsize = sizeof(buffer);
            base = buffer;
        } else {
            bufsize = 30000;
            base = new char[bufsize];
            release1.reset(base);
        }

        char* offptr = base;
        char* endptr = base+bufsize;
        if(offptr < endptr) {
            va_list args;
            va_start(args, fmt);
            offptr += vsnprintf(offptr, endptr - offptr, fmt, args);
            va_end(args);
        } 

        //vsnprintf:  写结果到字符串 buffer 。至多写入 buf_size 个字符。产生的字符串将以空字符终止，除非 buf_size 为零。若 buf_size 为零，则不写入任何内容，且 buffer 可为空指针，然而照样计算并返回返回值（本应写入的字节数，不包含空终止符）。
        if( offptr >= endptr ) {
            if(0 == iter) continue; //如果参数写入长度超过buffer[500]， 那就执行iter == 2的操作，重新申请内存空间30000的。
            else {
                offptr = endptr - 1;
                *offptr = '\0';
            }
        }
        break;  //如果没有超出长度就直接返回.
    }
    return base;
}


int64_t handy::util::timeMicro() {
    std::chrono::time_point<std::chrono::system_clock> p = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(p.time_since_epoch()).count();
}


int64_t handy::util::steadyMicro() {
    std::chrono::time_point<std::chrono::steady_clock> p = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(p.time_since_epoch()).count();
}


std::string handy::util::readableTime(time_t t) {
    struct tm tm1;
    localtime_r(&t, %tm1);
    return format("%04d-%02d-%02d %02d:%02d:%02d", tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
}


int handy::util::addFdFlag(int fd, int flag) {
    int ret = fcntl(fd, F_GETFD);
    return fcntl(fd, F_SETFD, ret | flag);
}