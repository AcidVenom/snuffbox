#pragma once
#include <Windows.h>

#include <string>
#include "../snuffbox/environment.h"
#include "../snuffbox/console/console.h"

namespace snuffbox
{
	static std::string lastLine = "";

	inline const char* const SeverityToString(const LogSeverity& severity){
		switch (severity)
		{
		case LogSeverity::kInfo:
			return "INFO";
		case LogSeverity::kDebug:
			return "DEBUG";
		case LogSeverity::kSuccess:
			return "SUCCESS";
		case LogSeverity::kError:
			return "ERROR";
		case LogSeverity::kFatal:
			return "FATAL";
    case LogSeverity::kWarning:
      return "WARNING";
		default:
			return "<UNKNOWN>";
		}
	}

	inline void log(const LogSeverity& severity, const char* msg)
	{
		OutputDebugStringA("[SNUFF]");
		OutputDebugStringA("[");
		OutputDebugStringA(SeverityToString(severity));
		OutputDebugStringA("] ");
		OutputDebugStringA(msg); 
		OutputDebugStringA("\n");

		if (strcmp(lastLine.c_str(), msg) != 0)
		{
			environment::console().Receive(severity, msg);
			lastLine = std::string(msg);
		}

		qApp->processEvents();
	}
}

#define SNUFF_LOG_INFO(msg) snuffbox::log(snuffbox::LogSeverity::kInfo,msg)
#define SNUFF_LOG_DEBUG(msg) snuffbox::log(snuffbox::LogSeverity::kDebug,msg)
#define SNUFF_LOG_WARNING(msg) snuffbox::log(snuffbox::LogSeverity::kWarning,msg)
#define SNUFF_LOG_SUCCESS(msg) snuffbox::log(snuffbox::LogSeverity::kSuccess,msg)
#define SNUFF_LOG_ERROR(msg) snuffbox::log(snuffbox::LogSeverity::kError,msg)
#define SNUFF_LOG_FATAL(msg) snuffbox::log(snuffbox::LogSeverity::kFatal,msg)
#define SNUFF_XASSERT(expr,msg) if (!(expr)){ std::string message = ##msg; std::string result = "\n\nSnuffbox assertion!\n----------------------\n" + message + "\n"; SNUFF_LOG_FATAL(result.c_str()); do{__debugbreak();}while(true);}
#define SNUFF_ASSERT(msg)	std::string message = ##msg; std::string result = "\n\nSnuffbox assertion!\n----------------------\n" + message + "\n"; SNUFF_LOG_FATAL(result.c_str()); do{__debugbreak();}while(true)
#define SNUFF_ASSERT_NOTNULL(ptr) SNUFF_XASSERT(ptr != nullptr, "Attempt to get a nullptr!")