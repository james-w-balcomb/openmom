//
//  Gfx.cpp
//  OpenMoM
//
//  Created by Jack on 7/18/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "Gfx.h"

#include "Texture.h"
#include "GfxData.h"
#include "common/Util.h"
#include "SDLHelper.h"

#include "LBX.h"

u32 Gfx::ticks = 0;
u32 Gfx::fticks = 0;

SurfaceWrapper* Gfx::buffer = nullptr;
SurfaceWrapper* Gfx::activeBuffer = nullptr;
SurfaceWrapper* Gfx::canvas = nullptr;

const ColorMap* Gfx::map = nullptr;
const Palette* Gfx::palette = nullptr;

IndexedPalette* Gfx::mainPalette = new IndexedPalette(256);
IndexedPalette* Gfx::mainPaletteOpaque = new IndexedPalette(256);
IndexedPalette* Gfx::loadPalette = new IndexedPalette(256);


void Gfx::init()
{
  buffer = new SurfaceWrapper(320, 200);
  canvas = new SurfaceWrapper(320, 200);
    
  activeBuffer = canvas;
}

void Gfx::deinit()
{
  delete buffer;
  delete canvas;
}

void Gfx::fillRect(u16 x, u16 y, u16 w, u16 h, Color color)
{
  if (color.a == 0xFF)
  {
    const SDL_Rect rect { x, y, w, h };
    SDL_FillRect(canvas->data, &rect, color);
  }
  else
  {
    for (size_t i = 0; i < w; ++i)
      for (size_t j = 0; j < h; ++j)
      {
        activeBuffer->set(x+i, y+j, Color(activeBuffer->at(x+i, y+j)).blend(color));
      }
  }
}

void Gfx::drawLine(Color color, u16 x1, u16 y1, u16 x2, u16 y2)
{
  lock(activeBuffer);
  
  u8 alpha = color.a;
  
  if (alpha == 255)
  {
    if (y1 == y2)
    {
      for (u16 x = x1; x < x2; ++x)
        activeBuffer->set(x, y1, color);
    }
    else if (x1 == x2)
    {
      for (u16 y = y1; y < y2; ++y)
        activeBuffer->set(x1, y, color);
    }
  }
  else
  {
    if (y1 == y2)
    {
      for (u16 x = x1; x < x2; ++x)
        activeBuffer->set(x, y1, Color(activeBuffer->at(x, y1)).blend(color, alpha));
    }
    else if (x1 == x2)
    {
      for (u16 y = y1; y < y2; ++y)
        activeBuffer->set(x1, y, Color(activeBuffer->at(x1, y)).blend(color, alpha));
    }
  }
  
  unlock(activeBuffer);
}

void Gfx::rect(u16 x, u16 y, u16 w, u16 h, Color color)
{  
  lock(canvas);
  
  if (color.a == 0xFF)
  {
    for (int i = 0; i <= w; ++i)
    {
      canvas->set(x+i, y, color);
      canvas->set(x+i, y+h, color);
    }
    
    for (int i = 0; i < h; ++i)
    {
      canvas->set(x, y+i, color);
      canvas->set(x+w, y+i, color);
    }
  }
  else
  {
    for (int i = 0; i <= w; ++i)
    {
      canvas->set(x+i, y, color.blend(canvas->at(x+i, y), color.a));
      canvas->set(x+i, y+h, color.blend(canvas->at(x+i, y+h), color.a));
    }
    
    for (int i = 0; i < h; ++i)
    {
      canvas->set(x, y+i, color.blend(canvas->at(x, y+1), color.a));
      canvas->set(x+w, y+i, color.blend(canvas->at(x+w, y+i), color.a));
    }
  }

  unlock(canvas);
}

constexpr color_d SHADOW_COLOR = 0xFF00FF00U;
constexpr color_d TRANSPARENT_COLOR = 0xFFFF00FFU;
constexpr color_d ALPHA_MASK = 0xFF000000U;
constexpr color_d RED_MASK = 0x00FF0000U;
constexpr color_d GREEN_MASK = 0x0000FF00U;
constexpr color_d BLUE_MASK = 0x000000FFU;

