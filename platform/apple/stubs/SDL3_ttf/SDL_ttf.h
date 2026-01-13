// SDL_ttf.h - iOS stub header
// Provides type definitions for SDL_ttf without actual implementation
// This is used because ugui.h includes SDL_ttf.h but doesn't use it

#pragma once

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque font type
typedef struct TTF_Font TTF_Font;
typedef struct TTF_TextEngine TTF_TextEngine;
typedef struct TTF_Text TTF_Text;

// Font style flags
#define TTF_STYLE_NORMAL        0x00
#define TTF_STYLE_BOLD          0x01
#define TTF_STYLE_ITALIC        0x02
#define TTF_STYLE_UNDERLINE     0x04
#define TTF_STYLE_STRIKETHROUGH 0x08

// Hinting
typedef enum TTF_HintingFlags {
    TTF_HINTING_NORMAL = 0,
    TTF_HINTING_LIGHT = 1,
    TTF_HINTING_MONO = 2,
    TTF_HINTING_NONE = 3,
    TTF_HINTING_LIGHT_SUBPIXEL = 4
} TTF_HintingFlags;

// Stub functions (not implemented on iOS - use CoreText instead)
static inline bool TTF_Init(void) { return false; }
static inline void TTF_Quit(void) {}
static inline TTF_Font* TTF_OpenFont(const char *file, float ptsize) { return NULL; }
static inline TTF_Font* TTF_OpenFontIO(SDL_IOStream *src, bool closeio, float ptsize) { return NULL; }
static inline void TTF_CloseFont(TTF_Font *font) {}
static inline bool TTF_SetFontSize(TTF_Font *font, float ptsize) { return false; }
static inline int TTF_GetFontStyle(const TTF_Font *font) { return TTF_STYLE_NORMAL; }
static inline void TTF_SetFontStyle(TTF_Font *font, int style) {}
static inline int TTF_GetFontOutline(const TTF_Font *font) { return 0; }
static inline void TTF_SetFontOutline(TTF_Font *font, int outline) {}
static inline int TTF_GetFontHinting(const TTF_Font *font) { return TTF_HINTING_NORMAL; }
static inline void TTF_SetFontHinting(TTF_Font *font, int hinting) {}
static inline int TTF_FontHeight(const TTF_Font *font) { return 0; }
static inline int TTF_FontAscent(const TTF_Font *font) { return 0; }
static inline int TTF_FontDescent(const TTF_Font *font) { return 0; }
static inline int TTF_FontLineSkip(const TTF_Font *font) { return 0; }
static inline bool TTF_GetStringSize(TTF_Font *font, const char *text, size_t length, int *w, int *h) { if(w)*w=0; if(h)*h=0; return false; }
static inline SDL_Surface* TTF_RenderText_Blended(TTF_Font *font, const char *text, size_t length, SDL_Color fg) { return NULL; }
static inline SDL_Surface* TTF_RenderGlyph_Blended(TTF_Font *font, Uint32 ch, SDL_Color fg) { return NULL; }
static inline bool TTF_FontHasGlyph(TTF_Font *font, Uint32 ch) { return false; }
static inline bool TTF_GetGlyphMetrics(TTF_Font *font, Uint32 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance) {
    if(minx) *minx = 0; if(maxx) *maxx = 0; if(miny) *miny = 0; if(maxy) *maxy = 0; if(advance) *advance = 0;
    return false;
}

#ifdef __cplusplus
}
#endif
