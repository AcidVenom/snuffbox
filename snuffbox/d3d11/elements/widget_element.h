#pragma once

#include "../../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Widget
	* @brief A UI widget element as a container or a visual representation
	* @author Daniël Konings
	*/ 
	class Widget : public RenderElement
	{
	public:

		/**
		* @enum snuffbox::Widget::WidgetAnchor
		* @brief An enumerator to keep track of different anchor types
		* @author Daniël Konings
		*/
		enum WidgetAnchor
		{
			kLeft,
			kRight,
			kTop,
			kBottom
		};

		/// Default constructor
		Widget();

		/// JavaScript constructor
		Widget(JS_ARGS);

		/// Default destructor
		~Widget();

		/// Creates the widget
		void Create();

		/// Sets the buffers for the render device to use
		void SetBuffers();

		/// The vertex buffer type
		VertexBufferType type(){ return VertexBufferType::kQuad; }

		/// Sets one or multiple anchor types
		void SetAnchor(WidgetAnchor anchor);

		/// Removes an anchor
		void RemoveAnchor(WidgetAnchor anchor);

		/// Registers extra functions for this render element
		void RegisterExtraFunctions();

	private:
		ID3D11Buffer* vertexBuffer_; ///< The vertex buffer of this element
		ID3D11Buffer* indexBuffer_; ///< The index buffer of this element
		bool					anchors_[4]; ///< Booleans to check if a widget should be anchored
	public:
		JS_NAME(Widget);

		static void JSSetAnchor(JS_ARGS);
		static void JSRemoveAnchor(JS_ARGS);
	};
}