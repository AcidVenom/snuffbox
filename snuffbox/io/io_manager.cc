#include "../../snuffbox/io/io_manager.h"
#include "../../snuffbox/js/js_wrapper.h"
#include <fstream>

namespace snuffbox
{
	namespace environment
	{
		namespace
		{
			IOManager* globalInstance = nullptr;
		}

		bool has_io_manager()
		{
			return globalInstance != nullptr;
		}

		IOManager& io_manager()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{
	//----------------------------------------------------------------------------------------
	IOManager::IOManager()
	{
		environment::globalInstance = this;
	}

	//----------------------------------------------------------------------------------------
	std::string IOManager::Open(std::string relativePath)
	{
		std::fstream fin(environment::js_state_wrapper().path() + "/" + relativePath);

		char ch;
		std::string result = "";
		if (fin)
		{
			while (fin >> std::noskipws >> ch) {
				result += ch;
			}
		}
		else
		{
			SNUFF_LOG_ERROR(std::string("Could not open file " + relativePath).c_str());
			fin.close();
			return "";
		}

		fin.close();

		return result;
	}

	//----------------------------------------------------------------------------------------
	void IOManager::Save(std::string relativePath, std::string data)
	{
		std::ofstream out(environment::js_state_wrapper().path() + "/" + relativePath);

		if (!out)
		{
			SNUFF_LOG_ERROR(std::string("Could not save to location " + relativePath).c_str());
		}
		else
		{
			out << std::noskipws << data;
		}

		out.close();
	}

	//----------------------------------------------------------------------------------------
	bool IOManager::FileExists(std::string relativePath)
	{
		std::fstream in(environment::js_state_wrapper().path() + "/" + relativePath);

		if (in)
		{
			return true;
		}

		in.close();

		return false;
	}

	//----------------------------------------------------------------------------------------
	void IOManager::RegisterJS(JS_TEMPLATE)
	{
		JSFunctionRegister funcs[] = {
			JSFunctionRegister("open", JSOpen),
			JSFunctionRegister("save", JSSave),
			JSFunctionRegister("fileExists", JSFileExists)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//----------------------------------------------------------------------------------------
	void IOManager::JSOpen(JS_ARGS)
	{
		JSWrapper wrapper(args);

		bool res = wrapper.CheckParams("S");

		if (res == false)
		{
			return;
		}

		std::string result = environment::io_manager().Open(wrapper.GetString(0));

		wrapper.ReturnString(result.c_str());
	}

	//----------------------------------------------------------------------------------------
	void IOManager::JSSave(JS_ARGS)
	{
		JSWrapper wrapper(args);

		bool res = wrapper.CheckParams("SS");

		if (res == false)
		{
			return;
		}

		environment::io_manager().Save(wrapper.GetString(0), wrapper.GetString(1));
	}

	void IOManager::JSFileExists(JS_ARGS)
	{
		JSWrapper wrapper(args);

		bool res = wrapper.CheckParams("S");

		if (res == false)
		{
			return;
		}

		wrapper.ReturnBool(environment::io_manager().FileExists(wrapper.GetString(0)));
	}
	//----------------------------------------------------------------------------------------
	IOManager::~IOManager()
	{
		environment::globalInstance = nullptr;
	}
}