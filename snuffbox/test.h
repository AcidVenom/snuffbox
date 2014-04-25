#pragma once

#include "../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	class TestObject : public JSObject
	{
	public:
		TestObject();
		TestObject(JS_ARGS){};
		~TestObject();

		void DoShit();


	private:
		int x_;
		int y_;

	public:

		JS_NAME(TestObject);
		static void RegisterJS(JS_TEMPLATE);
		static void JSDoShit(JS_ARGS);
	};
}