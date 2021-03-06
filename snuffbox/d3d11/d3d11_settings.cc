#include "../../snuffbox/d3d11/d3d11_settings.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/environment.h"
#include "../../snuffbox/game.h"
#include "../../snuffbox/win32/win32_window.h"

namespace snuffbox
{
	namespace environment
	{
		namespace
		{
			D3D11Settings* globalInstance = nullptr;
		}

		bool has_render_settings(){ return globalInstance != nullptr; }
		D3D11Settings& render_settings(){ 
			SNUFF_ASSERT_NOTNULL(globalInstance); 
			return *globalInstance; 
		}
	}
}
namespace snuffbox
{
	//-------------------------------------------------------------------------------
	const char* D3D11Settings::CullModeToString(D3D11_CULL_MODE mode)
	{
		switch (mode)
		{
		case D3D11_CULL_NONE:
			return "None";
		case D3D11_CULL_FRONT:
			return "Front";
		case D3D11_CULL_BACK:
			return "Back";
		}

		return "Unknown";
	}

	//-------------------------------------------------------------------------------
	D3D11Settings::D3D11Settings()
	{
		environment::globalInstance = this;
		settings_.buffer_colour = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//-------------------------------------------------------------------------------
	D3D11Settings::~D3D11Settings()
	{
		environment::globalInstance = nullptr;
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::JSSetCullmode(JS_ARGS)
	{
		JS_CHECK_PARAMS("N");
		unsigned int mode = wrapper.GetNumber<unsigned int>(0);
		environment::render_settings().settings().cull_mode = static_cast<D3D11_CULL_MODE>(mode);
		environment::render_device().SetCullMode(environment::render_settings().settings().cull_mode);
		SNUFF_LOG_INFO(std::string("[Settings] Cull mode: " + std::string(CullModeToString(environment::render_settings().settings().cull_mode))).c_str());
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::JSSetFullscreen(JS_ARGS)
	{
		JS_CHECK_PARAMS("B");
		bool mode = wrapper.GetBool(0);
		environment::render_settings().settings().fullscreen = mode;
		environment::render_device().SetFullscreen(mode);
		SNUFF_LOG_INFO(std::string("[Settings] Fullscreen: " + std::to_string(mode)).c_str());
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::JSSetResolution(JS_ARGS)
	{
		JS_CHECK_PARAMS("NN");
		unsigned int w, h;

		w = wrapper.GetNumber<unsigned int>(0);
		h = wrapper.GetNumber<unsigned int>(1);

		environment::render_settings().settings().resolution.w = w;
		environment::render_settings().settings().resolution.h = h;


		SNUFF_LOG_INFO(std::string("[Settings] Resized the window to: " + std::to_string(w) + "x" + std::to_string(h)).c_str());
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::JSSetVsync(JS_ARGS)
	{
		JS_CHECK_PARAMS("B");
		int mode = static_cast<int>(wrapper.GetBool(0));
		environment::render_settings().settings().vsync = mode;

		SNUFF_LOG_INFO(std::string("[Settings] VSync: " + std::to_string(mode)).c_str());
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::JSSetBackBufferColour(JS_ARGS)
	{
		JS_CHECK_PARAMS("NNNN");
		float rgb[4] = { 
			wrapper.GetNumber<float>(0), 
			wrapper.GetNumber<float>(1), 
			wrapper.GetNumber<float>(2), 
			wrapper.GetNumber<float>(3) 
		};
		environment::render_settings().settings().buffer_colour = D3DXCOLOR(rgb);
		
		SNUFF_LOG_INFO(std::string("[Settings] Changed back buffer colour to: (" + std::to_string(rgb[0]) + "," + std::to_string(rgb[1]) + "," + std::to_string(rgb[2]) + "," + std::to_string(rgb[3]) + ")").c_str());
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::RegisterJS(JS_TEMPLATE)
	{
		JSFunctionRegister funcs[] = {
			JSFunctionRegister("setCullMode", JSSetCullmode),
			JSFunctionRegister("setFullscreen", JSSetFullscreen),
			JSFunctionRegister("setResolution", JSSetResolution),
			JSFunctionRegister("setVsync", JSSetVsync),
			JSFunctionRegister("setBackBufferColour", JSSetBackBufferColour),
			JSFunctionRegister("resolution", JSResolution),
			JSFunctionRegister("setWindowSize", JSSetWindowSize),
			JSFunctionRegister("setYDown", JSSetYDown),
			JSFunctionRegister("setFixedStep", JSSetFixedStep)
		};

		obj->Set(JS_ISOLATE, "CullNone", Number::New(JS_ISOLATE, 1));
		obj->Set(JS_ISOLATE, "CullFront", Number::New(JS_ISOLATE, 2));
		obj->Set(JS_ISOLATE, "CullBack", Number::New(JS_ISOLATE, 3));

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::JSResolution(JS_ARGS)
	{
		JS_CHECK_PARAMS("V");

		float w = environment::render_settings().settings().resolution.w;
		float h = environment::render_settings().settings().resolution.h;

		wrapper.ReturnTuple(w, h, "w", "h");
	}
	
	//-------------------------------------------------------------------------------
	void D3D11Settings::JSSetYDown(JS_ARGS)
	{
		JS_CHECK_PARAMS("B");

		environment::render_settings().settings().y_down = wrapper.GetBool(0);
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::JSSetWindowSize(JS_ARGS)
	{
		JS_CHECK_PARAMS("NN");

		float w = wrapper.GetNumber<float>(0);
		float h = wrapper.GetNumber<float>(1);

		RECT clientSize;
		clientSize.left = clientSize.top = 0;
		clientSize.right = w;
		clientSize.bottom = h;

		int style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX;

		AdjustWindowRect(&clientSize, style, FALSE);
		unsigned int actualWidth = clientSize.right - clientSize.left;
		unsigned int actualHeight = clientSize.bottom - clientSize.top;

		environment::game().window()->params().w = w;
		environment::game().window()->params().h = h;

		SetWindowPos(environment::game().window()->handle(), HWND_TOP, (GetSystemMetrics(SM_CXSCREEN) - actualWidth) / 2, (GetSystemMetrics(SM_CYSCREEN) - actualHeight) / 2, actualWidth, actualHeight, NULL);

		environment::render_device().ResizeBuffers();
	}

	//-------------------------------------------------------------------------------
	void D3D11Settings::JSSetFixedStep(JS_ARGS)
	{
		JSWrapper wrapper(args);
		environment::game().SetFixedStep(wrapper.GetNumber<float>(0));
	}
}