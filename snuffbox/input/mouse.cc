#include "../../snuffbox/input/mouse.h"
#include "../../snuffbox/logging.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/win32/win32_window.h"
#include "../../snuffbox/d3d11/d3d11_settings.h"
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
		return button_states_[button].down;
	}

	//--------------------------------------------------------------------------------------
	bool Mouse::IsPressed(MouseEnums::MouseButton button)
	{
		return button_states_[button].pressed;
	}

  //--------------------------------------------------------------------------------------
	bool Mouse::IsReleased(MouseEnums::MouseButton button)
  {
    return button_states_[button].released;
  }

	//--------------------------------------------------------------------------------------
	bool Mouse::IsDoubleClicked(MouseEnums::MouseButton button)
	{
		return button_states_[button].dblclk;
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
			D3D11_VIEWPORT* vp = &environment::render_device().viewport();

			if (vp)
			{
				float x1 = vp->TopLeftX;
				float y1 = vp->TopLeftY;
				float x2 = vp->Width;
				float y2 = vp->Height;

				float xx = (x - x1) / x2;
				float yy = (y - y1) / y2;

				xx = xx * 2 - 1;
				yy = yy * 2 - 1;

				if (xx < -1)
				{
					xx = -1;
				}

				if (yy < -1)
				{
					yy = -1;
				}

				if (xx > 1)
				{
					xx = 1;
				}

				if (yy > 1)
				{
					yy = 1;
				}

				wrapper.ReturnTuple<double>(xx, yy);
			}
			else
			{
				wrapper.ReturnTuple<double>(0, 0);
			}
			
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
			button_states_[i].pressed = false;
			button_states_[i].released = false;
			button_states_[i].dblclk = false;
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
				button_states_[evt.button].pressed = true;
				break;

			case MouseEnums::MouseEvent::kDblClk:
				button_states_[evt.button].down = true;
				button_states_[evt.button].dblclk = true;
				x_ = evt.x;
				y_ = evt.y;
				break;

			case MouseEnums::MouseEvent::kMove:
				x_ = evt.x;
				y_ = evt.y;
				break;

			case MouseEnums::MouseEvent::kPressed:
        button_states_[evt.button].down = true;
        button_states_[evt.button].pressed = true;
        x_ = evt.x;
        y_ = evt.y;
        break;

			case MouseEnums::MouseEvent::kDown:
				button_states_[evt.button].down = true;
				x_ = evt.x;
				y_ = evt.y;
				break;

			case MouseEnums::MouseEvent::kUp:
				button_states_[evt.button].down = false;
				button_states_[evt.button].released = true;
				break;
			}
			queue_.pop();
		}

		dx_ = x_ - prev_x_;
		dy_ = y_ - prev_y_;

		prev_x_ = x_;
		prev_y_ = y_;
	}
}
