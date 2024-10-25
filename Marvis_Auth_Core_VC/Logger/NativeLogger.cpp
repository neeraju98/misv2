#include <iostream>
#include <cstdlib>
#include <ctime>

#include "NativeLogger.h"

#include <ShlObj.h>		//For SHCreateDirectoryExA
#pragma comment (lib, "Shell32.lib")

using namespace std;
using namespace NativeLogging;

CNativeLogger* CNativeLogger::m_pInstance = 0;
std::string g_strNativeLogFileName = "Native_";
CNativeLogger::CNativeLogger()
{
    m_LogLevel	= LOG_LEVEL_ERROR;
    m_LogType	= FILE_LOG;
	
    // Initialize mutex
#ifdef WIN32
    InitializeCriticalSection(&m_Mutex);
#else
    int ret=0;
    ret = pthread_mutexattr_settype(&m_Attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    if(ret != 0)
    {   
        printf("CNativeLogger::CNativeLogger() -- Mutex attribute not initialized!!\n");
        exit(0);
    }   
    ret = pthread_mutex_init(&m_Mutex,&m_Attr);
    if(ret != 0)
    {   
        printf("CNativeLogger::CNativeLogger() -- Mutex not initialized!!\n");
        exit(0);
    }   
#endif
}

CNativeLogger::~CNativeLogger()
{
    m_File.close();
#ifdef WIN32
    DeleteCriticalSection(&m_Mutex);
#else
    pthread_mutexattr_destroy(&m_Attr);
    pthread_mutex_destroy(&m_Mutex);
#endif	
}

CNativeLogger* CNativeLogger::getInstance() throw ()
{
    if (m_pInstance == 0) 
    {
        m_pInstance = new CNativeLogger();
    }
    return m_pInstance;
}

void CNativeLogger::lock()
{
#ifdef WIN32
    EnterCriticalSection(&m_Mutex);
#else
    pthread_mutex_lock(&m_Mutex);
#endif
}

void CNativeLogger::unlock()
{
#ifdef WIN32
    LeaveCriticalSection(&m_Mutex);
#else
    pthread_mutex_unlock(&m_Mutex);
#endif
}

void CNativeLogger::logIntoFile(std::string& data)
{
	lock();
	std::string strCurrentTime = getCurrentTime();
	m_logFileName.clear();

	if (!m_dirpath.empty())
		m_dirpath += "\\";

	//e.g. Native_YYYY-MM-DD-HH-MM.log
	m_logFileName += m_dirpath + g_strNativeLogFileName + m_strFileTimeStemp + ".log";

	if (m_File.is_open())
	{
		m_File.close();
	}
	m_File.open(m_logFileName.c_str(), ios::out | ios::app);

	m_File << strCurrentTime << data << endl;

	unlock();
}

void CNativeLogger::logOnConsole(std::string& data)
{
    cout << getCurrentTime() << "  " << data << endl;
}

string CNativeLogger::getCurrentTime()
{
	SYSTEMTIME stime;
	//structure to store system time (in usual time format)
	FILETIME ltime;
	//structure to store local time (local time in 64 bits)
	FILETIME ftTimeStamp;
	char TimeStamp[256] = { 0 };//to store TimeStamp information
	GetSystemTimeAsFileTime(&ftTimeStamp); //Gets the current system time

	FileTimeToLocalFileTime(&ftTimeStamp, &ltime);//convert in local time and store in ltime
	FileTimeToSystemTime(&ltime, &stime);//convert in system time and store in stime

	sprintf_s(TimeStamp, sizeof(TimeStamp), "%02d-%02d-%04d %02d:%02d:%02d.%03d", stime.wDay, stime.wMonth, stime.wYear,
		stime.wHour, stime.wMinute, stime.wSecond, stime.wMilliseconds);

	char TimeStampForFileName[256] = { 0 };

	sprintf_s(TimeStampForFileName, sizeof(TimeStampForFileName), "%04d-%02d-%02d-%02d-%02d", stime.wYear, stime.wMonth,
		stime.wDay, stime.wHour, stime.wMinute);

	m_strFileTimeStemp = TimeStampForFileName;

	return TimeStamp;
}

// Interface for Error Log
void CNativeLogger::error(const char* text) throw()
{
    string data;
    data.append("[ERROR]: ");
    data.append(text);

    // ERROR must be capture
    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_ERROR))
    {
        logIntoFile(data);
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_ERROR))
    {
        logOnConsole(data);
    }
}

