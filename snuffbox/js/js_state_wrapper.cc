#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/js/js_object_register.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/win32/win32_file_watch.h"
#include "../../snuffbox/js/js_wrapper.h"
#include <fstream>
#include <algorithm>
#include <comdef.h>

#define PRINT_RESULTS false

namespace snuffbox
{
	//---------------------------------------------------------------------------
	namespace environment
	{
		namespace
		{
			JSStateWrapper* globalInstance = nullptr;
		}

		bool has_js_state_wrapper(){ return globalInstance != nullptr; }

		JSStateWrapper& js_state_wrapper()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogDebug(JS_ARGS)
	{
    JSWrapper wrapper(args);
    SNUFF_LOG_DEBUG(wrapper.GetString(0).c_str());
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogInfo(JS_ARGS)
	{
    JSWrapper wrapper(args);
    SNUFF_LOG_INFO(wrapper.GetString(0).c_str());
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogWarning(JS_ARGS)
	{
    JSWrapper wrapper(args);
    SNUFF_LOG_WARNING(wrapper.GetString(0).c_str());
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogSuccess(JS_ARGS)
	{
    JSWrapper wrapper(args);
    SNUFF_LOG_SUCCESS(wrapper.GetString(0).c_str());
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogError(JS_ARGS)
	{
    JSWrapper wrapper(args);
    SNUFF_LOG_ERROR(wrapper.GetString(0).c_str());
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogFatal(JS_ARGS)
	{
    JSWrapper wrapper(args);
    SNUFF_LOG_FATAL(wrapper.GetString(0).c_str());
	}

  //---------------------------------------------------------------------------
  void JSStateWrapper::JSLogRGB(JS_ARGS)
  {
    JSWrapper wrapper(args);

    float r1 = wrapper.GetNumber<float>(1);
    float g1 = wrapper.GetNumber<float>(2);
    float b1 = wrapper.GetNumber<float>(3);

    float r2 = r1 / 2;
    float g2 = g1 / 2;
    float b2 = b1 / 2;

    if (wrapper.argLength() > 4)
    {
      r2 = wrapper.GetNumber<float>(4);
      g2 = wrapper.GetNumber<float>(5);
      b2 = wrapper.GetNumber<float>(6);
    }
    
    SNUFF_LOG_RGB(wrapper.GetString(0).c_str(), r1, g1, b1, r2, g2, b2);
  }

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogWatch(JS_ARGS)
	{
		JS_CHECK_PARAMS("SV");

		environment::console().Watch(wrapper.GetString(0), args[1]);
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSRequire(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("S");

		bool doReturn = wrapper.GetBool(1);

		if (doReturn)
		{
			environment::js_state_wrapper().CompileAndRun(wrapper.GetString(0).c_str(), false, &wrapper);
		}
		else
		{
			environment::js_state_wrapper().CompileAndRun(wrapper.GetString(0).c_str());
		}
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSAssert(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("S");
		SNUFF_ASSERT(wrapper.GetString(0).c_str());
	}

	//---------------------------------------------------------------------------
	Local<Context> JSStateWrapper::CreateContext()
	{
		Handle<ObjectTemplate> global = ObjectTemplate::New(isolate_);
		global_.Reset(isolate_, global);

		JS_REGISTER_GLOBAL_TYPELESS("Log");

		JSFunctionRegister logFunctions[] =
		{
			JSFunctionRegister("debug", JSLogDebug),
			JSFunctionRegister("info", JSLogInfo),
			JSFunctionRegister("warning", JSLogWarning),
			JSFunctionRegister("success", JSLogSuccess),
			JSFunctionRegister("error", JSLogError),
			JSFunctionRegister("fatal", JSLogFatal),
      JSFunctionRegister("rgb", JSLogRGB),
			JSFunctionRegister("watch", JSLogWatch)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, logFunctions, false);

		JSFunctionRegister funcRegister[] =
		{
			JSFunctionRegister("require", JSRequire),
			JSFunctionRegister("assert", JSAssert)
		};

		JS_REGISTER_FUNCTIONS(funcRegister);
		RegisterJSObjects();

		return Context::New(isolate_, NULL, global);
	}

	//---------------------------------------------------------------------------
	JSStateWrapper::JSStateWrapper()
	{
		isolate_ = Isolate::GetCurrent();

		environment::globalInstance = this;
		SNUFF_LOG_INFO("Initialised V8");
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::Initialise()
	{
		HandleScope scope(isolate_);
		Handle<Context> context = CreateContext();
		context_.Reset(isolate_, context);
    context->Enter();

		SNUFF_XASSERT(!context.IsEmpty(), "Failed creating the JavaScript context!");

		DWORD ftyp = GetFileAttributesA(path_.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
		{
			SNUFF_ASSERT("Source directory does not exist!");
		}

		CompileAndRun("main");
		SNUFF_LOG_SUCCESS("Succesfully initialised the JavaScript state");

		RegisterJSEnumerators();
	}

	//---------------------------------------------------------------------------
	JSStateWrapper::~JSStateWrapper()
	{
    Destroy();
	}

  //---------------------------------------------------------------------------
  void JSStateWrapper::Destroy()
  {
		SNUFF_LOG_DEBUG("Collecting all garbage..");
		V8::LowMemoryNotification();
		SNUFF_LOG_DEBUG(".. Collected garbage");
    global_.Reset();
    context_.Reset();

    V8::Dispose();
  }

	//---------------------------------------------------------------------------
	void JSStateWrapper::CompileAndRun(const char* path, bool reloading, JSWrapper* wrapper)
	{
		JS_CREATE_SCOPE;
		std::string file_path = path_ + "/" + path + ".js";
		std::string error = "Couldn't find JavaScript file '" + file_path + "'";
		std::string src;
		Local<Value> res;

		char ch;
		std::fstream fin(file_path);

		if (fin)
		{
			while (fin >> std::noskipws >> ch) {
				src += ch;
			}

			TryCatch try_catch;

			Local<Script> script = Script::Compile(String::NewFromUtf8(isolate_, src.c_str()), String::NewFromUtf8(isolate_, file_path.c_str()));
			res = script->Run();

			if (res.IsEmpty())
			{
				bool failed = false;
				std::string exception(GetException(&try_catch, &failed));
				if (!reloading)
				{
					SNUFF_XASSERT(failed == false, std::string("Failed compiling a JavaScript file! " + exception).c_str());
				}
				else
				{
					if (wrapper != nullptr)
					{
						wrapper->ReturnString("error");
					}
					SNUFF_LOG_ERROR(exception.c_str());
					return;
				}
			}
			else
			{
				if (PRINT_RESULTS && !res->IsUndefined())
				{
					std::string result = "'" + file_path + "' returned:\n" + *String::Utf8Value(res);

					SNUFF_LOG_INFO(result.c_str());
				}

				if (wrapper != nullptr)
				{
					wrapper->ReturnValue(res);
				}
			}
		}
		else
		{
			SNUFF_ASSERT(error.c_str());
		}

		fin.close();

		if (!reloading)
		{
			if (loadedFiles_.find(file_path) == loadedFiles_.end())
			{
				environment::file_watcher().AddFile(file_path, std::string(path), FileType::kScript);
				loadedFiles_.emplace(file_path, true);
			}
		}
	}

	//---------------------------------------------------------------------------
	std::string JSStateWrapper::GetException(TryCatch* try_catch, bool* failed)
	{
		HandleScope handle_scope(isolate_);
		String::Utf8Value exception(try_catch->Exception());
		Handle<Message> message = try_catch->Message();

		std::string error = "";

		if (!message.IsEmpty()){
			String::Utf8Value sourceline(message->GetSourceLine());
			error += "\n\n";

			std::string srcline = *sourceline;
			
			auto it = std::remove_if(std::begin(srcline), std::end(srcline), [](char c){ return (c == '\t'); });
			srcline.erase(it, std::end(srcline));

			error += srcline;
			error += "\n";

			int start = message->GetStartColumn();
			for (int i = 0; i < start; i++) {
				error += " ";
			}
			int end = message->GetEndColumn();
			for (int i = start; i < end; i++) {
				error += "^";
			}
			String::Utf8Value stack_trace(try_catch->StackTrace());

			error += "\n\t";
			error += *stack_trace;
			error += "\n";

			if (stack_trace.length() > 0) {
				*failed = true;
				return error;
			}
		}

		return std::string("");
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSDestroy(const v8::WeakCallbackData<v8::Object, JSObject>& data)
	{
		JSObject* ptr = data.GetParameter();

		RenderElement* renderPtr = dynamic_cast<RenderElement*>(ptr);

		if (renderPtr)
		{
			renderPtr->RemoveFromRenderer();
		}

		Camera* cameraPtr = dynamic_cast<Camera*>(ptr);

		if (cameraPtr)
		{
			if (environment::has_render_device())
			{
				environment::render_device().SetCamera(nullptr);
			}
		}

		int64_t size = -static_cast<int64_t>(sizeof(ptr));
		ptr->persistent().Reset();
		environment::memory().Destruct<JSObject>(ptr);
		data.GetValue().Clear();
		JS_ISOLATE->AdjustAmountOfExternalAllocatedMemory(size*10);
	}

	//---------------------------------------------------------------------------
	template<typename T>
	void JSStateWrapper::JSNew(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		T* ptr = environment::memory().ConstructShared<T>(args);
    Local<Context> ctx = JS_CONTEXT;
		Local<Object> global = ctx->Global();
		Local<Value> templ = global->Get(String::NewFromUtf8(JS_ISOLATE, ptr->get_class_name()));
		Local<Function> objTemplate = Local<Function>::Cast(templ);

		Handle<Object> obj = objTemplate->NewInstance();
		ptr->persistent().Reset(JS_ISOLATE, obj);
		ptr->persistent().SetWeak(static_cast<JSObject*>(ptr), JSDestroy);
		ptr->persistent().MarkIndependent();
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "__ptr"), External::New(JS_ISOLATE, static_cast<void*>(ptr)));
		int64_t size = static_cast<int64_t>(sizeof(ptr));

		JS_ISOLATE->AdjustAmountOfExternalAllocatedMemory(size*10);
    static_cast<JSObject*>(ptr)->RegisterExtraFunctions(obj);
		args.GetReturnValue().Set(obj);
	}
}