void Gfx::alphaBlend(const SDL_Rect& rect, Color color)
{
  lock(canvas);
  
  u32 a = (color & ALPHA_MASK) >> 24;
  u32 r = (color & RED_MASK) >> 16;
  u32 g = (color & GREEN_MASK) >> 8;
  u32 b = (color & BLUE_MASK);
  
  for (s32 y = 0; y < rect.h; ++y)
  {
    for (s32 x = 0; x < rect.w; ++x)
    {
      if (x+rect.x < canvas->tw() && y+rect.y < canvas->th() && x+rect.x >= 0 && y+rect.y >= 0)
      {
        u32 ps = canvas->at(rect.x+x, rect.y+y);
        
        u32 a2 = (ps & ALPHA_MASK);
        u32 r2 = (ps & RED_MASK) >> 16;
        u32 g2 = (ps & GREEN_MASK) >> 8;
        u32 b2 = (ps & BLUE_MASK);
        
        u32 rd = (r * a) + (r2 * (255-a));
        u32 gd = (g * a) + (g2 * (255-a));
        u32 bd = (b * a) + (b2 * (255-a));
        
        u32 rb = (rd+1 + (rd >> 8)) >> 8;
        u32 gb = (gd+1 + (gd >> 8)) >> 8;
        u32 bb = (bd+1 + (bd >> 8)) >> 8;
        
        canvas->set(rect.x+x, rect.y+y, a2 | (rb & 0xFF << 16) | (gb & 0xFF << 8) | (bb & 0xFF));
      }
    }
  }
  
  unlock(canvas);
}

#define ALPHA_SHIFT (1)

void Gfx::rawBlit(const SpriteSheet *gsrc, SpriteSheet *gdst, u16 fx, u16 fy, s16 tx, s16 ty, u16 w, u16 h, u16 r, u16 c)
{
  lock(gsrc);
  lock(gdst);

  bool nativePalette = !palette && gsrc->getPalette();
  if (nativePalette)
    palette = gsrc->getPalette();

  for (s32 y = 0; y < h; ++y)
  {
    for (s32 x = 0; x < w; ++x)
    {
      if (x+tx < gdst->tw() && y+ty < gdst->th() && x+tx >= 0 && y+ty >= 0)
      {
        u32 ps = gsrc->at(fx+x, fy+y, r, c);
        
        //TODO: treat color at index 0 always as transparent?
        if (ps == 0)
          continue;
        
        if (palette)
          ps = palette->get(ps);
        
        if (ps == SHADOW_COLOR)
        {
          Color pd = gdst->at(tx+x,ty+y);
          
          u8 r = pd.r, g = pd.g, b = pd.b;
          
          r >>= ALPHA_SHIFT;
          g >>= ALPHA_SHIFT;
          b >>= ALPHA_SHIFT;
          
          gdst->set(tx+x, ty+y, {r,g,b});
        }
        else if (ps == TRANSPARENT_COLOR)
          continue;
        else
        {
          // colorMap present

          
          if (map)
            ps = map->get(ps);
          
          u32 sa = ((ps & ALPHA_MASK) >> 24);
          
          if (sa == 0xFF)
            gdst->set(tx+x, ty+y, ps);
          else if (sa == 0x00)
            continue;
          else
          {
            int da = 256 - sa;
            
            Color pd = gdst->at(tx+x, ty+y);
            int r2 = (pd & RED_MASK) >> 16;
            int g2 = (pd & GREEN_MASK) >> 8;
            int b2 = (pd & BLUE_MASK);
            
            int r1 = (ps & RED_MASK) >> 16;
            int g1 = (ps & GREEN_MASK) >> 8;
            int b1 = (ps & BLUE_MASK);
            
            gdst->set(tx+x, ty+y, 0xFF000000 | (((r2*da + r1*sa) >> 8) << 16) | (((g2*da + g1*sa) >> 8) << 8) | ((b2*da + b1*sa) >> 8));
          }
        }
      }
    }
  }
  
  if (nativePalette)
    palette = nullptr;
  
  unlock(gsrc);
  unlock(gdst);
}

