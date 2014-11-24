#include "../../../snuffbox/d3d11/elements/text_element.h"
#include "../../../snuffbox/freetype/freetype_font_manager.h"
#include "../../../snuffbox/freetype/freetype_font_atlas.h"
#include "../../../snuffbox/freetype/freetype_font.h"

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
    spacing_y_(1.0f),
    spacing_x_(1.0f),
    alignment_(TextAlignment::kLeft)
	{
    current_font_ = environment::font_manager().default_font();
    font_ = "fonts/arial.ttf";
    font_size_ = 16;
	}

	//-------------------------------------------------------------------------------------------
  Text::Text(JS_ARGS) :
    Widget(RenderElement::ElementTypes::kText),
    current_font_(nullptr),
    spacing_y_(1.0f),
    spacing_x_(1.0f),
    alignment_(TextAlignment::kLeft)
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
		vertices().push_back({ 0.0f, -1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
		vertices().push_back({ 0.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
		vertices().push_back({ 1.0f, -1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
		vertices().push_back({ 1.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });

		indices().push_back(1);
		indices().push_back(0);
		indices().push_back(3);
		indices().push_back(2);

		vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
		index_buffer_ = environment::render_device().CreateIndexBuffer(indices());

    current_font_ = environment::font_manager().default_font();
	}

	//-------------------------------------------------------------------------------------------
  void Text::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(index_buffer_);
	}

  //-------------------------------------------------------------------------------------------
  void Text::SetText(std::string text)
  {
    text_ = text;

    const int buffsize = MultiByteToWideChar(CP_UTF8, NULL, text.c_str(), -1, NULL, NULL);

    wchar_t* widestr = new wchar_t[buffsize];

    MultiByteToWideChar(CP_UTF8, NULL, text.c_str(), -1, widestr, buffsize);
    
    SNUFF_XASSERT(current_font_ != nullptr, "Text widget has no font set!");

    vertices().clear();
    indices().clear();

    float pen_x = 0.0f;
    float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;
    float height = -(current_font_->line_gap() * 100.0f * spacing_y_ + current_font_->line_height() * 100.0f * spacing_y_);
    float tx, ty, tw, th;
    int index_offset = 0;

    float highest_x, highest_y;

    float kerning;

    for (int i = 0; i < buffsize - 1; ++i)
    {
      wchar_t ch = widestr[i];

      if (ch == L'\n')
      {
        height -= current_font_->line_gap() * 100.0f * spacing_y_ + current_font_->line_height() * 100.0f * spacing_y_;
        pen_x = 0.0f;
        continue;
      }

      FontGlyph* glyph = current_font_->glyph(ch);
     
      w = static_cast<float>(glyph->width);
      h = static_cast<float>(glyph->height);
      x = pen_x + glyph->x_offset;
      y = height - (h - glyph->y_offset);

      tx = glyph->tex_coords.left;
      th = glyph->tex_coords.top;
      tw = glyph->tex_coords.right;
      ty = glyph->tex_coords.bottom;

      Vertex verts[] = {
        { x, y + h, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(tx, th), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { x, y, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(tx, ty), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { x + w, y + h, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(tw, th), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { x + w, y, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(tw, ty), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
      };

      for (int j = 0; j < 4; ++j)
      {
        vertices().push_back(verts[j]);
        indices().push_back(index_offset * 4 + j);

        if (j == 3)
        {
          indices().push_back(index_offset * 4 + j);
          indices().push_back((index_offset+1) * 4);
        }
      }

      if (i - 1 >= 0)
      {
        auto it = glyph->kerning.find(widestr[i - 1]);
        if (it != glyph->kerning.end())
        {
          kerning = it->second;
        }
      }
      else
      {
        kerning = 0.0f;
      }
      pen_x += (glyph->x_advance + kerning) * spacing_x_;
      ++index_offset;
    }

    highest_x = std::numeric_limits<float>::infinity();
    highest_y = std::numeric_limits<float>::infinity();

    std::vector<Vertex>& v = vertices();

    for (auto& it : v)
    {
      if (highest_x == std::numeric_limits<float>::infinity())
      {
        highest_x = it.x;
      }
      else if (it.x > highest_x)
      {
        highest_x = it.x;
      }

      if (highest_y == std::numeric_limits<float>::infinity())
      {
        highest_y = it.y;
      }
      else if (it.y < highest_y)
      {
        highest_y = it.y;
      }
    }

    width_ = highest_x;
    height_ = std::abs(highest_y);

		if (alignment_ != TextAlignment::kLeft)
		{
			int offset = -1;
			int startAt = 0;
			for (int i = 0; i < buffsize; ++i)
			{
				wchar_t ch = widestr[i];

				if (ch == L'\n' || i == buffsize-1)
				{
					float x = v.at(offset).x;
					if (x < highest_x)
					{
						int delta = highest_x - x;
						
						if (alignment_ == TextAlignment::kCenter)
						{
							delta = static_cast<int>(delta / 2);
						}
						for (int j = startAt; j <= offset; ++j)
						{
							v.at(j).x += delta;
						}
					}

					startAt = offset+1;
				}
				else
				{
					offset += 4;
				}
			}

			for (Vertex& it : v)
			{
				it.x -= alignment_ == TextAlignment::kRight ? width_ : width_ / 2;
			}
		}

    set_texture(current_font_->texture());

    if (vertices().size() > 0)
    {
      SNUFF_SAFE_RELEASE(vertex_buffer_);
      SNUFF_SAFE_RELEASE(index_buffer_);

      vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
      index_buffer_ = environment::render_device().CreateIndexBuffer(indices());
    }

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
			JSFunctionRegister("setAlignment", JSSetAlignment)
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
  Text::~Text()
	{
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);
	}
}