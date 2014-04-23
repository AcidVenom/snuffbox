#pragma once

#define _WINSOCKAPI_
#define NOMINMAX
#include <Windows.h>
#include "../../snuffbox/platform/interfaces.h"

#define SNUFF_WINDOW_CLASS "Snuff Window Class"

namespace snuffbox
{

	enum MouseButton;

	/**
	* @class snuffbox::Win32Window
	* @brief A Windows window for use with the engine
	* @author Daniël Konings
	*/
	class Win32Window : public IPlatformWindow
	{
	public:
		/// Default constructor
		Win32Window();

		/**
		* @brief Constructs a window with a width, height and position
		* @param[in] name (const char*) The name of the window
		* @param[in] w (int) The width of the window
		* @param[in] h (int) The height of the window
		* @param[in] x (int) The x position of the window (optional, will center if left out)
		* @param[in] y (int) The y position of the window (optional, will center if left out)
		*/
		Win32Window(const char* name, int w, int h, int x = SNUFF_WINDOW_CENTERED, int y = SNUFF_WINDOW_CENTERED);

		/// Default destructor
		~Win32Window();

		/// Creates the window
		void Create();
		/// Shows the window
		void Show();
		/// Destroys the window
		void Destroy();
		/// When the window is focussed
		void OnSetFocus();
		/// When the window focus is killed
		void OnKillFocus();
		/// When the window is closed
		void OnClose();
		/// When the mouse is moved
		void OnMouseMove(LPARAM lParam, WPARAM wParam);
		/// When the mouse is held down
		void OnMouseDown(MouseButton button, LPARAM lParam, WPARAM wParam);
		/// When the mouse is released
		void OnMouseUp(MouseButton button);
		/// Processes all messages
		void ProcessMessages();

	private:
		HWND handle_; ///< The reference to the window handle used by this window
    HINSTANCE instance_; ///< The reference to the hInstance
		bool focussed_; ///< Check to see if the window is focussed or not
	};

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); ///< The windows message loop
}