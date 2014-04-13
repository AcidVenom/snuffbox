#pragma once

#include <v8.h>
#include <string>

using namespace v8;

namespace snuffbox
{
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

    /// Creates the JavaScript context
    Handle<Context> CreateContext(Isolate* isolate);

    void Log(const v8::FunctionCallbackInfo<v8::Value>& args);

  private:
    Isolate* isolate_; ///< The JavaScript isolate created at startup
    std::string path_; ///< The source directory path
  };
}