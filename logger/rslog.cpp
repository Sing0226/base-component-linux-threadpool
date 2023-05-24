#include "rslog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include "timestamp.h"
#include "fmt.h"
#include <cassert>
#include "curthread.h"
#include <stdarg.h>
#include <string.h>
#include <vector>
#ifdef __linux__
#include <dirent.h>
#include <unistd.h>
#elif defined(_WIN64) || defined(_WIN32)
#include <Shlobj.h>
#include <io.h>
#include <direct.h>
#include "win/dirent.h"
#include <process.h>
#endif

RsLog* RsLog::instance_ = nullptr;
RsLog::RsLoggerDc RsLog::s_rs_log_dc_;
std::mutex RsLog::instance_mutex_;
// RsLog RsLog::instance_;

#define DEFAULT_LOG_DIR "/rs_default_dir"
#define RS_LOG_DIR "/log"
#define DEFAULT_FILE_NAME_PREFIXX "RS-LOG-PROJECT-"

#define BUF_SIZE 2048
#define MAX_BUF_SIZE 3*1024*1024    // 3M

#define LOGGER_HEADER_LINUX "Header Linux Log Start..."
#define LOGGER_HEADER_WIN "Header Windows Log Start..."

#define MAX_LOG_SIZE_TEST 1024
#define MAX_LOG_SIZE 1024*1024*100



#if defined(__linux__)
static void get_pid_by_proc_name(pid_t *pid, char *task_name)
{
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[50];
    char cur_task_name[50];
    char buf[BUF_SIZE+1];

    dir = opendir("/proc");
    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL) //循环读取/proc下的每一个文件/文件夹
        {
            //如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (DT_DIR != ptr->d_type)
                continue;

            sprintf(filepath, "/proc/%s/status", ptr->d_name);//生成要读取的文件的路径
            fp = fopen(filepath, "r");
            if (NULL != fp)
            {
                if( fgets(buf, BUF_SIZE, fp)== NULL ){
                    fclose(fp);
                    continue;
                }
                sscanf(buf, "%*s %s", cur_task_name);

                //如果文件内容满足要求则打印路径的名字（即进程的PID）
                if (!strcmp(task_name, cur_task_name)){
                    sscanf(ptr->d_name, "%d", pid);
                }
                fclose(fp);
            }
        }
        closedir(dir);
    }
}
#endif
const char* RsLogLevelName[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static std::string getLogFileName(const std::string &basename)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    time_t now = time(NULL);
#ifdef __linux__
    localtime_r(&now, &tm);
#elif defined(_WIN64) || defined(_WIN32)
    localtime_s(&tm, &now);
#endif // __linux__
    strftime(timebuf, sizeof(timebuf), "%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    char hostname[256];
    if (::gethostname(hostname, sizeof(hostname)) == 0)
    {
        hostname[sizeof(hostname) - 1] = '\0';
        filename += hostname;
    }
    else
      filename += "unknownhost";

    char pidbuf[32];
#ifdef __linux__
    snprintf(pidbuf, sizeof pidbuf, ".%d", ::getpid());
#elif defined(_WIN64) || defined(_WIN32)
    sprintf_s(pidbuf, sizeof(pidbuf), ".%d", ::getpid());
#endif
    filename += pidbuf;

    filename += ".log";

    return filename;
}

#ifdef __linux__
static bool get_proc_name_by_pid(pid_t pid, char *task_name)
{
    bool res = false;
    char proc_pid_path[BUF_SIZE + 1];
    char buf[BUF_SIZE + 1];

    sprintf(proc_pid_path, "/proc/%d/status", pid);
    FILE* fp = fopen(proc_pid_path, "r");
    if(NULL != fp){
        if( fgets(buf, BUF_SIZE, fp)== NULL ){
            fclose(fp);
        }
        fclose(fp);
        sscanf(buf, "%*s %s", task_name);
        res = true;
    }

    return res;
}
#endif

static std::string formatCurrentTime()
{
    RsTimestamp timeStamp(RsTimestamp::now());
    int64_t microSecondsSinceEpoch = timeStamp.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / RsTimestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % RsTimestamp::kMicroSecondsPerSecond);
    struct tm tm_time;
#ifdef __linux__
    localtime_r(&seconds, &tm_time);
#elif defined(_WIN64) || defined(_WIN32)
    localtime_s(&tm_time, &seconds);
#endif // __linux__

    char t_time[64];
    memset(t_time, 0, 64);
#ifdef __linux__
    int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17);
