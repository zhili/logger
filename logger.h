#ifndef LOGGER_H_SW2KHZHX
#define LOGGER_H_SW2KHZHX

#include <sstream>

#define COMPACT_LOG_INFO \
  LogMessage(__FILE__, __LINE__)
#define COMPACT_LOG_WARNING \
  LogMessage(__FILE__, __LINE__, WARNING)
#define COMPACT_LOG_ERR \
  LogMessage(__FILE__, __LINE__, ERR)
#define COMPACT_LOG_FATAL \
  LogMessage(__FILE__, __LINE__, FATAL)

#define LOG(level) COMPACT_LOG_ ## level.stream()
#define LOG_IF(level, condition) !(condition) ? (void) 0 : LogMessageVoidify() & LOG(level)

// logging levels
enum LogLevel {
	INFO,
	WARNING,
	ERR,
	FATAL,
	totalLevels
};
// logging destionation
enum LogDestination {
	FILE_ONLY,
	CONSOLE_ONLY,
	FILE_AND_CONSOLE
};
// logging lock state
enum LogState {
	LOCK_FILE,
	NO_LOCK_FILE
};
// deal the old one
enum theOldOne {
	DeleteTheOldFile,
	AppendToTheOldFile
}; 


class LogMessage {
public:
	//LogMessage(const char* file, int line, LogLevel level, int ctr);
	LogMessage(const char* file, int line);
	LogMessage(const char* file, int line, LogLevel level);
	~LogMessage();
	
	std::ostream& stream() { return _stream; }
private:
	void Init(const char* file, int line);
	
	LogLevel _level;
	std::stringstream _stream;
	LogMessage(const LogMessage&);
	void operator=(const LogMessage&);
};


class LogMessageVoidify {
public:
	LogMessageVoidify() { }
	// This has to be an operator with a precedence lower than << but
	// higher than ?:
	void operator&(std::ostream&) { }
};

void InitLogging(const char* new_log_file, LogDestination _logdest, theOldOne oldOne);
void CloseLogFile();
#endif /* end of include guard: LOGGER_H_SW2KHZHX */
