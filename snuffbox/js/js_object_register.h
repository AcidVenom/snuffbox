#pragma once

#include "../../snuffbox/game.h"
#include "../../snuffbox/input/mouse.h"
#include "../../snuffbox/input/keyboard.h"
#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/test.h"

namespace snuffbox
{
	/// Registers all C++ objects to the JavaScript environment for later use
	static void RegisterJSObjects()
	{
		JSRegister<Game,false>::Register();
		JSRegister<Mouse,false>::Register();
		JSRegister<Keyboard,false>::Register();
		JSRegister<TestObject,true>::Register();
	}
}