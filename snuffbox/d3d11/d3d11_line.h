#pragma once

#include "../../snuffbox/js/js_state_wrapper.h"


namespace snuffbox
{
	/**
	* @class snuffbox::D3D11Line
	* @brief A line class to draw lines
	* @author Daniël Konings
	*/
	class D3D11Line : public JSObject
	{
	protected:
		class Line;
	public:
		JS_NAME(Line);

		static void RegisterJS(JS_TEMPLATE);
		static void JSDraw(JS_ARGS);
	};
}