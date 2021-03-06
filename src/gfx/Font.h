//
//  Font.h
//  OpenMoM
//
//  Created by Jack on 18/07/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _FONT_H_
#define _FONT_H_

#include "Texture.h"

#include <string>
#include <vector>
#include <unordered_map>

enum FontType
{
  FONT_TINY = 0,
  FONT_SMALL,
  FONT_MEDIUM,
  FONT_MEDIUM_THICK,
  FONT_SERIF,
  FONT_HUGE,
  FONT_TINY_CRYPT,
  FONT_SERIF_CRYPT,
  
  FONT_TYPE_COUNT
};


enum TextAlign : u8
{
  ALIGN_RIGHT = 0,
  ALIGN_CENTER,
  ALIGN_LEFT
};

struct FontPalette : public Palette
{
  enum class Type
  {
    SOLID,
    SOLID_WITH_LOW_SHADOW,
    SOLID_WITH_FULL_STROKE,
  };
  
  Type type;
  
  Color main;
  Color single;

  Color highShadow;
  Color lowShadow;
  Color mixShadow;
  
  FontPalette() { }
  
  FontPalette(Type type, Color main, Color single, Color highShadow, Color lowShadow, Color mixShadow) :
    type(type), main(main), single(single), highShadow(highShadow), lowShadow(lowShadow), mixShadow(mixShadow) { }
  
  Color get(index_t index) const override
  {
    switch (index) {
      case 0: return Color(0,0,0);
      case 1: return highShadow;
      case 2: return mixShadow;
      case 3: return lowShadow;
      case 4: return single;
      case 5: return main;
        
      default: return main;
    }
  }
  
public:
  static FontPalette ofSolid(Color color) { return { Type::SOLID, color, color, 0, 0, 0 }; }
  static FontPalette ofSolid(Color color, Color single) { return { Type::SOLID, color, single, 0, 0, 0 }; }
  
  static FontPalette ofStroked(Color color, Color stroke) {
    return { Type::SOLID_WITH_FULL_STROKE, color, color, stroke, stroke, stroke };
  }
  
  /* TODO: mix shadow transparent or not? */
  static FontPalette ofSolidWithLowShadow(Color color, Color lowShadow) {
    return { Type::SOLID_WITH_LOW_SHADOW, color, color, Color::NONE, lowShadow, Color::NONE };
  }
  
  static FontPalette ofSolidWithLowShadow(Color color, Color single, Color lowShadow) {
    return { Type::SOLID_WITH_LOW_SHADOW, color, single, Color::NONE, lowShadow, Color::NONE };
  }
};


constexpr size_t GLYPH_COUNT = 96;

class SpriteRawData
{
  
public:
  virtual byte* dataAt(index_t x, index_t y) const = 0;
  virtual index_t rows() const = 0;
  virtual index_t columns() const = 0;
  
  virtual index_t tw() const = 0;
  
  virtual index_t w(index_t x = 0) const = 0;
  virtual index_t h(index_t x = 0) const = 0;
};

/* glyph data is stored in a monodimensional array in which all glyphs are stored contiguously, eg
   XXXYYYZZZ
   XXXYYYZZZ becomes XXXYYYZZZXXXYYYZZZXXXYYYZZZ
   XXXYYYZZZ
 */
class FontData : public SpriteRawData
{
private:
  byte* data;
  s8 glyphWidth[GLYPH_COUNT];
  const u8 width;
  const u8 height;
  
public:
  FontData(FontType type, u8 height, u8 width) : type(type), height(height), width(width), data(new byte[width*height*GLYPH_COUNT])
  {
    std::fill(data, data + width*height*GLYPH_COUNT, 0);
  }
  
  ~FontData()
  {
    delete [] data;
  }
  
  void setGlyphWidth(u8 index, s8 width) {  glyphWidth[index] = width; }
  s8 getGlyphWidth(u8 index) const { return glyphWidth[index - ' ']; }
  
