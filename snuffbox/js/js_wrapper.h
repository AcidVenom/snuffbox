#pragma once

#define JS_CHECK_PARAMS(format) JSWrapper wrapper(args); bool validParams = wrapper.CheckParams(format); if(!validParams){ SNUFF_ASSERT("Invalid parameters!"); }

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

		/// Returns a string to JavaScript
		void ReturnString(const char* str);

		/// Returns a JavaScript value
		void ReturnValue(Handle<Value> val);

		/// Returns the type as a string
		std::string TypeOf(Local<Value> value);

		/// Throws an error with the callee as prefix
		void Error(std::string error);

		/// Checks if the parameters given in the function are of the right type
		bool CheckParams(std::string format);

	private:
		const FunctionCallbackInfo<Value>& args_; //!< The JavaScript arguments passed by a function
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

		if (boolean->IsUndefined())
		{
			return false;
		}

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
	inline void JSWrapper::ReturnString(const char* str)
	{
		args_.GetReturnValue().Set(String::NewFromUtf8(JS_ISOLATE, str));
	}

	//------------------------------------------------------------------------------
	inline void JSWrapper::ReturnValue(Handle<Value> val)
	{
		args_.GetReturnValue().Set(val);
	}

	//------------------------------------------------------------------------------
	inline std::string JSWrapper::GetString(int arg)
	{
		return std::string(*String::Utf8Value(args_[arg]->ToString()));
	}

	//------------------------------------------------------------------------------
	inline void JSWrapper::Error(std::string error)
	{
		std::string objName = *String::Utf8Value(args_.This()->ToString());
		std::string functionName = *String::Utf8Value(args_.Callee()->ToString());

		SNUFF_LOG_ERROR(std::string("\n" + objName + "\n\t" + functionName + "\n\t\t" + error).c_str());
	}

	//------------------------------------------------------------------------------
	inline std::string JSWrapper::TypeOf(Local<Value> value)
	{
		if (value->IsFunction())
		{
			return "Function";
		}
		else if (value->IsArray())
		{
			return "Array";
		}
		else if (value->IsObject())
		{
			return "Object";
		}
		else if (value->IsNumber())
		{
			return "Number";
		}
		else if (value->IsNull())
		{
			return "Null";
		}
		else if (value->IsBoolean())
		{
			return "Boolean";
		}
		else if (value->IsUndefined())
		{
			return "Undefined";
		}
		else
		{
			return "Unknown";
		}
	}

	//------------------------------------------------------------------------------
	inline bool JSWrapper::CheckParams(std::string format)
	{
		unsigned int count = 0;

		for (auto it = format.c_str(); *it; it++)
		{
			char type = *it;

			if (tolower(type) == *"s")
			{
				if (!args_[count]->IsString())
				{
					Error(std::string("Expected String but got ") + TypeOf(args_[count]) + ", at argument " + std::to_string(count + 1));
					return false;
				}
			}
			else if (tolower(type) == *"f")
			{
				if (!args_[count]->IsFunction())
				{
					Error(std::string("Expected Function but got ") + TypeOf(args_[count]) + ", at argument " + std::to_string(count + 1));
					return false;
				}
			}
			else if (tolower(type) == *"a")
			{
				if (!args_[count]->IsArray())
				{
					Error(std::string("Expected Array but got ") + TypeOf(args_[count]) + ", at argument " + std::to_string(count + 1));
					return false;
				}
			}
			else if (tolower(type) == *"o")
			{
				if (!args_[count]->IsObject())
				{
					Error(std::string("Expected Object but got ") + TypeOf(args_[count]) + ", at argument " + std::to_string(count + 1));
					return false;
				}
			}
			else if (tolower(type) == *"n")
			{
				if (!args_[count]->IsNumber())
				{
					Error(std::string("Expected Number but got ") + TypeOf(args_[count]) + ", at argument " + std::to_string(count + 1));
					return false;
				}
			}
			else if (tolower(type) == *"b")
			{
				if (!args_[count]->IsBoolean())
				{
					Error(std::string("Expected Boolean but got ") + TypeOf(args_[count]) + ", at argument " + std::to_string(count + 1));
					return false;
				}
			}
			else if (tolower(type) == *"v")
			{
				return true;
			}
			else
			{
				SNUFF_ASSERT("Unknown parameter format!");
			}

			++count;

			if (count > static_cast<unsigned int>(args_.Length()))
			{
				Error("Not enough actual parameters!");
				return false;
			}
			
		}

		return true;
	}
}