#pragma once

#include "../../snuffbox/js/js_state_wrapper.h"
#include "../../snuffbox/js/js_callback.h"

namespace snuffbox
{
	class Widget;

	/**
	* @struct snuffbox::MouseAreaMetrics
	* @brief Mouse area metrics to use with mouse areas
	* @author Daniël Konings
	*/
	struct MouseAreaMetrics
	{
		float w, h, x, y;
	};

	struct MouseAreaCallback
	{
		Persistent<Value> callee;
		JSCallback callback;
		bool callback_set = false;
	};

	/**
	* @class snuffbox::MouseArea
	* @brief A mouse area to use for clickable and hoverable areas in an interface
	* @author Daniël Konings
	*/
	class MouseArea : public JSObject
	{
	public:
		/** 
		* @enum snuffbox::MouseAreaStates
		* @brief The states of a mouse area, corresponding to the mouse
		* @author Daniël Konings
		*/
		enum MouseAreaStates
		{
			kEnter,
			kLeave,
			kPressed,
			kReleased,
			kDown
		};

		/// Default constructor
		MouseArea();

		/// JavaScript constructor
		MouseArea(JS_ARGS);

		/// Default destructor
		~MouseArea();

		/**
		* @brief Sets the metrics of the mouse area
		* @param[in] x (float) The x position of the mouse area
		* @param[in] y (float) The y position of the mouse area
		* @param[in] w (float) The width of the mouse area
		* @param[in] h (float) The height of the mouse area
		*/
		void SetMetrics(float x, float y, float w, float h);

		/**
		* @brief Switch between the activated state of this mouse area
		* @param[in] activated (bool) The boolean value
		*/
		void SetActivated(bool activated);

		/**
		* @brief Switch between the hovered state of this mouse are
		* @param[in] hovered (bool) The boolean value
		*/
		void SetHovered(bool hovered);

		/**
		* @brief Updates the mouse area's metrics and returns them
		* @return (snuffbox::MouseAreaMetrics) The mouse area's metrics
		*/
		MouseAreaMetrics metrics();

		/**
		* @brief Notifies the mouse area about a state change, trigger a callback for it
		* @param[in] type (snuffbox::MouseArea::MouseAreaStates) The state type that was triggered
		* @param[in] button (int) The button id of the mouse that triggered this event
		*/
		void Notify(MouseAreaStates type, int button);

		/**
		* @brief Updates the mouse area to have the metrics of its parent
		*/
		void UpdateFromParent();

		/**
		* @brief Sets a callback assigned to a specific state
		* @param[in] type (snuffbox::MouseArea::MouseAreaStates) The state type
		* @param[in] callback (Handle<Function>) The callback to trigger
		* @param[in] callee (Handle<Object>) The callee of the function
		*/
		void SetCallback(MouseAreaStates type, Handle<Function> callback, Local<Value> callee);

		/**
		* @return (bool) Is this mouse area activated?
		*/
		bool activated();

		/**
		* @return (bool) Is this mouse area hovered?
		*/
		bool hovered();

	private:
		Widget* parent_; //!< The widget parent of this mouse area
		MouseAreaMetrics metrics_; //!< The metrics of this mouse area
		bool activated_; //!< Is this mouse area activated?
		MouseAreaCallback callbacks_[5]; //!< The callbacks per state
		bool hovered_; //!< Is this mouse area being hovered?

	public:
		JS_NAME(MouseArea);
		static void RegisterJS(JS_TEMPLATE);

	private:
		static void JSSetMetrics(JS_ARGS);
		static void JSSetActivated(JS_ARGS);
		static void JSSetOnEnter(JS_ARGS);
		static void JSSetOnLeave(JS_ARGS);
		static void JSSetOnPressed(JS_ARGS);
		static void JSSetOnReleased(JS_ARGS);
		static void JSSetOnDown(JS_ARGS);
		static void JSActivated(JS_ARGS);
	};
}