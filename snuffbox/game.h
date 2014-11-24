#pragma once

#include <thread>
#include <chrono>

using namespace std::chrono;

#include <Windows.h>
#include "../snuffbox/memory/shared_ptr.h"
#include "../snuffbox/js/js_state_wrapper.h"
#include "../snuffbox/js/js_callback.h"
#include "../snuffbox/js/js_wrapper.h"

#define SNUFF_MAIN CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)

namespace snuffbox
{
	class Win32Window;
	class Mouse;
	class Keyboard;
	class D3D11DisplayDevice;

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
		/// Construct with a window
		Game(Win32Window* window, QApplication& app);

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

		/// Checks if the console is enabled
		bool consoleEnabled(){ return consoleEnabled_; }

		/// Runs the game
		void Run();

		/// Returns the path the game is running in
		std::string path(){ return path_; }

		/// Returns the qt application
		QApplication& app(){ return qtApp_; }

	private:
		SharedPtr<Win32Window> window_; //!< The Win32 window hooked to the game
		SharedPtr<Mouse> mouse_; //!< The mouse object
		SharedPtr<Keyboard> keyboard_; //!< The keyboard object
		SharedPtr<D3D11DisplayDevice> device_; //!< The D3D11 device
		bool started_;	//!< Is the game started yet?
		JSCallback initialise_; //!< The JavaScript update callback
		JSCallback update_; //!< The JavaScript update callback
		JSCallback draw_; //!< The JavaScript update callback
		JSCallback shutdown_; //!< The JavaScript update callback
		JSCallback onReload_; //!< The JavaScript onReload callback
		double deltaTime_;	//!< The current delta time
		bool consoleEnabled_; //!< Is the console enabled?
		std::string path_; //!< The path the game is running in
		int gameTime_; //!< The game time
		high_resolution_clock::time_point lastTime_;	//!< The last clock time for delta timing
		QApplication& qtApp_; //!< The qt application
		bool shouldQuit_; //!< If the application should quit
		bool shouldReload_; //!< If the application should reload
		bool doReload_; //!< Should all content be reloaded?
	public:
		JS_NAME(Game);
		static void RegisterJS(JS_TEMPLATE); //!< Registers all JavaScript functions
		static void JSRender(JS_ARGS); //!< Renders the scene
		static void JSClearRenderer(JS_ARGS); //!< Clears the entire renderer
    static void JSCleanUp(JS_ARGS); //!< Forces a garabage collection
		static void JSSetName(JS_ARGS); //!< Sets the window name
		static void JSShowConsole(JS_ARGS); //!< Shows the console
    static void JSQuit(JS_ARGS); //!< Quits the game
	};
}
