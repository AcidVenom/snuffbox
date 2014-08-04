#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/content/content.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/memory/allocated_memory.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/fbx/fbx_loader.h"
#include "../../snuffbox/win32/win32_file_watch.h"

namespace snuffbox
{
	namespace environment
	{
		namespace {
			ContentManager* globalInstance = nullptr;
		}

		bool has_content_manager(){ return globalInstance != nullptr; }

		ContentManager& content_manager()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	ContentManager::ContentManager()
	{
		environment::globalInstance = this;
	}

	//-------------------------------------------------------------------------------------------
	ContentManager::~ContentManager()
	{
		UnloadAll();
		environment::globalInstance = nullptr;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	bool ContentManager::Load<Texture>(std::string path)
	{
		if (loaded_textures_.find(path) != loaded_textures_.end())
			return false;
		SNUFF_LOG_INFO(std::string("Loading texture " + path).c_str());
		SharedPtr<Texture> texture = environment::memory().ConstructShared<Texture>(std::string(path));
		SharedPtr<Content<Texture>> content = environment::memory().ConstructShared<Content<Texture>>(ContentTypes::kTexture, texture);

		loaded_textures_.emplace(path, content);
		return true;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	bool ContentManager::Load<Shader>(std::string path)
	{
		if (loaded_shaders_.find(path) != loaded_shaders_.end())
			return false;
		SNUFF_LOG_INFO(std::string("Loading shader " + path).c_str());
		SharedPtr<Shader> shader = environment::memory().ConstructShared<Shader>(std::string(path));
		SharedPtr<Content<Shader>> content = environment::memory().ConstructShared<Content<Shader>>(ContentTypes::kShader, shader);

		loaded_shaders_.emplace(path, content);
		return true;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	bool ContentManager::Load<FBXModel>(std::string path)
	{
		if (loaded_models_.find(path) != loaded_models_.end())
			return false;

		SNUFF_LOG_INFO(std::string("Loading model " + path).c_str());
		SharedPtr<FBXModel> model = environment::memory().ConstructShared<FBXModel>(environment::fbx_loader().Load(path), path);
		SharedPtr<Content<FBXModel>> content = environment::memory().ConstructShared<Content<FBXModel>>(ContentTypes::kModel, model);

		loaded_models_.emplace(path, content);
		return true;
	}

	//-------------------------------------------------------------------------------------------
	template<>
	void ContentManager::Unload<Texture>(std::string path)
	{
		SNUFF_XASSERT(loaded_textures_.find(path) != loaded_textures_.end(), "The texture '" + path + "' was never loaded!");

		loaded_textures_.erase(loaded_textures_.find(path));
		environment::file_watcher().RemoveWatchedFile(path);
		SNUFF_LOG_INFO(std::string("Unloaded texture " + path).c_str());
	}

	//-------------------------------------------------------------------------------------------
	template<>
	void ContentManager::Unload<Shader>(std::string path)
	{
		SNUFF_XASSERT(loaded_shaders_.find(path) != loaded_shaders_.end(), "The shader '" + path + "' was never loaded!");

		loaded_shaders_.erase(loaded_shaders_.find(path));
		environment::file_watcher().RemoveWatchedFile(path);
		SNUFF_LOG_INFO(std::string("Unloaded shader " + path).c_str());
	}

	//-------------------------------------------------------------------------------------------
	template<>
	void ContentManager::Unload<FBXModel>(std::string path)
	{
		SNUFF_XASSERT(loaded_models_.find(path) != loaded_models_.end(), "The model '" + path + "' was never loaded!");

		loaded_models_.erase(loaded_models_.find(path));
		environment::file_watcher().RemoveWatchedFile(path);
		SNUFF_LOG_INFO(std::string("Unloaded model " + path).c_str());
	}

	//-------------------------------------------------------------------------------------------
	template<>
	SharedPtr<Texture>& ContentManager::Get<Texture>(std::string path)
	{
		Content<Texture>* contentPtr = nullptr;

		SNUFF_XASSERT(loaded_textures_.find(path) != loaded_textures_.end(), std::string("Texture not loaded '" + path + "'!"));

		contentPtr = loaded_textures_.find(path)->second.get();

		return contentPtr->Get();
	}

	//-------------------------------------------------------------------------------------------
	template<>
	SharedPtr<Shader>& ContentManager::Get<Shader>(std::string path)
	{
		Content<Shader>* contentPtr = nullptr;

		SNUFF_XASSERT(loaded_shaders_.find(path) != loaded_shaders_.end(), std::string("Shader not loaded '" + path + "'!"));

		contentPtr = loaded_shaders_.find(path)->second.get();

		return contentPtr->Get();
	}

	//-------------------------------------------------------------------------------------------
	template<>
	SharedPtr<FBXModel>& ContentManager::Get<FBXModel>(std::string path)
	{
		Content<FBXModel>* contentPtr = nullptr;

		SNUFF_XASSERT(loaded_models_.find(path) != loaded_models_.end(), std::string("Model not loaded '" + path + "'!"));

		contentPtr = loaded_models_.find(path)->second.get();

		return contentPtr->Get();
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = {
			JSFunctionRegister("load", JSLoad),
			JSFunctionRegister("unload", JSUnload),
			JSFunctionRegister("unloadAll", JSUnloadAll)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::UnloadAll()
	{
		for (auto it = loaded_textures_.begin(); it != loaded_textures_.end(); ++it)
		{
			environment::file_watcher().RemoveWatchedFile(it->first);
		}
		loaded_textures_.clear();

		for (auto it = loaded_shaders_.begin(); it != loaded_shaders_.end(); ++it)
		{
			environment::file_watcher().RemoveWatchedFile(it->first);
		}
		loaded_shaders_.clear();

		for (auto it = loaded_models_.begin(); it != loaded_models_.end(); ++it)
		{
			environment::file_watcher().RemoveWatchedFile(it->first);
		}
		loaded_models_.clear();
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::JSLoad(JS_ARGS)
	{
		JS_CHECK_PARAMS("SS");

		bool isContent = false;
		std::string contentType = wrapper.GetString(0).c_str();
		std::string contentPath = wrapper.GetString(1).c_str();
		PendingContent content;
		content.path = contentPath;

		if (strcmp(contentType.c_str(), "texture") == 0)
		{
			isContent = true;
			environment::content_manager().Load<Texture>(contentPath);
		}

		if (strcmp(contentType.c_str(), "shader") == 0)
		{
			isContent = true;
			environment::content_manager().Load<Shader>(contentPath);
		}

		if (strcmp(contentType.c_str(), "model") == 0)
		{
			isContent = true;
			environment::content_manager().Load<FBXModel>(contentPath);
		}

		if (!isContent)
		{
			SNUFF_LOG_ERROR(std::string("Content type '" + contentType + "' does not exist").c_str());
		}
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::JSUnload(JS_ARGS)
	{
		JS_CHECK_PARAMS("SS");

		bool isContent = false;
		std::string contentType = wrapper.GetString(0).c_str();
		std::string contentPath = wrapper.GetString(1).c_str();

		if (strcmp(contentType.c_str(), "texture") == 0)
		{
			isContent = true;
			environment::content_manager().Unload<Texture>(contentPath);
		}

		if (strcmp(contentType.c_str(), "shader") == 0)
		{
			isContent = true;
			environment::content_manager().Unload<Shader>(contentPath);
		}

		if (strcmp(contentType.c_str(), "model") == 0)
		{
			isContent = true;
			environment::content_manager().Unload<FBXModel>(contentPath);
		}

		if (!isContent)
		{
			SNUFF_LOG_ERROR(std::string("Content type '" + contentType + "' does not exist!").c_str());
		}
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::JSUnloadAll(JS_ARGS)
	{
		JS_CHECK_PARAMS("V");
		environment::content_manager().UnloadAll();
	}
}