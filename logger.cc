#include <iomanip>
#include <ctime>
#include <iostream>
#include <fstream>
#include "logger.h"
#include "mutex.h"

#define __MACOS__ 1

#if defined(WIN32)
    #include <direct.h>
    #define GetCurrentDir _getcwd
	#include <process.h>         /* for _getpid */
	typedef int pid_t;
	#define getpid  _getpid
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

#if  defined(WIN32)
#include <windows.h>
typedef DWORD ThreadId;
#elif defined(__MACOS__)
#include <mach/mach.h>
typedef mach_port_t ThreadId;
#else  // OS_POSIX && !OS_MACOSX
#include <unistd.h>
typedef pid_t ThreadId;
#endif

ThreadId GetCurrentId() {
#if defined(__MACOS__)
	return mach_thread_self();
#elif defined(LINUX)
	return syscall(__NR_gettid);
#elif defined(WIN32)
	return GetCurrentThreadId();
#endif
}

std::ofstream outFileHandle;
char logFileName[FILENAME_MAX] = { 0 };

static Mutex* logLock = NULL;

LogDestination logDest = FILE_ONLY;

const char* const LogLevelNames[totalLevels] = {
  "INFO", "WARNING", "ERROR", "FATAL" };

LogMessage::LogMessage(const char* file, int line) : _level(INFO) {
	Init(file, line);
}

LogMessage::LogMessage(const char* file, int line, LogLevel level) : _level(level) {
	Init(file, line);
}

void LogMessage::Init(const char* file, int line) {
#if defined(WIN32) 
	const char* last_slash = strrchr(file, '\\');
#else
	const char* last_slash = strrchr(file, '/');
#endif
	if (last_slash) 
		file = last_slash + 1;
	_stream << '[';
	_stream << getpid() << ";";
	_stream << GetCurrentId() << ";";
	time_t t = time(NULL);
	struct tm tm_time;
#if defined(WIN32)   
	localtime_s(&tm_time, &t);
#else
	localtime_r(&t, &tm_time);
#endif
	_stream << std::setfill('0')
		<< std::setw(2) << 1 + tm_time.tm_mon
		<< std::setw(2) << tm_time.tm_mday
		<< ';'
		<< std::setw(2) << tm_time.tm_hour
		<< ':'
		<< std::setw(2) << tm_time.tm_min
		<< ':'
		<< std::setw(2) << tm_time.tm_sec
		<< ']';
	
	_stream << LogLevelNames[_level] << ":" << file << "(" << line << ")";
	//_msg_start = _stream.pcount();
}


bool InitLogFile(theOldOne oldOne) {
	if (outFileHandle.is_open())
		return true;
	char currentPath[FILENAME_MAX];
	if (!GetCurrentDir(currentPath, sizeof(currentPath)))
		return false;
#if defined(WIN32)
	strcat(currentPath, "\\");
#else
	strcat(currentPath, "/");
#endif
	if (!logFileName[0]) {
		strcat(currentPath, "debug.log");
	} else {
		strcat(currentPath, logFileName);
	}
	if (oldOne == AppendToTheOldFile) {
		outFileHandle.open(currentPath, std::ios_base::app);
	} else {
		outFileHandle.open(currentPath, std::ios_base::out);
	}
	if (!outFileHandle.is_open()) {
		return false;
	}
	return true;
}

LogMessage::~LogMessage() {
	std::string str_newline(_stream.str());
	if (logDest != FILE_ONLY)
		std::cerr << str_newline << std::endl;
	if (!logLock) {
		logLock = new Mutex();
	}
	logLock->Lock();
	if (logDest != CONSOLE_ONLY && InitLogFile(DeleteTheOldFile)) {
		outFileHandle << str_newline << std::endl;
	}
	logLock->Unlock();
}


void CloseLogFile() {
  if (!outFileHandle.is_open())
    return;

  outFileHandle.close();
}

void InitLogging(const char* new_log_file, LogDestination _logdest, theOldOne oldOne) {
	if (outFileHandle.is_open()) {
		CloseLogFile();
	}
	
	logDest = _logdest;
	
	if (logDest == CONSOLE_ONLY)
		return;
	if (new_log_file != NULL) {
		strcpy(logFileName, new_log_file);
	}
	if (!logLock) {
		logLock = new Mutex();
	}
	InitLogFile(oldOne);
}