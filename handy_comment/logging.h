
#ifndef HANDY_LOGGING_H_
#define HANDY_LOGGING_H_



#include<stdio.h>
#include<string>
#include<atomic>
#include"util.h"




namespace handy {


    struct Logger : private noncopyable {

        enum LogLevel { LFATAL=0, LERROR, LUERR, LWARN, LINFO, LDEBUG, LTRACE, LALL };
        Logger();
        ~Logger();
        void logv(LogLevel level, const char* file, int line, const char* func, const char*fmt...);

        void setFileName(const std::string& filename);
        void setLogLevel(const std::string& level);
        void setLogLevel(LogLevel level) { level_ = level;}

        LogLevel getLogLevel() {return level_;}
        const char* getLogLevelStr() {return levelStrs_[level_];}
        int getFd() { return fd_; }


        void adjustLogLevel(int adjust) { setLogLevel(level_ + adjust);}
        void setRotateInterval(long rotateInterval) { retateInterval_ = rotateInterval; }
        
        
        //获得单例.
        static Logger& getLogger();


        private:
            void maybeRotate();
            static const char *levelStrs_[LALL + 1];
            int fd_;
            LogLevel level_;
            long lastRotate_;                           //最后一次写入日志的时间戳
            std::atomic<int64_t> realRotate_;           //真实
            long rotateInterval_;                       //写入时间间隔
            std::string filename_;

    };
}







#endif