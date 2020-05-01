


#ifndef HANDY_UTIL_H_
#define HANDY_UTIL_H_

//工具库

#include<string>
#include<string.h>
#include<vector>
#include<cstdlib>
#include<utility>
#include<functional>



namespace handy {

    struct noncopyable {
        noncopyable() = default;
        virtual ~noncopyable() = default;
        noncopyable(const noncopyable&) = delete;
        noncopyable operator = (const noncopyable&) = delete;
    };


    struct util {

        static std::string format(char* fmt, ...);
        static int64_t timeMicro();
        static int64_t timeMilli() {return timeMicro() / 1000;}
        static int64_t steadyMicro();
        static int64_t steadyMilli() {return steadyMicro()/1000;}

        static std::string readableTime(time_t);
        static int64_t atoi(const char *b, const char *e) { return strtol(b, (char **) &e, 10); }
        static int64_t atoi2(const char *b, const char *e) {
            char **ne = (char **) &e;
            int64_t v = strtol(b, ne, 10);
            return ne == (char **) &e ? v : -1;
        }
        static int64_t atoi(const char *b) { return atoi(b, b + strlen(b)); }
        static int addFdFlag(int fd, int flag);
    };

    struct ExitCaller : private noncopyable {
        ~ExitCaller() { functor_(); }
        ExitCaller(std::function<void()> &&functor) : functor_(std::move(functor)) {}

    private:
        std::function<void()> functor_;
    };


}


#endif