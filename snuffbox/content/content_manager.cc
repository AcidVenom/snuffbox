#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/content/content.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/memory/allocated_memory.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/fbx/fbx_loader.h"

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
	void ContentManager::Load<Texture>(std::string path)
	{
		if (loadedTextures_.find(path) != loadedTextures_.end())
			return;
		SharedPtr<Texture> texture = environment::memory().ConstructShared<Texture>(std::string(path));
		SharedPtr<Content<Texture>> content = environment::memory().ConstructShared<Content<Texture>>(ContentTypes::kTexture, texture);

		loadedTextures_.emplace(path, content);
	}

	//-------------------------------------------------------------------------------------------
	template<>
	void ContentManager::Load<Shader>(std::string path)
	{
		if (loadedShaders_.find(path) != loadedShaders_.end())
			return;
		SharedPtr<Shader> shader = environment::memory().ConstructShared<Shader>(std::string(path));
		SharedPtr<Content<Shader>> content = environment::memory().ConstructShared<Content<Shader>>(ContentTypes::kShader, shader);

		loadedShaders_.emplace(path, content);
	}

	//-------------------------------------------------------------------------------------------
	template<>
	void ContentManager::Load<FBXModel>(std::string path)
	{
		if (loadedModels_.find(path) != loadedModels_.end())
			return;
		SharedPtr<FBXModel> model = environment::memory().ConstructShared<FBXModel>(environment::fbx_loader().Load(path), path);
		SharedPtr<Content<FBXModel>> content = environment::memory().ConstructShared<Content<FBXModel>>(ContentTypes::kModel, model);

		loadedModels_.emplace(path, content);
	}

	//-------------------------------------------------------------------------------------------
	template<>
	void ContentManager::Unload<Texture>(std::string path)
	{
		SNUFF_XASSERT(loadedTextures_.find(path) != loadedTextures_.end(), "The texture '" + path + "' was never loaded!");

		loadedTextures_.erase(loadedTextures_.find(path));
	}

	//-------------------------------------------------------------------------------------------
	template<>
	void ContentManager::Unload<Shader>(std::string path)
	{
		SNUFF_XASSERT(loadedShaders_.find(path) != loadedShaders_.end(), "The shader '" + path + "' was never loaded!");

		loadedShaders_.erase(loadedShaders_.find(path));
	}

	//-------------------------------------------------------------------------------------------
	template<>
	void ContentManager::Unload<FBXModel>(std::string path)
	{
		SNUFF_XASSERT(loadedModels_.find(path) != loadedModels_.end(), "The model '" + path + "' was never loaded!");

		loadedModels_.erase(loadedModels_.find(path));
	}

	//-------------------------------------------------------------------------------------------
	template<>
	SharedPtr<Texture>& ContentManager::Get<Texture>(std::string path)
	{
		Content<Texture>* contentPtr = nullptr;
		 
		SNUFF_XASSERT(loadedTextures_.find(path) != loadedTextures_.end(), std::string("Texture not loaded '" + path + "'!"));

		contentPtr = loadedTextures_.find(path)->second.get();
		
		return contentPtr->Get();
	}

	//-------------------------------------------------------------------------------------------
	template<>
	SharedPtr<Shader>& ContentManager::Get<Shader>(std::string path)
	{
		Content<Shader>* contentPtr = nullptr;

		SNUFF_XASSERT(loadedShaders_.find(path) != loadedShaders_.end(), std::string("Shader not loaded '" + path + "'!"));

		contentPtr = loadedShaders_.find(path)->second.get();

		return contentPtr->Get();
	}

	//-------------------------------------------------------------------------------------------
	template<>
	SharedPtr<FBXModel>& ContentManager::Get<FBXModel>(std::string path)
	{
		Content<FBXModel>* contentPtr = nullptr;

		SNUFF_XASSERT(loadedModels_.find(path) != loadedModels_.end(), std::string("Model not loaded '" + path + "'!"));

		contentPtr = loadedModels_.find(path)->second.get();

		return contentPtr->Get();
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = {
			JSFunctionRegister("load", JSLoad),
			JSFunctionRegister("unload", JSUnload),
			JSFunctionRegister("unloadAll", JSUnloadAll),
			JSFunctionRegister("idleCallback", JSIdle)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::LoadPendingContent()
	{
		while (!pendingContent_.empty())
		{
			auto& it = pendingContent_.front();

			SNUFF_LOG_INFO(std::string("Loading " + it.path).c_str());

			switch (it.type)
			{
			case ContentTypes::kTexture:
				Load<Texture>(it.path);
				break;
			case ContentTypes::kShader:
				Load<Shader>(it.path);
				break;
			case ContentTypes::kModel:
				Load<FBXModel>(it.path);
				break;
			}

			SNUFF_LOG_DEBUG(std::string("Loaded " + it.path).c_str());
			pendingContent_.pop();
		}

		JS_CREATE_SCOPE;
		Handle<Context> ctx = JS_CONTEXT;

		while (!idleCallbacks_.empty())
		{
			auto& it = idleCallbacks_.front();

			it->Call(0, 0);
			idleCallbacks_.pop();
		}
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::UnloadAll()
	{
		loadedTextures_.clear();
		loadedShaders_.clear();
		loadedModels_.clear();
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
			content.type = ContentManager::ContentTypes::kTexture;
			environment::content_manager().AddPendingContent(content);
		}

		if (strcmp(contentType.c_str(), "shader") == 0)
		{
			isContent = true;
			content.type = ContentManager::ContentTypes::kShader;
			environment::content_manager().AddPendingContent(content);
		}

		if (strcmp(contentType.c_str(), "model") == 0)
		{
			isContent = true;
			content.type = ContentManager::ContentTypes::kModel;
			environment::content_manager().AddPendingContent(content);
		}

		if (!environment::game().started())
		{
			environment::content_manager().LoadPendingContent();
		}

		SNUFF_XASSERT(isContent == true, std::string("Content type '" + contentType + "' does not exist!").c_str());
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

		SNUFF_XASSERT(isContent == true, std::string("Content type '" + contentType + "' does not exist!").c_str());
		SNUFF_LOG_DEBUG(std::string("Unloaded " + contentPath).c_str());
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::JSUnloadAll(JS_ARGS)
	{
		JS_CHECK_PARAMS("V");
		environment::content_manager().UnloadAll();
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::JSIdle(JS_ARGS)
	{
		JS_CHECK_PARAMS("F");
		JS_CREATE_SCOPE;
		const Handle<Value>& value = wrapper.GetFunction(0);
		SharedPtr<JSCallback> callback = environment::memory().ConstructShared<JSCallback>(Handle<Function>::Cast(value));
		
		environment::content_manager().idleCallbacks().emplace(callback);
	}
}