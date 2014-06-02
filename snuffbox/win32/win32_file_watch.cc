#include "../../snuffbox/win32/win32_file_watch.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	namespace environment
	{
		namespace {
			FileWatcher* globalInstance = nullptr;
		}

		bool has_file_watcher(){ return globalInstance != nullptr; }

		FileWatcher& file_watcher()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{
	//-------------------------------------------------------------------
	FileWatcher::FileWatcher()
	{
		environment::globalInstance = this;
	}

	//-------------------------------------------------------------------
	FileWatcher::~FileWatcher()
	{

	}

	//-------------------------------------------------------------------
	void FileWatcher::AddFile(std::string& path, std::string& relativePath, FileType type)
	{
		WatchedFile file;
		file.path = path;
		file.relativePath = relativePath;
		bool failed = false;
		file.lastTime = GetTimeForFile(path,&failed);
		file.type = type;

		if (failed)
		{
			SNUFF_LOG_ERROR(std::string("Failed to add file '" + path + "' to the file watch! File will not be hot reloaded!").c_str());
		}
		else
		{
			files_.push_back(file);
		}
	}

	//-------------------------------------------------------------------
	void FileWatcher::WatchFiles()
	{
		if (!environment::has_game())
			return;
		for (auto &it : files_)
		{
			bool failed = false;
			FILETIME lastTime = GetTimeForFile(it.path, &failed);
			if (!failed)
			{
				if (CompareFileTime(&it.lastTime, &lastTime) != 0)
				{
					switch (it.type)
					{
					case FileType::kScript:
						ReloadJSFile(it);
						break;
					case FileType::kShader:
						ReloadShaderFile(it);
						break;
					case FileType::kTexture:
						ReloadTextureFile(it);
						break;
					}
					it.lastTime = lastTime;
				}
			}
		}
	}

	//-------------------------------------------------------------------
	void FileWatcher::ReloadJSFile(WatchedFile& file)
	{
		environment::js_state_wrapper().CompileAndRun(file.relativePath.c_str(), true);
		environment::game().CreateCallbacks();
		environment::game().NotifyEvent(GameEvents::kReload);
		V8::LowMemoryNotification();
		SNUFF_LOG_INFO(std::string("Hot reloaded JavaScript file: " + file.path).c_str());
	}

	//-------------------------------------------------------------------
	void FileWatcher::ReloadShaderFile(WatchedFile& file)
	{
		Shader* shader = environment::content_manager().Get<Shader>(file.relativePath).get();
		shader->Reload(file.relativePath);
		environment::render_device().ResetCurrentShader();
		SNUFF_LOG_INFO(std::string("Hot reloaded Shader file: " + file.path).c_str());
	}

	//-------------------------------------------------------------------
	void FileWatcher::ReloadTextureFile(WatchedFile& file)
	{
		Texture* texture = environment::content_manager().Get<Texture>(file.relativePath).get();
		texture->Reload(file.relativePath);
		environment::render_device().ResetCurrentTexture();
		SNUFF_LOG_INFO(std::string("Hot reloaded Texture file: " + file.path).c_str());
	}

	//-------------------------------------------------------------------
	FILETIME FileWatcher::GetTimeForFile(std::string& path, bool* failed)
	{
		FILETIME creationTime;
		FILETIME lastAccessTime;
		FILETIME lastWriteTime;

		HANDLE file = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

		if (file == INVALID_HANDLE_VALUE)
		{
			*failed = true;
			FILETIME error = {};
			return error;
		}

		BOOL result = GetFileTime(file, &creationTime, &lastAccessTime, &lastWriteTime);
		SNUFF_XASSERT(result == TRUE, std::string("Could not get time for file: " + path).c_str());

		CloseHandle(file);
		return lastWriteTime;
	}
}