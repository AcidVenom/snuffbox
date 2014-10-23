#include "../../snuffbox/input/mouse_area.h"
#include "../../snuffbox/d3d11/elements/widget_element.h"
#include "../../snuffbox/input/mouse.h"

namespace snuffbox
{
	//----------------------------------------------------------------------------------
	MouseArea::MouseArea()
	{

	}

	//----------------------------------------------------------------------------------
	MouseArea::MouseArea(JS_ARGS) :
		activated_(true),
		hovered_(false)
	{
		JSWrapper wrapper(args);
		wrapper.SetLogErrors(false);
		
		if (wrapper.CheckParams("NNNN") == false)
		{
			wrapper.SetLogErrors(true);
			if (wrapper.CheckParams("O") == true)
			{
				parent_ = wrapper.GetPointer<Widget>(0);

				UpdateFromParent();
			}
		}
		else
		{
			metrics_.x = wrapper.GetNumber<float>(0);
			metrics_.y = wrapper.GetNumber<float>(1);
			metrics_.w = wrapper.GetNumber<float>(2);
			metrics_.h = wrapper.GetNumber<float>(3);
		}

		environment::mouse().RegisterMouseArea(this);
	}

	//----------------------------------------------------------------------------------
	void MouseArea::UpdateFromParent()
	{
		if (parent_ != nullptr)
		{
			XMVECTOR size = parent_->scale() * parent_->size();
			metrics_.w = XMVectorGetX(size);
			metrics_.h = XMVectorGetZ(size);

			XMMATRIX& trans = parent_->world_matrix(nullptr);

			metrics_.x = trans._41;
			metrics_.y = -trans._42;
		}
	}

	//----------------------------------------------------------------------------------
	MouseAreaMetrics MouseArea::metrics()
	{
		UpdateFromParent();
		return metrics_;
	}

	//----------------------------------------------------------------------------------
	void MouseArea::SetActivated(bool activated)
	{
		activated_ = activated;
	}

	//----------------------------------------------------------------------------------
	void MouseArea::SetMetrics(float x, float y, float w, float h)
	{
		metrics_.x = x;
		metrics_.y = y;
		metrics_.w = w;
		metrics_.h = h;
	}

	//----------------------------------------------------------------------------------
	void MouseArea::SetCallback(MouseAreaStates type, Handle<Function> callback, Local<Value> callee)
	{
		int idx = static_cast<int>(type);
		MouseAreaCallback& cb = callbacks_[idx];
		cb.callback.SetFunction(callback);
		cb.callback_set = true;
		cb.callee.Reset(JS_ISOLATE,callee);
	}

	//----------------------------------------------------------------------------------
	void MouseArea::Notify(MouseAreaStates type, int button)
	{
		if (type == MouseAreaStates::kEnter)
		{
			hovered_ = true;
		}

		if (type == MouseAreaStates::kLeave)
		{
			hovered_ = false;
		}

		MouseAreaCallback& cb = callbacks_[static_cast<int>(type)];
		if (cb.callback_set == true)
		{
			JS_CREATE_SCOPE;
			Handle<Value> obj = Local<Value>::New(JS_ISOLATE, cb.callee);
			Handle<Number> num = Number::New(JS_ISOLATE, button);

			Handle<Value> argv[] = {
				obj,
				num
			};
			cb.callback.Call(2, argv);
		}
	}

	//----------------------------------------------------------------------------------
	bool MouseArea::activated()
	{
		return activated_;
	}

	//----------------------------------------------------------------------------------
	bool MouseArea::hovered()
	{
		return hovered_;
	}

	//----------------------------------------------------------------------------------
	void MouseArea::SetHovered(bool hovered)
	{
		hovered_ = hovered;
	}

	//----------------------------------------------------------------------------------
	void MouseArea::RegisterJS(JS_TEMPLATE)
	{
		JSFunctionRegister funcs[] = {
			JSFunctionRegister("setMetrics", JSSetMetrics),
			JSFunctionRegister("setActivated", JSSetActivated),
			JSFunctionRegister("activated", JSActivated),
			JSFunctionRegister("setOnEnter", JSSetOnEnter),
			JSFunctionRegister("setOnLeave", JSSetOnLeave),
			JSFunctionRegister("setOnPressed", JSSetOnPressed),
			JSFunctionRegister("setOnReleased", JSSetOnReleased),
			JSFunctionRegister("setOnDown", JSSetOnDown),
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}

	//----------------------------------------------------------------------------------
	void MouseArea::JSSetActivated(JS_ARGS)
	{
		JS_SETUP(MouseArea, "B");

		self->SetActivated(wrapper.GetBool(0));
	}

	//----------------------------------------------------------------------------------
	void MouseArea::JSActivated(JS_ARGS)
	{
		JS_SETUP(MouseArea, "V");
		wrapper.ReturnBool(self->activated());
	}
	
	//----------------------------------------------------------------------------------
	void MouseArea::JSSetMetrics(JS_ARGS)
	{
		JS_SETUP(MouseArea, "NNNN");

		self->SetMetrics(wrapper.GetNumber<float>(0), wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2), wrapper.GetNumber<float>(3));
	}

	//----------------------------------------------------------------------------------
	void MouseArea::JSSetOnEnter(JS_ARGS)
	{
		JS_SETUP(MouseArea, "F");

		Handle<Function> func = Handle<Function>::Cast(wrapper.GetValue(0));
		Handle<Object> callee = Handle<Object>::Cast(wrapper.GetValue(1));

		self->SetCallback(MouseAreaStates::kEnter, func, callee);
	}

	//----------------------------------------------------------------------------------
	void MouseArea::JSSetOnLeave(JS_ARGS)
	{
		JS_SETUP(MouseArea, "F");
		Handle<Function> func = Handle<Function>::Cast(wrapper.GetValue(0));
		Handle<Object> callee = Handle<Object>::Cast(wrapper.GetValue(1));

		self->SetCallback(MouseAreaStates::kLeave, func, callee);
	}

	//----------------------------------------------------------------------------------
	void MouseArea::JSSetOnPressed(JS_ARGS)
	{
		JS_SETUP(MouseArea, "F");
		Handle<Function> func = Handle<Function>::Cast(wrapper.GetValue(0));
		Local<Value> callee = wrapper.GetValue(1);

		self->SetCallback(MouseAreaStates::kPressed, func, callee);
	}

	//----------------------------------------------------------------------------------
	void MouseArea::JSSetOnReleased(JS_ARGS)
	{
		JS_SETUP(MouseArea, "F");
		Handle<Function> func = Handle<Function>::Cast(wrapper.GetValue(0));
		Handle<Object> callee = Handle<Object>::Cast(wrapper.GetValue(1));

		self->SetCallback(MouseAreaStates::kReleased, func, callee);
	}

	//----------------------------------------------------------------------------------
	void MouseArea::JSSetOnDown(JS_ARGS)
	{
		JS_SETUP(MouseArea, "F");
		Handle<Function> func = Handle<Function>::Cast(wrapper.GetValue(0));
		Handle<Object> callee = Handle<Object>::Cast(wrapper.GetValue(1));

		self->SetCallback(MouseAreaStates::kDown, func, callee);
	}

	//----------------------------------------------------------------------------------
	MouseArea::~MouseArea()
	{
		for (unsigned int i = 0; i < 5; ++i)
		{
			callbacks_[i].callee.Reset();
		}
	}
}