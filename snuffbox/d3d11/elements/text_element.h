#pragma once

#include "../../../snuffbox/d3d11/elements/widget_element.h"

namespace snuffbox
{
  class Font;

	/**
	* @class snuffbox::Widget
	* @brief A UI widget element as a container or a visual representation
	* @author Daniël Konings
	*/ 
	class Text : public Widget
	{
	public:

    /**
    * @enum snuffbox::TextAlignment
    * @brief Text alignment enumeration
    * @author Daniël Konings
    */
    enum TextAlignment
    {
      kLeft,
      kCenter,
      kRight
    };

		/// Default constructor
    Text();

		/// JavaScript constructor
    Text(JS_ARGS);

		/// Default destructor
    ~Text();

		/// Creates the text
		void Create();

		/// Sets the buffers for the render device to use
		void SetBuffers();

		/**
		* @return (snuffbox::VertexBufferType) The vertex buffer type
		*/
    VertexBufferType type(){ return VertexBufferType::kText; }

    /**
    * @brief Sets the text of this text widget
    * @param[in] text (std::string) The actual text
    */
    void SetText(std::string text);

    /**
    * @brief Sets the font of this text widget
    * @param[in] fontPath (std::string) The font path
    */
    void SetFont(std::string fontPath);

    /**
    * @brief Sets the font size of this element
    * @param[in] fontSize (float) The font size
    */
    void SetFontSize(float fontSize);

    /**
    * @brief Sets the spacing of the font
    * @param[in] x (float) The X spacing
    * @param[in] y (float) The Y spacing
    */
    void SetSpacing(float x, float y);

	private:
		ID3D11Buffer*							vertex_buffer_; //!< The vertex buffer of this element
		ID3D11Buffer*							index_buffer_; //!< The index buffer of this element
    std::string               text_; //!< The string of this text
    Font*                     current_font_; //!< The font this text uses
    float                     spacing_y_; //!< The vertical spacing
    float                     spacing_x_; //!< The horizontal spacing
    float                     font_size_; //!< The current font size
    std::string               font_; //!< The current font
    TextAlignment             alignment_; //!< The text alignment of this text object
    float                     width_; //!< The width of this element
    float                     height_; //!< The height of this element

	public:
		JS_NAME(Text);
    void RegisterExtraFunctions(JS_EXTRA);

    static void JSSetText(JS_ARGS);
    static void JSSetFont(JS_ARGS);
    static void JSSetSpacing(JS_ARGS);
    static void JSSetFontSize(JS_ARGS);
    static void JSText(JS_ARGS);
    static void JSFontSize(JS_ARGS);
    static void JSFont(JS_ARGS);
    static void JSSpacing(JS_ARGS);
    static void JSMetrics(JS_ARGS);
	};
}