void Gfx::drawPixel(Color color, u16 x, u16 y)
{
  lock(activeBuffer);
  activeBuffer->set(x, y, color);
  unlock(activeBuffer);
}

void Gfx::resetBuffer(u16 w, u16 h, u16 x, u16 y)
{
  lock(buffer);
  //TODO: can be optimized
  for (int yy = 0; yy < h; ++yy)
    for (int xx = 0; xx < w; ++xx)
      buffer->set(x + xx, y + yy, 0x00000000);
  unlock(buffer);
}

void Gfx::drawClipped(const SpriteSheet* sheet, s16 x, s16 y, s16 fx, s16 fy, s16 w, s16 h)
{
  s16 tw = w != 0 ? (w > 0 ? w : sheet->tw() + w - fx) : sheet->tw() - fx;
  s16 th = h != 0 ? (h > 0 ? h : sheet->th() + h - fy) : sheet->th() - fy;
  s16 tx = fx;
  s16 ty = fy;
  s16 dx = x;
  s16 dy = y;
  
  blit(sheet, activeBuffer, tx, ty, dx, dy, tw, th);
}

void Gfx::drawClipped(SpriteInfo info, u16 sx, u16 sy, s16 tx, s16 ty, s16 w, s16 h)
{
  const auto* sheet = info.sheet();
  s16 tw = w != 0 ? (w > 0 ? w : info.sw() + w - tx) : info.sw() - tx;
  s16 th = h != 0 ? (h > 0 ? h : info.sh() + h - ty) : info.sh() - ty;
  
  blit(sheet, activeBuffer, tx, ty, sx, sy, tw, th, 0, info.y());

}


void Gfx::rawDraw(SpriteInfo info, u16 x, u16 y)
{
  blit(info.sheet(), canvas, 0, 0, x, y, info.sw(), info.sh(), info.x(), info.y());
}

void Gfx::draw(const SpriteSheet* sheet, s16 x, s16 y, u16 r, u16 c)
{
  blit(sheet, activeBuffer, 0, 0, x, y, sheet->sw(r,c), sheet->sh(r,c), r, c);
}

void Gfx::drawAnimated(SpriteInfo info, u16 x, u16 y, s16 offset, s16 animFactor)
{
  const auto* sheet = info.sheet();
  
  if (info.isLBX())
  {
    const auto* lsheet = static_cast<const lbx::LBXSpriteData*>(sheet);
    
    if (lsheet->count > 1)
      draw(lsheet, x, y, ((offset+fticks)/animFactor) % lsheet->count, 0);
    else
      draw(lsheet, x, y, 0, 0);
  }
  else
  {
    const auto* tex = static_cast<const Texture*>(sheet);
    animFactor = tex->animFactor;
    const auto count = tex->animatedSprites.empty() ? tex->cols : tex->animatedSprites[info.x()];
    draw(sheet, x, y, info.y(), ( ( (offset+fticks) / tex->animFactor) % count));
  }
}