  byte* dataAt(index_t x = 0, index_t y = 0) const override { return &data[y*width]; }
  index_t rows() const override { return 1; }
  index_t columns() const override { return GLYPH_COUNT; }
  index_t w(index_t x = 0) const override { return width; }
  index_t h(index_t x = 0) const override { return height; }
  index_t tw() const override { return width*GLYPH_COUNT; }
  
  const FontType type;
  
  static constexpr const u8 LIGHT_STROKE_VALUE = 0x01;
  static constexpr const u8 EDGE_STROKE_VALUE = 0x02;
  static constexpr const u8 DARK_STROKE_VALUE = 0x03;
  static constexpr const u8 FIRST_FREE_COLOR_INDEX = DARK_STROKE_VALUE + 1;
  
  static FontData* fonts[FONT_TYPE_COUNT];
};

class FontSpriteSheet : public SpriteSheet
{
private:
  FontData *rawData;
  const Palette* palette;
  
  FontSpriteSheet(const FontSpriteSheet&) = delete;

public:
  FontSpriteSheet(FontData *data, const Palette* palette, s16 hor, s16 ver) : rawData(data), palette(palette), hor(hor), ver(ver) { }
  FontSpriteSheet(FontData *data, color_list palette, s16 hor, s16 ver) : rawData(data), palette(new IndexedPalette(palette)), hor(hor), ver(ver) { }

public:
  // TODO: FontSpriteSheet shouldn't delete palette or we can't use addresses to temporary, check leaks and such
  ~FontSpriteSheet() { /*delete palette;*/ }
  
  u32 at(index_t x, index_t y, index_t r = 0, index_t c = 0) const override
  {
    const byte* data = rawData->dataAt(r,c);
    byte value = data[x + y*rawData->tw()];
    
    //return {255,0,0};
    
    return value;
  }
  
  index_t tw() const override { return rawData->tw(); }
  index_t th() const override { return rawData->h(); }
  
  index_t sw(index_t r, index_t c = 0) const override { return rawData->w(); }
  index_t sh(index_t r = 0, index_t c = 0) const override { return rawData->h(); }
  
  const s16 hor, ver;
  
  const s8 charWidth(s8 c) const { return rawData->getGlyphWidth(c); }
  
  u16 stringWidth(const std::string& str, s8 hor, s8 spaceAdj) const
  {
    u16 l = 0;
    for (s8 c : str)
    {
      if (c == '^') continue;
      else if (c == ' ') l += charWidth(c) + hor - spaceAdj;
      else l += charWidth(c) + hor;
    }
    
    return l;
  }
  
  const Palette* getPalette() const override { return palette; }
  
  friend class Fonts;
};

namespace fonts
{
  using FontFace = FontSpriteSheet;
  
  template<FontType FONT, typename TYPE>
  class SpecificFontFace : public FontFace
  {
  protected:
    SpecificFontFace(const Palette* palette);
  };
  
  class MediumBoldFont : public SpecificFontFace<FONT_MEDIUM_THICK, MediumBoldFont>
  {
  public:
    MediumBoldFont(const Palette* palette) : SpecificFontFace<FONT_MEDIUM_THICK, MediumBoldFont>(palette) { };
    MediumBoldFont(Color color);
    MediumBoldFont(Color color, Color shadow);
    
  private:
  };
  
  class MediumFont : public SpecificFontFace<FONT_MEDIUM, MediumFont>
  {
  public:
    MediumFont(const Palette* palette) : SpecificFontFace<FONT_MEDIUM, MediumFont>(palette) { }
  };
  
  class SerifFont : public SpecificFontFace<FONT_SERIF, SerifFont>
  {
  public:
    SerifFont(const Palette* palette) : SpecificFontFace<FONT_SERIF, SerifFont>(palette) { };
    
  private:
    SerifFont(const std::array<Color, 9>& colors);
    SerifFont(Color color);
    SerifFont(Color color, Color shadow);
    
