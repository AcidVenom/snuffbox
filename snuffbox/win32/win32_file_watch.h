#pragma once

#include <string>
#include <map>
#include <queue>
#include <Windows.h>

namespace snuffbox
{
	/**
	* @enum snuffbox::FileType
	* @brief A file type enum to use with file watch
	* @author Daniël Konings
	*/
	enum FileType
	{
		kScript,
		kShader,
		kTexture,
		kModel
	};

	/**
	* @struct snuffbox::WatchedFile
	* @brief A structure to hold file watch data
	* @author Daniël Konings
	*/
	struct WatchedFile
	{
		std::string path;
		std::string relativePath;
		FILETIME lastTime;
		FileType type;
	};

	/**
	* @class snuffbox::FileWatcher
	* @brief Checks files for changes and reloads them
	* @author Daniël Konings
	*/
	class FileWatcher
	{
	public:
		/// Default constructor
		FileWatcher();
		/// Default destructor
		~FileWatcher();

		/// Checks for changes
		void WatchFiles();

		/// Reloads a watched JavaScript file when it has been changed
		void ReloadJSFile(WatchedFile& file);

		/// Reloads a watched shader when it has been changed
		void ReloadShaderFile(WatchedFile& file);

		/// Reloads a watched texture when it has been changed
		void ReloadTextureFile(WatchedFile& file);

		/// Reloads a watched model when it has been changed
		void ReloadModelFile(WatchedFile& file);

		/// Adds a file to the watched files list
		void AddFile(std::string& path, std::string& relativePath, FileType type);

		/// Get file time
		FILETIME GetTimeForFile(std::string& path, bool* failed);

		/// Removes a watched file
		void RemoveWatchedFile(std::string path);

	private:
		std::map<std::string,WatchedFile> files_; ///< The vector containing all files to watch
		std::queue<WatchedFile> queue_; ///< The queue for files that have to be added
		std::queue<std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string,WatchedFile>>>>> toDelete_; ///< The queue for files that have to be removed
	};
}