#pragma once

#include "../../snuffbox/game.h"
#include "../../snuffbox/input/mouse.h"
#include "../../snuffbox/input/keyboard.h"
#include "../../snuffbox/input/mouse_area.h"
#include "../../snuffbox/d3d11/d3d11_camera.h"
#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/d3d11/elements/terrain_element.h"
#include "../../snuffbox/d3d11/elements/quad_element.h"
#include "../../snuffbox/d3d11/elements/billboard_element.h"
#include "../../snuffbox/d3d11/elements/text_element.h"
#include "../../snuffbox/d3d11/elements/mesh_element.h"
#include "../../snuffbox/d3d11/elements/polygon_element.h"
#include "../../snuffbox/d3d11/d3d11_settings.h"
#include "../../snuffbox/d3d11/d3d11_line.h"
#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/io/io_manager.h"
#include "../../snuffbox/d3d11/d3d11_post_processing.h"
#include "../../snuffbox/fmod/fmod_sound_system.h"

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
		JSRegister<Quad2D, true>::Register();
		JSRegister<Billboard, true>::Register();
		JSRegister<Widget, true>::Register();
    JSRegister<Text, true>::Register();
		JSRegister<Polygon, true>::Register();
		JSRegister<MouseArea, true>::Register();
		JSRegister<Mesh, true>::Register();
		JSRegister<ContentManager, false>::Register();
		JSRegister<D3D11Settings, false>::Register();		
		JSRegister<D3D11Line, false>::Register();
		JSRegister<IOManager, false>::Register();
		JSRegister<PostProcessing, false>::Register();
    JSRegister<SoundSystem, false>::Register();
	}
}