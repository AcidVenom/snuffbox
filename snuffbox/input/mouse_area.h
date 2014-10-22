#pragma once

#include "../../snuffbox/js/js_state_wrapper.h"

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

	/**
	* @class snuffbox::MouseArea
	* @brief A mouse area to use for clickable and hoverable areas in an interface
	* @author Daniël Konings
	*/
	class MouseArea : public JSObject
	{
	public:
		/// Default constructor
		MouseArea();

		/// JavaScript constructor
		MouseArea(JS_ARGS);

		/**
		* @brief Sets the metrics of the mouse area
		* @param[in] x (float) The x position of the mouse area
		* @param[in] y (float) The y position of the mouse area
		* @param[in] w (float) The width of the mouse area
		* @param[in] h (float) The height of the mouse area
		*/
		void SetMetrics(float x, float y, float w, float h);

	private:
		Widget* parent_; //!< The widget parent of this mouse area
		MouseAreaMetrics metrics_; //!< The metrics of this mouse area
		
	public:
		JS_NAME(MouseArea);
		static void RegisterJS(JS_TEMPLATE);

	private:
		static void JSSetMetrics(JS_ARGS);
	};
}