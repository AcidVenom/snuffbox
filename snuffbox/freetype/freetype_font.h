#pragma once

#include <map>
#include "../../snuffbox/memory/shared_ptr.h"

struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace snuffbox
{
  class FontAtlas;
  struct FontGlyph;
  class Texture;

  /**
  * @class snuffbox::Font
  * @brief A wrapper class for a freetype font, use this to draw to a texture atlas
  * @author Daniël Konings
  */
  class Font
  {
  public:
    /// Default constructor
    Font();

    /// Default destructor
    ~Font();

    /**
    * @brief Loads a font from a relative path and stores its information in this class
    * @param[in] relativePath (std::string) The relative path to the font file
    * @param[in] size (float) The font size to load
    */
    void Load(std::string relativePath, float size);

    /**
    * @brief Loads a glyph from a given charcode
    * @param[in] charcode (char) The character code to load as a glyph
    * @return (snuffbox::FontGlyph*) The new or old glyph
    */
    FontGlyph* LoadGlyph(wchar_t charcode);

    /**
    * @brief Loads an entire string of glyphs
    * @param[in] str (const char*) The string
    * @return (int) Failed (> 0) Succes (0)
    */
    int LoadGlyphs(wchar_t *str);

    /**
    * @return (snuffbox::FontGlyph*) The glyph for the specific charcode
    */
    FontGlyph* glyph(wchar_t charcode);

		/**
		* @return (float) The ascender of this font
		*/
		float ascender();

		/**
		* @return (float) The line gap of this font
		*/
		float line_gap();

		/**
		* @return (float) The line height of this font
		*/
		float line_height();

  private:
    std::map<wchar_t, SharedPtr<FontGlyph>>   glyphs_; //!< The loaded font glyphs
    FT_LibraryRec_*             library_; //!< The freetype library
    FT_FaceRec_*                face_; //!< The freetype face
    float                       size_;
    unsigned char               lcd_weights_[5];
    float                       height_;
    float                       linegap_;
    float                       ascender_;
    float                       descender_;
  };


  /**
  * @struct snuffbox::FontTexCoords
  * @brief Texture coordinates for a font glyph
  * @author Daniël Konings
  */
  struct FontTexCoords
  {
    FontTexCoords() : left(0.0f), right(0.0f), top(0.0f), bottom(0.0f){}
    float left, right, top, bottom;
  };

  /**
  * @struct snuffbox::FontGlyph
  * @brief Stores glyph information from freetype
  * @author Daniël Konings
  */
  struct FontGlyph
  {
    wchar_t charcode; //!< The character code of this glyph
    int width; //!< Pixel width
    int height; //!< Pixel height
    int x_offset; //!< Left bearing
    int y_offset; //!< Top bearing
    float x_advance; //!< Pen X advance distance
    float y_advance; //!< Pen Y advance distance
    FontTexCoords tex_coords; //!< Texture coordinates of this glyph in the atlas
  };
}