void Gfx::drawMasked(SpriteInfo ssrc, SpriteInfo smask, coord_t dx, coord_t dy, coord_t ox, coord_t oy)
{
  /* this is a dummy sprite sheet which uses a mask to return trasparent or the original color */
  class MaskedSpriteSheet : public SpriteSheet
  {
    SpriteInfo ssrc, smask;
    const SpriteSheet *src, *mask;
    coord_t ox, oy;
    
  public:
    MaskedSpriteSheet(SpriteInfo src, SpriteInfo mask, coord_t ox, coord_t oy) : ssrc(src), smask(mask), src(src.sheet()), mask(mask.sheet()), ox(ox), oy(oy) { }
    
    u32 at(index_t x, index_t y, index_t c = 0, index_t r = 0) const override {
      bool isInside = x+ox < mask->sw(smask.x(), smask.y()) && y < mask->sh(smask.x(), smask.y());
      
      if (isInside)
        return mask->at(x+ox, y+oy, smask.x(), smask.y()) == 0 ? src->at(x,y, ssrc.x(), ssrc.y()) : 0;
      else
        return 0;
    }
    index_t tw() const override { return src->tw(); }
    index_t th() const override { return src->th(); }
    
    index_t sw(index_t r, index_t c) const override { return src->sw(r,c); }
    index_t sh(index_t r, index_t c) const override { return src->sh(r,c); }
    
    const Palette* getPalette() const override { return src->getPalette(); }
  };
  
  MaskedSpriteSheet sheet(ssrc, smask, ox, oy);
  Gfx::blit(&sheet, activeBuffer, 0, 0, dx, dy, ssrc.sw(), ssrc.sh(), ssrc.x(), ssrc.y());
}

template<typename OutlineGfx>
void Gfx::drawOutline(const SpriteSheet* sprite, s16 x, s16 y, s16 r, s16 c, const OutlineGfx& gfx)
{
  s32 w = sprite->sw(r,c);
  s32 h = sprite->sh(r,c);
  
  resetBuffer((w+2)*2,h+2);
  bindBuffer();
  draw(sprite,1,1,r,c);
  bindCanvas();
  
  lock(buffer);
  
  for (s16 i = 0; i < w+2; ++i)
  {
    for (s16 j = 0; j < h+2; ++j)
    {
      bool found = false;
      Color pixel = buffer->at(i,j);
      
      /* if pixel is transparent */
      if ((pixel & 0x00FFFFFF) == 0)
      {
        /* for each orthogonal neighbour */
        for (const auto& delta : Util::ODIRS)
        {
          s32 dx = i + delta.x, dy = j + delta.y;
          
          /* if neighbour pixel is inside size of sprite */
          if (dx > 0 && dx < w+2 && dy > 0 && dy < h+2)
          {
            /* and neighbour pixel is not trasparent */
            if ((buffer->at(dx,dy) & 0x00FFFFFF) != 0)
            {
              /* then it's an outline pixel */
              found = true;
              break;
            }
          }
        }
      }
      
      if (found)
        buffer->set(w+2+i, j, gfx.getColor(i,j));
    }
  }
  
  unlock(buffer);
  mergeBuffer(w+2, 0, x-1, y-1, w+2, h+2);
}

void Gfx::drawGlow(SpriteInfo info, s16 x, s16 y, School school)
{
  struct GlowColors
  {
    const decltype(MiscMaps::SCHOOL_GLOW_COLORS)::mapped_type& data;
    const int phase;
    
    // TODO: phase is too much linear compared to real one
    GlowColors(School school) : data(MiscMaps::SCHOOL_GLOW_COLORS[school]), phase(fticks%50/3) { }
    inline Color getColor(int x, int y) const { return data[(phase+x+y) % data.size()]; }
  };
    
  const GlowColors colors(school);
  drawOutline(info.sheet(), x, y, info.x(), info.y(), colors);
}

void Gfx::drawSolidOutline(SpriteInfo info, s16 x, s16 y, Color color)
{
  struct OutlineColors
  {
    const Color color;
    OutlineColors(Color color) : color(color) { }
    inline Color getColor(int x, int y) const { return color; }
  };
  
  const OutlineColors colors(color);
  
  drawOutline(info.sheet(), x, y, info.x(), info.y(), colors);
}

void Gfx::drawGrayScale(const SpriteSheet* src, u16 r, u16 c, u16 x, u16 y)
{
  /*bindColorMap(&MiscMaps::GRAYSCALE);
  draw(src, x, y, r, c);
  unbindColorMap();*/
  
  u16 tw = src->sw(r,c), th = src->sh(r,c);
  
  resetBuffer(tw,th);
  bindBuffer();
  draw(src, 0, 0, r, c);
  
  lock(buffer);
  
  for (s16 xx = 0; xx < tw; ++xx)
    for (s16 yy = 0; yy < th; ++yy)
      buffer->set(xx, yy, MiscMaps::GRAYSCALE.get(buffer->at(xx, yy)));
  
  unlock(buffer);
  mergeBuffer(0, 0, x, y, tw, th);
  bindCanvas();
}

