#pragma once

#include "../../../snuffbox/d3d11/elements/widget_element.h"

namespace snuffbox
{
  class Font;
	struct RichTextOperation;

	/**
	* @class snuffbox::Widget
	* @brief A UI widget element as a container or a visual representation
	* @author Daniël Konings
	*/ 
	class Text : public Widget
	{
	public:

		struct RichTextMarkup
		{
			Font* font;
			XMFLOAT4 colour;
		};

    /**
    * @enum snuffbox::TextAlignment
    * @brief Text alignment enumeration
    * @author Daniël Konings
    */
    enum TextAlignment
    {
      kLeft,
      kRight,
			kCenter
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

		/**
		* @brief Sets the shadow offset of this text element
		* @param[in] x (float) The X offset
		* @param[in] y (float) The Y offset
		*/
		void SetShadowOffset(float x, float y);

		/**
		* @brief Sets the shadow colour of this text element
		* @param[in] r (float) The red value
		* @param[in] g (float) The green value
		* @param[in] b (float) The blue value
		* @param[in] a (float) The alpha value
		*/
		void SetShadowColour(float r, float g, float b, float a);

		/// Clears the shadow of this element
		void ClearShadow();

		/// Returns if this element has a shadow set
		bool shadow_set();

		/// Prepares the shadow before drawing
		void PrepareShadow();

		/// Resets the text after drawing the shadow
		void Reset(XMFLOAT3 blend, float alpha);

		/// Fills a vertex buffer and an index buffer
		void FillBuffers(std::wstring& buffer);

		/// Aligns the text
		void Align(std::wstring* buffer);

	private:
		ID3D11Buffer*							vertex_buffer_; //!< The vertex buffer of this element
		ID3D11Buffer*							index_buffer_; //!< The index buffer of this element
		
    std::string               text_; //!< The string of this text
    Font*                     current_font_; //!< The font this text uses
		XMFLOAT4									current_colour_; //!< The current font colour
    float                     spacing_y_; //!< The vertical spacing
    float                     spacing_x_; //!< The horizontal spacing
    float                     font_size_; //!< The current font size
    std::string               font_; //!< The current font
    TextAlignment             alignment_; //!< The text alignment of this text object
    float                     width_; //!< The width of this element
    float                     height_; //!< The height of this element
		bool											shadow_set_; //!< Does this font have a shadow set?
		XMFLOAT4									shadow_colour_; //!< The shadow colour of this text element
		XMFLOAT2									shadow_offset_; //!< The shadow offset of this text element
		XMFLOAT2									pen_; //!< The pen position
		bool											align_vertical_; //!< Should this text be vertically aligned?

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
		static void JSSetAlignment(JS_ARGS);
		static void JSSetShadowOffset(JS_ARGS);
		static void JSSetShadowColour(JS_ARGS);
		static void JSClearShadow(JS_ARGS);
		static void JSAlignVertical(JS_ARGS);
	};
}