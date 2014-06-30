#pragma once

#include <map>
#include <queue>
#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/js/js_callback.h"
#include "../../snuffbox/d3d11/d3d11_texture.h"
#include "../../snuffbox/d3d11/d3d11_shader.h"
#include "../../snuffbox/fbx/fbx_model.h"

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
			kModel
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

		/// Loads a piece of content from a path
		template<typename T>
		bool Load(std::string path);

		/// Unloads a piece of content from a path
		template<typename T>
		void Unload(std::string path);

		/// Retrieves a piece of loaded content if available
		template<typename T>
		SharedPtr<T>& Get(std::string path);

		/// Loads all pending content
		void LoadPendingContent();
		
		/// Adds pending content to the content manager
		void AddPendingContent(PendingContent content){ pendingContent_.emplace(content); }

		/// Returns if the content manager is idle
		bool idle(){ return pendingContent_.empty(); }

		/// Returns the list of idle callbacks
		std::queue<SharedPtr<JSCallback>>& idleCallbacks(){ return idleCallbacks_; }

		/// Unloads all loaded content
		void UnloadAll();

	private:
		std::map<std::string, SharedPtr<Content<Texture>>> loadedTextures_; ///< A map by path of all loaded textures
		std::map<std::string, SharedPtr<Content<Shader>>> loadedShaders_; ///< A map by path of all loaded shaders
		std::map<std::string, SharedPtr<Content<FBXModel>>> loadedModels_; ///< A map by path of all loaded models
		std::queue<PendingContent>												pendingContent_; ///< A queue for pending content
		std::queue<SharedPtr<JSCallback>>									idleCallbacks_;	///< A queue for idle callbacks
	public:
		JS_NAME(ContentManager);
		static void RegisterJS(JS_TEMPLATE);
		static void JSLoad(JS_ARGS);
		static void JSUnload(JS_ARGS);
		static void JSIdle(JS_ARGS);
		static void JSUnloadAll(JS_ARGS);
	};
}