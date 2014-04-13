#pragma once

#include <v8.h>

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

    /// Returns the JavaScript context
    Handle<Context>& context(){ return context_; }

    /// Compiles and runs a JavaScript file
    void CompileAndRun(const char* path);

    /// Returns an error of a TryCatch object
    void GetException(TryCatch* try_catch);

  private:
    Isolate* isolate_; ///< The JavaScript isolate created at startup
    Handle<Context> context_; ///< The JavaScript context
  };
}