  public:
    static const SerifFont* of(Color color, Color shadow) { return new SerifFont(color, shadow); }
    static const SerifFont* withStripes(const std::array<Color, 4>& stripes, Color single);
    static const SerifFont* withShadow(const SerifFont* font, Color shadow);
    static const SerifFont* withShadowAndSingle(const SerifFont* font, Color single, Color shadow);
  };
  
  class TinyFont : public SpecificFontFace<FONT_TINY, TinyFont>
  {
  private:
    TinyFont(const Palette* palette) : SpecificFontFace<FONT_TINY, TinyFont>(palette) { };
  public:
    static const TinyFont* of(Color color, Color single, Color shadow);
  };
  
  
  struct base
  {
    static const SerifFont* SERIF_GOLD;
  };
}

class FontFaces
{
public:
  class Tiny {
  public:
    const static FontSpriteSheet *WHITE, *WHITE_STROKE, *YELLOW_STROKE, *RED_STROKE, *BROWN, *GOLD_COMBAT, *BLACK_COMBAT;
    
    class Palette : public IndexedPalette
    {
      
    public:
      Palette(Color highShadow, Color lowShadow, Color singlePixels, Color mainColor) : IndexedPalette({0, highShadow, lowShadow, singlePixels, mainColor}) { }
    };
  };
  
  class Small {
  public:
    const static FontSpriteSheet *YELLOW, *BLUE_MAGIC, *WHITE_PALE, *YELLOW_PALE, *RED_PALE, *WHITE, *TEAL, *TEAL_DARK, *BROWN, *BROWN_HELP, *GREENW, *BLUEW, *REDW, *PURPLEW, *YELLOWW, *GRAY_ITEM_CRAFT, *BLINK_WHITE_GREY;
  };
  
  class Medium {
  public:
    const static FontSpriteSheet *TEAL, *TEAL_STROKE, *BLACK, *TEAL_BRIGHT, *BLUE_MAGIC;
  };
  
  class MediumBold {
  public:
    const static FontSpriteSheet *BROWN_ITEM_CRAFT, *GOLD_ITEM_CRAFT, *GRAY_ITEM_CRAFT;
  };
  
  class Serif {
  public:
    const static FontSpriteSheet *TEAL, *BROWN, *BROWN_HELP, *YELLOW_SHADOW, *GOLD_SHADOW, *SILVER_SHADOW, *WHITE_SURVEY, *DARK_BROWN;
    
    const static FontSpriteSheet *BLACK_INFO_MENU, *BLACK_INFO_MENU_HOVER;
  };
  
  class Crypt {
  public:
    const static FontSpriteSheet *SERIF_BROWN, *TINY_BROWN;
  };
  
  class Huge {
  public:
    const static FontSpriteSheet *GOLD, *BROWN_CARTOGRAPHER;
  };
  
  
  class Palettes {
  public:
    const static BlinkingPalette *BLINK_WHITE_BLUE;
    const static Palette *SMALL_WHITE_PALE, *SMALL_YELLOW_PALE;
    const static Palette *WHITE_PRODUCTION;
  };
  
  struct Spacings
  {
    s16 hor, ver;
  };
  
  static const Spacings& defaultSpacingForType(FontType type);
  
  static void buildFonts();
  
  const FontSpriteSheet* generate(FontType, Color color, Color shadow);
};

class Fonts
{
private:
  static const FontSpriteSheet* font;
  static s16 vSpace, hSpace;
  static s16 spaceCharAdj;
  static const Palette *palette, *opalette;
  static std::unordered_map<char, const Palette*> fontColors;
  
public:
  static std::string format(const char* str, ...);
  
  static inline u16 stringWidth(const FontSpriteSheet* face, const std::string& string) { return face->stringWidth(string, hSpace, spaceCharAdj); }
  static inline u16 stringHeight() { return 0; }
    
  static inline void setFace(const FontSpriteSheet* face, s16 v, s16 h) { setFace(face); setVerHorSpace(v,h); }
  static inline void setFace(const FontSpriteSheet* face, const Palette* palette, s16 v, s16 h) { setFace(face,v,h); setPalette(palette); }
  