void Gfx::mergeBuffer(u16 xf, u16 yf, u16 xt, u16 yt, u16 w, u16 h, const ColorFilter* const filter)
{
  /* TODO: todo, this could be optimized to use SDL directly, is it good? */
  /*buffer->lock();
  canvas->lock();
  SDL_Rect src{xf, yf, w, h}, dest{xt, yt, w, h};
  SDL_BlitSurface(buffer->data, &src, canvas->data, &dest);
  buffer->unlock();
  canvas->unlock(); */
  
  //TODO this was blit(buffer, canvas, xf, yf, xt, yt, w, h); before but doesn't work
  // with new indexed mode
  
  for (u32 y = 0; y < h; ++y)
    for (u32 x = 0; x < w; ++x)
    {
      const u32 dx = xt + x, dy = yt + y;
      
      //TODO consider having a specialized method without filter for performance if it's needed
      Color src = canvas->at(dx, dy);
      Color color = buffer->at(xf + x, yf + y);
      if (color.a && dx >= 0 && dx < WIDTH && dy >= 0 && dy < HEIGHT)
        canvas->set(dx, dy, src.blend(filter ? Color(filter->get(color)) : color));
    }
}

void Gfx::mergeBufferDownScaled(u16 xf, u16 yf, u16 xt, u16 yt, u16 w, u16 h)
{
  assert(w % 2 == 0 && h % 2 == 0);
  const u16 fw = w/2;
  const u16 fh = h/2;
  
  for (u16 xx = 0; xx < fw; ++xx)
  {
    for (u16 yy = 0; yy < fh; ++yy)
    {
      const u16 bx = xf + xx*2, by = yf + yy*2;
      
      Color c1 = buffer->at(bx, by);
      Color c2 = buffer->at(bx + 1, by);
      Color c3 = buffer->at(bx, by + 1);
      Color c4 = buffer->at(bx + 1, by + 1);
      Color f = c1.blend(c2, 0.5f).blend(c3.blend(c4, 0.5f), 0.5f);
      
      canvas->set(xt + xx, yt + yy, f);
    }
  }
}


#define PIXEL(s, w, x, y) s[(y)*(w) + x]
void Gfx::saveScreenshot(const char* path)
{
  auto* surface = Gfx::createSurface(canvas->data->w*2, canvas->data->h*2);
  
  auto src = canvas->data;
  auto dst = surface;
  auto w = canvas->data->w, h = canvas->data->h;
  constexpr auto sx = 0, sy = 0, dx = 0, dy = 0;
  auto f = 2;
  
  SDL_LockSurface(src);
  SDL_LockSurface(dst);
  
  u32 *sp = (u32*)src->pixels;
  u32 *dp = (u32*)dst->pixels;
  
  // for each pixel in source
  for (u16 x = sx; x < sx+w; ++x)
    for (u16 y = sy; y < sy+h; ++y)
    {
      u32 color = PIXEL(sp, src->w, x, y);
      // for each dest pixel according to scale factor
      for (u16 ix = 0; ix < f; ++ix)
        for (u16 iy = 0; iy < f; ++iy)
        {
          PIXEL(dp, dst->w, dx + x*f + ix, dy + y*f + iy) = color;
        }
    }
  
  SDL_UnlockSurface(src);
  SDL_UnlockSurface(dst);
  
  IMG_SavePNG(surface, path);
  SDL_FreeSurface(surface);
}

void CursorManager::hideCursor()
{
  SDL_ShowCursor(false);
}

void CursorManager::showDefaultCursor()
{
  SDL_ShowCursor(true);
}


