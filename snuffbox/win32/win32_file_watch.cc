#include "../../snuffbox/win32/win32_file_watch.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/content/content_manager.h"
#include <strsafe.h>

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
			if (files_.find(file.relativePath) != files_.end())
				return;
			queue_.push(file);
		}
	}

	//-------------------------------------------------------------------
	void FileWatcher::WatchFiles()
	{
		if (!environment::has_game())
			return;

		while (!queue_.empty())
		{
			auto it = queue_.front();
			files_.emplace(it.relativePath, it);
			queue_.pop();
		}

		while (!toDelete_.empty())
		{
			auto it = toDelete_.front();
			files_.erase(it);
			toDelete_.pop();
		}

		ReloadAll();
	}

	//-------------------------------------------------------------------
	void FileWatcher::ReloadAll()
	{
		for (auto& pair = files_.begin(); pair != files_.end(); ++pair)
		{
			auto& it = pair->second;

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
					case FileType::kModel:
						ReloadModelFile(it);
						break;
          case FileType::kUnknown:
            SNUFF_LOG_INFO(std::string("Hot reloaded custom file: " + it.path).c_str());
            environment::game().NotifyEvent(GameEvents::kReload);
            break;
					}
					it.lastTime = lastTime;
          last_reloaded_ = it.relativePath;
					break;
				}
			}
		}
	}

  //-------------------------------------------------------------------
  std::string& FileWatcher::last_reloaded()
  {
    return last_reloaded_;
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
	void FileWatcher::ReloadModelFile(WatchedFile& file)
	{
		FBXModel* model = environment::content_manager().Get<FBXModel>(file.relativePath).get();
		model->Reload(file.relativePath);
		environment::render_device().ResetCurrentModel();
		SNUFF_LOG_INFO(std::string("Hot reloaded FBX Model file: " + file.path).c_str());
	}

	//-------------------------------------------------------------------
	void FileWatcher::RemoveWatchedFile(std::string path)
	{
		auto it = files_.find(path);

		if (it == files_.end())
		{
			return;
		}
		else
		{
			toDelete_.push(it);
		}
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
			LPTSTR msg = nullptr;
			DWORD err = GetLastError();
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&msg,
				0, NULL);

			LocalFree(msg);
			FILETIME error = {};
			return error;
		}

		BOOL result = GetFileTime(file, &creationTime, &lastAccessTime, &lastWriteTime);
		SNUFF_XASSERT(result == TRUE, std::string("Could not get time for file: " + path).c_str());

		CloseHandle(file);
		return lastWriteTime;
	}
}