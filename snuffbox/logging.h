#pragma once
#include <Windows.h>

#include <string>
#include "../snuffbox/environment.h"
#include "../snuffbox/console/console.h"

namespace snuffbox
{

	inline const char* const SeverityToString(const LogSeverity& severity){
		switch (severity)
		{
		case LogSeverity::kInfo:
			return "Info";
		case LogSeverity::kDebug:
			return "Debug";
		case LogSeverity::kSuccess:
			return "Success";
		case LogSeverity::kError:
			return "Error";
		case LogSeverity::kFatal:
			return "Fatal";
    case LogSeverity::kWarning:
      return "Warning";
		default:
			return "Unknown";
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

		environment::console().Receive(severity, msg);
		qApp->processEvents();
	}
}

#define SNUFF_LOG_INFO(msg) snuffbox::log(snuffbox::LogSeverity::kInfo,msg)
#define SNUFF_LOG_DEBUG(msg) snuffbox::log(snuffbox::LogSeverity::kDebug,msg)
#define SNUFF_LOG_WARNING(msg) snuffbox::log(snuffbox::LogSeverity::kWarning,msg)
#define SNUFF_LOG_SUCCESS(msg) snuffbox::log(snuffbox::LogSeverity::kSuccess,msg)
#define SNUFF_LOG_ERROR(msg) snuffbox::log(snuffbox::LogSeverity::kError,msg)
#define SNUFF_LOG_FATAL(msg) snuffbox::log(snuffbox::LogSeverity::kFatal,msg)
#define SNUFF_XASSERT(expr,msg) if (!(expr)){ std::string message = ##msg; std::string result = "\n\nSnuffbox assertion!\n----------------------\n" + message + "\n"; SNUFF_LOG_FATAL(result.c_str()); if(environment::console().isLoaded()){ qApp->exec(); } do{__debugbreak();}while(true);}
#define SNUFF_ASSERT(msg)	std::string message = ##msg; std::string result = "\n\nSnuffbox assertion!\n----------------------\n" + message + "\n"; SNUFF_LOG_FATAL(result.c_str()); if(environment::console().isLoaded()){ qApp->exec(); } do{__debugbreak();}while(true)
#define SNUFF_ASSERT_NOTNULL(ptr) SNUFF_XASSERT(ptr != nullptr, "Attempt to get a nullptr!")