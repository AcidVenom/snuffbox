#include "../snuffbox/test.h"
#include "../snuffbox/logging.h"

namespace snuffbox
{
	//---------------------------------------------------------------------------------
	TestObject::TestObject()
	{

	}

	//---------------------------------------------------------------------------------
	TestObject::~TestObject()
	{

	}

	void TestObject::DoShit()
	{
		SNUFF_LOG_INFO("I DID SHIT");
	}
	
	//---------------------------------------------------------------------------------
	void TestObject::JSDoShit(JS_ARGS)
	{
		JS_CREATE_ARGUMENT_SCOPE;

		JS_SETUP(TestObject);

		self->DoShit();
	}

	//---------------------------------------------------------------------------------
	void TestObject::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = {
			JSFunctionRegister("doshit", JSDoShit)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}
}