void CNativeLogger::error(std::string& text) throw()
{
    error(text.data());
}

void CNativeLogger::error(std::ostringstream& stream) throw()
{
    string text = stream.str();
    error(text.data());
}

// Interface for Alarm Log 
void CNativeLogger::alarm(const char* text) throw()
{
    string data;
    data.append("[ALARM]: ");
    data.append(text);

    // ALARM must be capture
    if(m_LogType == FILE_LOG)
    {
        logIntoFile(data);
    }
    else if(m_LogType == CONSOLE)
    {
        logOnConsole(data);
    }
}

void CNativeLogger::alarm(std::string& text) throw()
{
    alarm(text.data());
}

void CNativeLogger::alarm(std::ostringstream& stream) throw()
{
    string text = stream.str();
    alarm(text.data());
}

// Interface for Always Log 
void CNativeLogger::always(const char* text) throw()
{
    string data;
    //data.append("[ALWAYS]: ");
    data.append(text);

    // No check for ALWAYS logs
    if(m_LogType == FILE_LOG)
    {
        logIntoFile(data);
    }
    else if(m_LogType == CONSOLE)
    {
        logOnConsole(data);
    }
}

void CNativeLogger::always(std::string& text) throw()
{
    always(text.data());
}

void CNativeLogger::always(std::ostringstream& stream) throw()
{
    string text = stream.str();
    always(text.data());
}

// Interface for Buffer Log 
void CNativeLogger::buffer(const char* text) throw()
{
    // Buffer is the special case. So don't add log level
    // and timestamp in the buffer message. Just log the raw bytes.
    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_BUFFER))
    {
        lock();
        m_File << text << endl;
        unlock();
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_BUFFER))
    {
        cout << text << endl;
    }
}

void CNativeLogger::buffer(std::string& text) throw()
{
    buffer(text.data());
}

void CNativeLogger::buffer(std::ostringstream& stream) throw()
{
    string text = stream.str();
    buffer(text.data());
}

// Interface for Info Log
void CNativeLogger::info(const char* text) throw()
{
    string data;
    data.append("[INFO]: ");
    data.append(text);

    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_INFO))
    {
        logIntoFile(data);
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_INFO))
    {
        logOnConsole(data);
    }
}

void CNativeLogger::info(std::string& text) throw()
{
    info(text.data());
}

void CNativeLogger::info(std::ostringstream& stream) throw()
{
    string text = stream.str();
    info(text.data());
}

// Interface for Trace Log
void CNativeLogger::trace(const char* text) throw()
{
    string data;
    data.append("[TRACE]: ");
    data.append(text);

    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_TRACE))
    {
        logIntoFile(data);
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_TRACE))
    {
        logOnConsole(data);
    }
}

void CNativeLogger::trace(std::string& text) throw()
{
    trace(text.data());
}

void CNativeLogger::trace(std::ostringstream& stream) throw()
{
    string text = stream.str();
    trace(text.data());
}

// Interface for Debug Log
void CNativeLogger::debug(const char* text) throw()
{
    string data;
    data.append("[DEBUG]: ");
    data.append(text);

    if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_DEBUG))
    {
        logIntoFile(data);
    }
    else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_DEBUG))
    {
        logOnConsole(data);
    }
}

void CNativeLogger::debug(std::string& text) throw()
{
    debug(text.data());
}

void CNativeLogger::debug(std::ostringstream& stream) throw()
{
    string text = stream.str();
    debug(text.data());
}

// Interfaces to control log levels
void CNativeLogger::updateLogLevel(LogLevel logLevel)
{
    m_LogLevel = logLevel;
}

// Enable all log levels
void CNativeLogger::enableLog()
{
    m_LogLevel = ENABLE_LOG; 
}

// Disable all log levels, except error and alarm
void CNativeLogger::disableLog()
{
    m_LogLevel = DISABLE_LOG;
}

// Interfaces to control log Types
void CNativeLogger::updateLogType(LogType logType)
{
    m_LogType = logType;
}

void CNativeLogger::enableConsoleLogging()
{
    m_LogType = CONSOLE; 
}

void CNativeLogger::enableFileLogging()
{
    m_LogType = FILE_LOG ;
}

void CNativeLogger::setFilepath(const char* filepath)
{
	if (filepath != NULL)
	{
		SHCreateDirectoryExA(NULL, filepath, NULL);
		m_dirpath = filepath;
	}
}

void  CNativeLogger::deleteInstance()
{
	delete CNativeLogger::getInstance();
	return;
}
