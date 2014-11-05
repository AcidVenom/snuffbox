#pragma once

#include <map>
#include <queue>
#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/js/js_callback.h"
#include "../../snuffbox/d3d11/d3d11_texture.h"
#include "../../snuffbox/d3d11/d3d11_shader.h"
#include "../../snuffbox/fbx/fbx_model.h"
#include "../../snuffbox/freetype/freetype_font.h"

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
			kFont
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

	private:
		std::map<std::string, SharedPtr<Content<Texture>>>	loaded_textures_; //!< A map by path of all loaded textures
		std::map<std::string, SharedPtr<Content<Shader>>>		loaded_shaders_; //!< A map by path of all loaded shaders
		std::map<std::string, SharedPtr<Content<FBXModel>>> loaded_models_; //!< A map by path of all loaded models
		std::map<std::string, SharedPtr<Content<Font>>>			loaded_fonts_; //!< A map by path of all loaded models
		std::queue<PendingContent>													pending_content_; //!< A queue for pending content
	
	public:
		JS_NAME(ContentManager);
		static void RegisterJS(JS_TEMPLATE);

	private:
		static void JSLoad(JS_ARGS);
		static void JSUnload(JS_ARGS);
		static void JSIdle(JS_ARGS);
		static void JSUnloadAll(JS_ARGS);
	};
}