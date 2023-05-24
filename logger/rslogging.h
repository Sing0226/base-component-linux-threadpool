#ifndef __RSLOGGING_H_
#define __RSLOGGING_H_
#include "rslogger_declare.h"
#include "logstream.h"
#include <stdio.h>
#include <time.h>
#include "rslog.h"


class RsLoggerImpl;
/// <summary>
/// SourceFile
/// </summary>
class RS_CLASS_DECL SourceFile
{
public:
    template <int N>
    SourceFile(const char(&arr)[N]) : data_(arr), size_(N - 1)
    {
#ifdef __linux__
        const char* slash = strrchr(data_, '/');
#elif defined(_WIN64) || defined(_WIN32)
        const char* slash = strrchr(data_, '\\');
#endif
        if (slash)
        {
            data_ = slash + 1;
            size_ -= static_cast<int>(data_ - arr);
        }
    }

    explicit SourceFile(const char* filename) : data_(filename)
    {
#ifdef __linux__
        const char* slash = strrchr(filename, '/');
#elif defined(_WIN64) || defined(_WIN32)
        const char* slash = strrchr(filename, '\\');
#endif
        if (slash)
        {
            data_ = slash + 1;
        }
        size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
};

/// <summary>
/// RsLogger
/// </summary>
class RS_CLASS_DECL RsLogger
{
public:
  RsLogger(SourceFile file, int line);
  RsLogger(SourceFile file, int line, ERsLogLevel level);
  RsLogger(SourceFile file, int line, ERsLogLevel level, const char *func);
  RsLogger(SourceFile file, int line, bool toAbort);
  ~RsLogger();

  static ERsLogLevel logLevel();
  static void setLogLevel(ERsLogLevel level);

  RsLogStream &stream();
  // static void setTimeZone(const struct timezone &tz);

private:
  RsLoggerImpl* m_impl_;
};

#define RSLOG_TRACE                                          \
  if (RsLogger::logLevel() <= ERsLogLevel::RS_LOG_TRACE) \
  (RsLogger(__FILE__, __LINE__, ERsLogLevel::RS_LOG_TRACE, __func__).stream())
#define RSLOG_DEBUG                                          \
  if (RsLogger::logLevel() <= ERsLogLevel::RS_LOG_DEBUG) \
  (RsLogger(__FILE__, __LINE__, ERsLogLevel::RS_LOG_DEBUG, __func__).stream())
#define RSLOG_INFO                                          \
  if (RsLogger::logLevel() <= ERsLogLevel::RS_LOG_INFO) \
  (RsLogger(__FILE__, __LINE__, ERsLogLevel::RS_LOG_INFO, __func__).stream())
#define RSLOG_WARN (RsLogger(__FILE__, __LINE__, ERsLogLevel::RS_LOG_WARN).stream())
#define RSLOG_ERROR (RsLogger(__FILE__, __LINE__, ERsLogLevel::RS_LOG_ERROR).stream())
#define RSLOG_FATAL (RsLogger(__FILE__, __LINE__, ERsLogLevel::RS_LOG_FATAL).stream())

#define RSLOG_SYSERR (RsLogger(__FILE__, __LINE__, false).stream())
#define RSLOG_SYSFATAL (RsLogger(__FILE__, __LINE__, true).stream())

const char *strerror_tl(int savedErrno);

//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

    RS_FUNC_DECL const char* source_file(const char* filePath);

#ifdef __cplusplus
}
#endif
#define __RS_FILE__ source_file(__FILE__)
// LOG_TRACE
#define LOG_TRACE_F(x1) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1)
#define LOG_TRACE_F2(x1, x2) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2)
#define LOG_TRACE_F3(x1, x2, x3) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3)
#define LOG_TRACE_F4(x1, x2, x3, x4) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4)
#define LOG_TRACE_F5(x1, x2, x3, x4, x5) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5)
#define LOG_TRACE_F6(x1, x2, x3, x4, x5, x6) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6)
#define LOG_TRACE_F7(x1, x2, x3, x4, x5, x6, x7) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7)
#define LOG_TRACE_F8(x1, x2, x3, x4, x5, x6, x7, x8) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8)
#define LOG_TRACE_F9(x1, x2, x3, x4, x5, x6, x7, x8, x9) g_RsLog.trace(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8, x9)

