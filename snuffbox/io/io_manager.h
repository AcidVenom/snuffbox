#pragma once

#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	class IO;

	/**
	* @class snuffbox::IOManager
	* @brief Handles all input and output streams for file operations
	* @author Daniël Konings
	*/
	class IOManager : public JSObject
	{
	public:
		///Default constructor
		IOManager();

		/// Default destructor
		~IOManager();

		/**
		* @brief Opens a file and parses it as a string
		* @param[in] relativePath (std::string) The relative path to the file from the working directory
		* @return (std::string) The parsed result
		*/
		std::string Open(std::string relativePath);

		/**
		* @brief Saves a file with string information
		* @param[in] relativePath (std::string) The path to save to from the working directory
		* @param[in] data (std::string) The data to save
		*/
		void Save(std::string relativePath, std::string data);

		/**
		* @brief Does a file exist?
		* @param[in] relativePath (std::string) The relative path to the file from the working directory
		* @return (bool) The boolean value
		*/
		bool FileExists(std::string relativePath);

	private:

	public:
		JS_NAME(IO);

		static void RegisterJS(JS_TEMPLATE);
		static void JSOpen(JS_ARGS);
		static void JSSave(JS_ARGS);
		static void JSFileExists(JS_ARGS);
	};
}