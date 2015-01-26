#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/d3d11/d3d11_settings.h"
#include "../../snuffbox/d3d11/d3d11_line.h"
#include "../../snuffbox/js/js_wrapper.h"

namespace snuffbox
{
	//----------------------------------------------------------------------------------------
	void D3D11Line::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = {
			JSFunctionRegister("draw",JSDraw)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//----------------------------------------------------------------------------------------
	void D3D11Line::JSDraw(JS_ARGS)
	{
		JS_CHECK_PARAMS("NNNNNNNNNNNN");

		float x1 = wrapper.GetNumber<float>(0);
		float y1 = wrapper.GetNumber<float>(1);
		float z1 = wrapper.GetNumber<float>(2);

		float r1 = wrapper.GetNumber<float>(3);
		float g1 = wrapper.GetNumber<float>(4);
		float b1 = wrapper.GetNumber<float>(5);

		float x2 = wrapper.GetNumber<float>(6);
		float y2 = wrapper.GetNumber<float>(7);
		float z2 = wrapper.GetNumber<float>(8);

		float r2 = wrapper.GetNumber<float>(9);
		float g2 = wrapper.GetNumber<float>(10);
		float b2 = wrapper.GetNumber<float>(11);

    if (environment::render_settings().y_down() == true)
    {
      y2 = -y2;
      y1 = -y1;
    }

		environment::render_device().DrawLine(x1, y1, z1, r1, g1, b1, x2, y2, z2, r2, g2, b2);
	}
}