#elif defined(_WIN64) || defined(_WIN32)
    int len = sprintf_s(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17);
#endif
    std::string currTime = t_time;

    Fmt us(".%06d ", microseconds);
    assert(us.length() == 8);
    currTime.append(us.data(), 8);

    return currTime;
}

RsLog& RsLog::Instance()
{
    if (instance_ == nullptr)
    {
        std::lock_guard<std::mutex> lock(instance_mutex_);
        if (instance_ == nullptr)
        {
            instance_ = new RsLog();
        }
    }

    return *instance_;
}

RsLog::RsLog()
    : m_base_file_name_(DEFAULT_FILE_NAME_PREFIXX)
#ifdef __linux__
    , log_title_(LOGGER_HEADER_LINUX)
#elif defined(_WIN64) || defined(_WIN32)
    , log_title_(LOGGER_HEADER_WIN)
#endif
    , log_level_(ERsLogLevel::RS_LOG_WARN)
    , msg_(nullptr)
{
#ifdef __linux__
    char* homePath = getenv("HOME");
    m_log_path_ = homePath;
    char procName[256];
    memset(procName, 0, 256);
    pid_t pid = getpid();
    bool bRes = get_proc_name_by_pid(pid, procName);
    if (bRes)
    {
        m_log_path_.append("/.config/");
        m_log_path_.append(procName);
        if (access(m_log_path_.c_str(), 6) != 0)
        {
            mkdir(m_log_path_.c_str(), 0755);
        }
        m_log_path_.append(RS_LOG_DIR);
        if (access(m_log_path_.c_str(), 6) != 0)
        {
            mkdir(m_log_path_.c_str(), 0755);
        }
    }
    else
    {
        m_log_path_.append("/.config/");
        m_log_path_.append(DEFAULT_LOG_DIR);
        if (access(m_log_path_.c_str(), 6)  != 0)
        {
            mkdir(m_log_path_.c_str(), 0755);
        }
        m_log_path_.append(RS_LOG_DIR);
        if (access(m_log_path_.c_str(), 6)  != 0)
        {
            mkdir(m_log_path_.c_str(), 0755);
        }
    }
#elif defined(_WIN64) || defined(_WIN32)
    char m_lpszDefaultDir[MAX_PATH];
    char szDocument[MAX_PATH] = { 0 };
    memset(m_lpszDefaultDir, 0, _MAX_PATH);

    LPITEMIDLIST pidl = NULL;
    SHGetSpecialFolderLocation(NULL, CSIDL_LOCAL_APPDATA, &pidl);
    if (pidl && SHGetPathFromIDList(pidl, szDocument))
    {
        GetShortPathName(szDocument, m_lpszDefaultDir, _MAX_PATH);
    }
    m_log_path_ = m_lpszDefaultDir;
    char moduleFile[MAX_PATH];
    memset(moduleFile, 0, sizeof(moduleFile));
    GetModuleFileName(NULL, moduleFile, MAX_PATH);
    m_log_path_.append("\\");

    std::string strProcName;
    char* prefixName = strrchr(moduleFile, '\\');
    if (prefixName != NULL)
    {
        prefixName++;
    }
    if (prefixName && *prefixName != '\0')
    {
        char* pSuffix = strrchr(prefixName, '.');
        if (pSuffix != NULL)
        {
            *pSuffix = '\0';
            m_log_path_.append(prefixName);
        }
        else
        {
            m_log_path_.append("unknown_app");
        }  
    }
    else
    {
        m_log_path_.append("unknown_app");
    }
   
    if (_access(m_log_path_.c_str(), 6) == -1)
    {
        _mkdir(m_log_path_.c_str());
    }
#endif

    m_file_name_ = getLogFileName(m_base_file_name_);

    msg_ = new char[BUF_SIZE + 1];

    reset();
}

RsLog::~RsLog()
{
    if (msg_)
    {
        delete msg_;
        msg_ = nullptr;
    }
}


