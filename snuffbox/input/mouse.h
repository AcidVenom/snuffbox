#pragma once

#include <queue>
#include <tuple>
#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	/**
	* @struct snuffbox::MouseState
	* @brief Structure to hold the state of a mouse button
	* @author Daniël Konings
	*/
	struct MouseButtonState
	{
		bool down, pressed, released;
	};

	/**
	* @enum snuffbox::MouseEvent
	* @brief The different mouse event types
	* @author Daniël Konings
	*/
	enum MouseEvent
	{
		kPressed,
		kDown,
		kUp,
		kMove
	};

	/**
	* @enum snuffbox::MouseButton
	* @brief Contains all mouse buttons as an enumeration
	* @author Daniël Konings
	*/
	enum MouseButton
	{
		kLeft,
		kRight,
		kMiddle
	};

	/**
	* @struct snuffbox::MouseData
	* @brief Structure to hold the data of the mouse in the message loop
	* @author Daniël Konings
	*/
	struct MouseData
	{
		MouseEvent type;
		MouseButton button;
		float x, y;
	};

	/**
	* @class snuffbox::Mouse
	* @brief Class to store captured mouse input in
	* @author Daniël Konings
	*/
	class Mouse : public JSObject
	{
	public:
		/// Default constructor
		Mouse();
		/// Default destructor
		~Mouse();

		/// Resets all buttonstates
		void ResetStates();

		/// Receive events
		void ReceiveEvent(MouseData& evt);

		/// Process events
		void Update();

		/// Checks if a certain button is down
		bool IsDown(MouseButton button);

		/// Checks if a certain button is pressed
		bool IsPressed(MouseButton button);

		/// Checks if a certain button is released
		bool IsReleased(MouseButton button);


		/// Returns the position of the mouse
		std::tuple<double, double> position(){ return std::tuple<double, double>(x_,y_); }

	private:
		MouseButtonState buttonStates_[3]; ///< The button states of every button
		float x_, y_;											 ///< X and Y position of the mouse
		std::queue<MouseData> queue_;			 ///< The queue to handle messages from

	public:
		JS_NAME(Mouse);
		static void RegisterJS(JS_TEMPLATE); ///< Registers all JavaScript functions

	private:
		static void JSGetPosition(JS_ARGS);	///< Retrieves the position of the mouse
		static void JSIsPressed(JS_ARGS);	///< Returns if a mouse button was pressed this frame
		static void JSIsDown(JS_ARGS);	///< Returns if a mouse button is down this frame
		static void JSIsReleased(JS_ARGS);	///< Returns if a mouse button is released this frame
	};
}