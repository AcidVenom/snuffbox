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
		bool down; //!< Is a mouse button down?
		bool pressed; //!< Was a mouse button pressed?
		bool released; //!< Was a mouse button released?
		bool dblclk; //!< Was a mouse button double clicked?
	};

	/**
	* @struct snuffbox::MouseData
	* @brief Structure to hold the data of the mouse in the message loop
	* @author Daniël Konings
	*/
	struct MouseData
	{
		MouseEnums::MouseEvent type; //!< The type of the mouse event
		MouseEnums::MouseButton button; //!< The button in question
		float x; //!< The x position of the mouse
		float y; //!< The y position of the mouse
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

		/**
		* @brief Receive an event and push it to the queue
		* @param[in] evt (snuffbox::MouseData&) The mouse data to evaluate
		*/
		void ReceiveEvent(MouseData& evt);

		/// Process events
		void Update();

		/**
		* @brief Checks if a button is currently down
		* @param[in] button (snuffbox::MouseEnums::MouseButton) The button in question
		* @return (bool) The result
		*/
		bool IsDown(MouseEnums::MouseButton button);

		/**
		* @brief Checks if a button is currently pressed
		* @param[in] button (snuffbox::MouseEnums::MouseButton) The button in question
		* @return (bool) The result
		*/
		bool IsPressed(MouseEnums::MouseButton button);

		/**
		* @brief Checks if a button is currently released
		* @param[in] button (snuffbox::MouseEnums::MouseButton) The button in question
		* @return (bool) The result
		*/
		bool IsReleased(MouseEnums::MouseButton button);

		/**
		* @brief Checks if a button is currently double clicked
		* @param[in] button (snuffbox::MouseEnums::MouseButton) The button in question
		* @return (bool) The result
		*/
		bool IsDoubleClicked(MouseEnums::MouseButton button);


		/**
		* @return (std::tuple<double, double>) The position of the mouse
		*/
		std::tuple<double, double> position(){ return std::tuple<double, double>(x_,y_); }

		/**
		* @return (std::tuple<double, double>) The movement of the mouse
		*/
		std::tuple<double, double> movement(){ return std::tuple<double, double>(dx_, dy_); }

		/**
		* @return (std::tuple<double, double>) The relative position of the mouse
		*/
		std::tuple<double, double> GetRelativePosition();

	private:
		MouseButtonState button_states_[5]; //!< The button states of every button
		float x_, y_;											  //!< X and Y position of the mouse
		float dx_, dy_;										  //!< The movement of the mouse
		float prev_x_, prev_y_;							//!< The previous mouse position
		std::queue<MouseData> queue_;			  //!< The queue to handle messages from

	public:
		JS_NAME(Mouse);
		static void RegisterJS(JS_TEMPLATE); //!< Registers all JavaScript functions

	private:
		static void JSGetMovement(JS_ARGS); //!< Returns the movement of the mouse this frame
		static void JSGetPosition(JS_ARGS);	//!< Retrieves the position of the mouse
		static void JSIsPressed(JS_ARGS);	//!< Returns if a mouse button was pressed this frame
		static void JSIsDown(JS_ARGS);	//!< Returns if a mouse button is down this frame
		static void JSIsReleased(JS_ARGS);	//!< Returns if a mouse button is released this frame
		static void JSIsDoubleClicked(JS_ARGS);	//!< Returns if a mouse button is double clicked this frame
		static void JSWheelUp(JS_ARGS);	//!< Returns if the mouse was scrolled up this frame
		static void JSWheelDown(JS_ARGS); //!< Returns if the mouse was scrolled down this frame
	};
}