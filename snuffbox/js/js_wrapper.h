#pragma once

#include <v8.h>
#include "../../snuffbox/js/js_state_wrapper.h"

using namespace v8;

namespace snuffbox
{
	/**
	* @class snuffbox::JSWrapper
	* @brief Wrapper to make your life easier retrieving JavaScript values
	* @author Daniël Konings
	*/
	class JSWrapper
	{
	public:
		/// Default constructor
		JSWrapper(const FunctionCallbackInfo<Value>& args) : args_(args){}

		/// Default destructor
		~JSWrapper(){}

		/// Retrieves a double value from a given argument index
		template<typename T>
		T GetNumber(int arg);

		/// Retrieves a pointer from a JavaScript object
		template<typename T>
		T* GetPointer(int arg);

	private:
		const FunctionCallbackInfo<Value>& args_; ///< The JavaScript arguments passed by a function
	};

	//------------------------------------------------------------------------------
	template<typename T>
	inline T JSWrapper::GetNumber(int arg)
	{
		return static_cast<T>(args_[arg]->NumberValue());
	}

	//------------------------------------------------------------------------------
	template<typename T>
	inline T* JSWrapper::GetPointer(int arg)
	{
		Handle<Object> obj = args_[arg]->ToObject();
		Handle<Value> val = obj->Get(String::NewFromUtf8(environment::js_state_wrapper().isolate(), "__ptr"));
		Handle<External> ptr = val.As<External>();

		return static_cast<T*>(ptr->Value());
	}
}