#include "../../../snuffbox/d3d11/elements/text_element.h"
#include "../../../snuffbox/freetype/freetype_font_manager.h"
#include "../../../snuffbox/freetype/freetype_font_atlas.h"
#include "../../../snuffbox/freetype/freetype_font.h"
#include "../../../snuffbox/freetype/freetype_rich_text.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_LCD_FILTER_H

#define HRES  64
#define HRESf 64.f
#define DPI   72

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Text::Text() :
		Widget(RenderElement::ElementTypes::kText),
		current_font_(nullptr),
		spacing_y_(0.0f),
		spacing_x_(0.0f),
		alignment_(TextAlignment::kLeft),
		shadow_set_(false),
		shadow_offset_(0.0f, 0.0f),
		shadow_colour_(0.0f, 0.0f, 0.0f, 1.0f),
		current_colour_(1.0f, 1.0f, 1.0f, 1.0f)
	{
    current_font_ = environment::font_manager().default_font();
    font_ = "fonts/arial.ttf";
    font_size_ = 16;
	}

	//-------------------------------------------------------------------------------------------
  Text::Text(JS_ARGS) :
    Widget(RenderElement::ElementTypes::kText),
    current_font_(nullptr),
    spacing_y_(0.0f),
    spacing_x_(0.0f),
    alignment_(TextAlignment::kLeft),
		shadow_set_(false),
		shadow_offset_(0.0f, 0.0f),
		shadow_colour_(0.0f, 0.0f, 0.0f, 1.0f),
		current_colour_(1.0f, 1.0f, 1.0f, 1.0f)
	{
		JSWrapper wrapper(args);
		Create();

		if (args.Length() > 0 && !wrapper.GetValue(0)->IsUndefined())
		{
      SetParent(wrapper.GetPointer<Widget>(0));
      parent()->AddChild(this);
		}

    current_font_ = environment::font_manager().default_font();
    font_ = "fonts/arial.ttf";
    font_size_ = 16;
	}

	//-------------------------------------------------------------------------------------------
  void Text::Create()
	{
    current_font_ = environment::font_manager().default_font();
	}

	//-------------------------------------------------------------------------------------------
  void Text::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(index_buffer_);
	}

	//-------------------------------------------------------------------------------------------
	void Text::FillBuffers(std::wstring& buffer)
	{
		float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;

		float tx, ty, tw, th;
		int index_offset = static_cast<int>(vertices().size()) / 4;

		RichTextMarkup markup;
		markup.font = current_font_;
		markup.colour = current_colour_;

		for (int i = 0; i < buffer.size(); ++i)
		{
			wchar_t ch = buffer.at(i);

			if (ch == L'\n')
			{
				pen_.y -= (markup.font->line_gap() * 100.0f + markup.font->line_height() * 100.0f);
				pen_.x = 0.0f;
				continue;
			}

			FontGlyph* glyph = markup.font->glyph(ch);

			w = static_cast<float>(glyph->width);
			h = static_cast<float>(glyph->height);
			x = pen_.x + glyph->x_offset;
			y = pen_.y - (h - glyph->y_offset) - markup.font->ascender();

			tx = glyph->tex_coords.left;
			th = glyph->tex_coords.top;
			tw = glyph->tex_coords.right;
			ty = glyph->tex_coords.bottom;

			Vertex verts[] = {
				{ x, y + h, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(tx, th), markup.colour },
				{ x, y, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(tx, ty), markup.colour },
				{ x + w, y + h, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(tw, th), markup.colour },
				{ x + w, y, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(tw, ty), markup.colour },
			};

			for (int j = 0; j < 4; ++j)
			{
				vertices().push_back(verts[j]);
				indices().push_back(index_offset * 4 + j);

				if (j == 3)
				{
					indices().push_back(index_offset * 4 + j);
					indices().push_back((index_offset + 1) * 4);
				}
			}

			pen_.x += (glyph->x_advance) + spacing_x_;
			++index_offset;
		}
	}

  //-------------------------------------------------------------------------------------------
  void Text::SetText(std::string text)
  {
		if (vertex_buffer_ != nullptr && index_buffer_ != nullptr)
		{
			SNUFF_SAFE_RELEASE(vertex_buffer_);
			SNUFF_SAFE_RELEASE(index_buffer_);
		}

		vertices().clear();
		indices().clear();

		if (text.size() == 0)
		{
			text = " ";
		}

    text_ = text;

    const int buffsize = MultiByteToWideChar(CP_UTF8, NULL, text.c_str(), -1, NULL, NULL);

    wchar_t* widestr = new wchar_t[buffsize];

    MultiByteToWideChar(CP_UTF8, NULL, text.c_str(), -1, widestr, buffsize);

    SNUFF_XASSERT(current_font_ != nullptr, "Text widget has no font set!");

		std::wstring buffer(widestr);
		MarkupOperations operations = RichTextParser::Parse(&buffer);

		Font* defaultFont = current_font_;
		float defaultSize = font_size_;

		bool isBold = false;
		bool isItalic = false;
		std::string fontString = font_;

		for (auto& it : operations)
		{
			if (it.isDefault)
			{
				FillBuffers(it.text);
				current_font_ = defaultFont;
				current_colour_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				isBold = false;
				isItalic = false;
				font_size_ = defaultSize;
				fontString = font_;
			}
			else if (it.doColour)
			{
				current_colour_ = XMFLOAT4(it.colour[0], it.colour[1], it.colour[2], 1.0f);
			}
			else if (it.bold)
			{
				isBold = true;
				if (isItalic == false)
				{
					current_font_ = environment::font_manager().GetFont(font_ + 'b', font_size_);
					fontString = font_ + 'b';
				}
				else
				{
					current_font_ = environment::font_manager().GetFont(font_ + 'z', font_size_);
					fontString = font_ + 'z';
				}
			}
			else if (it.italic)
			{
				isItalic = true;
				if (isBold == false)
				{
					current_font_ = environment::font_manager().GetFont(font_ + 'i', font_size_);
					fontString = font_ + 'i';
				}
				else
				{
					current_font_ = environment::font_manager().GetFont(font_ + 'z', font_size_);
					fontString = font_ + 'z';
				}
			}
			else if (it.doSize)
			{
				font_size_ = it.size;
				current_font_ = environment::font_manager().GetFont(fontString, font_size_);
			}
			else if (it.newFont)
			{
				fontString = it.font;
				
				if (isItalic)
				{
					fontString = it.font + 'i';
				}
				else if (isBold)
				{
					fontString = it.font + 'b';
				}

				if (isItalic && isBold)
				{
					fontString = it.font + 'z';
				}

				current_font_ = environment::font_manager().GetFont(fontString, font_size_);
			}
		}

		current_font_ = defaultFont;
		current_colour_ = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		isBold = false;
		isItalic = false;
		font_size_ = defaultSize;
		fontString = font_;

		Align();

		vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
		index_buffer_ = environment::render_device().CreateIndexBuffer(indices());

		pen_.x = 0.0f;
		pen_.y = 0.0f;

    delete[] widestr;
  }

  //-------------------------------------------------------------------------------------------
  void Text::SetFont(std::string fontPath)
  {
    font_ = fontPath;
    current_font_ = environment::font_manager().GetFont(font_, font_size_);
    SetText(text_);
  }

  //-------------------------------------------------------------------------------------------
  void Text::SetSpacing(float x, float y)
  {
    spacing_x_ = x;
    spacing_y_ = y;

    SetText(text_);
  }

  //-------------------------------------------------------------------------------------------
  void Text::SetFontSize(float fontSize)
  {
    font_size_ = fontSize;
    current_font_ = environment::font_manager().GetFont(font_, font_size_);
    SetText(text_);
  }

	//-------------------------------------------------------------------------------------------
	void Text::SetShadowColour(float r, float g, float b, float a)
	{
		shadow_set_ = true;
		shadow_colour_ = XMFLOAT4(r, g, b, a);
	}

	//-------------------------------------------------------------------------------------------
	void Text::SetShadowOffset(float x, float y)
	{
		shadow_set_ = true;
		shadow_offset_ = XMFLOAT2(x, y);
	}

	//-------------------------------------------------------------------------------------------
	bool Text::shadow_set()
	{
		return shadow_set_;
	}

	//-------------------------------------------------------------------------------------------
	void Text::Align()
	{
		float highest_x;
		float highest_y;
		bool set = false;

		for (auto& it : vertices())
		{
			it.y -= (current_font_->line_gap() * 100.0f + current_font_->line_height() * 100.0f);
		}

		for (auto& it : vertices())
		{
			if (set == false)
			{
				highest_x = it.x;
				highest_y = it.y;
				set = true;
				continue;
			}

			if (it.x > highest_x)
			{
				highest_x = it.x;
			}

			if (it.y < highest_y)
			{
				highest_y = it.y;
			}
		}

		width_ = highest_x;
		height_ = abs(highest_y);
	}

	//-------------------------------------------------------------------------------------------
	void Text::ClearShadow()
	{
		shadow_set_ = false;
		shadow_colour_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		shadow_offset_ = XMFLOAT2(0.0f, 0.0f);
	}

	//-------------------------------------------------------------------------------------------
	void Text::PrepareShadow()
	{
		TranslateBy(shadow_offset_.x, shadow_offset_.y, 0.0f);
		SetBlend(shadow_colour_.x, shadow_colour_.y, shadow_colour_.z);
		set_alpha(shadow_colour_.w);
	}

	//-------------------------------------------------------------------------------------------
	void Text::Reset(XMFLOAT3 blend, float alpha)
	{
		TranslateBy(-shadow_offset_.x, -shadow_offset_.y, 0.0f);
		SetBlend(blend.x, blend.y, blend.z);
		set_alpha(alpha);
	}

  //-------------------------------------------------------------------------------------------
  void Text::RegisterExtraFunctions(JS_EXTRA)
  {
    JSFunctionRegister funcs[] = {
      JSFunctionRegister("setText", JSSetText),
      JSFunctionRegister("setFontFamily", JSSetFont),
      JSFunctionRegister("setFontSize", JSSetFontSize),
      JSFunctionRegister("setSpacing", JSSetSpacing),
      JSFunctionRegister("text", JSText),
      JSFunctionRegister("fontFamily", JSFont),
      JSFunctionRegister("fontSize", JSFontSize),
      JSFunctionRegister("spacing", JSSpacing),
      JSFunctionRegister("metrics", JSMetrics),
			JSFunctionRegister("setAlignment", JSSetAlignment),
			JSFunctionRegister("setShadowOffset", JSSetShadowOffset),
			JSFunctionRegister("setShadowColour", JSSetShadowColour),
			JSFunctionRegister("clearShadow", JSClearShadow)
    };

    JS_REGISTER_OBJECT_FUNCTIONS_EXTRA(obj, funcs);
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSSetText(JS_ARGS)
  {
    JS_SETUP(Text, "S");

    self->SetText(wrapper.GetString(0));
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSSetFont(JS_ARGS)
  {
    JS_SETUP(Text, "S");

    self->SetFont(wrapper.GetString(0));
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSSetFontSize(JS_ARGS)
  {
    JS_SETUP(Text, "N");

    self->SetFontSize(wrapper.GetNumber<float>(0));
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSSetSpacing(JS_ARGS)
  {
    JS_SETUP(Text, "NN");

    self->SetSpacing(wrapper.GetNumber<float>(0), wrapper.GetNumber<float>(1));
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSText(JS_ARGS)
  {
    JS_SETUP(Text, "V");

    wrapper.ReturnString(self->text_.c_str());
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSFont(JS_ARGS)
  {
    JS_SETUP(Text, "V");

    wrapper.ReturnString(self->font_.c_str());
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSFontSize(JS_ARGS)
  {
    JS_SETUP(Text, "V");

    wrapper.ReturnNumber(self->font_size_);
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSSpacing(JS_ARGS)
  {
    JS_SETUP(Text, "V");

    wrapper.ReturnTuple<float>(self->spacing_x_, self->spacing_y_);
  }

  //-------------------------------------------------------------------------------------------
  void Text::JSMetrics(JS_ARGS)
  {
    JS_SETUP(Text, "V");

    wrapper.ReturnTuple<float>(self->width_, self->height_, "width", "height");
  }

	//-------------------------------------------------------------------------------------------
	void Text::JSSetAlignment(JS_ARGS)
	{
		JS_SETUP(Text, "N");

		self->alignment_ = static_cast<Text::TextAlignment>(wrapper.GetNumber<int>(0));
		self->SetText(self->text_);
	}

	//-------------------------------------------------------------------------------------------
	void Text::JSSetShadowOffset(JS_ARGS)
	{
		JS_SETUP(Text, "NN");

		self->SetShadowOffset(wrapper.GetNumber<float>(0), wrapper.GetNumber<float>(1));
	}

	//-------------------------------------------------------------------------------------------
	void Text::JSSetShadowColour(JS_ARGS)
	{
		JS_SETUP(Text, "NN");

		self->SetShadowColour(wrapper.GetNumber<float>(0), wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2), wrapper.GetNumber<float>(3));
	}

	//-------------------------------------------------------------------------------------------
	void Text::JSClearShadow(JS_ARGS)
	{
		JS_SETUP(Text, "V");

		self->ClearShadow();
	}

  //-------------------------------------------------------------------------------------------
  Text::~Text()
	{
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);
	}
}