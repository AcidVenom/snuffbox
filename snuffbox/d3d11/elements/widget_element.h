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
    VertexBufferType type(){ return VertexBufferType::kQuad2D; }

		/**
		* @brief Override the world matrix to use our own ui world matrix
		* @param[in] camera (snuffbox::Camera*) The currently in-use camera
		* @return (XMMATRIX&) The calculated world matrix
		*/
    XMMATRIX& world_matrix(Camera* camera);

		/**
		* @brief Sets this widget to a text widget, with a corresponding string
		* @param[in] text (std::string) The text to display
		*/
		void SetText(std::string text);

		/**
		* @return (bool) Is this widget text or not?
		*/
		bool is_text();

		/**
		* @return (snuffbox::Widget*) The widget's parent
		*/
		Widget* parent(){ return parent_; }

	private:
		ID3D11Buffer*							vertex_buffer_; //!< The vertex buffer of this element
		ID3D11Buffer*							index_buffer_; //!< The index buffer of this element
		XMMATRIX									world_; //!< The world matrix
		Widget*										parent_; //!< The widget's parent
		bool											is_text_; //!< Is this a text widget?
		std::string								text_;	//!< The text of this widget if it is a text
		Font*											current_font_; //!< The font to use for text rendering
	public:
		JS_NAME(Widget);

		static void JSSetText(JS_ARGS);

    void RegisterExtraFunctions(JS_EXTRA);
	};
}