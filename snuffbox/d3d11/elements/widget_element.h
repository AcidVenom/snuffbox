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

		/**
		* @return (snuffbox::VertexBufferType) The vertex buffer type
		*/
		VertexBufferType type(){ return VertexBufferType::kQuad; }

		/**
		* @brief Sets one or multiple anchor types
		* @param[in] anchor (snuffbox::WidgetAnchor) The position to anchor to
		*/
		void SetAnchor(WidgetAnchor anchor);

		/**
		* @brief Removes an anchor
		* @param[in] anchor (snuffbox::WidgetAnchor) The anchor to remove
		*/
		void RemoveAnchor(WidgetAnchor anchor);

		/**
		* @return (XMMATRIX) Returns a calculated matrix for anchoring
		*/
		XMMATRIX anchor();

		/**
		* @brief Override the world matrix to use our own ui world matrix
		* @param[in] camera (snuffbox::Camera*) The currently in-use camera
		* @return (XMMATRIX&) The calculated world matrix
		*/
    XMMATRIX& world_matrix(Camera* camera);

	private:
		ID3D11Buffer* vertex_buffer_; //!< The vertex buffer of this element
		ID3D11Buffer* index_buffer_; //!< The index buffer of this element
		int						anchors_[4]; //!< Booleans to check if a widget should be anchored
		XMMATRIX			world_; //!< The world matrix
		Widget*				parent_; //!< The widget's parent
	public:
		JS_NAME(Widget);

		static void JSSetAnchorLeft(JS_ARGS);
		static void JSSetAnchorRight(JS_ARGS);
		static void JSSetAnchorTop(JS_ARGS);
		static void JSSetAnchorBottom(JS_ARGS);

    void RegisterExtraFunctions(JS_EXTRA);
	};
}