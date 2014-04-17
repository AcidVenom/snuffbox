#pragma once

#include <v8.h>
#include <string>

using namespace v8;

#define JS_ISOLATE environment::js_state_wrapper().isolate()
#define JS_CONTEXT environment::js_state_wrapper().context()
#define JS_GLOBAL environment::js_state_wrapper().global()
#define JS_ARGS const v8::FunctionCallbackInfo<v8::Value>& args
#define JS_TEMPLATE const Handle<FunctionTemplate>& obj
#define JS_REGISTER_OBJECT_FUNCTIONS(obj,func,cons) for (unsigned int i = 0; i < ARRAYSIZE(func); ++i){if (cons){ obj->PrototypeTemplate()->Set(String::NewFromUtf8(JS_ISOLATE, func[i].name), FunctionTemplate::New(JS_ISOLATE, func[i].cb)); }else{obj->Set(String::NewFromUtf8(JS_ISOLATE, func[i].name), FunctionTemplate::New(JS_ISOLATE, func[i].cb)); }} 
#define JS_CREATE_SCOPE HandleScope handle_scope(JS_ISOLATE);
#define JS_REGISTER_GLOBAL(name) Handle<FunctionTemplate> obj = FunctionTemplate::New(JS_ISOLATE); JS_GLOBAL->Set(String::NewFromUtf8(JS_ISOLATE, name), obj);
#define JS_REGISTER_FUNCTIONS(func) for (unsigned int i = 0; i < ARRAYSIZE(func); ++i){JS_GLOBAL->Set(String::NewFromUtf8(JS_ISOLATE, func[i].name), FunctionTemplate::New(JS_ISOLATE, func[i].cb));}
#define JS_NAME(className)static const char* static_class_name() { return #className; } virtual const char* get_class_name() const { return static_class_name(); }
#define JS_SETUP_CALLBACKS Handle<Function> cb; Handle<Value> func;
#define JS_OBJECT_CALLBACK(name,obj) func = obj->Get(String::NewFromUtf8(JS_ISOLATE, name));cb = Handle<Function>::Cast(func);

namespace snuffbox
{
  /**
  * @struct snuffbox::JSFunctionRegister
  * @brief A structure to hold a name and a function associated with it, use as an array
  * @author Daniël Konings
  */
  struct JSFunctionRegister
  {
    JSFunctionRegister(const char* n, FunctionCallback f) : name(n), cb(f){}
    const char* name;
    FunctionCallback cb;
  };
  /**
  * @class snuffbox::JSStateWrapper
  * @brief Wraps the whole JavaScript state for use throughout the whole application
  * @author Daniël Konings
  */
  class JSStateWrapper
  {
  public:
    /// Default constructor
    JSStateWrapper();

    /// Default destructor
    ~JSStateWrapper();

    /// Returns the JavaScript isolate created at startup
    Isolate* isolate(){ return isolate_; }

    /// Returns the source directory path
    std::string& path(){ return path_; }

    /// Compiles and runs a JavaScript file
    void CompileAndRun(const char* path);

    /// Returns an error of a TryCatch object
    void GetException(TryCatch* try_catch);

    /// Initialises the JavaScript state
    void Initialise();

    /// Returns the global handle
    Local<ObjectTemplate> global(){ return Local<ObjectTemplate>::New(isolate_,global_); }

		/// Returns the context
		Local<Context> context(){ return Local<Context>::New(isolate_, context_); }

    /// Creates the JavaScript context
		Handle<Context> CreateContext();

  private:
    Isolate* isolate_; ///< The JavaScript isolate created at startup
    std::string path_; ///< The source directory path
		Persistent<ObjectTemplate, CopyablePersistentTraits<ObjectTemplate>> global_; ///< The function registry
		Persistent<Context, CopyablePersistentTraits<Context>> context_;	/// The JavaScript context

  private:
    /// JavaScript log debug
		static void JSLogDebug(JS_ARGS);

    /// JavaScript log info
		static void JSLogInfo(JS_ARGS);

    /// JavaScript log warning
		static void JSLogWarning(JS_ARGS);

    /// JavaScript log error
		static void JSLogError(JS_ARGS);

    /// JavaScript log fatal
		static void JSLogFatal(JS_ARGS);

    /// JavaScript log success
		static void JSLogSuccess(JS_ARGS);

		/// JavaScript require
		static void JSRequire(JS_ARGS);
  };

	/**
	* @class snuffbox::JSObject
	* @brief Every C++ object that will be extended to JS should extend from this
	* @author Daniël Konings
	*/
	class JSObject
	{
	public: 
		virtual ~JSObject(){}

		JS_NAME(JSObject);
		static void RegisterJS(JS_TEMPLATE);
	};

	template<typename T>
	class JSRegister
	{
	public:
		static void Register()
		{
			JS_CREATE_SCOPE;
			JS_REGISTER_GLOBAL(T::static_class_name());
			T::RegisterJS(obj);
		}
	};
}