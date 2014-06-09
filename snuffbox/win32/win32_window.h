#pragma once

#include <Windows.h>
#include "../../snuffbox/input/mouse.h"

#define SNUFF_WINDOW_CENTERED -1
#define SNUFF_WINDOW_CLASS "Snuff Window Class"

namespace snuffbox
{

	/**
	* @struct snuffbox::WindowParameters
	* @brief Contains information of the parameters a window was created with
	* @author Daniël Konings
	*/
	struct WindowParameters
	{
		int x, y, w, h;
		std::string name;
	};

	/**
	* @class snuffbox::Win32Window
	* @brief A Windows window for use with the engine
	* @author Daniël Konings
	*/
	class Win32Window
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
		Win32Window(std::string name, int w, int h, int x = SNUFF_WINDOW_CENTERED, int y = SNUFF_WINDOW_CENTERED);

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
    void OnMouseMove(float x, float y);
		/// When the mouse is held down
		void OnMouseDown(MouseEnums::MouseButton button, float x, float y);
		/// When the mouse is released
		void OnMouseUp(MouseEnums::MouseButton button);
		/// When the mouse is double clicked
		void OnMouseDbl(MouseEnums::MouseButton button, float x, float y);
		/// When the key is pressed
		void OnKeyDown(LPARAM lParam, WPARAM wParam);
		/// When the key is released
		void OnKeyUp(LPARAM lParam, WPARAM wParam);
		/// When the mouse wheel is scrolled
		void OnWheel(LPARAM lParam, WPARAM wParam);
		/// When the window is resized
		void OnResize(LPARAM lParam, WPARAM wParam);
		/// Processes all messages
		void ProcessMessages();
		/// Returns the parameters
		WindowParameters& params(){ return params_; }
		/// Returns the window handle
		HWND& handle(){ return handle_; }

	private:
		HWND handle_; ///< The reference to the window handle used by this window
    HINSTANCE instance_; ///< The reference to the hInstance
		bool focussed_; ///< Check to see if the window is focussed or not
		WindowParameters params_; ///< The parameters of this window
	};

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); ///< The windows message loop
}