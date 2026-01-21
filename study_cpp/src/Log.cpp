#include "Log.h"
#include <iostream>

void Log::setLogLevel(LogLevel level) {
	m_logLevel = level;
};

void Log::debug(const char* msg) {
	if (m_logLevel <= LEVEL_DEBUG) {
		std::cout << "[DEBUG] " << msg << std::endl;
	}
}

void Log::info(const char* msg) {
	if (m_logLevel <= LEVEL_INFO) {
		std::cout << "[INFO] " << msg << std::endl;
	}
}

void Log::warning(const char* msg) {
	if (m_logLevel <= LEVEL_WARNING) {
		std::cout << "[WARNING] " << msg << std::endl;
	}
}

void Log::error(const char* msg) {
	if (m_logLevel <= LEVEL_ERROR) {
		std::cout << "[ERROR] " << msg << std::endl;
	}
}