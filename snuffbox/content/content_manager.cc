#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/content/content.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/memory/allocated_memory.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/fbx/fbx_loader.h"
#include "../../snuffbox/win32/win32_file_watch.h"

#include <fstream>

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
	bool ContentManager::Load<Font>(std::string path)
	{
		if (loaded_fonts_.find(path) != loaded_fonts_.end())
			return false;

		SNUFF_LOG_INFO(std::string("Loading font " + path).c_str());
		SharedPtr<Font> font = environment::memory().ConstructShared<Font>(path);
		SharedPtr<Content<Font>> content = environment::memory().ConstructShared<Content<Font>>(ContentTypes::kFont, font);

		loaded_fonts_.emplace(path, content);
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
	void ContentManager::Unload<Font>(std::string path)
	{
		SNUFF_XASSERT(loaded_fonts_.find(path) != loaded_fonts_.end(), "The font '" + path + "' was never loaded!");

		loaded_fonts_.erase(loaded_fonts_.find(path));
		SNUFF_LOG_INFO(std::string("Unloaded font " + path).c_str());
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
	template<>
	SharedPtr<Font>& ContentManager::Get<Font>(std::string path)
	{
		Content<Font>* contentPtr = nullptr;

		SNUFF_XASSERT(loaded_fonts_.find(path) != loaded_fonts_.end(), std::string("Font not loaded '" + path + "'!"));

		contentPtr = loaded_fonts_.find(path)->second.get();

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

		loaded_fonts_.clear();
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

		if (strcmp(contentType.c_str(), "font") == 0)
		{
			isContent = true;
			environment::content_manager().Load<Font>(contentPath);
		}

		if (strcmp(contentType.c_str(), "box") == 0)
		{
			isContent = true;
			environment::content_manager().ParsePackage(contentPath);
		}

		if (!isContent)
		{
			SNUFF_LOG_ERROR(std::string("Content type '" + contentType + "' does not exist").c_str());
		}
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::ParsePackage(std::string contentPath)
	{
		std::fstream content(environment::js_state_wrapper().path() + "/" + contentPath);

		if (content)
		{
			std::string package;
			char ch;

			while (content >> std::noskipws >> ch)
			{
				package += ch;
			}

			std::string current = "";
			std::string expected = "load";

			std::map<std::string, std::vector<std::string>> parsedValues;
			bool checkType = true;

			for (int i = 0; i < package.length(); ++i)
			{
				ch = package.at(i);

				SkipWhiteSpaces(i, package, ch);

				bool result = Consume(i, package, expected);

				if (result == false)
				{
					return;
				}
				else if (expected == "load")
				{
					expected = "(";
				}
				else if (expected == "(")
				{
					expected = "\"";
				}
				else if (expected == "\"")
				{
					++i;
					std::string val = GetValue(i, package, "\"");

					if (checkType == true)
					{
						SkipWhiteSpaces(i, package, ch);
						++i;
						result = Consume(i, package, ",");

						if (result == true)
						{
							SkipWhiteSpaces(i, package, ch);
							++i;
							result = Consume(i, package, "\"");

							if (result == true)
							{
								++i;
								std::string val2 = GetValue(i, package, "\"");
								auto it = parsedValues.find(val);

								if (it == parsedValues.end())
								{
									std::vector<std::string> vec;
									vec.push_back(val2);
									parsedValues.emplace(val, vec);
								}
								else
								{
									it->second.push_back(val2);
								}
								++i;

								bool result = Consume(i, package, ")");

								if (result == true)
								{
									expected = "load";
								}
							}
						}
					}
				}
			}
		}
		else
		{
			SNUFF_LOG_ERROR(std::string("Could not open box file " + contentPath).c_str());
		}

		content.close();
	}

	//-------------------------------------------------------------------------------------------
	void ContentManager::SkipWhiteSpaces(int& i, std::string& str, char& curChar)
	{
		char ch = str.at(i);
		while (i < str.length() && ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')
		{
			++i;
			curChar = ch;
			ch = str.at(i);
		}
	}

	//-------------------------------------------------------------------------------------------
	bool ContentManager::Consume(int& i, std::string& str, std::string toConsume)
	{
		int idx = 0;
		std::string result = "";
		char ch = str.at(i);

		while (i < str.length() && idx < toConsume.length() && str.at(i) == toConsume.at(idx))
		{
			result += str.at(i);
			++idx;
			++i;
		}

		if (result != toConsume)
		{
			result += str.at(i);
			SNUFF_LOG_ERROR(std::string("Expected ' " + toConsume + " ' but found ' " + result + " '").c_str());
			return false;
		}

		--i;
		return true;
	}

	//-------------------------------------------------------------------------------------------
	std::string ContentManager::GetValue(int& i, std::string& str, std::string endAt)
	{
		char ch = str.at(i);
		std::string val = "";

		while (ch != endAt.at(0))
		{
			val += ch;
			++i;
			ch = str.at(i);
		}

		return val;
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

		if (strcmp(contentType.c_str(), "font") == 0)
		{
			isContent = true;
			environment::content_manager().Unload<Font>(contentPath);
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