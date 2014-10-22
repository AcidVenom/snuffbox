#include "../../snuffbox/input/mouse_area.h"
#include "../../snuffbox/d3d11/elements/widget_element.h"

namespace snuffbox
{
	//----------------------------------------------------------------------------------
	MouseArea::MouseArea()
	{

	}

	//----------------------------------------------------------------------------------
	MouseArea::MouseArea(JS_ARGS)
	{
		JSWrapper wrapper(args);
		wrapper.SetLogErrors(false);
		
		if (wrapper.CheckParams("NNNN") == false)
		{
			wrapper.SetLogErrors(true);
			if (wrapper.CheckParams("O") == true)
			{
				parent_ = wrapper.GetPointer<Widget>(0);

				if (parent_ != nullptr)
				{
					XMVECTOR size = parent_->scale() * parent_->size();
					metrics_.w = XMVectorGetX(size);
					metrics_.h = XMVectorGetZ(size);

					XMMATRIX offset = parent_->offset();
					XMVECTOR trans = parent_->translation();
					metrics_.x = XMVectorGetX(trans) + offset._41;
					metrics_.y = XMVectorGetY(trans) + offset._43;
				}
			}
		}
		else
		{
			metrics_.x = wrapper.GetNumber<float>(0);
			metrics_.y = wrapper.GetNumber<float>(1);
			metrics_.w = wrapper.GetNumber<float>(2);
			metrics_.h = wrapper.GetNumber<float>(3);
		}
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
	void MouseArea::RegisterJS(JS_TEMPLATE)
	{
		JSFunctionRegister funcs[] = {
			JSFunctionRegister("setMetrics", JSSetMetrics)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}
	
	//----------------------------------------------------------------------------------
	void MouseArea::JSSetMetrics(JS_ARGS)
	{
		JS_SETUP(MouseArea, "NNNN");

		self->SetMetrics(wrapper.GetNumber<float>(0), wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2), wrapper.GetNumber<float>(3));
	}
}