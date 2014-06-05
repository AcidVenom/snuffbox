#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/content/content.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/memory/allocated_memory.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/js/js_wrapper.h"

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
		SharedPtr<Shader> texture = environment::memory().ConstructShared<Shader>(std::string(path));
		SharedPtr<Content<Shader>> content = environment::memory().ConstructShared<Content<Shader>>(ContentTypes::kShader, texture);

		loadedShaders_.emplace(path, content);
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
	void ContentManager::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = {
			JSFunctionRegister("load", JSLoad),
			JSFunctionRegister("unload", JSUnload),
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

			switch (it.type)
			{
			case ContentTypes::kTexture:
				Load<Texture>(it.path);
				break;
			case ContentTypes::kShader:
				Load<Shader>(it.path);
				break;
			}

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
	void ContentManager::JSLoad(JS_ARGS)
	{
		JSWrapper wrapper(args);

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

		if (!environment::game().started())
		{
			environment::content_manager().LoadPendingContent();
		}

		SNUFF_XASSERT(isContent == true, std::string("Content type '" + contentType + "' does not exist!").c_str());
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::JSUnload(JS_ARGS)
	{
		JSWrapper wrapper(args);

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
		}

		SNUFF_XASSERT(isContent == true, std::string("Content type '" + contentType + "' does not exist!").c_str());
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::JSIdle(JS_ARGS)
	{
		JSWrapper wrapper(args);
		JS_CREATE_SCOPE;
		const Handle<Value>& value = wrapper.GetFunction(0);
		SharedPtr<JSCallback> callback = environment::memory().ConstructShared<JSCallback>(Handle<Function>::Cast(value));
		
		environment::content_manager().idleCallbacks().emplace(callback);
	}
}