#include "logging.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <utility>
#include "port_posix.h"



namespace handy {

    //构造函数.
    Logger::Logger() : level_(LINFO), lastRotate_(time(NULL)), rotateInterval_(86400) {//86400s = 24h
        tzset();
        fd_ = -1;
        realRotate_ = lastRotate_;
    }

    Logger::~Logger() {
        if(-1 != fd_) {
            close(fd_);
        }
    }

    const char *Logger::levelStrs_[LALL + 1] = {"FATAL", "ERROR", "UERR", "WARN", "INFO", "DEBUG", "TRACE", "ALL"};

    Logger& Logger::getLogger() { 
        //在c++中static变量会在所有线程开始之前初始化。
        static Logger logger;
        return logger;
     }

     void Logger::setLogLevel(const std::string& level) {
         LogLevel item = LogLevel::LINFO;
         for(size_t indx = 0; indx<sizeof(levelStrs_) / sizeof(const char*); indx++) {
             if(0 == strcasecmp(levelStrs_[indx], level)) {
                 item = LogLevel(indx);
                 break;
             }
         }
         setLogLevel(item);
     }

    void Logger::setFileName(const std::string& filename) {
        int fd = open(filename.c_str(), O_APPEND | O_RDONLY | O_WRONLY | O_CLOEXEC, DEFFILEMODE);
        if(0 > fd) {
            fprintf(stderr, "open log file %s failed. msg: %s ignored\n", filename.c_str(), strerror(errno));
            return;
        }

        filename_ = filename;
        if(-1 == fd_) fd_ = fd;
        else {
            int r = dup2(fd, fd_);          //dup的原理需要去了解，我现在只能记得大概了。
            fatalif(r < 0; "dup2 failed.");
            close(fd);
        }
    }

    void Logger::maybeRotate() {
        time_t now = time(NULL);
        if(filename_.empty() || (now - timezone)/rotateInterval_ == (lastRotate_-timezone)/rotateInterval_) {
            return;
        }

        lastRotate_ = now;
        long old = realRotate_.exchange(now);
        //如果realRotate的值是新的，那么返回，否则，获得了旧值，进行rotate
        if ((old - timezone) / rotateInterval_ == (lastRotate_ - timezone) / rotateInterval_) {
            return; 
        }

        struct tm ntm;
        localtime_r(&now, &ntm);
        char newname[4096];
        snprintf(newname, sizeof(newname), "%s.%d%02d%02d%02d%02d", filename_.c_str(), ntm.tm_year + 1900, ntm.tm_mon + 1, ntm.tm_mday, ntm.tm_hour, ntm.tm_min);
        const char* oldname = filename_.c_str();
        int err = rename(oldname, newname);
        if(0 != err) {
            fprintf(stderr, "rename logfile %s -> %s failed msg: %s\n", oldname, newname, strerror(errno));
            return;
        }
        int fd = open(filename_.c_str(), O_APPEND | O_CREAT | O_WRONLY | O_CLOEXEC, DEFFILEMODE);
        if(0 > fd) {
            fprintf(stderr, "open log file %s failed. msg: %s ignored\n", newname, strerror(errno));
            return;
        }
        dup2(fd, fd_);
        close(fd);
    }

    static thread_local uint64_t tid;
    void Logger::logv(Logger::LogLevel level, const char* file, const char* func, const char*fmt...) {
        if(0 == tid) {
            
        }
    }

}