  static inline void setFace(const FontSpriteSheet* face)
  {
    font = face;
    hSpace = font->hor;
    vSpace = font->ver;
    palette = font->getPalette();
    opalette = palette;
    spaceCharAdj = 0;
  }
  
  static inline void setFace(const FontSpriteSheet* face, Palette* palette, s16 v, s16 h) { setFace(face); vSpace = v; hSpace = h; }
  static inline void setHorSpace(s16 h) { hSpace = h; }
  static inline void setVerSpace(s16 v) { vSpace = v; }
  static inline void setSpaceAdj(s16 s) { spaceCharAdj = s; }
  static inline void setVerHorSpace(s16 v, s16 h) { vSpace = v; hSpace = h; }
  static inline void setPalette(const Palette *p) { palette = p; opalette = p; }
  
  static u16 drawString(const std::string& string, u16 x, u16 y, TextAlign align, const Palette *palette)
  {
    setPalette(palette);
    u16 r = drawString(string,x,y,align);
    return r;
  }
  
  /* TODO: doesn't work because FontSpriteSheet destructor tries to deallocate palette */
  static u16 drawString(const std::string& string, u16 x, u16 y, TextAlign align, FontType font, const Palette* palette)
  {
    const auto& spacings = FontFaces::defaultSpacingForType(font);
    const FontSpriteSheet sheet{FontData::fonts[font], palette, spacings.ver, spacings.hor};
    return drawString(string, &sheet, x, y, align);
  }
  
  static u16 drawString(const std::string& string, const FontSpriteSheet* face, const Point& point, TextAlign align)
  {
    return drawString(string, face, point.x, point.y, align);
  }

  
  static u16 drawString(const std::string& string, const FontSpriteSheet* face, u16 x, u16 y, TextAlign align, const Palette *palette)
  {
    setFace(face);
    return drawString(string, x, y, align, palette);
  }
  
  static u16 drawString(const std::string& string, const FontSpriteSheet* face, u16 x, u16 y, TextAlign align)
  {
    setFace(face);
    return drawString(string, x, y, align);
  }
  
  static u16 drawString(const std::string& string, Point pt, TextAlign align) { return drawString(string, pt.x, pt.y, align); }
  static u16 drawString(const std::string& string, u16 x, u16 y, TextAlign align);
  static u16 drawStringContext(const std::string& string, u16 x, u16 y, TextAlign align);
  
  static u16 drawStringBounded(const std::string& string, const FontSpriteSheet* face, u16 x, u16 y, s16 bound, TextAlign align, const Palette* palette = nullptr)
  {
    setFace(face);
    
    if (palette)
      setPalette(palette);
    
    return drawStringBounded(string, x, y, bound, align);
  }
  
  static u16 drawStringBounded(const std::string& string, int x, int y, int bound, TextAlign align, const Palette* palette = nullptr);
  
  static std::pair<std::string, u16> drawStringBounded(const std::string& string, const FontSpriteSheet* font, Point p, int maxWidth, int maxLines, TextAlign align)
  {
    setFace(font);
    return drawStringBounded(string, p, maxWidth, maxLines, align);
  }
  
  static std::pair<std::string, u16> drawStringBounded(const std::string& string, Point p, int maxWidth, int maxLines, TextAlign align);

  
  static const Palette* paletteFor(char c) { return fontColors[c]; }
    
  static const FontSpriteSheet* fontForColor(PlayerColor color)
  {
    switch (color)
    {
      case GREEN: return FontFaces::Small::GREENW;
      case BLUE: return FontFaces::Small::BLUEW;
      case RED: return FontFaces::Small::REDW;
      case PURPLE: return FontFaces::Small::PURPLEW;
      case YELLOW: return FontFaces::Small::YELLOWW;
      default: return FontFaces::Small::WHITE;
    }
  }
};

#endif