void RsLog::setLogPath(const char* logPath)
{
    if (logPath == nullptr)
        return;

    if (m_log_path_ == logPath)
    {
        return;
    }

    const char *tokseps = "/";
    char szLogPath[1024];
    memset(szLogPath, 0, 1024);
#ifdef __linux__
    snprintf(szLogPath, 1023, "%s", logPath);
#elif defined(_WIN64) || defined(_WIN32)
    sprintf_s(szLogPath, sizeof(szLogPath), "%s", logPath);
#endif

    std::string tmpDirPath;
    if (szLogPath[0] == '/')
    {
        tmpDirPath = "/";
    }

    char* pt = strtok(szLogPath, tokseps);
    while (pt)
    {
        tmpDirPath.append(pt);
#ifdef __linux__
        if (access(tmpDirPath.c_str(), 6)  != 0)
        {
            mkdir(tmpDirPath.c_str(), 0755);
        }
#elif defined(_WIN64) || defined(_WIN32)
        if (_access(tmpDirPath.c_str(), 6) == -1)
        {
            _mkdir(tmpDirPath.c_str());
        }
#endif
        pt = strtok(NULL,tokseps);
        if (pt)
        {
            tmpDirPath.append("/");
        }
    }

    std::lock_guard<std::mutex> lock(mutex_);
    m_log_path_ = tmpDirPath;
    reset(log_title_.c_str());
}

void RsLog::setBaseLogName(const char* logName)
{
    if (logName == nullptr)
        return;

    std::lock_guard<std::mutex> lock(mutex_);
    m_base_file_name_ = logName;
    m_file_name_ = getLogFileName(m_base_file_name_);
    reset(log_title_.c_str());
}

void RsLog::setLogLevel(ERsLogLevel logLevel)
{
    log_level_ = logLevel;
}

ERsLogLevel RsLog::getLogLevel()
{
    return log_level_;
}

void RsLog::reset(const char* title)
{
#ifdef __linux__
    log_title_ = (title) ? title : LOGGER_HEADER_LINUX;
#elif defined(_WIN64) || defined(_WIN32)
    log_title_ = (title) ? title : LOGGER_HEADER_WIN;
#endif

    char logFilePath[1024];
    memset(logFilePath, 0, 1024);
#ifdef __linux__
    snprintf(logFilePath, 1023, "%s/%s", m_log_path_.c_str(), m_file_name_.c_str());
#elif defined(_WIN64) || defined(_WIN32)
    sprintf_s(logFilePath, sizeof(logFilePath), "%s/%s", m_log_path_.c_str(), m_file_name_.c_str());
#endif
   
    FILE* fp = fopen(logFilePath, "w+");
    if (fp)
    {
        std::string currentTime = formatCurrentTime();
#ifdef __linux__
        fprintf(fp, "%s - # %s \n", currentTime.c_str(), log_title_.c_str());
#elif defined(_WIN64) || defined(_WIN32)
        fprintf_s(fp, "%s - # %s \n", currentTime.c_str(), log_title_.c_str());
#endif
        fflush(fp);
        fclose(fp);
    }
}

void RsLog::trace(const char* file, int line, const char* func, const char* msg, ...)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_loggable(ERsLogLevel::RS_LOG_TRACE))
    {
        va_list args;
        va_start(args, msg);
        sprintf(ERsLogLevel::RS_LOG_TRACE, file, line, func, msg, args);
        va_end(args);
    }
}

void RsLog::debug(const char* file, int line, const char* func, const char* msg, ...)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_loggable(ERsLogLevel::RS_LOG_DEBUG))
    {
        va_list args;
        va_start(args, msg);
        sprintf(ERsLogLevel::RS_LOG_DEBUG, file, line, func, msg, args);
        va_end(args);
    }
}

void RsLog::info(const char* file, int line, const char* func, const char* msg, ...)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_loggable(ERsLogLevel::RS_LOG_INFO))
    {
        va_list args;
        va_start(args, msg);
        sprintf(ERsLogLevel::RS_LOG_INFO, file, line, func, msg, args);
        va_end(args);
    }
}
void RsLog::warn(const char* file, int line, const char* func, const char* msg, ...)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_loggable(ERsLogLevel::RS_LOG_WARN))
    {
        va_list args;
        va_start(args, msg);
        sprintf(ERsLogLevel::RS_LOG_WARN, file, line, nullptr, msg, args);
        va_end(args);
    }
}
void RsLog::error(const char* file, int line, const char* func, const char* msg, ...)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_loggable(ERsLogLevel::RS_LOG_ERROR))
    {
        va_list args;
        va_start(args, msg);
        sprintf(ERsLogLevel::RS_LOG_ERROR, file, line, nullptr, msg, args);
        va_end(args);
    }
}

void RsLog::fatal(const char* file, int line, const char* func, const char* msg, ...)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_loggable(ERsLogLevel::RS_LOG_FATAL))
    {
        va_list args;
        va_start(args, msg);
        sprintf(ERsLogLevel::RS_LOG_FATAL, file, line, nullptr, msg, args);
        va_end(args);
    }
}