// RSLOG_INFO
#define RSLOG_INFO_F(x1) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1)
#define RSLOG_INFO_F2(x1, x2) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2)
#define RSLOG_INFO_F3(x1, x2, x3) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3)
#define RSLOG_INFO_F4(x1, x2, x3, x4) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4)
#define RSLOG_INFO_F5(x1, x2, x3, x4, x5) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5)
#define RSLOG_INFO_F6(x1, x2, x3, x4, x5, x6) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6)
#define RSLOG_INFO_F7(x1, x2, x3, x4, x5, x6, x7) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7)
#define RSLOG_INFO_F8(x1, x2, x3, x4, x5, x6, x7, x8) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8)
#define RSLOG_INFO_F9(x1, x2, x3, x4, x5, x6, x7, x8, x9) g_RsLog.info(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8, x9)

// RSLOG_DEBUG
#define RSLOG_DEBUG_F(x1) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1)
#define RSLOG_DEBUG_F2(x1, x2) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2)
#define RSLOG_DEBUG_F3(x1, x2, x3) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3)
#define RSLOG_DEBUG_F4(x1, x2, x3, x4) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4)
#define RSLOG_DEBUG_F5(x1, x2, x3, x4, x5) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5)
#define RSLOG_DEBUG_F6(x1, x2, x3, x4, x5, x6) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6)
#define RSLOG_DEBUG_F7(x1, x2, x3, x4, x5, x6, x7) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7)
#define RSLOG_DEBUG_F8(x1, x2, x3, x4, x5, x6, x7, x8) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8)
#define RSLOG_DEBUG_F9(x1, x2, x3, x4, x5, x6, x7, x8, x9) g_RsLog.debug(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8, x9)


// RSLOG_ERROR
#define RSLOG_ERROR_F(x1) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1)
#define RSLOG_ERROR_F2(x1, x2) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2)
#define RSLOG_ERROR_F3(x1, x2, x3) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3)
#define RSLOG_ERROR_F4(x1, x2, x3, x4) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4)
#define RSLOG_ERROR_F5(x1, x2, x3, x4, x5) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5)
#define RSLOG_ERROR_F6(x1, x2, x3, x4, x5, x6) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6)
#define RSLOG_ERROR_F7(x1, x2, x3, x4, x5, x6, x7) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7)
#define RSLOG_ERROR_F8(x1, x2, x3, x4, x5, x6, x7, x8) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8)
#define RSLOG_ERROR_F9(x1, x2, x3, x4, x5, x6, x7, x8, x9) g_RsLog.error(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8, x9)


// LOG_WARNING
#define LOG_WARN_F(x1) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1)
#define LOG_WARN_F2(x1, x2) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2)
#define LOG_WARN_F3(x1, x2, x3) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3)
#define LOG_WARN_F4(x1, x2, x3, x4) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4)
#define LOG_WARN_F5(x1, x2, x3, x4, x5) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5)
#define LOG_WARN_F6(x1, x2, x3, x4, x5, x6) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6)
#define LOG_WARN_F7(x1, x2, x3, x4, x5, x6, x7) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7)
#define LOG_WARN_F8(x1, x2, x3, x4, x5, x6, x7, x8) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8)
#define LOG_WARN_F9(x1, x2, x3, x4, x5, x6, x7, x8, x9) g_RsLog.warn(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8, x9)


// LOG_FATAL
#define LOG_FATAL_F(x1) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1)
#define LOG_FATAL_F2(x1, x2) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2)
#define LOG_FATAL_F3(x1, x2, x3) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3)
#define LOG_FATAL_F4(x1, x2, x3, x4) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4)
#define LOG_FATAL_F5(x1, x2, x3, x4, x5) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5)
#define LOG_FATAL_F6(x1, x2, x3, x4, x5, x6) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6)
#define LOG_FATAL_F7(x1, x2, x3, x4, x5, x6, x7) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7)
#define LOG_FATAL_F8(x1, x2, x3, x4, x5, x6, x7, x8) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8)
#define LOG_FATAL_F9(x1, x2, x3, x4, x5, x6, x7, x8, x9) g_RsLog.fatal(source_file(__FILE__), __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8, x9)


#endif // RSLOGGING_H
