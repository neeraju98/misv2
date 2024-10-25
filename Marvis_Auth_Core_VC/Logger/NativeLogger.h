#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifdef WIN32
// Win Socket Header File(s)
#include <Windows.h>
#include <process.h>
#else
// POSIX Socket Header File(s)
#include <errno.h>
#include <pthread.h>
#endif

namespace NativeLogging
{
    // Interface to write log into log file or console.
    #define LOG_ERROR(x)    CNativeLogger::getInstance()->error(x)
    #define LOG_ALARM(x)	CNativeLogger::getInstance()->alarm(x)
    #define LOG_ALWAYS(x)	CNativeLogger::getInstance()->always(x)
    #define LOG_INFO(x)     CNativeLogger::getInstance()->info(x)
    #define LOG_BUFFER(x)   CNativeLogger::getInstance()->buffer(x)
    #define LOG_TRACE(x)    CNativeLogger::getInstance()->trace(x)
    #define LOG_DEBUG(x)    CNativeLogger::getInstance()->debug(x)
	#define LOG_SETPATH(x)  CNativeLogger::getInstance()->setFilepath(x)
    #define LOG_CLOSE()		CNativeLogger::getInstance()->deleteInstance()
	
    // enum for LOG_LEVEL
    typedef enum LOG_LEVEL
    {								   
        DISABLE_LOG       = 0,
		LOG_LEVEL_ERROR   = 1,
        LOG_LEVEL_INFO    = 2,
        LOG_LEVEL_BUFFER  = 3,
        LOG_LEVEL_TRACE   = 4,
        LOG_LEVEL_DEBUG   = 5,
        ENABLE_LOG        = 6,
    } LogLevel;

    // enum for LOG_TYPE
    typedef enum LOG_TYPE
    {
        NO_LOG            = 1,
        CONSOLE           = 2,
        FILE_LOG          = 3,
    } LogType;

    class CNativeLogger
    {
    public:
        static CNativeLogger* getInstance() throw ();

        // Interface for Error Log 
        void error(const char* text) throw();
        void error(std::string& text) throw();
        void error(std::ostringstream& stream) throw();

        // Interface for Alarm Log 
        void alarm(const char* text) throw();
        void alarm(std::string& text) throw();
        void alarm(std::ostringstream& stream) throw();

        // Interface for Always Log 
        void always(const char* text) throw();
        void always(std::string& text) throw();
        void always(std::ostringstream& stream) throw();

        // Interface for Buffer Log 
        void buffer(const char* text) throw();
        void buffer(std::string& text) throw();
        void buffer(std::ostringstream& stream) throw();

        // Interface for Info Log 
        void info(const char* text) throw();
        void info(std::string& text) throw();
        void info(std::ostringstream& stream) throw();

        // Interface for Trace log 
        void trace(const char* text) throw();
        void trace(std::string& text) throw();
        void trace(std::ostringstream& stream) throw();

        // Interface for Debug log 
        void debug(const char* text) throw();
        void debug(std::string& text) throw();
        void debug(std::ostringstream& stream) throw();

        // Error and Alarm log must be always enable
        // Hence, there is no interfce to control error and alarm logs

        // Interfaces to control log levels
        void updateLogLevel(LogLevel logLevel);
        void enableLog();  // Enable all log levels
        void disableLog(); // Disable all log levels, except error and alarm

        // Interfaces to control log Types
        void updateLogType(LogType logType);
        void enableConsoleLogging();
        void enableFileLogging();
		void setFilepath(const char* filepath);
		void deleteInstance();
		~CNativeLogger();

	protected:
        CNativeLogger();
		
        // Wrapper function for lock/unlock
        // For Extensible feature, lock and unlock should be in protected
        void lock();
        void unlock();

        std::string getCurrentTime();

    private:
        void logIntoFile(std::string& data);
        void logOnConsole(std::string& data);
        CNativeLogger(const CNativeLogger&) {}
        void operator=(const CNativeLogger&) {}

    private:
        static CNativeLogger  *m_pInstance;
        std::ofstream    m_File;
		std::string m_logFileName;
		std::string		m_dirpath;
		std::string		m_strFileTimeStemp;

#ifdef	WIN32
        CRITICAL_SECTION        m_Mutex;
#else
        pthread_mutexattr_t     m_Attr; 
        pthread_mutex_t         m_Mutex;
#endif

        LogLevel                m_LogLevel;
        LogType                 m_LogType;

        static const int       CTIME_BUFFER_SIZE = 26;
    };

} // End of namespace

#endif // End of _LOGGER_H_