void RsLog::output(const char* msg, int len)
{
     std::lock_guard<std::mutex> lock(mutex_);
     char logFilePath[1024];
     memset(logFilePath, 0, 1024);
#ifdef  __linux__
     snprintf(logFilePath, 1023, "%s/%s", m_log_path_.c_str(), m_file_name_.c_str());
#elif defined(_WIN64) || defined(_WIN32)
     sprintf_s(logFilePath, sizeof(logFilePath), "%s/%s", m_log_path_.c_str(), m_file_name_.c_str());
#endif //  __linux__

     FILE* fp = fopen(logFilePath, "a");
     if (fp)
     {
         fprintf(fp, "%s", msg);
         fflush(fp);
         fclose(fp);
         rollFile();
     }
}


const char* RsLog::getLogLevelName(ERsLogLevel logLevel)
{
    if (logLevel == ERsLogLevel::RS_LOG_TRACE)
    {
        return RsLogLevelName[0];
    }
    else if (logLevel == ERsLogLevel::RS_LOG_DEBUG)
    {
        return RsLogLevelName[1];
    }
    else if (logLevel == ERsLogLevel::RS_LOG_INFO)
    {
        return RsLogLevelName[2];
    }
    else if (logLevel == ERsLogLevel::RS_LOG_WARN)
    {
        return RsLogLevelName[3];
    }
    else if (logLevel == ERsLogLevel::RS_LOG_ERROR)
    {
        return RsLogLevelName[4];
    }
    else if (logLevel == ERsLogLevel::RS_LOG_FATAL)
    {
        return RsLogLevelName[5];
    }
    return RsLogLevelName[2];
}

int RsLog::sprintf(ERsLogLevel log_level, const char* file, int line, const char* func, const char* msg, va_list args)
{
    int ret = -1;
    if (file == NULL || func == NULL || msg == NULL)
    {
        return ret;
    }

    char logFilePath[1024];
    memset(logFilePath, 0, 1024);
#ifdef __linux__
    snprintf(logFilePath, 1023, "%s/%s", m_log_path_.c_str(), m_file_name_.c_str());
#elif defined(_WIN64) || defined(_WIN32)
    sprintf_s(logFilePath, sizeof(logFilePath), "%s/%s", m_log_path_.c_str(), m_file_name_.c_str());
#endif
    

    FILE* fp = fopen(logFilePath, "a");
    if (fp)
    {
        CurrentThread::tid(); //取得该线程的tid
        // 输入线程id
        std::string currentTime = formatCurrentTime();
        const char* sourceFile = nullptr;
        if (func == nullptr)
        {
#ifdef __linux__
            fprintf(fp, "%s [%s] - %s - ", currentTime.c_str(),
                getLogLevelName(log_level),
                CurrentThread::tidString());
#elif defined(_WIN64) || defined(_WIN32)
            fprintf_s(fp, "%s [%s] - %s - ", currentTime.c_str(),
                getLogLevelName(log_level),
                CurrentThread::tidString());
#endif
        }
        else
        {
#ifdef __linux__
            fprintf(fp, "%s [%s] - %s - %s: ", currentTime.c_str(),
               getLogLevelName(log_level),
                CurrentThread::tidString(),
                func);
#elif defined(_WIN64) || defined(_WIN32)
            fprintf_s(fp, "%s [%s] - %s - %s: ", currentTime.c_str(),
                getLogLevelName(log_level),
                CurrentThread::tidString(),
                func);
#endif
        }
        ret = strlen(msg);
        bool reSetNew = false;
        if (ret > BUF_SIZE)
        {
            delete msg_;
            if (ret >= MAX_BUF_SIZE)
            {
                ret = MAX_BUF_SIZE;
            }
            int tmpLen = ret + 1;
            msg_ = new char[tmpLen];
            reSetNew = true;
        }
        else
        {
            ret = BUF_SIZE;
        }

        memset(msg_, 0, sizeof(char)*(ret + 1));
#ifdef __linux__
        vsnprintf(msg_, ret, msg, args);
        fprintf(fp, "%s - %s:%d\n", msg_, file, line);
#elif defined(_WIN64) || defined(_WIN32)
        vsprintf_s(msg_, ret, msg, args);
        fprintf_s(fp, "%s - %s:%d\n", msg_, file, line);
#endif

        fflush(fp);
        fclose(fp);

        if (reSetNew)
        {
            delete msg_;
            msg_ = new char[BUF_SIZE + 1];
        }
        rollFile();
    }

    return ret;
}

