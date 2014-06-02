#pragma once

#define _WINSOCKAPI_
#define NOMINMAX
#include <thread>

#include "../snuffbox/memory/shared_ptr.h"

#include "../snuffbox/win32/win32_window.h"
#include "../snuffbox/win32/win32_file_watch.h"

#include "../snuffbox/js/js_state_wrapper.h"
#include "../snuffbox/js/js_callback.h"

#include "../snuffbox/networking/connection.h"

#include "../snuffbox/input/mouse.h"
#include "../snuffbox/input/keyboard.h"

#include "../snuffbox/d3d11/d3d11_display_device.h"
#include "../snuffbox/d3d11/d3d11_camera.h"

#include "../snuffbox/content/content_manager.h"

#define SNUFF_MAIN CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)

namespace snuffbox
{
	/**
	* @enum snuffbox::GameEvents
	* @brief Contains all enumerations for the events of the game
	* @author Daniël Konings
	*/
	enum GameEvents
	{
		kQuit,
		kReload
	};
	/**
	* @class snuffbox::Game
	* @brief This is where all the magic happens
	* @author Daniël Konings
	*/
	class Game : public JSObject
	{
	public:
		/// Default constructor
		Game();

		/// Construct with a window
		Game(Win32Window* window);

		/// Default destructor
		~Game();

    /// Parses the command line
    void ParseCommandLine();
    
    /// Finds a command
    std::string GetCommand(const std::string& cmdLine, const char* option);

    /// Checks if a command exists
    bool CommandExists(const std::string& cmdLine, const char* option);

		/// Initialises the game
		void Initialise();

		/// Updates the game
		void Update();

		/// Draws the game
		void Draw();

		/// Shuts the game down
		void Shutdown();

		/// Returns the window the game is running in 
		Win32Window* window(){ return window_.get(); }

		/// Returns if the game is started or not
		bool started(){ return started_; }

		/// Notify the game of an event
		void NotifyEvent(GameEvents evt);

    /// Initialises, creates and shows the window
    void InitialiseWindow();

		/// Creates the JavaScript callbacks
		void CreateCallbacks();

		/// When a file is hot reloaded
		void Reload();

		/// Checks if the console is enabled
		bool consoleEnabled(){ return consoleEnabled_; }

		/// Returns the path the game is running in
		std::string path(){ return path_; }

	private:
		SharedPtr<Win32Window> window_; ///< The Win32 window hooked to the game
		SharedPtr<Mouse> mouse_; ///< The mouse object
		SharedPtr<Keyboard> keyboard_; ///< The keyboard object
		SharedPtr<D3D11DisplayDevice> device_; ///< The D3D11 device
		bool started_;	///< Is the game started yet?
		JSCallback initialise_; ///< The JavaScript update callback
		JSCallback update_; ///< The JavaScript update callback
		JSCallback draw_; ///< The JavaScript update callback
		JSCallback shutdown_; ///< The JavaScript update callback
		JSCallback onReload_; ///< The JavaScript onReload callback
		double deltaTime_;	///< The current delta time
		bool consoleEnabled_; ///< Is the console enabled?
		std::string path_; ///< The path the game is running in
		int gameTime_; ///< The game time
	public:
		JS_NAME(Game);
		static void RegisterJS(JS_TEMPLATE); ///< Registers all JavaScript functions
		static void JSRender(JS_ARGS); ///< Renders the scene
	};
}
