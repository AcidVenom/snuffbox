#include "../../snuffbox/input/mouse.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/win32/win32_window.h"
#include "../../snuffbox/game.h"

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
			JSFunctionRegister("movement", JSGetMovement),
			JSFunctionRegister("isPressed",JSIsPressed),
			JSFunctionRegister("isDown",JSIsDown),
			JSFunctionRegister("isReleased", JSIsReleased),
			JSFunctionRegister("isDoubleClicked", JSIsDoubleClicked),
			JSFunctionRegister("wheelUp", JSWheelUp),
			JSFunctionRegister("wheelDown", JSWheelDown)
		};

		obj->Set(String::NewFromUtf8(JS_ISOLATE, "Pixel"), Number::New(JS_ISOLATE, 0));
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "Screen"), Number::New(JS_ISOLATE, 1));

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSGetPosition(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("N");

		std::tuple<double, double> pos = environment::mouse().position();
		double x = std::get<0>(pos);
		double y = std::get<1>(pos);
		unsigned int type = wrapper.GetNumber<unsigned int>(0);

		switch (type)
		{
		case 0:
			wrapper.ReturnTuple<double>(x, y);
			break;
		case 1:
			float nx = x / (environment::game().window()->params().w - SM_CXFIXEDFRAME*2);
			float ny = y / (environment::game().window()->params().h - 100);

			nx = nx * 2 - 1;
			ny = ny * 2 - 1;
			wrapper.ReturnTuple<double>(nx, ny);
			break;
		}
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSGetMovement(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("V");
		std::tuple<double, double> movement = environment::mouse().movement();
		wrapper.ReturnTuple<double>(std::get<0>(movement), std::get<1>(movement));
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSWheelUp(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("V");
		bool check = environment::mouse().IsPressed(MouseEnums::MouseButton::kWheelUp);
		wrapper.ReturnBool(check);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSWheelDown(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("V");
		bool check = environment::mouse().IsPressed(MouseEnums::MouseButton::kWheelDown);
		wrapper.ReturnBool(check);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsPressed(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("N");
		int button = wrapper.GetNumber<int>(0);

		bool check = environment::mouse().IsPressed(static_cast<MouseEnums::MouseButton>(button));
		wrapper.ReturnBool(check);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsDown(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("N");
		int button = wrapper.GetNumber<int>(0);

		bool check = environment::mouse().IsDown(static_cast<MouseEnums::MouseButton>(button));
		wrapper.ReturnBool(check);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsReleased(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("N");
		int button = wrapper.GetNumber<int>(0);

		bool check = environment::mouse().IsReleased(static_cast<MouseEnums::MouseButton>(button));
		wrapper.ReturnBool(check);
	}

	//--------------------------------------------------------------------------------------
	void Mouse::JSIsDoubleClicked(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;
		JS_CHECK_PARAMS("N");
		int button = wrapper.GetNumber<int>(0);

		bool check = environment::mouse().IsDoubleClicked(static_cast<MouseEnums::MouseButton>(button));
		wrapper.ReturnBool(check);
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

		dx_ = 0.0f;
		dy_ = 0.0f;

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

		dx_ = x_ - prevX_;
		dy_ = y_ - prevY_;

		prevX_ = x_;
		prevY_ = y_;
	}
}
