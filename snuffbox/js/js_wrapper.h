#pragma once

#include <v8.h>
#include "../../snuffbox/js/js_state_wrapper.h"

using namespace v8;

namespace snuffbox
{
	/**
	* @class snuffbox::JSWrapper
	* @brief Wrapper to make your life easier retrieving JavaScript values
	* @author Dani�l Konings
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

		/// Sets a tuple as the return value of the arguments in .x and .y
		template<typename T>
		void ReturnTuple(T x, T y);

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

	//------------------------------------------------------------------------------
	template<typename T>
	inline void JSWrapper::ReturnTuple(T x, T y)
	{
		auto isolate = environment::js_state_wrapper().isolate();
		Local<Object> retVal = Object::New(isolate);
		retVal->Set(String::NewFromUtf8(isolate, "x"), Number::New(isolate, static_cast<T>(x)));
		retVal->Set(String::NewFromUtf8(isolate, "y"), Number::New(isolate, static_cast<T>(y)));

		args_.GetReturnValue().Set(retVal);
	}
}