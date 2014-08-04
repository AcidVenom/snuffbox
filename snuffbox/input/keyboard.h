#pragma once

#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/input/key.h"
#include <queue>

namespace snuffbox
{

	class KeyboardEnums
	{
	public:

		/**
		* @enum snuffbox::KeyEvent
		* @brief The list of events a keyboard key can trigger
		* @author Daniël Konings
		*/
		enum KeyEvent
		{
			kPressed,
			kDown,
			kReleased
		};
	};
	
	/**
	* @struct snuffbox::KeyData
	* @brief Contains event information about keys
	* @author Daniël Konings
	*/
	struct KeyData
	{
		KeyboardEnums::KeyEvent type;
		Key key;
	};

	/**
	* @struct snuffbox::KeyState
	* @brief The states a key on the keyboard can be in
	* @author Daniël Konings
	*/
	struct KeyState
	{
		bool pressed, down, released;
	};

	/**
	* @class snuffbox::Keyboard
	* @brief Class to store captured keyboard input in
	* @author Daniël Konings
	*/
	class Keyboard : public JSObject
	{
	public:

		/// Default constructor
		Keyboard();
		/// Default destructor
		~Keyboard();

		/// Checks if a key is currently pressed
		bool IsPressed(Key key);

		/// Checks if a key is currently down
		bool IsDown(Key key);

		/// Checks if a key is currently released
		bool IsReleased(Key key);

		/// Resets all keystates to their default value
		void ResetStates();

		/// Pushes an event into the queue
		void ReceiveEvent(KeyData evt);

		/// Updates the entire keyboard
		void Update();

	private:
		KeyState keyStates_[255]; //!< All the states of every key
		std::queue<KeyData> queue_; //!< The queue with key events

	public:
		JS_NAME(Keyboard);
		static void RegisterJS(JS_TEMPLATE); //!< Register all JS functions
		static void JSIsPressed(JS_ARGS); //!< Checks if a key is pressed
		static void JSIsDown(JS_ARGS); //!< Checks if a key is down
		static void JSIsReleased(JS_ARGS); //!< Checks if a key is released
	};
}