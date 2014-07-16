#pragma once

#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	/**
	* @struct snuffbox::Resolution
	* @brief A rendering resolution structure
	* @author Daniël Konings
	*/
	struct Resolution
	{
		Resolution(unsigned int width, unsigned int height) : w(width), h(height){}
		unsigned int w, h;
	};

	/**
	* @struct snuffbox::RenderSettings
	* @brief Holds the actual information of the currently used render settings
	* @author Daniël Konings
	*/
	struct RenderSettings
	{
		RenderSettings() : vsync(1), fullscreen(false), cullMode(D3D11_CULL_FRONT), resolution(1280,720){}
		Resolution resolution;
		unsigned int vsync;
		bool fullscreen;
		D3D11_CULL_MODE cullMode;
		D3DXCOLOR bufferColor;
	};
	/**
	* @class snuffbox::D3D11Settings
	* @brief A class to set different rendering settings for Direct3D 11
	* @author Daniël Konings
	*/
	class D3D11Settings : public JSObject
	{
	public:
		/// Default constructor
		D3D11Settings();

		/// Default destructor
		~D3D11Settings();
		
		/// Returns the render settings
		RenderSettings& settings(){ return settings_; }

		/// Converts a cull mode enum to a string
		static const char* CullModeToString(D3D11_CULL_MODE mode);

	private:
		RenderSettings settings_;
	public:
		JS_NAME(RenderSettings);
		static void RegisterJS(JS_TEMPLATE);
		static void JSSetCullmode(JS_ARGS);
		static void JSSetResolution(JS_ARGS);
		static void JSSetFullscreen(JS_ARGS);
		static void JSSetVsync(JS_ARGS);
		static void JSSetBackBufferColor(JS_ARGS);
		static void JSResolution(JS_ARGS);
		static void JSSetWindowSize(JS_ARGS);
	};
}