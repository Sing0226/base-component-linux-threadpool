#ifndef __RSLOG_H_
#define __RSLOG_H_
#include "rslogger_declare.h"
#include <mutex>
#include <string>


class RS_CLASS_DECL RsLog
{
public:
    static RsLog& Instance();

    void setLogPath(const char* logPath);

    void setBaseLogName(const char* logName);

    void setLogLevel(ERsLogLevel logLevel);
    ERsLogLevel getLogLevel();

    void reset(const char* title = NULL);

    void trace(const char* file, int line, const char* func, const char* msg, ...);

    void debug(const char* file, int line, const char* func, const char* msg, ...);

    void info(const char* file, int line, const char* func, const char* msg, ...);

    void warn(const char* file, int line, const char* func, const char* msg, ...);

    void error(const char* file, int line, const char* func, const char* msg, ...);

    void fatal(const char* file, int line, const char* func, const char* msg, ...);

    void output(const char* msg, int len);

    const char* getLogLevelName(ERsLogLevel logLevel);

    void setMaxLogFiles(int maxFiles);


protected:
   int sprintf(ERsLogLevel log_level, const char* file, int line, const char* func, const char* msg, va_list args);

   bool is_loggable(ERsLogLevel level);

   bool rollFile();

   bool clearExpiredLogs();

private:
    RsLog();

    RsLog(const RsLog& log);
    RsLog& operator=(const RsLog& log);

    ~RsLog();

    static RsLog* instance_;
    static std::mutex instance_mutex_;
    // static RsLog instance_;
    class RsLoggerDc
    {
    public:
        RsLoggerDc()
        {

        }
        ~RsLoggerDc()
        {
            if (instance_!= nullptr)
            {
                delete instance_;
                instance_ = nullptr;
            }
        }
    };

    static RsLoggerDc s_rs_log_dc_;

private:
    std::string m_log_path_;
    std::string m_file_name_;
    std::string m_base_file_name_;
    std::string log_title_;
    ERsLogLevel log_level_;
    char* msg_;
    std::mutex mutex_;
    int m_maxLogFiles;
};

//////////////////////////////////////////////////////////////////////////
#define g_RsLog (RsLog::Instance())


#endif // RSLOG_H
