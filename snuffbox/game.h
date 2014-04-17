#pragma once

#include <Windows.h>
#include "../snuffbox/memory/shared_ptr.h"
#include "../snuffbox/platform/definitions.h"
#include "../snuffbox/js/js_state_wrapper.h"
#include "../snuffbox/js/js_callback.h"

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
		kQuit ///< Quit event
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
		Game(PlatformWindow* window);

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
		PlatformWindow* window(){ return window_.get(); }

		/// Returns if the game is started or not
		bool started(){ return started_; }

		/// Notify the game of an event
		void NotifyEvent(GameEvents evt);

    /// Initialises, creates and shows the window
    void InitialiseWindow();

		/// Creates the JavaScript callbacks
		void CreateCallbacks();

	private:
		SharedPtr<PlatformWindow> window_; ///< The Win32 window hooked to the game
		bool started_;	///< Is the game started yet?
		JSCallback initialise_; ///< The JavaScript update callback
		JSCallback update_; ///< The JavaScript update callback
		JSCallback draw_; ///< The JavaScript update callback
		JSCallback shutdown_; ///< The JavaScript update callback
		double deltaTime_;	///< The current delta time

	public:
		JS_NAME(Game);
		static void RegisterJS(JS_TEMPLATE); ///< Registers all JavaScript functions

	private:
		static void JSInit(JS_ARGS);	///< Creates a new instance of this object
	};
}
