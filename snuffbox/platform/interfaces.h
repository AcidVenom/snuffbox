#pragma once

#define SNUFF_WINDOW_CENTERED -1

namespace snuffbox
{

	/**
	* @struct snuffbox::PlatformWindowParams
	* @brief Contains all parameters a window was created with
	* @author Daniël Konings
	*/
	struct PlatformWindowParams
	{
		int x, y, w, h;
		const char* name;
	};

	/**
	* @class snuffbox::IPlatformWindow
	* @brief The base class of all platform windows, all methods should be implemented
	* @author Daniël Konings
	*/
	class IPlatformWindow
	{
	public:
		/// Creates the window
		virtual void Create() = 0;

		/// Shows the window
		virtual void Show() = 0;

		/// Destroys the window
		virtual void Destroy() = 0;

		/// When the window is focussed
		virtual void OnSetFocus() = 0;

		/// When the window focus is killed
		virtual void OnKillFocus() = 0;

		/// When the window is closed
		virtual void OnClose() = 0;

		/// Processes all messages
		virtual void ProcessMessages() = 0;

		/// Returns the parameters structure of this window
		PlatformWindowParams& params(){ return params_; }

	private:
		PlatformWindowParams params_; ///< The window parameters
	};

}