#pragma once

#include "../../snuffbox/game.h"
#include "../../snuffbox/input/mouse.h"
#include "../../snuffbox/input/keyboard.h"
#include "../../snuffbox/d3d11/d3d11_camera.h"
#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/d3d11/elements/terrain_element.h"
#include "../../snuffbox/d3d11/elements/quad_element.h"
#include "../../snuffbox/d3d11/elements/billboard_element.h"
#include "../../snuffbox/d3d11/d3d11_settings.h"
#include "../../snuffbox/content/content_manager.h"

namespace snuffbox
{
	/// Registers all C++ objects to the JavaScript environment for later use
	static void RegisterJSObjects()
	{
		JSRegister<Game,false>::Register();
		JSRegister<Mouse,false>::Register();
		JSRegister<Keyboard,false>::Register();
		JSRegister<Camera, true>::Register();
		JSRegister<Terrain, true>::Register();
		JSRegister<Quad, true>::Register();
		JSRegister<Billboard, true>::Register();
		JSRegister<ContentManager, false>::Register();
		JSRegister<D3D11Settings, false>::Register();
	}
}