#ifndef COMMON_H
#define COMMON_H

#include "spdlog/sinks/rotating_file_sink.h"

// Logger used in all files except ConsoleHandler.h/.cpp (since that one is run async).
// Actual defenition is done in Hub.cpp
extern std::shared_ptr<spdlog::logger> logger;

#endif // COMMON_H
