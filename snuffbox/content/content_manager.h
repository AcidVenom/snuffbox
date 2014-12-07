#pragma once

#include <map>
#include <queue>
#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/js/js_callback.h"
#include "../../snuffbox/d3d11/d3d11_texture.h"
#include "../../snuffbox/d3d11/d3d11_shader.h"
#include "../../snuffbox/fbx/fbx_model.h"
#include "../../snuffbox/fmod/fmod_sound.h"

namespace snuffbox
{
	template<typename T>
	class Content;

	/**
	* @class snuffbox::ContentManager
	* @brief Loads and manages all content provided from JavaScript
	* @author Daniël Konings
	*/
	class ContentManager : public JSObject
	{
	public:
		/**
		* @enum snuffbox::ContentManager::ContentTypes
		* @brief An enumerator for the different content types the content manager can load
		* @author Daniël Konings
		*/
		enum ContentTypes
		{
			kTexture,
			kShader,
			kModel,
      kSound
		};

		/**
		* @struct snuffbox::ContentManager::PendingContent
		* @brief A pending content structure so the content manager doesn't interfere with the file watcher
		* @author Daniël Konings
		*/
		struct PendingContent
		{
			std::string path;
			ContentTypes type;
		};
	public:
		/// Default constructor
		ContentManager();

		/// Default destructor
		~ContentManager();

		/**
		* @brief Loads a piece of content from a path, type T is the content type here
		* @param[in] path (std::string) The path to load from
		*/
		template<typename T>
		bool Load(std::string path);

		/**
		* @brief Unloads a piece of content from a path, type T is the content type here
		* @param[in] path (std::string) The path to unload from
		*/
		template<typename T>
		void Unload(std::string path);

		/**
		* @brief Retrieves a piece of loaded content if available
		* @return (snuffbox::SharedPtr<T>&) Only returns a shared pointer if the content actually exists
		*/
		template<typename T>
		SharedPtr<T>& Get(std::string path);

		/// Loads all pending content
		void LoadPendingContent();

		/**
		* @brief Adds pending content to the content manager
		* @param[in] content (snuffbox::PendingContent) The pending content data 
		*/
		void AddPendingContent(PendingContent content){ pending_content_.emplace(content); }

		/**
		* @return Is the content manager idle?
		*/
		bool idle(){ return pending_content_.empty(); }

		/// Unloads all loaded content
		void UnloadAll();

		/**
		* @brief Parses a package from a relative path
		* @param[in] contentPath (std::string) The relative path
    * @return (std::map<std::string,std::vector<std::string>>) The parsed package as a table
		*/
    std::map<std::string,std::vector<std::string>> ParsePackage(std::string contentPath);

		/**
		* @brief Skips white spaces of a string
		* @param[in] i (int&) The index we're currently at
		* @param[in] str (std::string&) The string to consume whitespaces of
		* @param[in] curChar (char&) The character to change
		*/
		void SkipWhiteSpaces(int& i, std::string& str, char& curChar);

		/**
		* @brief Consumes a keyword
		* @param[in] i (int&) The current index in the string
		* @param[in] str (std::string&) The current string being parsed
		* @param[in] toConsume (std::string) The string to consume
		* @return (bool) Was the right keyword found?
		*/
		bool Consume(int& i, std::string& str, std::string toConsume);

		/**
		* @brief Retrieves a value
		* @param[in] i (int&) The current index in the string
		* @param[in] str (std::string&) The current string being parsed
		* @param[in] endAt (std::string) The character to stop at
		* @return (std::string) The value
		*/
		std::string GetValue(int& i, std::string& str, std::string endAt);

    /**
    * @brief Loads content using a content type
    * @param[in] contentType (std::string) The content type to load
    * @param[in] contentPath (std::string) The path to load
    */
    void Load(std::string contentType, std::string contentPath);

    /**
    * @brief Unloads content using a content type
    * @param[in] contentType (std::string) The content type to unload
    * @param[in] contentPath (std::string) The path to unload
    */
    void Unload(std::string contentType, std::string contentPath);

	private:
		std::map<std::string, SharedPtr<Content<Texture>>>	loaded_textures_; //!< A map by path of all loaded textures
		std::map<std::string, SharedPtr<Content<Shader>>>		loaded_shaders_; //!< A map by path of all loaded shaders
		std::map<std::string, SharedPtr<Content<FBXModel>>> loaded_models_; //!< A map by path of all loaded models
    std::map<std::string, SharedPtr<Content<Sound>>>    loaded_sounds_; //!< A map by path of all loaded sounds
		std::queue<PendingContent>													pending_content_; //!< A queue for pending content
	
	public:
		JS_NAME(ContentManager);
		static void RegisterJS(JS_TEMPLATE);

	private:
		static void JSLoad(JS_ARGS);
		static void JSUnload(JS_ARGS);
		static void JSIdle(JS_ARGS);
		static void JSUnloadAll(JS_ARGS);
    static void JSWatch(JS_ARGS);
    static void JSLoadFont(JS_ARGS);
	};
}