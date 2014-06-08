#pragma once

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

		/// Retrieves a function from a given argument index
		const Handle<Value> GetFunction(int arg);

		/// Retrieves a boolean value from a given argument index
		bool GetBool(int arg);

		/// Retrieves a number value from a given argument index
		template<typename T>
		T GetNumber(int arg);

		/// Retrieves a pointer from a JavaScript object
		template<typename T>
		T* GetPointer(int arg);

		/// Gets a string for the arguments
		std::string GetString(int arg);

		/// Returns a boolean
		void ReturnBool(bool value);

		/// Returns a number
		template<typename T>
		void ReturnNumber(T value);

		/// Sets a tuple as the return value of the arguments in .x and .y (defaults)
		template<typename T>
		void ReturnTuple(T x, T y, const char* n1 = "x", const char* n2 = "y");

		/// Sets a tuple as the return value of the arguments in .x, .y and .z (defaults)
		template<typename T>
		void ReturnTriple(T x, T y, T z, const char* n1 = "x", const char* n2 = "y", const char* n3 = "z");

	private:
		const FunctionCallbackInfo<Value>& args_; ///< The JavaScript arguments passed by a function
	};

	//------------------------------------------------------------------------------
	inline const Handle<Value> JSWrapper::GetFunction(int arg)
	{
		JS_CREATE_SCOPE;
		const Handle<Value>& func = args_[arg];

		if (!func->IsFunction())
		{
			Local<Value> funcName = args_.Callee();
			Local<String> functionName = funcName->ToString();
			SNUFF_ASSERT(std::string("Tried to insert a non-function as argument in function " + std::string(*String::Utf8Value(functionName))).c_str());
		}
		return Handle<Function>::Cast(func);
	}

	//------------------------------------------------------------------------------
	inline bool JSWrapper::GetBool(int arg)
	{
		JS_CREATE_SCOPE;
		const Handle<Value>& boolean = args_[arg];

		return boolean->BooleanValue();
	}

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
		Local<Value> obj = args_[arg];
		if (obj->IsUndefined())
		{
			Local<Value> funcName = args_.Callee();
			Local<String> functionName = funcName->ToString();
			SNUFF_ASSERT(std::string("Tried to get the pointer of an undefined object in: " + std::string(*String::Utf8Value(functionName))).c_str());
		}
		Local<Object> object = obj->ToObject();
		Local<Value> val = object->Get(String::NewFromUtf8(environment::js_state_wrapper().isolate(), "__ptr"));
		Local<External> ptr = val.As<External>();

		return static_cast<T*>(ptr->Value());
	}

	//------------------------------------------------------------------------------
	inline void JSWrapper::ReturnBool(bool value)
	{
		args_.GetReturnValue().Set(Boolean::New(JS_ISOLATE, value));
	}

	//------------------------------------------------------------------------------
	template<typename T>
	inline void JSWrapper::ReturnNumber(T value)
	{
		args_.GetReturnValue().Set(Number::New(JS_ISOLATE, static_cast<T>(value)));
	}

	//------------------------------------------------------------------------------
	template<typename T>
	inline void JSWrapper::ReturnTuple(T x, T y, const char* n1, const char* n2)
	{
		Local<Object> retVal = Object::New(JS_ISOLATE);
		retVal->Set(String::NewFromUtf8(JS_ISOLATE, n1), Number::New(JS_ISOLATE, static_cast<T>(x)));
		retVal->Set(String::NewFromUtf8(JS_ISOLATE, n2), Number::New(JS_ISOLATE, static_cast<T>(y)));

		args_.GetReturnValue().Set(retVal);
	}

	//------------------------------------------------------------------------------
	template<typename T>
	inline void JSWrapper::ReturnTriple(T x, T y, T z, const char* n1, const char* n2, const char* n3)
	{
		Local<Object> retVal = Object::New(JS_ISOLATE);
		retVal->Set(String::NewFromUtf8(JS_ISOLATE, n1), Number::New(JS_ISOLATE, static_cast<T>(x)));
		retVal->Set(String::NewFromUtf8(JS_ISOLATE, n2), Number::New(JS_ISOLATE, static_cast<T>(y)));
		retVal->Set(String::NewFromUtf8(JS_ISOLATE, n3), Number::New(JS_ISOLATE, static_cast<T>(z)));

		args_.GetReturnValue().Set(retVal);
	}

	//------------------------------------------------------------------------------
	inline std::string JSWrapper::GetString(int arg)
	{
		return std::string(*String::Utf8Value(args_[arg]->ToString()));
	}
}