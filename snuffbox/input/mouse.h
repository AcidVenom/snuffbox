#pragma once

#include <queue>
#include <tuple>
#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	class MouseEnums
	{
	public:
		/**
		* @enum snuffbox::MouseButton
		* @brief Contains all mouse buttons as an enumeration
		* @author Daniël Konings
		*/
		enum MouseButton
		{
			kLeft,
			kRight,
			kMiddle,
			kWheelDown,
			kWheelUp
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
			kMove,
			kDblClk,
			kWheel
		};
	};
	/**
	* @struct snuffbox::MouseState
	* @brief Structure to hold the state of a mouse button
	* @author Daniël Konings
	*/
	struct MouseButtonState
	{
		bool down, pressed, released, dblclk;
	};

	/**
	* @struct snuffbox::MouseData
	* @brief Structure to hold the data of the mouse in the message loop
	* @author Daniël Konings
	*/
	struct MouseData
	{
		MouseEnums::MouseEvent type;
		MouseEnums::MouseButton button;
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
		bool IsDown(MouseEnums::MouseButton button);

		/// Checks if a certain button is pressed
		bool IsPressed(MouseEnums::MouseButton button);

		/// Checks if a certain button is released
		bool IsReleased(MouseEnums::MouseButton button);

		/// Checks if a certain button is double clicked
		bool IsDoubleClicked(MouseEnums::MouseButton button);


		/// Returns the position of the mouse
		std::tuple<double, double> position(){ return std::tuple<double, double>(x_,y_); }

		/// Returns the movement of the mouse
		std::tuple<double, double> movement(){ return std::tuple<double, double>(dx_, dy_); }

	private:
		MouseButtonState buttonStates_[5]; ///< The button states of every button
		float x_, y_;											 ///< X and Y position of the mouse
		float dx_, dy_;										 ///< The movement of the mouse
		float prevX_, prevY_;							 ///< The previous mouse position
		std::queue<MouseData> queue_;			 ///< The queue to handle messages from

	public:
		JS_NAME(Mouse);
		static void RegisterJS(JS_TEMPLATE); ///< Registers all JavaScript functions

	private:
		static void JSGetMovement(JS_ARGS); ///< Returns the movement of the mouse this frame
		static void JSGetPosition(JS_ARGS);	///< Retrieves the position of the mouse
		static void JSIsPressed(JS_ARGS);	///< Returns if a mouse button was pressed this frame
		static void JSIsDown(JS_ARGS);	///< Returns if a mouse button is down this frame
		static void JSIsReleased(JS_ARGS);	///< Returns if a mouse button is released this frame
		static void JSIsDoubleClicked(JS_ARGS);	///< Returns if a mouse button is double clicked this frame
		static void JSWheelUp(JS_ARGS);	///< Returns if the mouse was scrolled up this frame
		static void JSWheelDown(JS_ARGS); ///< Returns if the mouse was scrolled down this frame
	};
}