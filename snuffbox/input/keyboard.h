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
		KeyboardEnums::KeyEvent type; //!< The key event type
		Key key; //!< The key in question
	};

	/**
	* @struct snuffbox::KeyState
	* @brief The states a key on the keyboard can be in
	* @author Daniël Konings
	*/
	struct KeyState
	{
		bool pressed; //!< Was the key pressed?
		bool down; //!< Is the key down?
		bool released; //!< Was the key released?
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

		/**
		* @brief Checks if a key is currently pressed
		* @param[in] key (snuffbox::Key) The key in question
		* @return (bool) The result
		*/
		bool IsPressed(Key key);

		/**
		* @brief Checks if a key is currently down
		* @param[in] key (snuffbox::Key) The key in question
		* @return (bool) The result
		*/
		bool IsDown(Key key);

		/**
		* @brief Checks if a key is currently released
		* @param[in] key (snuffbox::Key) The key in question
		* @return (bool) The result
		*/
		bool IsReleased(Key key);

		/// Resets all keystates to their default value
		void ResetStates();

		/**
		* @brief Pushes an event into the queue
		* @param[in] evt (snuffbox::KeyData) The key data to evaluate
		*/
		void ReceiveEvent(KeyData evt);

		/// Updates the entire keyboard
		void Update();

	private:
		KeyState key_states_[255]; //!< All the states of every key
		std::queue<KeyData> queue_; //!< The queue with key events

	public:
		JS_NAME(Keyboard);
		static void RegisterJS(JS_TEMPLATE); //!< Register all JS functions
		static void JSIsPressed(JS_ARGS); //!< Checks if a key is pressed
		static void JSIsDown(JS_ARGS); //!< Checks if a key is down
		static void JSIsReleased(JS_ARGS); //!< Checks if a key is released
	};
}