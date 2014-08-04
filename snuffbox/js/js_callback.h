#pragma once

#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/environment.h"

namespace snuffbox
{
	class JSCallback
	{
	public:
		/// Default constructor
		JSCallback()
		{

		}
		/// Construction through function handle
		JSCallback(const Handle<Function>& func)
		{
			SetFunction(func);
		}
		/// Default destructor
		~JSCallback(){}

		/// Sets the persistent function
		inline void SetFunction(const Handle<Function>& func)
		{
			func_.Reset(JS_ISOLATE,func);
		}

		/// Calls the function with or without arguments
		inline void Call(int argc,Handle<Value>* argv = {})
		{
			JS_CREATE_SCOPE;

			Handle<Function> func = Local<Function>::New(JS_ISOLATE, func_);
			Local<Context> ctx = JS_CONTEXT;
			TryCatch try_catch;

			bool failed = false;
			std::string exception;

			if (argc == 0)
			{
				func->Call(ctx->Global(), argc, 0);
				exception = environment::js_state_wrapper().GetException(&try_catch,&failed);
			}
			else if (argc > 0)
			{
				func->Call(ctx->Global(), argc, argv);
				exception = environment::js_state_wrapper().GetException(&try_catch, &failed);
			}

			if (failed)
			{
				SNUFF_LOG_ERROR(exception.c_str());
			}
		}
	private:
		Persistent<Function> func_; //!< The actual function of this callback
	};
}