#include "../../snuffbox/input/mouse.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/environment.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------
	namespace environment {
		namespace
		{
			Mouse* globalInstance = nullptr;
		}

		bool has_mouse() { return globalInstance != nullptr; }

		Mouse& mouse()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

namespace snuffbox
{
	//--------------------------------------------------------------------------------------
	Mouse::Mouse() : x_(0), y_(0)
	{
		ResetStates();
		environment::globalInstance = this;
	}
	
	//--------------------------------------------------------------------------------------
	Mouse::~Mouse()
	{

	}

	//--------------------------------------------------------------------------------------
	bool Mouse::IsDown(MouseButton button)
	{
		return buttonStates_[button].down;
	}

	//--------------------------------------------------------------------------------------
	bool Mouse::IsPressed(MouseButton button)
	{
		return buttonStates_[button].pressed;
	}

	//--------------------------------------------------------------------------------------
	void Mouse::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = 
		{
			JSFunctionRegister("position",JSGetPosition),
			JSFunctionRegister("IsPressed",JSIsPressed),
			JSFunctionRegister("IsDown",JSIsDown),
			JSFunctionRegister("IsReleased", JSIsReleased)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSGetPosition(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		std::tuple<double, double> pos = environment::mouse().position();
		Local<Object> retVal = Object::New(JS_ISOLATE);
		retVal->Set(String::NewFromUtf8(JS_ISOLATE,"x"), Number::New(JS_ISOLATE, std::get<0>(pos)));
		retVal->Set(String::NewFromUtf8(JS_ISOLATE, "y"), Number::New(JS_ISOLATE, std::get<1>(pos)));

		args.GetReturnValue().Set(retVal);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsPressed(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		int button = args[0]->Int32Value();

		bool check = environment::mouse().IsPressed(static_cast<MouseButton>(button));
		args.GetReturnValue().Set(Boolean::New(JS_ISOLATE, check));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsDown(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		int button = args[0]->Int32Value();

		bool check = environment::mouse().IsDown(static_cast<MouseButton>(button));
		args.GetReturnValue().Set(Boolean::New(JS_ISOLATE, check));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsReleased(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
	}

	//--------------------------------------------------------------------------------------
	void Mouse::ResetStates()
	{
		for (unsigned int i = 0; i < 3; ++i)
		{
			buttonStates_[i].down = false;
			buttonStates_[i].pressed = false;
			buttonStates_[i].released = false;
		}
	}

	//--------------------------------------------------------------------------------------
	void Mouse::ReceiveEvent(MouseData& evt)
	{
		queue_.push(evt);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::Update()
	{
		while (!queue_.empty())
		{
			const MouseData& evt = queue_.front();
			switch (evt.type)
			{
			case MouseEvent::kMove:
				x_ = evt.x;
				y_ = evt.y;
				break;

			case MouseEvent::kDown:
				buttonStates_[evt.button].down = true;
				x_ = evt.x;
				y_ = evt.y;
				break;

			case MouseEvent::kUp:
				buttonStates_[evt.button].down = false;
				buttonStates_[evt.button].released = true;
				break;
			}
			queue_.pop();
		}
	}
}
