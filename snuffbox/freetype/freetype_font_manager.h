#pragma once

#include <string>
#include <map>

#include "../../snuffbox/freetype/freetype_font.h"
#include "../../snuffbox/freetype/freetype_font_atlas.h"
#include "../../snuffbox/memory/shared_ptr.h"

namespace snuffbox
{
  class Font;
  typedef std::map<std::string, std::map<float, SharedPtr<Font>>> FontMap;

  class FontManager
  {
  public:
    FontManager();
    ~FontManager();

    /**
    * @brief Requests a font to retrieve, if it doesn't exist; create it
    * @param[in] path (std::string) The relative path
    * @param[in] fontSize (float) The requested float size
    */
    Font* GetFont(std::string path, float fontSize);

    /**
    * @brief Requests the default font from the font manager
    * @return (snuffbox::Font*) The pointer to the default font
    */
    Font* default_font();

		/**
		* @return (snuffbox::FontAtlas*) The pointer to the font atlas used
		*/
		FontAtlas* atlas();

  private:
    FontMap map_; //!< Contains all font name <-> font size mappings
    Font* default_font_; //!< The default font
		SharedPtr<FontAtlas> atlas_; //!< The atlas to hold all fonts
  };
}