bool RsLog::is_loggable(ERsLogLevel level)
{
    return log_level_ <= level;
}

//日志滚动
bool RsLog::rollFile()
{
    struct stat statbuf;

    char logFilePath[1024];
    memset(logFilePath, 0, 1024);
#ifdef __linux__
    snprintf(logFilePath, 1023, "%s/%s", m_log_path_.c_str(), m_file_name_.c_str());
#elif defined(_WIN64) || defined(_WIN32)
    sprintf_s(logFilePath, sizeof(logFilePath), "%s/%s", m_log_path_.c_str(), m_file_name_.c_str());
#endif
    
    int ret = stat(logFilePath, &statbuf);
    if (ret != 0)
    {
        return false;
    }

    if (statbuf.st_size >= MAX_LOG_SIZE)
    {
        std::string newFileName = getLogFileName(m_base_file_name_);
        if (newFileName == m_file_name_)
        {
            bool stopflag = false;
            unsigned int lastID = 1;
            char newname[1024];
            char oldname[1024];
            // find the next available name
            while(!stopflag)
            {
                memset(newname, 0, 1024);
                
#ifdef __linux__
                snprintf(newname, 1023, "%s/%s.%d", m_log_path_.c_str(), m_file_name_.c_str(), lastID);
#elif defined(_WIN64) || defined(_WIN32)
                sprintf_s(newname, sizeof(newname), "%s/%s.%d", m_log_path_.c_str(), m_file_name_.c_str(), lastID);
#endif
                FILE* tmpfile = fopen(newname, "r");
                if (tmpfile)
                {
                    fclose(tmpfile);
                    lastID++;
                }
                else
                {
                    stopflag = true;
                }
            }

            // rename
            for(int t=lastID;t>1;t--)
            {
                memset(newname, 0, 1024);
                memset(oldname, 0, 1024);
#ifdef __linux__
                snprintf(newname, 1023, "%s/%s.%d", m_log_path_.c_str(), m_file_name_.c_str(), lastID);
                snprintf(oldname, 1023, "%s/%s.%d", m_log_path_.c_str(), m_file_name_.c_str(), lastID - 1);
#elif defined(_WIN64) || defined(_WIN32)
                sprintf_s(newname, sizeof(newname), "%s/%s.%d", m_log_path_.c_str(), m_file_name_.c_str(), lastID);
                sprintf_s(oldname, sizeof(oldname), "%s/%s.%d", m_log_path_.c_str(), m_file_name_.c_str(), lastID - 1);
#endif
                rename(oldname, newname);
            }
#ifdef __linux__
            snprintf(newname, 1023, "%s/%s.%d", m_log_path_.c_str(), m_file_name_.c_str(), 1);
#elif defined(_WIN64) || defined(_WIN32)
            sprintf_s(newname, sizeof(newname), "%s/%s.%d", m_log_path_.c_str(), m_file_name_.c_str(), 1);
#endif           
            remove(newname);
            rename(logFilePath, newname);
        }
        m_file_name_ = newFileName;
        reset(log_title_.c_str());

        return true;
    }

    return false;
}


void RsLog::setMaxLogFiles(int maxFiles)
{
    m_maxLogFiles = maxFiles;
}

bool RsLog::clearExpiredLogs()
{
    if(m_maxLogFiles <= 0)  // 不限制
        return false;

    DIR *d;
    struct dirent* file = new dirent();
    if(!(d = opendir(m_log_path_.c_str())))
    {
        printf("erroe dirs %s!!!\n", m_log_path_.c_str());
        return false;
    }

    std::vector<std::string> vecLogsFiles;
    int count = 0;
    while((file = readdir(d)) != NULL)
    {
        if(strncmp(file->d_name, ".", 1) == 0
                || strncmp(file->d_name, "..", 1) == 0)
             continue;

        // 如果是普通的文件
        if(file->d_type == 8)
        {
            int len = m_log_path_.length() + strlen(file->d_name) + 2;
            char filePath[1024];
            memset(filePath, 0, 1024);
            strcat(filePath, m_log_path_.c_str());
            strcat(filePath, "/");
            strcat(filePath, file->d_name);
            vecLogsFiles.push_back(filePath);
            count++;

         }
     }
     closedir(d);
     if (file)
     {
         delete file;
         file = nullptr;
     }

     for (int i = count-1; i >  m_maxLogFiles-1; i--)
     {
        remove(vecLogsFiles[i].c_str());
     }

     return true;
}
