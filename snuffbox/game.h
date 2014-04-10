#pragma once

#include <Windows.h>
#include "../snuffbox/memory/shared_ptr.h"
#include "../snuffbox/platform/definitions.h"

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
	class Game
	{
	public:
		/// Default constructor
		Game();

		/// Construct with a window
		Game(PlatformWindow* window);

		/// Default destructor
		~Game();

		/// Initialises the game
		void Initialise();

		/// Updates the game
		void Update();

		/// Shuts the game down
		void Shutdown();

		/// Returns the window the game is running in 
		PlatformWindow* window(){ return window_.get(); }

		/// Returns if the game is started or not
		bool started(){ return started_; }

		/// Notify the game of an event
		void NotifyEvent(GameEvents evt);

		/// Creates the global pointer in the environment
		void MakeGlobal();

	private:
		SharedPtr<PlatformWindow> window_; ///< The Win32 window hooked to the game
		bool started_;	///< Is the game started yet?
	};
}
