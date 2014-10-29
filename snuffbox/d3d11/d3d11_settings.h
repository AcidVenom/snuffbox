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
		/**
		* @brief Creates the resolution structure with a width and height
		* @param[in] width (unsigned int) The width
		* @param[in] height (unsigned int) The height
		*/
		Resolution(unsigned int width, unsigned int height) : w(width), h(height){}

		unsigned int w; //!< The width of the resolution
		unsigned int h; //!< The height of the resolution
	};

	/**
	* @struct snuffbox::RenderSettings
	* @brief Holds the actual information of the currently used render settings
	* @author Daniël Konings
	*/
	struct RenderSettings
	{
		/**
		* @brief Creates the render settings with default settings
		*/
		RenderSettings() :
			vsync(1),
			fullscreen(false),
			cull_mode(D3D11_CULL_FRONT),
			resolution(640, 480),
			buffer_colour(0.0f, 0.0f, 0.0f, 1.0f),
			y_down(false)
		{}

		Resolution resolution; //!< The resolution
		unsigned int vsync; //!< Is vsync on or off
		bool fullscreen; //!< Is the application running fullscreen?
		bool y_down; //!< Is the Y axis flipped?
		D3D11_CULL_MODE cull_mode; //!< The cull mode
		D3DXCOLOR buffer_colour; //!< The buffer colour
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
		
		/**
		* @return (snuffbox::RenderSettings&) The actual settings
		*/
		RenderSettings& settings(){ return settings_; }

		/**
		* @brief Converts a cull mode enum to a string
		* @param[in] mode (D3D11_CULL_MODE) The cull mode to use
		* @return (const char*) The final string
		*/
		static const char* CullModeToString(D3D11_CULL_MODE mode);

		/**
		* @return (bool) Is the Y axis flipped?
		*/
		bool y_down(){ return settings_.y_down; }

	private:
		RenderSettings settings_; //!< The actual settings

	public:
		JS_NAME(RenderSettings);
		static void RegisterJS(JS_TEMPLATE);
		static void JSSetCullmode(JS_ARGS);
		static void JSSetResolution(JS_ARGS);
		static void JSSetFullscreen(JS_ARGS);
		static void JSSetVsync(JS_ARGS);
		static void JSSetBackBufferColour(JS_ARGS);
		static void JSResolution(JS_ARGS);
		static void JSSetWindowSize(JS_ARGS);
		static void JSSetYDown(JS_ARGS);
	};
}