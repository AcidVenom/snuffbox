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
	bool Mouse::IsDown(MouseEnums::MouseButton button)
	{
		return buttonStates_[button].down;
	}

	//--------------------------------------------------------------------------------------
	bool Mouse::IsPressed(MouseEnums::MouseButton button)
	{
		return buttonStates_[button].pressed;
	}

  //--------------------------------------------------------------------------------------
	bool Mouse::IsReleased(MouseEnums::MouseButton button)
  {
    return buttonStates_[button].released;
  }

	//--------------------------------------------------------------------------------------
	bool Mouse::IsDoubleClicked(MouseEnums::MouseButton button)
	{
		return buttonStates_[button].dblclk;
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
			JSFunctionRegister("IsReleased", JSIsReleased),
			JSFunctionRegister("IsDoubleClicked", JSIsDoubleClicked),
			JSFunctionRegister("WheelUp", JSWheelUp),
			JSFunctionRegister("WheelDown", JSWheelDown)
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
	void Mouse::JSWheelUp(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;

		bool check = environment::mouse().IsPressed(MouseEnums::MouseButton::kWheelUp);
		args.GetReturnValue().Set(Boolean::New(JS_ISOLATE, check));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSWheelDown(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;

		bool check = environment::mouse().IsPressed(MouseEnums::MouseButton::kWheelDown);
		args.GetReturnValue().Set(Boolean::New(JS_ISOLATE, check));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsPressed(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		int button = args[0]->Int32Value();

		bool check = environment::mouse().IsPressed(static_cast<MouseEnums::MouseButton>(button));
		args.GetReturnValue().Set(Boolean::New(JS_ISOLATE, check));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsDown(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		int button = args[0]->Int32Value();

		bool check = environment::mouse().IsDown(static_cast<MouseEnums::MouseButton>(button));
		args.GetReturnValue().Set(Boolean::New(JS_ISOLATE, check));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsReleased(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
    int button = args[0]->Int32Value();

		bool check = environment::mouse().IsReleased(static_cast<MouseEnums::MouseButton>(button));
    args.GetReturnValue().Set(Boolean::New(JS_ISOLATE, check));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsDoubleClicked(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		int button = args[0]->Int32Value();

		bool check = environment::mouse().IsDoubleClicked(static_cast<MouseEnums::MouseButton>(button));
		args.GetReturnValue().Set(Boolean::New(JS_ISOLATE, check));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::ResetStates()
	{
		for (unsigned int i = 0; i < 5; ++i)
		{
			buttonStates_[i].pressed = false;
			buttonStates_[i].released = false;
			buttonStates_[i].dblclk = false;
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
    ResetStates();

		while (!queue_.empty())
		{
			const MouseData& evt = queue_.front();
			switch (evt.type)
			{
			case MouseEnums::MouseEvent::kWheel:
				buttonStates_[evt.button].pressed = true;
				break;

			case MouseEnums::MouseEvent::kDblClk:
				buttonStates_[evt.button].down = true;
				buttonStates_[evt.button].dblclk = true;
				x_ = evt.x;
				y_ = evt.y;
				break;

			case MouseEnums::MouseEvent::kMove:
				x_ = evt.x;
				y_ = evt.y;
				break;

			case MouseEnums::MouseEvent::kPressed:
        buttonStates_[evt.button].down = true;
        buttonStates_[evt.button].pressed = true;
        x_ = evt.x;
        y_ = evt.y;
        break;

			case MouseEnums::MouseEvent::kDown:
				buttonStates_[evt.button].down = true;
				x_ = evt.x;
				y_ = evt.y;
				break;

			case MouseEnums::MouseEvent::kUp:
				buttonStates_[evt.button].down = false;
				buttonStates_[evt.button].released = true;
				break;
			}
			queue_.pop();
		}
	}
}
