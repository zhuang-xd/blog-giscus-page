#pragma once

class Log {
public:
	static enum LogLevel : unsigned char
	{
		LEVEL_DEBUG,
		LEVEL_INFO,
		LEVEL_WARNING,
		LEVEL_ERROR
	};
	LogLevel m_logLevel = LEVEL_DEBUG;
public:
	void setLogLevel(LogLevel level);
	void debug(const char* msg);
	void info(const char* msg);
	void warning(const char* msg);
	void error(const char* msg);
};