#pragma once

#include <v8.h>
#include <string>

using namespace v8;

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
    Handle<ObjectTemplate> global(){ return global_; }

    /// Creates the JavaScript context
    Handle<Context> CreateContext();

  private:
    Isolate* isolate_; ///< The JavaScript isolate created at startup
    std::string path_; ///< The source directory path
    Handle<ObjectTemplate> global_; ///< The function registry

  private:
    /// JavaScript log debug
    static void JSLogDebug(const v8::FunctionCallbackInfo<v8::Value>& args);

    /// JavaScript log info
    static void JSLogInfo(const v8::FunctionCallbackInfo<v8::Value>& args);

    /// JavaScript log warning
    static void JSLogWarning(const v8::FunctionCallbackInfo<v8::Value>& args);

    /// JavaScript log error
    static void JSLogError(const v8::FunctionCallbackInfo<v8::Value>& args);

    /// JavaScript log fatal
    static void JSLogFatal(const v8::FunctionCallbackInfo<v8::Value>& args);

    /// JavaScript log success
    static void JSLogSuccess(const v8::FunctionCallbackInfo<v8::Value>& args);
  };
}