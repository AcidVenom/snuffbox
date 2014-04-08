#pragma once

#include <Windows.h>

namespace snuffbox
{
	enum LogSeverity
	{
		kInfo,
		kDebug,
		kSuccess,
		kError,
		kFatal
	};

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
	}
}

#define SNUFF_LOG_INFO(msg) snuffbox::log(snuffbox::LogSeverity::kInfo,msg)
#define SNUFF_LOG_DEBUG(msg) snuffbox::log(snuffbox::LogSeverity::kDebug,msg)
#define SNUFF_LOG_SUCCESS(msg) snuffbox::log(snuffbox::LogSeverity::kSuccess,msg)
#define SNUFF_LOG_ERROR(msg) snuffbox::log(snuffbox::LogSeverity::kError,msg)
#define SNUFF_LOG_FATAL(msg) snuffbox::log(snuffbox::LogSeverity::kFatal,msg)
#define SNUFF_XASSERT(expr,msg) if (!(expr)){SNUFF_LOG_FATAL("Snuffbox assertion!"); SNUFF_LOG_FATAL("----------------------"); SNUFF_LOG_FATAL(msg); OutputDebugStringA("\n"); do{__debugbreak();}while(true);}
#define SNUFF_ASSERT(msg)	SNUFF_LOG_FATAL("Snuffbox assertion!"); SNUFF_LOG_FATAL("----------------------"); SNUFF_LOG_FATAL(msg); OutputDebugStringA("\n"); do{__debugbreak();}while(true)
#define SNUFF_ASSERT_NOTNULL(ptr) SNUFF_XASSERT(ptr != nullptr, "Attempt to get a nullptr!")