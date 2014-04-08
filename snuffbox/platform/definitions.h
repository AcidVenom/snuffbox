#pragma once

#include "../../snuffbox/win32/win32_window.h"
#include "../../snuffbox/platform/interfaces.h"

#ifdef _WIN32
namespace snuffbox { typedef Win32Window PlatformWindow; }
#endif
#ifdef _LINUX
namespace snuffbox { typedef LinuxWindow PlatformWindow; }
#endif
#ifdef _MACOSX
namespace snuffbox { typedef OsxWindow PlatformWindow; }
#endif
