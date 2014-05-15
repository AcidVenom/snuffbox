#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/js/js_object_register.h"
#include "../../snuffbox/logging.h"
#include <fstream>
#include <algorithm>

#define PRINT_RESULTS false
#define JS_LOG(severity)                                  \
bool first = true;                                        \
for (int i = 0; i < args.Length(); i++) {                 \
  HandleScope handle_scope(args.GetIsolate());						\
  if (first) {                                            \
    first = false;                                        \
  }                                                       \
	String::Utf8Value str(args[i]);													\
  log(severity, *str);                                    \
}                                                         \

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
		JS_LOG(LogSeverity::kDebug);
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogInfo(JS_ARGS)
	{
		JS_LOG(LogSeverity::kInfo);
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogWarning(JS_ARGS)
	{
		JS_LOG(LogSeverity::kWarning);
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogSuccess(JS_ARGS)
	{
		JS_LOG(LogSeverity::kSuccess);
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogError(JS_ARGS)
	{
		JS_LOG(LogSeverity::kError);
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSLogFatal(JS_ARGS)
	{
		JS_LOG(LogSeverity::kFatal);
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSRequire(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		String::Utf8Value str(args[0]);
		environment::js_state_wrapper().CompileAndRun(*str);
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::JSAssert(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		String::Utf8Value str(args[0]);
		SNUFF_ASSERT(*str);
	}

	//---------------------------------------------------------------------------
	Handle<Context> JSStateWrapper::CreateContext()
	{
		Handle<ObjectTemplate> global = ObjectTemplate::New(isolate_);
		global_.Reset(isolate_, global);

		RegisterJSObjects();

		JS_REGISTER_GLOBAL_TYPELESS("Log");

		JSFunctionRegister logFunctions[] =
		{
			JSFunctionRegister("debug", JSLogDebug),
			JSFunctionRegister("info", JSLogInfo),
			JSFunctionRegister("warning", JSLogWarning),
			JSFunctionRegister("success", JSLogSuccess),
			JSFunctionRegister("error", JSLogError),
			JSFunctionRegister("fatal", JSLogFatal),
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, logFunctions, false);

		JSFunctionRegister funcRegister[] =
		{
			JSFunctionRegister("require", JSRequire),
			JSFunctionRegister("assert", JSAssert)
		};

		JS_REGISTER_FUNCTIONS(funcRegister);

		return Context::New(isolate_, NULL, global);
	}

	//---------------------------------------------------------------------------
	JSStateWrapper::JSStateWrapper()
	{
		isolate_ = Isolate::GetCurrent();

		environment::globalInstance = this;
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::Initialise()
	{
		HandleScope scope(isolate_);
		Handle<Context> context = CreateContext();
		context_.Reset(isolate_, context);

		SNUFF_XASSERT(!context.IsEmpty(), "Failed creating the JavaScript context!");

		context->Enter();

		DWORD ftyp = GetFileAttributesA(path_.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
		{
			SNUFF_ASSERT("Source directory does not exist!");
		}

		CompileAndRun("main");

		context->Exit();
	}

	//---------------------------------------------------------------------------
	JSStateWrapper::~JSStateWrapper()
	{
		global_.Reset();
		context_.Reset();

		while (!V8::IdleNotification()){} //Garbage collection

		V8::Dispose();
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::CompileAndRun(const char* path)
	{
		std::string file_path = path_ + "/" + path + ".js";
		std::string error = "Couldn't find JavaScript file '" + file_path + "'";
		std::string src;

		char ch;
		std::fstream fin(file_path);

		if (fin)
		{
			while (fin >> std::noskipws >> ch) {
				src += ch;
			}

			TryCatch try_catch;

			Handle<Script> script = Script::Compile(String::NewFromUtf8(isolate_, src.c_str()), String::NewFromUtf8(isolate_, file_path.c_str()));
			Handle<Value> res = script->Run();

			if (res.IsEmpty())
			{
				bool failed = false;
				std::string exception(GetException(&try_catch, &failed));
				SNUFF_XASSERT(failed == false, std::string("Failed compiling a JavaScript file! " + exception).c_str());
			}
			else
			{
				if (PRINT_RESULTS && !res->IsUndefined())
				{
					std::string result = "'" + file_path + "' returned: " + *String::Utf8Value(res);

					SNUFF_LOG_INFO(result.c_str());
				}
			}
		}
		else
		{
			SNUFF_ASSERT(error.c_str());
		}

		FileWatch fileWatch;
		fileWatch.path = file_path;
		fileWatch.lastTime = GetTimeForFile(file_path);

		filesToWatch_.push_back(fileWatch);
	}

	//---------------------------------------------------------------------------
	SYSTEMTIME JSStateWrapper::GetTimeForFile(std::string path)
	{
		FILETIME creationTime;
		FILETIME lastAccessTime;
		FILETIME lastWriteTime;

		std::ifstream file;

		BOOL result = GetFileTime(&file, &creationTime, &lastAccessTime, &lastWriteTime);
		SNUFF_XASSERT(result == TRUE, std::string("Could not get time for file: " + path).c_str());

		SYSTEMTIME sysTime;
		FileTimeToSystemTime(&lastWriteTime, &sysTime);

		return sysTime;
	}

	//---------------------------------------------------------------------------
	void JSStateWrapper::WatchFiles()
	{
		for (auto it : filesToWatch_)
		{
			SYSTEMTIME lastTime = GetTimeForFile(it.path);
			if (it.lastTime.wMilliseconds != lastTime.wMilliseconds)
			{
				SNUFF_LOG_INFO(std::string("Hot reloaded JavaScript file: " + it.path).c_str());
				it.lastTime = lastTime;
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
		int64_t size = -static_cast<int64_t>(sizeof(data.GetParameter()));
		data.GetParameter()->persistent().Reset();
		environment::memory().Destruct<JSObject>(data.GetParameter());
		JS_ISOLATE->AdjustAmountOfExternalAllocatedMemory(size);
		data.GetValue().Clear();
	}

	//---------------------------------------------------------------------------
	template<typename T>
	void JSStateWrapper::JSNew(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		T* ptr = environment::memory().ConstructShared<T>(args);
		Handle<Context> ctx = JS_CONTEXT;
		ctx->Enter();
		Handle<Object> global = ctx->Global();
		Handle<Value> templ = global->Get(String::NewFromUtf8(JS_ISOLATE, ptr->get_class_name()));
		Handle<Function> objTemplate = Handle<Function>::Cast(templ);

		Handle<Object> obj = objTemplate->NewInstance();
		ptr->persistent().Reset(JS_ISOLATE, obj);
		ptr->persistent().SetWeak(static_cast<JSObject*>(ptr), JSDestroy);
		ptr->persistent().MarkIndependent();
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "__ptr"), External::New(JS_ISOLATE, static_cast<void*>(ptr)));
		int64_t allocated = JS_ISOLATE->AdjustAmountOfExternalAllocatedMemory(sizeof(ptr));
		ctx->Exit();

		args.GetReturnValue().Set(obj);
	}
}