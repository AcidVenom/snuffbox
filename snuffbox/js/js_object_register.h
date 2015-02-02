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
#include "../../snuffbox/d3d11/d3d11_render_target.h"
#include "../../snuffbox/content/content_manager.h"
#include "../../snuffbox/io/io_manager.h"
#include "../../snuffbox/fmod/fmod_sound_system.h"


#define NEW_JS_ENUM(name) Handle<Object> ##name = Object::New(JS_ISOLATE);
#define JS_ENUM_SET(name, field, value) name->Set(String::NewFromUtf8(JS_ISOLATE, field), Number::New(JS_ISOLATE, value));
#define JS_REGISTER_ENUM(variable, name) JS_CONTEXT->Global()->Set(String::NewFromUtf8(JS_ISOLATE, name), variable);

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
    JSRegister<SoundSystem, false>::Register();
		JSRegister<RenderTarget, true>::Register();
	}

	static void RegisterJSEnumerators()
	{
		NEW_JS_ENUM(text_alignment);
		JS_ENUM_SET(text_alignment, "Left", 0);
		JS_ENUM_SET(text_alignment, "Right", 1);
		JS_ENUM_SET(text_alignment, "Center", 2);

		NEW_JS_ENUM(topology);
		JS_ENUM_SET(topology, "LineList", 2);
		JS_ENUM_SET(topology, "LineStrip", 3);
		JS_ENUM_SET(topology, "TriangleList", 4);
		JS_ENUM_SET(topology, "TriangleStrip", 5);

		NEW_JS_ENUM(sampler_type);
		JS_ENUM_SET(sampler_type, "Linear", 0);
		JS_ENUM_SET(sampler_type, "Point", 1);

    NEW_JS_ENUM(blend_states);
    JS_ENUM_SET(blend_states, "Default", 0);
    JS_ENUM_SET(blend_states, "PreMultiplied", 1);

		JS_REGISTER_ENUM(text_alignment, "TextAlignment");
		JS_REGISTER_ENUM(topology, "Topology");
		JS_REGISTER_ENUM(sampler_type, "Sampling");
    JS_REGISTER_ENUM(blend_states, "BlendStates");
	}
}