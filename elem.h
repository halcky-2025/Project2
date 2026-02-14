// GlyphAtlas_bgfx.hpp
#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <bgfx/bgfx.h>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstring>
#include <cassert>
#include "mem.h"



enum Position {
	Relative, Pos_Fixed, Absolute  // Fixed renamed to Pos_Fixed to avoid conflict with MacTypes.h
};
enum Layout {
    ocupay, none, left, right
};
enum MouseCall {
	MouseDown, MouseUp, DoubleClick, MouseMove
};
enum KeyCall {
	KeyDown, KeyUp, KeyPress
};
enum Align {
    Left, Center, Right, Separate
};
enum SizeType {
    Auto, Break, Page, Range, Scroll
};
enum LetterType {
	_None, _ElemEnd, _Main, _End, _Kaigyou, _Line, _VLine, _Letter, _CloneElem,
	_Name, _Number, _BracketL, _BracketR, _BlockL, _BlockR, _BraceL, _BraceR, _Semicolon, _Comma, _Plus, _Minus, _Mul, _Div, _Mod,
	_Space, _Equal
};
class FontAtlas;
#ifndef __MACTYPES__  // Avoid conflict with MacTypes.h Point on Apple platforms
typedef struct {
    int x;
    int y;
} Point;
#endif
struct CloneElement;
struct Measure;
struct Graphic;
struct Local;
struct Select;
struct EventObj {
	char* blk;
	MemTable* table;
};
typedef struct MemObj {
	char* blk;
	MemTable* req;
	MemTable* res;
};

typedef struct MemFunc {
	char* blk;
	MemObj* obj;
	void (*func)(Frame* frame);
};

typedef struct PointI {
	int x;
	int y;
}; 
enum class RenderGroupFlags : uint8_t {
	None = 0,
	Pinned = 1 << 0,
	Transient = 1 << 1,
};
struct FontId {
	uint64_t name = 0;   // �t�H���g�� or �p�X
	int size = 0;    
	FontId(const char* n, int s) : size(s) {
		size_t len = strnlen(n, 8);
		memcpy(&name, n, len);
	} 

	// �L�����`�F�b�N
	bool valid() const { return name != 0 && size > 0; }

	// ��r���Z�q�istd::map�p�j
	bool operator<(const FontId& o) const {
		if (name != o.name) return name < o.name;
		return size < o.size;
	}

	// �������Z�q�istd::unordered_map�p�j
	bool operator==(const FontId& o) const {
		return name == o.name && size == o.size;
	}

	bool operator!=(const FontId& o) const {
		return !(*this == o);
	}

	// �f�o�b�O�p������
	std::string toString() const {
		return name + "@" + std::to_string(size);
	}
	uint64_t computeHash() const {
		// FNV-1a �n�b�V��
		uint64_t h = 14695981039346656037ULL;

		h ^= name;
		h *= 1099511628211ULL;

		// size��������
		h ^= static_cast<uint64_t>(size);
		h *= 1099511628211ULL;
		return h;
	}
};

struct ResolvedTexture {
	bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
	float u0 = 0, v0 = 0, u1 = 1, v1 = 1;
	uint16_t width = 0;
	uint16_t height = 0;

	bool isValid() const { return bgfx::isValid(texture); }
};
FontId getFont(const char* name, int size) {
	FontId id(name, size);
	return id;
}
enum class ImageIdDomain : uint8_t {
	Invalid = 0x00,
	File = 0x01,  // �t�@�C���p�X���琶��
	Memory = 0x02,  // �������f�[�^���琶��
	Offscreen = 0x03,  // �I�t�X�N���[���e�N�X�`��
	Generated = 0x04,  // ���I�����iprocedural�j
	FontGlyph = 0x05,  // FontAtlas �̃O���t/�摜
	Thumbnail = 0x06,  // �T���l�C�� (Grid/Shelf)
};

using ImageId = uint64_t;

inline constexpr ImageId makeImageId(ImageIdDomain domain, uint64_t localId) {
	return (uint64_t(domain) << 56) | (localId & 0x00FFFFFFFFFFFFFFULL);
}

inline constexpr ImageIdDomain getImageIdDomain(ImageId id) {
	return static_cast<ImageIdDomain>((id >> 56) & 0xFF);
}

inline constexpr uint64_t getImageIdLocal(ImageId id) {
	return id & 0x00FFFFFFFFFFFFFFULL;
}

inline constexpr bool isValidImageId(ImageId id) {
	return id != 0 && getImageIdDomain(id) != ImageIdDomain::Invalid;
}

//=============================================================================
// ImageId ����
//=============================================================================

class ImageIdGenerator {
public:
	static ImageId fromFile(const std::string& path) {
		return makeImageId(ImageIdDomain::File, std::hash<std::string>{}(path));
	}

	static ImageId fromMemory() {
		static std::atomic<uint64_t> counter{ 0 };
		return makeImageId(ImageIdDomain::Memory, ++counter);
	}

	static ImageId forOffscreen() {
		static std::atomic<uint64_t> counter{ 0 };
		return makeImageId(ImageIdDomain::Offscreen, ++counter);
	}

	static ImageId forGenerated() {
		static std::atomic<uint64_t> counter{ 0 };
		return makeImageId(ImageIdDomain::Generated, ++counter);
	}

	static ImageId forFontGlyph(uint64_t localId) {
		return makeImageId(ImageIdDomain::FontGlyph, localId);
	}

	static ImageId forThumbnail(uint64_t contentId) {
		return makeImageId(ImageIdDomain::Thumbnail, contentId);
	}
};
//=============================================================================
// StandaloneTextureInfo - 1���G�e�N�X�`�����
//=============================================================================

enum class ImageOrigin : uint8_t {
	File,
	Memory,
	Offscreen,
	Generated,
};
struct StandaloneTextureInfo {
	bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;
	bgfx::FrameBufferHandle fbo = BGFX_INVALID_HANDLE;
	PointI size;
	uint32_t refCount = 0;
	uint64_t lastUsedFrame = 0;
	bool persistent = false;
	bool isRenderTarget = false;
	ImageOrigin origin = ImageOrigin::File;
};
struct LayerInfo;
struct ExtendedRenderGroup;
struct ImageMaster;
struct FontAtlas;
StandaloneTextureInfo* mygetStandaloneTextureInfo(ThreadGC* thgc, ImageId imageId);
ImageId queueOffscreenNew(ThreadGC* thgc, int width, int height);
void queueOffscreenResize(ThreadGC* thgc, ImageId offscreenid, int width, int height);
ImageId myloadTexture2D(ThreadGC* thgc, const char* path);
ExtendedRenderGroup& createGroup(ThreadGC* thgc);
void drawTextUTF8(LayerInfo* layer, FontAtlas& atlas, FontId font,
	const char* text, int length, float x, float y,
	float zIndex, ExtendedRenderGroup* group, uint32_t color);
void drawString(LayerInfo* layer, FontAtlas& atlas, FontId font,
	String* text, float x, float y,
	float zIndex, uint32_t color, ExtendedRenderGroup* group, bgfx::FrameBufferHandle* targetFBO, PointI *fbsize, uint8_t viewId);
void drawUnderPagingBar(LayerInfo* layer, FontAtlas& atlas, FontId font, float x, float y, float width, float height, float currentPage, float totalPages, float zIndex,
	ExtendedRenderGroup* group, bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);
void drawRightPagingBar(LayerInfo* layer, FontAtlas& atlas, FontId font,
	float x, float y, float width, float height,
	float currentPage, float totalPage,
	float zIndex, ExtendedRenderGroup* group,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);
FontAtlas* getAtlas(ThreadGC* thgc);
TTF_Font* getFont(FontAtlas* atlas);
void drawUnderScrollBar(LayerInfo* layer,
	float x, float y, float width, float height,
	float currentX, float pageWidth, float totalWidth,
	float zIndex,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);
void drawRightScrollBar(LayerInfo* layer,
	float x, float y, float width, float height,
	float currentY, float pageHeight, float totalHeight,
	float zIndex,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);
#include "newelem.h"

typedef struct TreeElement {
	String* id;
	NewElement* elem;
	TreeElement* parent;
	List* children;
	bool del;
} TreeElement;
void TreeElementAdd(ThreadGC* thgc, NewLocal* local, TreeElement* self, TreeElement* child) {
	if (self->children == NULL) {
		self->children = create_list(thgc, sizeof(TreeElement*), true);
	}
	add_list(thgc, self->children, (char*)child);
	child->parent = self;
	add_mapy(thgc, local->temap, child->id, (char*)child);
}
void TreeElementMove(ThreadGC* thgc, TreeElement* self, TreeElement* child) {
	if (child->parent != NULL) {
		remove_list_val(thgc, child->parent->children, (char*)child);
		child->parent = self;
	}
}
void TreeElementRemove(ThreadGC* thgc, NewLocal* local, TreeElement* self) {
	if (self->parent != NULL) {
		remove_list_val(thgc, self->parent->children, (char*)self);
	}
	remove_mapy(thgc, local->temap, self->id);
}
/*typedef struct Element {
	struct Element* next, * before, * parent, * childend;
	int margins[4];
	int paddings[4];
	Point pos, pos2, scroll, size, size2;
	enum SizeType xtype, ytype;
	enum Position position;
	enum Layout layout;
	bool selectable;
	GlyphAtlas* atlas;
	bool single;
	//Func<MouseEvent, Local, bool> mouse;
	//Func<KeyEvent, Local, bool> key;
	bool update, recompile;
	SDL_Color color;
	SDL_Color background;
	enum LetterType type;
	enum Align align;
	int index;
	List* groups; // Corrected the syntax error here
	//public Func<String, Local, List<Element>> Recompile
	int (*Count)(struct Element*);
	struct Element* (*Measure)(ThreadGC*, struct Element*, Measure*, Local*, int*);
	void (*Draw)(ThreadGC*, struct Element*, Graphic*, Local*, bool*, RenderCommandQueue* q);
	int (*Mouse)(ThreadGC*, struct Element*, MouseEvent*, Local*);
	int (*Key)(ThreadGC*, struct Element*, KeyEvent*, Local*, bool*);
	MemFunc* GoMouseDown; MemFunc* BackMouseDown; MemFunc* GoKeyDown; MemFunc* BackKeyDown;
	bgfx::FrameBufferHandle fbPing = BGFX_INVALID_HANDLE;
	bgfx::FrameBufferHandle fbPong = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle texPing = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle texPong = BGFX_INVALID_HANDLE;
	bool initTex;
	std::mutex m;
	bool ping = false;
};
void createSurface(Element* elem, int w, int h) {
	const uint64_t flags = BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

	// Use BGRA8 for Metal compatibility
	elem->texPing = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, flags);
	elem->texPong = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::BGRA8, flags);

	// Create depth textures for offscreen framebuffers (required for Metal depth testing)
	bgfx::TextureHandle depthPing = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT);
	bgfx::TextureHandle depthPong = bgfx::createTexture2D(w, h, false, 1, bgfx::TextureFormat::D24S8, BGFX_TEXTURE_RT);

	// Use bgfx::Attachment to properly attach color and depth
	bgfx::Attachment attPing[2];
	attPing[0].init(elem->texPing);  // color attachment
	attPing[1].init(depthPing);       // depth attachment

	bgfx::Attachment attPong[2];
	attPong[0].init(elem->texPong);  // color attachment
	attPong[1].init(depthPong);       // depth attachment

	elem->fbPing = bgfx::createFrameBuffer(2, attPing, true);
	elem->fbPong = bgfx::createFrameBuffer(2, attPong, true);
}
Element;
typedef struct HoppyWindow {
	NewLocal* local;
	bool invalidate;
	bool update;
} HoppyWindow;
typedef struct Local {
    Element base;
	HoppyWindow* window;
	Select* selects[2];
	int seln;
	int countn;
	Map* temap;
} Local;
void LocalCheck(ThreadGC* thgc, char* self) {
	Local* local = (Local*)self;
	local->window = (HoppyWindow*)copy_object(thgc, (char*)local->window);
	local->selects[0] = (Select*)copy_object(thgc, (char*)local->selects[0]);
	local->selects[1] = (Select*)copy_object(thgc, (char*)local->selects[1]);
	local->temap = (Map*)copy_object(thgc, (char*)local->temap);
}
typedef struct State {
	List* elements;//List<Element>
	Element* element;
	List* histories;//List<Element>
	int n;
} State;
void StateCheck(ThreadGC* thgc, char* self) {
	State* state = (State*)self;
	state->elements = (List*)copy_object(thgc, (char*)state->elements);
	state->histories = (List*)copy_object(thgc, (char*)state->histories);
	state->element = (Element*)copy_object(thgc, (char*)state->element);
}
typedef struct Select {
	State* state;
	int n;
} Select;
void SelectCheck(ThreadGC* thgc, char* self) {
	Select* select = (Select*)self;
	select->state = (State*)copy_object(thgc, (char*)select->state);
}
typedef struct SelectE {
	State* state;
	void (*Select)(struct SelectE*, Element*, Select*, Select*);
} SelectE;
typedef struct Measure {
	float x, y;
	float px, py;
	float sizex, sizey;
	float h;
	enum SizeType xtype, ytype;
	GlyphAtlas* atlas;
	SDL_Renderer* g;
	HoppyWindow* window;
	State* state;
	List** (*Recompile)(String*, Local*);//List<Element>
} Measure;
typedef struct Graphic {
	SDL_Renderer* g;
	GlyphAtlas* atlas;
	float x, y;
	float px, py;
	float h;
} Graphic;
typedef struct EndElement {
	Element base;
} EndElement;
typedef struct VLine VLine;
typedef struct Line {
	Element base;
	VLine* vline;
} Line;
typedef struct VLine {
	Element base;
	Element* childstart;
	Element* childfinish;
	int nstart;
	int nend;
} VLine;
typedef struct LetterPart {
	String* text;
	String* name;
	String* error;
	GlyphAtlas* atlas;
} LetterPart;
void LetterPartCheck(ThreadGC* thgc, char* self) {
	LetterPart* part = (LetterPart*)self;
	part->text = (String*)copy_object(thgc, (char*)part->text);
	part->name = (String*)copy_object(thgc, (char*)part->name);
	part->error = (String*)copy_object(thgc, (char*)part->error);
}
typedef struct Letter {
	Element base;
	LetterPart* value;
} Letter;
typedef struct Status{
	Map * statuses;
} Status;
typedef struct Div {
	Element base;
	Status base2;

};
typedef struct Sheet {
	Element base;
} Sheet;
typedef struct CloneElement {
	Element base;
	Element* childstart;
} CloneElement;
void initElement(ThreadGC* thgc, Element* self);
void initLocal(ThreadGC* thgc, Local* self, HoppyWindow*);
void initEndElement(ThreadGC* thgc, EndElement* self, Element* parent);
void initLine(ThreadGC* thgc, Line* self);
void initVLine(ThreadGC* thgc, VLine* self);
void initLetter(ThreadGC* thgc, Letter* self, enum LetterType type);
void initKaigyou(ThreadGC* thgc, Letter* self, enum LetterType type);
void ElementAdd(ThreadGC* thgc, Element* self, Element* elem);
void initHoppyWindow(ThreadGC* thgc, HoppyWindow* self) {
	self->local = (NewLocal*)GC_malloc(thgc, _LocalC);
	initNewLocal(thgc, self->local);
	self->invalidate = true;
	self->update = true;
}
void HoppyWindowCheck(ThreadGC* thgc, char* self) {
	HoppyWindow* hoppy = (HoppyWindow*)self;
	hoppy->local = (NewLocal*)copy_object(thgc, (char*)hoppy->local);
}
void CloneElementCheck(ThreadGC* thgc, char* self) {
	CloneElement* clone = (CloneElement*)self;
	clone->childstart = (Element*)copy_object(thgc, (char*)clone->childstart);
	clone->base.before = (Element*)copy_object(thgc, (char*)clone->base.before);
	clone->base.next = (Element*)copy_object(thgc, (char*)clone->base.next);
	clone->base.parent = (Element*)copy_object(thgc, (char*)clone->base.parent);
	clone->base.childend = (Element*)copy_object(thgc, (char*)clone->base.childend);
	clone->base.groups = (List*)copy_object(thgc, (char*)clone->base.groups);
}
void initLocal(ThreadGC* thgc, Local* local, HoppyWindow* window) {
	initElement(thgc, (Element*)local);
	Select* select = (Select*)GC_malloc(thgc, _Select);
	select->state = (State*)GC_malloc(thgc, _State);
	select->state->elements = create_list(thgc, sizeof(Element*), true);
	Element* elem = (Element*)GC_malloc(thgc, _Element);
	initElement(thgc, elem);
	add_list(thgc, select->state->elements, (char*)elem);
	local->selects[0] = local->selects[1] = select;
	local->window = window;
	local->temap = create_mapy(thgc, true);
}
void Next(Element* self, Element* elem) {
	elem->parent = self->parent;
	elem->next = self->next;
	elem->before = self;
	self->next->before = elem;
	self->next = elem;
}
void Before(Element* self, Element* elem) {
	elem->parent = self->parent;
	elem->next = self;
	elem->before = self->before;
	self->before->next = elem;
	self->before = elem;
}
void AddRange(Element* self, Element* elem) {
	Element* next = NULL;
	for (; elem->type != _ElemEnd; elem = next) {
		next = elem->next;
		Before(self->childend, elem);
	}
}
void FirstRange(Element* self, Element* elem) {
	Element* before = NULL;
	for (; elem->type != _ElemEnd; elem = before) {
		before = elem->before;
		Next(self->childend, elem);
	}
}
void RemoveBefore(Element* self) {
	self->before = self->before->before;
	self->before->next = self;
}
void RemoveNext(Element* self) {
	self->next = self->next->next;
	self->next->before = self;
}
void ElementAdd(ThreadGC* thgc, Element* self , Element* elem) {
	if (self == NULL) return;
	if (elem->single) {
		if (self->childend->before->type == _Line) {
			if (elem->type == _Kaigyou || elem->type == _End) {
				Line* line = (Line*)GC_malloc(thgc, _LineC);
				initLine(thgc, line);
				Before(self->childend, (Element *)line);
			}
			else {
				Before(self->childend->before->childend, elem);
			}
		}
		else if (elem->type != _Kaigyou && elem->type != _End) {
			Line* line = (Line*)GC_malloc(thgc, _LineC);
			initLine(thgc, line);
			Before(self->childend, (Element*)line);
			Before(line->base.childend, elem);
		}
	}
	else Before(self->childend, elem);
}
State* StateClone(ThreadGC* thgc, State* self) {
	State* state = (State*)GC_malloc(thgc, _State);
	state->elements = create_list(thgc, sizeof(Element*), true);
	for(int i = 0; i < self->elements->size; i++) add_list(thgc, state->elements, *get_list(self->elements, i));
	state->histories = create_list(thgc, sizeof(Element*), true);
	state->n = self->n;
	return state;
}
void StateUpdate(State* self) {
	for (int i = 0; i < self->elements->size; i++) {
		Element* elem = (Element*)*get_list(self->elements, i);
		elem->update = true;
		elem->recompile = true;
	}
}
void initState(ThreadGC* thgc, State* self) {
	self->elements = create_list(thgc, sizeof(Element*), true);
	self->histories = create_list(thgc, sizeof(Element*), true);
	self->n = 0;
}
Element* ElementMeasure(ThreadGC* thgc, Element* self, Measure* m, Local* local, int* order) {
	if (self->update) {
		self->pos2.x = m->x + m->px;
		self->pos2.y = m->y + m->py;
		Measure measure;
		measure.x = 0; measure.y = 0; measure.px = 0; measure.py = 0; measure.sizex = 0; measure.sizey = 16; measure.xtype = self->xtype, measure.ytype = self->ytype;
		measure.atlas = m->atlas; measure.g = m->g; measure.window = m->window; measure.state = m->state; measure.Recompile = m->Recompile;
		if (self->atlas != NULL) measure.atlas = self->atlas;
		for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
			elem->Measure(thgc, elem, &measure, local, order);
		}
		self->size2.x = measure.sizex;
		self->size2.y = measure.sizey;
		self->update = false;
	}
	return NULL;
}
void ElementCheck(ThreadGC* thgc, char* self) {
	Element* elem = (Element*)self;
	elem->before = (Element*)copy_object(thgc, (char*)elem->before);
	elem->next = (Element*)copy_object(thgc, (char*)elem->next);
	elem->parent = (Element*)copy_object(thgc, (char*)elem->parent);
	elem->childend = (Element*)copy_object(thgc, (char*)elem->childend);
	elem->groups = (List*)copy_object(thgc, (char*)elem->groups);
}
void ElementDraw(ThreadGC* thgc, Element* self, Graphic*g, Local* local, bool* select, RenderCommandQueue* q) {
	float px = g->px;
	float py = g->py;
	Graphic graphic;
	graphic.g = g->g; graphic.atlas = g->atlas; graphic.x = g->px; graphic.y = g->py; graphic.px = 0; graphic.py = 0;
	if (self->xtype == Auto) {}//no layer
	else if (self->xtype == Break) {}//no layer
	else if (self->xtype == Scroll) {}//layer
	else if (self->xtype == Limit) {}//layer
	if (self->ytype == Auto) {}//no layer
	else if (self->ytype == Break || self->ytype == Limit) {} //layer
	else if (self->ytype == Scroll) {}//layer
	if (self->background == Alpha) {}// layer
	else if (self->background == Image) {}//layer
	else if (self->background == Shader) {}//layer
	if (self->borderradius) {}//layer
	else if (self->shadow) {}//layer
	else if (self->resized) {}//if layer init
	else if (chldrencmds > 20) {//layer
		if (dirtyarerate < 0.5f) {} //layer
	}

	//maker 9-slice �p�� or background
	//make ui
	//make shadow
	if (self->atlas != NULL) graphic.atlas = self->atlas;
	for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
		elem->Draw(thgc, elem, &graphic, local, select, q);
	}

}
int ElementMouse(ThreadGC* thgc, Element* self, MouseEvent* e, Local* local) {
	if (self->GoMouseDown != NULL) {
		MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->GoMouseDown->obj);
		MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
		res->table = (Map*)create_mapy(thgc, true);
		MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
		mo->req = NULL; mo->res = res;
		auto rn = GC_AddRoot(thgc);
		auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoMouseDown->func, (char*)mo)));
		h.promise().state = -2;
		thgc->queue->push(h);
	}
	add_list(thgc, e->state->elements, (char*)self);
	if (e->y < self->childend->next->pos2.y) {
		Select* select = (Select*)GC_malloc(thgc, _Select);
		select->state = StateClone(thgc, e->state);
		Element* elem = self->childend->next;
	head:
		add_list(thgc, select->state->elements, (char*)elem);
		if (elem->type == _CloneElem) {
			CloneElement* clone = (CloneElement*)elem;
			elem = clone->childstart;
			goto head;
		}
		else if (!elem->single) {
			elem = elem->childend->next;
			goto head;
		}
		select->n = 0;
		local->selects[1] = select;
		if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN) local->selects[0] = local->selects[1];
		if (self->BackMouseDown != NULL) {
			MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackMouseDown->obj);
			MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
			res->table = (Map*)create_mapy(thgc, true);
			MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
			mo->req = NULL; mo->res = res;
			auto rn = GC_AddRoot(thgc);
			auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackMouseDown->func, (char*)mo)));
			h.promise().state = -2;
			thgc->queue->push(h);
		}
		return -1;
	}
	for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
		if (elem->pos2.y <= e->y && e->y < elem->pos2.y + elem->size2.y) {
			int n = elem->Mouse(thgc, elem, e, local);
			if (self->BackMouseDown != NULL) {
				MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackMouseDown->obj);
				MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
				res->table = (Map*)create_mapy(thgc, true);
				MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
				mo->req = NULL; mo->res = res;
				auto rn = GC_AddRoot(thgc);
				auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackMouseDown->func, (char*)mo)));
				h.promise().state = -2;
				thgc->queue->push(h);
			}
			if (n == -2) return -2;
			return -1;
		}
	}
	Select* select0 = (Select*)GC_malloc(thgc, _Select);
	select0->state = StateClone(thgc, e->state);
	Element* elem2 = self->childend->before;
head2:
	add_list(thgc, select0->state->elements, (char*)elem2);
	if (elem2->type == _CloneElem) {
		elem2 = elem2->childend;
		goto head2;
	}
	else if (elem2->type == _ElemEnd) {}
	else if (!elem2->single) {
		elem2 = elem2->childend->before;
		goto head2;
	}
	select0->n = 1;
	local->selects[1] = select0;
	if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN) local->selects[0] = local->selects[1];
	e->state->elements->size--;
	if (self->BackMouseDown != NULL) {
		MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackMouseDown->obj);
		MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
		res->table = (Map*)create_mapy(thgc, true);
		MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
		mo->req = NULL; mo->res = res;
		auto rn = GC_AddRoot(thgc);
		auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackMouseDown->func, (char*)mo)));
		h.promise().state = -2;
		thgc->queue->push(h);
	}
	return -1;
}
int ElementKey(ThreadGC* thgc, Element* self, KeyEvent* e, Local* local, bool* select) {
	bool ok = false;
	if ((Element*)*get_list(local->selects[0]->state->elements, local->selects[0]->state->n) == self) {
		local->selects[0]->state->n++;
		ok = true;
	}
	if ((Element*)*get_list(local->selects[1]->state->elements, local->selects[1]->state->n) == self) {
		local->selects[1]->state->n++;
		ok = true;
	}
	if (ok) {
		if (self->GoKeyDown != NULL) {
			MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->GoKeyDown->obj);
			MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
			res->table = (Map*)create_mapy(thgc, true);
			MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
			mo->req = NULL; mo->res = res;
			auto rn = GC_AddRoot(thgc);
			auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoKeyDown->func, (char*)mo)));
			h.promise().state = -2;
			thgc->queue->push(h);
		}
		add_list(thgc, e->state->elements, (char*)self);
		for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
			int n = elem->Key(thgc, elem, e, local, select);
			if (local->seln == 2) return -1;
		}
		if (self->BackKeyDown != NULL) {
			MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackKeyDown->obj);
			MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
			res->table = (Map*)create_mapy(thgc, true);
			MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
			mo->req = NULL; mo->res = res;
			auto rn = GC_AddRoot(thgc);
			auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackKeyDown->func, (char*)mo)));
			h.promise().state = -2;
			thgc->queue->push(h);
		}
		e->state->elements->size--;
	}
	return -1;
}
int OneCount(Element* self) {
	return 1;
}
void initElement(ThreadGC* thgc, Element* self) {
	self->next = self->before = self;
	self->parent = NULL;
	EndElement* end = (EndElement*)GC_malloc(thgc, _EndC);
	initEndElement(thgc, end, self);
	self->childend = (Element*)end;
	self->margins[0] = self->margins[1] = self->margins[2] = self->margins[3] = 0;
	self->paddings[0] = self->paddings[1] = self->paddings[2] = self->paddings[3] = 0;
	self->pos.x = self->pos.y = 0;
	self->scroll.x = self->scroll.y = 0;
	self->size.x = self->size.y = 0;
	self->size2.x = self->size2.y = 0;
	self->xtype = self->ytype = Auto;
	self->position = Relative;
	self->layout = ocupay;
	self->selectable = true;
	self->atlas = NULL;
	self->single = false;
	self->update = self->recompile = true;
	self->color.r = self->color.g = self->color.b = self->color.a = 255;
	self->background.r = self->background.g = self->background.b = self->background.a = 255;
	self->type = (LetterType)0;
	self->align = Left;
	self->index = 0;
	self->groups = create_list(thgc, sizeof(Element*), true);
	self->Measure = ElementMeasure;
	self->Draw = ElementDraw;
	self->Mouse = ElementMouse;
	self->Key = ElementKey;
}
void initElement2(ThreadGC* thgc, Element* self, Element* parent) {
	self->next = self->before = self;
	self->parent = parent;
	self->childend = NULL;
	self->margins[0] = self->margins[1] = self->margins[2] = self->margins[3] = 0;
	self->paddings[0] = self->paddings[1] = self->paddings[2] = self->paddings[3] = 0;
	self->pos.x = self->pos.y = 0;
	self->scroll.x = self->scroll.y = 0;
	self->size.x = self->size.y = 0;
	self->size2.x = self->size2.y = 0;
	self->xtype = self->ytype = Auto;
	self->position = Relative;
	self->layout = ocupay;
	self->selectable = true;
	self->atlas = NULL;
	self->single = false;
	self->update = self->recompile = false;
	self->color.r = self->color.g = self->color.b = self->color.a = 255;
	self->background.r = self->background.g = self->background.b = self->background.a = 255;
	self->type = (LetterType)0;
	self->align = Left;
	self->index = 0;
	self->groups = create_list(thgc, sizeof(Element*), true);
	self->Measure = ElementMeasure;
	self->Draw = ElementDraw;
	self->Mouse = ElementMouse;
	self->Key = ElementKey;
}
void initEndElement(ThreadGC* thgc, EndElement* self, Element* parent) {
	initElement2(thgc, (Element*)self, parent);
	self->base.type = _ElemEnd;
	self->base.Count = OneCount;
}
void EndCheck(ThreadGC* thgc, char* self) {
	EndElement* elem = (EndElement*)self;
	elem->base.before = (Element*)copy_object(thgc, (char*)elem->base.before);
	elem->base.next = (Element*)copy_object(thgc, (char*)elem->base.next);
	elem->base.parent = (Element*)copy_object(thgc, (char*)elem->base.parent);
	elem->base.childend = (Element*)copy_object(thgc, (char*)elem->base.childend);
	elem->base.groups = (List*)copy_object(thgc, (char*)elem->base.groups);
}
void LineRecompile(ThreadGC* thgc, Element* self, Local* local) {
	String* str = (String*)GC_malloc(thgc, _String);
	str->size = 0;
	str->esize = 1;
	GlyphAtlas* atlas = self->childend->next->atlas;
	int n = 0;
	int n1 = -1, n2 = -1;
	for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
		if (((Object*)((char*)elem - sizeof(Object)))->type == _LetterC) {
			str = StringAdd2(thgc, str, ((Letter*)elem)->value->text);
		}
		if ((Element*)*get_last(local->selects[0]->state->elements) == elem) {
			n1 = n + local->selects[0]->n;
		}
		if ((Element*)*get_last(local->selects[1]->state->elements) == elem) {
			n2 = n + local->selects[1]->n;
		}
		n += elem->Count(elem);
	}
	self->childend->next = self->childend->before = self->childend;
	List* list = Compile(thgc, str, atlas);
	for (int i = 0; i < list->size; i++) {
		List* list1 = (List*)*get_list(list, i);
		if (list1->size == 0) continue;
		if (i >= 1) {
			Line* line = (Line*)GC_malloc(thgc, _LineC);
			initLine(thgc, line);
			Next(self, (Element*)line);
			self = (Element*)line;
		}
		for (int j = 0; j < list1->size; j++) {
			Letter* letter = (Letter*)*get_list(list1, j);
			Before(self->childend, (Element*)letter);
		}
	}
	n = 0;
	if (n1 != -1) {
		for (Element* elem = self->childend->next; ; elem = elem->next) {
			if (elem->type == _ElemEnd) {
				set_last(local->selects[0]->state->elements, (char*)elem->before);
				local->selects[0]->n = 0;
				break;
			}
			int n3 = n + elem->Count(elem);
			if (n3 >= n1) {
				if (elem->type == _Kaigyou || elem->type == _End) {
					set_last(local->selects[0]->state->elements, (char*)elem);
					local->selects[0]->n = 0;
					break;
				}
				set_last(local->selects[0]->state->elements, (char*)elem);
				local->selects[0]->n = n1 - n;
				break;
			}
			n = n3;
		}
	}
	n = 0;
	if (n2 != -1) {
		for (Element* elem = self->childend->next; ; elem = elem->next) {
			if (elem->type == _ElemEnd) {
				set_last(local->selects[0]->state->elements, (char*)elem->before);
				local->selects[0]->n = 0;
				break;
			}
			int n3 = n + elem->Count(elem);
			if (n3 >= n2) {
				if (elem->type == _Kaigyou || elem->type == _End) {
					set_last(local->selects[0]->state->elements, (char*)elem);
					local->selects[0]->n = 0;
					break;
				}
				set_last(local->selects[1]->state->elements, (char*)elem);
				local->selects[1]->n = n2 - n;
				break;
			}
			n = n3;
		}
	}
}
Element* LineMeasure(ThreadGC* thgc, Element* self, Measure* m, Local* local, int* order) {
	Line* line = (Line*)self;
	if (self->update) {
		if (self->recompile) {
			LineRecompile(thgc, self, local);
		}
		line->vline = NULL;
		VLine* vl = NULL;
		Element* childstart = self->childend->next;
		int n = 0;
		bool check = true;
		for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
			Element* elem2 = elem;
		head:
			elem2 = elem2->Measure(thgc, elem, m, local, order);
			if (elem2 != NULL) {
				//Range* range = (Range*)elem2;
				VLine* vline = (VLine*)GC_malloc(thgc, _VLineC);
				vline->base.size2.x = m->px;
				vline->base.size2.y = m->h;
				m->px = 0;
				m->py += m->h;
				m->h = 0;
				initVLine(thgc, vline);
				if (check) {
					//vline->childstart = vline->childfinish = range;
				}
				else if (n == 0) {
					vline->childstart = childstart;
					//vline->childfinish = range;
				}
				else {
					//Range* range1 = vline->childstart = GC_malloc(sizeof(Range));
					// initRange(range1);
					//range1.childend = childstart; range1.n1 = n; range1.n2 = childstart.Count(); range1->base.next = elem->next;
					//vline->childstart = range1;
					//vline->childfinish = range;
				}
				vline->base.childend = elem;
				childstart = elem;
				//elem2 = range;
				if (vl == NULL) line->vline = vl = vline;
				else {
					Next((Element*)vl, (Element*)vline);
					vl = vline;
				}
				check = true;
				goto head;
			}
			check = false;
		}
		if (vl != NULL) {
			VLine* vline = (VLine*)GC_malloc(thgc, _VLineC);
			initVLine(thgc, vline);
			vline->childstart = childstart;
			vline->nstart = n;
			vline->base.childend = self->childend;
			vline->nend = 0;
			Next((Element*)vl, (Element*)vline);
		}
		self->size2.y = m->h;
		self->size2.x = m->px;
		m->py += m->h;
		m->px = m->x;
		m->h = 0;
		self->update = false;
	}
	else {
		m->py += self->size2.y;
	}
	return NULL;
}
void LineDraw(ThreadGC* thgc, Element* self, Graphic* g, Local* local, bool* select, RenderCommandQueue* q) {
	Line* line = (Line*)self;
	if (line->vline == NULL) {
		for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
			elem->Draw(thgc, elem, g, local, select, q);
		}
	}
	else {
		for (Element* elem = (Element*)line->vline; elem->type != _ElemEnd; elem = elem->next) {
			elem->Draw(thgc, elem, g, local, select, q);
		}
	}
	g->py += self->size2.y;
}
int LineMouse(ThreadGC* thgc, Element* self, MouseEvent* e, Local* local) {
	if (self->GoMouseDown != NULL) {
		MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->GoMouseDown->obj);
		MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
		res->table = (Map*)create_mapy(thgc, true);
		MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
		mo->req = NULL; mo->res = res;
		auto rn = GC_AddRoot(thgc);
		auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoMouseDown->func, (char*)mo)));
		h.promise().state = -2;
		thgc->queue->push(h);
	}
	Line* line = (Line*)self;
	add_list(thgc, e->state->elements, (char*)self);
	if (line->vline == NULL) {
		if (e->x < self->childend->next->pos2.x && (e->action != SDL_EVENT_MOUSE_MOTION || e->click)) {
			Select* select = (Select*)GC_malloc(thgc, _Select);
			State* s0 = StateClone(thgc, e->state);
			add_list(thgc, s0->elements, (char*)self->childend->next);
			select->state = s0;
			select->n = 0;
			local->selects[1] = select;
			if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN) local->selects[0] = local->selects[1];
			local->window->invalidate = true;

		}
		for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
			int n = elem->Mouse(thgc, elem, e, local);
			if (n == -2) return -2;
			if (n >= 0 && (e->action != SDL_EVENT_MOUSE_MOTION || e->click)) {
				Select* select = (Select*)GC_malloc(thgc, _Select);
				State* state = StateClone(thgc, e->state);
				add_list(thgc, state->elements, (char*)elem);
				select->state = state;
				select->n = n;
				local->selects[1] = select;
				if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN)local->selects[0] = local->selects[1];
				local->window->invalidate = true;
				if (self->BackMouseDown != NULL) {
					MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackMouseDown->obj);
					MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
					res->table = (Map*)create_mapy(thgc, true);
					MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
					mo->req = NULL; mo->res = res;
					auto rn = GC_AddRoot(thgc);
					auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackMouseDown->func, (char*)mo)));
					h.promise().state = -2;
					thgc->queue->push(h);
				}
				return -1;
			}
		}
		if (e->action != SDL_EVENT_MOUSE_MOTION || e->click) {
			Select* select0 = (Select*)GC_malloc(thgc, _Select);
			State* s1 = StateClone(thgc, e->state);
			add_list(thgc, s1->elements, (char*)self->childend->before);
			select0->state = s1;
			select0->n = 0;
			local->selects[1] = select0;
			if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN) local->selects[0] = local->selects[1];
			local->window->invalidate = true;
		}
		if (self->BackMouseDown != NULL) {
			MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackMouseDown->obj);
			MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
			res->table = (Map*)create_mapy(thgc, true);
			MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
			mo->req = NULL; mo->res = res;
			auto rn = GC_AddRoot(thgc);
			auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackMouseDown->func, (char*)mo)));
			h.promise().state = -2;
			thgc->queue->push(h);
		}
	}
	else {
		for (Element* elem = (Element*)line->vline; elem->type != _ElemEnd; elem = elem->next) {
			int n = elem->Mouse(thgc, elem, e, local);
		}
	}
	e->state->elements->size--;
	return -1;
}
int LineKey(ThreadGC* thgc, Element* self, KeyEvent* e, Local* local, bool* select) {
	bool ok = false;

	if ((Element*)*get_list(local->selects[0]->state->elements, local->selects[0]->state->n) == self) {
		local->selects[0]->state->n++;
		ok = true;
	}
	if ((Element*)*get_list(local->selects[1]->state->elements, local->selects[1]->state->n) == self) {
		local->selects[1]->state->n++;
		ok = true;
	}
	if (ok) {
		if (self->GoKeyDown != NULL) {
			MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->GoKeyDown->obj);
			MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
			res->table = (Map*)create_mapy(thgc, true);
			MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
			mo->req = NULL; mo->res = res;
			auto rn = GC_AddRoot(thgc);
			auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoKeyDown->func, (char*)mo)));
			h.promise().state = -2;
			thgc->queue->push(h);
		}
		add_list(thgc, e->state->elements, (char*)self);
		for (Element* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
			add_list(thgc, e->state->elements, (char*)elem);
			int n = elem->Key(thgc, elem, e, local, select);
			e->state->elements->size--;
			if (local->seln == 2) return -1;
		}
		if (self->BackKeyDown != NULL) {
			MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackKeyDown->obj);
			MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
			res->table = (Map*)create_mapy(thgc, true);
			MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
			mo->req = NULL; mo->res = res;
			auto rn = GC_AddRoot(thgc);
			auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackKeyDown->func, (char*)mo)));
			h.promise().state = -2;
			thgc->queue->push(h);
		}
		e->state->elements->size--;
	}
	return -1;
}
void initLine(ThreadGC* thgc, Line* self) {
	initElement(thgc, (Element*)self);
	self->base.type = _Line;
	self->base.Count = OneCount;
	self->base.Measure = LineMeasure;
	self->base.Draw = LineDraw;
	self->base.Key = LineKey;
	self->base.Mouse = LineMouse;
}
void LineCheck(ThreadGC* thgc, char* self) {
	Line* line = (Line*)self;
	line->base.before = (Element*)copy_object(thgc, (char*)line->base.before);
	line->base.next = (Element*)copy_object(thgc, (char*)line->base.next);
	line->base.parent = (Element*)copy_object(thgc, (char*)line->base.parent);
	line->base.childend = (Element*)copy_object(thgc, (char*)line->base.childend);
	line->base.groups = (List*)copy_object(thgc, (char*)line->base.groups);
	line->vline = (VLine*)copy_object(thgc, (char*)line->vline);
}
void VLineDraw(ThreadGC* thgc, Element* self, Graphic* g, Local* local, bool* select, RenderCommandQueue* q) {
	VLine* vline = (VLine*)self;
	vline->childstart->Draw(thgc, vline->childstart, g, local, select, q);
	if (vline->childstart == vline->base.childend) {
		return;
	}
	for (Element* elem = vline->childstart->next; ; elem = elem->next) {
		if (elem == self->childend) {
			//vline->childfinish->Draw(vline->childfinish, g, local, select);
			break;
		}
		elem->Draw(thgc, elem, g, local, select, q);
	}
}
int VLineMouse(ThreadGC* thgc, Element* self, MouseEvent* e, Local* local) {
	VLine* vline = (VLine*)self;
	vline->childstart->Mouse(thgc, vline->childstart, e, local);
	if (vline->childstart == vline->base.childend) {
		return -1;
	}
	for (Element* elem = vline->childstart->next; ; elem = elem->next) {
		if (elem == self->childend) {
			//vline->childfinish->Mouse(vline->childfinish, e, local);
			break;
		}
		elem->Mouse(thgc, elem, e, local);
	}
	return -1;
}
void initVLine(ThreadGC* thgc, VLine* self) {
	initElement(thgc, (Element*)self);
	self->base.Count = OneCount;
}
void VLineCheck(ThreadGC* thgc, char* self) {
	VLine* vline = (VLine*)self;
	vline->base.before = (Element*)copy_object(thgc, (char*)vline->base.before);
	vline->base.next = (Element*)copy_object(thgc, (char*)vline->base.next);
	vline->base.parent = (Element*)copy_object(thgc, (char*)vline->base.parent);
	vline->base.childend = (Element*)copy_object(thgc, (char*)vline->base.childend);
	vline->base.groups = (List*)copy_object(thgc, (char*)vline->base.groups);
	vline->childstart = (Element*)copy_object(thgc, (char*)vline->childstart);
	vline->childfinish = (Element*)copy_object(thgc, (char*)vline->childfinish);
}
Element* LetterMeasure(ThreadGC* thgc, Element* self, Measure* m, Local* local, int* order) {
	Letter* letter = (Letter*)self;
	LetterPart* let = letter->value;
	if (self->atlas != NULL) m->atlas = self->atlas;
	size_t n = -1;
	if (let->text->size == 0) {
		self->size2.x = 0;
	}
	else {
		int hei;
		////MeasureString(*m->atlas, let->text, let->text->size / let->text->esize, 1000, &self->size2.x, &hei, &n);
		if (n < let->text->size) {
			//Range* range = (Range*)GC_malloc(sizeof(Range));
			//initRange(range);
			//range->n1 = 0;
			//range->n2 = let->n;
			//range->base.childend = self;
			//return range;
		}
	}
	self->size2.y = TTF_GetFontHeight(getFont(m->atlas));
	self->pos2.x = m->x + m->px + self->pos.x;
	self->pos2.y = self->pos.y;
	if (m->h < self->size2.y) m->h = self->size2.y;
	m->px += self->size2.x;
	return NULL;
}
void LetterDraw(ThreadGC* thgc, Element* self, Graphic* g, Local* local, bool* select, RenderCommandQueue* q) {
	Letter* letter = (Letter*)self;
	LetterPart* let = letter->value;
	if (self->atlas != NULL) g->atlas = self->atlas;
	if (*select) {
		Select* sel2 = local->selects[(local->seln + 1) % 2];
		SDL_FRect rect;
		rect.x = self->pos2.x + self->pos.x; rect.y = self->pos2.y + self->pos.y; rect.h = self->size2.y;
		if ((Element*)*get_last(sel2->state->elements) == self) {
			float width;
			size_t n = -1;
			float hei;
			MeasureString(*g->atlas, let->text, sel2->n, 1000, &width, &hei, &n);
			*select = false;
			rect.w = width;
		}
		else {
			rect.w = self->size2.x;
		}
		//drawRect(thgc->commandQueue, *g->atlas, rect.x, rect.y, rect.w, rect.h, 0, 0xFFCCCCCC);
	}
	else {
		for (int i = 0; i < 2; i++) {
			Element* el = *(Element**)get_last(local->selects[i]->state->elements);
			if (*((Element**)get_last(local->selects[i]->state->elements)) == self) {
				// �l�p�`�̒�`
				SDL_FRect rect;
				rect.y = g->py + self->pos2.y; rect.h = self->size2.y;
				if ((Element*)*get_last(local->selects[(i + 1) % 2]->state->elements) == self) {
					int n1 = local->selects[i]->n, n2 = local->selects[(i + 1) % 2]->n;
					if (n1 > n2) {
						int ins = n1;
						n1 = n2;
						n2 = ins;
					}
					float width1;
					float hei;
					size_t n = -1;
					if (n1 == 0) width1 = 0;
					else MeasureString(*g->atlas, let->text, n1, 1000, &width1, &hei, &n);
					float width2;
					if (n2 == 0) width2 = 0;
					else MeasureString(*g->atlas, let->text, n2, 1000, &width2, &hei, &n);
					rect.x = self->pos2.x + width1;
					rect.w = width2 - width1 + 2;
				}
				else {
					float width1;
					float hei;
					size_t n;
					MeasureString(*g->atlas, let->text, local->selects[i]->n, 1000, &width1, &hei, &n);
					rect.x = self->pos2.x + width1;
					rect.w = self->size2.x - width1;
					local->seln = i;
					*select = true;
				}
				//drawRect(thgc->commandQueue, *g->atlas, rect.x, rect.y, rect.w, rect.h, 0, 0xFFCCCCCC);
				break;
			}
		}
	}
	if (let->text->size != 0) {
		SDL_FRect rect;
		rect.x = self->pos2.x; rect.y = g->py + self->pos2.y; rect.w = self->size2.x; rect.h = TTF_GetFontHeight(getFont(g->atlas));
		//drawString(thgc->commandQueue, *g->atlas, letter->value->text, rect.x, rect.y, 0, 0xFF000000);
		g->px += self->size2.x;
	}
	let->atlas = g->atlas;
}
int LetterMouse(ThreadGC* thgc, Element* self, MouseEvent* e, Local* local) {
	Letter* letter = (Letter*)self;
	if (self->pos2.x + self->pos.x <= e->x && e->x < self->pos2.x + self->pos.x + self->size2.x) {
		if (self->pos2.y + self->pos.y <= e->y && e->y < self->pos2.y + self->pos.y + self->size2.y) {
			if (self->GoMouseDown != NULL) {
				MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->GoMouseDown->obj);
				MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
				res->table = (Map*)create_mapy(thgc, true);
				MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
				mo->req = NULL; mo->res = res;
				auto rn = GC_AddRoot(thgc);
				auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoMouseDown->func, (char*)mo)));
				h.promise().state = -2;
				thgc->queue->push(h);
			}
			float width;
			LetterPart* let = letter->value;
			size_t n;
			float hei;
			if (letter->value->atlas == NULL) return -2;
			MeasureString(*letter->value->atlas, let->text, let->text->size / let->text->esize, e->x - self->pos2.x - self->pos.x, &width, &hei, &n);

			if (self->BackMouseDown != NULL) {
				MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackMouseDown->obj);
				MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
				res->table = (Map*)create_mapy(thgc, true);
				MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
				mo->req = NULL; mo->res = res;
				auto rn = GC_AddRoot(thgc);
				auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackMouseDown->func, (char*)mo)));
				h.promise().state = -2;
				thgc->queue->push(h);
			}
			return n;
		}
	}
	return -1;
}
Element* Counting(Element* line, int* n) {
	Element* elem;
	for (Element* elem = line->childend->next; ; elem = elem->next) {
		int n2 = *n - elem->Count(elem);
		if (elem->type == _ElemEnd) {
			*n = elem->before->Count(elem->before);
			return elem->before;
		}
		if (elem->type == _Kaigyou) {
			*n = 0;
			return elem;
		}
		if (n2 < 0) {
			return elem;
		}
		*n = n2;
	}
}
int LetterKey(ThreadGC* thgc, Element* self, KeyEvent* e, Local* local, bool* select) {
	Letter* letter = (Letter*)self;
	if (*select) {
		Select* sel2 = local->selects[(local->seln + 1) % 2 ];
		if ((Element*)*get_list(sel2->state->elements, sel2->state->n) == self) {
			if (self->BackKeyDown != NULL) {
				MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackKeyDown->obj);
				MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
				res->table = (Map*)create_mapy(thgc, true);
				MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
				mo->req = NULL; mo->res = res;
				auto rn = GC_AddRoot(thgc);
				auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackKeyDown->func, (char*)mo)));
				h.promise().state = -2;
				thgc->queue->push(h);
			}
			State* state = StateClone(thgc, e->state);
			set_last(state->elements, (char*)self);
			Select* sel = (Select*)GC_malloc(thgc, _Select);
			sel->state = state; sel->n = 0;
			local->selects[0] = local->selects[1] = sel;
			((LetterPart*)letter->value)->text->size = sel2->n;
			*select = false;
			set_last(e->state->elements, (char*)self->next);
			return 0;
		}
		else {
			RemoveBefore(self->next);
			set_last(e->state->elements, (char*)self->next);
			return 0;
		}
	}
	else {
		for (int i = 0; i < 2; i++) {
			if ((Element*)*get_list(local->selects[i]->state->elements, local->selects[i]->state->n) == self) {
				if (self->GoKeyDown != NULL) {
					MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->GoKeyDown->obj);
					MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
					res->table = (Map*)create_mapy(thgc, true);
					MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
					mo->req = NULL; mo->res = res;
					auto rn = GC_AddRoot(thgc);
					auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoKeyDown->func, (char*)mo)));
					h.promise().state = -2;
					thgc->queue->push(h);
				}
				//if (self.Key != NULL) key(e, local);
				Select* sel2 = local->selects[(i + 1) % 2];
				if ((Element*)*get_last(sel2->state->elements) == self) {
					if (self->BackKeyDown != NULL) {
						MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackKeyDown->obj);
						MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
						res->table = (Map*)create_mapy(thgc, true);
						MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
						mo->req = NULL; mo->res = res;
						auto rn = GC_AddRoot(thgc);
						auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackKeyDown->func, (char*)mo)));
						h.promise().state = -2;
						thgc->queue->push(h);
					}
					local->seln = 2;
					int seln = i;
					int n1 = local->selects[i]->n, n2 = local->selects[(i + 1) % 2]->n;
					if (n1 > n2) {
						int ins = n1;
						n1 = n2;
						n2 = ins;
						seln = (i + 1) % 2;
					}
					if (e->key == SDLK_KP_ENTER || e->key == SDLK_RETURN) {
						local->window->update = true;
						Line* line = (Line*)GC_malloc(thgc, _LineC);
						initLine(thgc, line);
						Line* parent = *(Line**)get_list(e->state->elements, e->state->elements->size - 2);
						AddRange((Element*)line, self->next);
						Letter* letter2 = (Letter*)GC_malloc(thgc, _LetterC);
						initLetter(thgc, letter2, _Letter);
						letter2->base.atlas = letter->base.atlas;
						letter2->value = (LetterPart*)GC_malloc(thgc, _LetterPart);
						letter2->value->text = SubString(thgc, letter->value->text, n2, letter->value->text->size - n2);
						letter->value->text = SubString(thgc,letter->value->text, 0, n1);
						Next(line->base.childend, (Element*)letter2);
						self->next = parent->base.childend;
						parent->base.childend->before = self;
						Letter* kaigyou = (Letter*)GC_malloc(thgc, _LetterC);
						initKaigyou(thgc, kaigyou, _Kaigyou);
						kaigyou->value = (LetterPart*)GC_malloc(thgc, _LetterPart);
						kaigyou->value->text = createString(thgc, (char*)"\n", 1, 1);
						Before(parent->base.childend, (Element*)kaigyou);
						set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)line);
						set_last(local->selects[i]->state->elements, (char*)letter2);
						local->selects[i]->n = 0;
						local->selects[(i + 1) % 2] = local->selects[i];
						Next((Element*)parent, (Element*)line);
						parent->base.recompile = true;
						StateUpdate(e->state);
						set_last(e->state->elements, (char*)self->next);
						return 0;
					}
					else if (e->key == SDLK_BACKSPACE) {
						local->window->update = true;
						if (n1 == n2) {
							if (n1 == 0) {
								Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
								if (line->childend->next == self) {
									Element* line2 = line;
									line2 = line2->before;
									if (line2->type == _ElemEnd) {
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
									else {
										RemoveBefore(line2->childend);
										FirstRange(line, line2->childend->before);
										State* state2 = StateClone(thgc, e->state);
										set_last(state2->elements, (char*)self);
										Select* select = (Select*)GC_malloc(thgc, _Select);
										select->state = state2; select->n = 0;
										local->selects[0] = local->selects[1] = select;
										StateUpdate(e->state);
										RemoveBefore(line2->next);
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
								}
								else {
									if (self->before->type == _Letter) {
										Letter* letter0 = (Letter*)self->before;
										letter0->value->text->size--;
										StateUpdate(e->state);
									}
									else RemoveBefore(self);
									set_last(e->state->elements, (char*)self->next);
									return 0;
								}
							}
							else {
								letter->value->text = StringRemove(thgc, letter->value->text, n1 - 1);
								State* state2 = StateClone(thgc, e->state);
								set_last(state2->elements, (char*)self);
								Select* select = (Select*)GC_malloc(thgc, _Select);
								select->state = state2; select->n = n1 - 1;
								local->selects[0] = local->selects[1] = select;
								StateUpdate(e->state);
								set_last(e->state->elements, (char*)self->next);
								return 0;
							}
						}
					}
					else if (e->key == SDLK_DELETE) {
						local->window->update = true;
						if (n1 == n2) {
							letter->value->text = StringRemove(thgc, letter->value->text, n1);
							StateUpdate(e->state);
							set_last(e->state->elements, (char*)self->next);
							return 0;
						}
					}
					else if (e->key == SDLK_LEFT) {
						local->window->invalidate = true;
						if (n1 == n2) {
							if (n1 == 0) {
								Element* elem = self->before;
								Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
								Element* body = NULL;
								if (local->selects[seln]->state->elements->size >= 3) body = *(Element**)get_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3);
								if (body != NULL && elem->type == _ElemEnd && body->type == _CloneElem) {
									CloneElement* clone = (CloneElement*)body;
									if (line == clone->childstart) {
										Element* elem2 = body->before;
										if (elem2->type == _Line) {
											elem = elem2->childend->before;
											local->selects[seln]->state->elements->size --;
											set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2);
											set_last(local->selects[seln]->state->elements, (char*)elem2->childend->before);
											local->selects[seln]->n = elem->Count(elem) - 1;
											local->selects[(seln + 1) % 2] = local->selects[seln];
											set_last(e->state->elements, (char*)self->next);
											return 0;
										}
										else if (elem2->type == _CloneElem) {
											clone = (CloneElement*)elem2;
											set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3, (char*)clone);
											set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)clone->base.childend);
											set_last(local->selects[seln]->state->elements, (char*)clone->base.childend->childend->before);
											local->selects[seln]->n = 0;
											local->selects[(seln + 1) % 2] = local->selects[seln];
											set_last(e->state->elements, (char*)self->next);
											return 0;
										}
										else {
											set_last(e->state->elements, (char*)self->next);
											return 1;
										}
									}
								}
								if (elem->type == _ElemEnd) {
									line = line->before;
									if (line->type == _ElemEnd) {
										set_last(e->state->elements, (char*)self->next);
										return 1;
									}
									else if (line->type == _CloneElem) {
										Element* elem = line->childend->childend->before;
										set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)line->childend);
										set_last(local->selects[i]->state->elements, (char*)line->childend);
										add_list(thgc, local->selects[i]->state->elements, (char*)elem);
										local->selects[i]->n = elem->Count(elem) - 1;
										local->selects[(i + 1) % 2] = local->selects[seln];
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
									else {
										elem = line->childend->before;
										set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)line);
										set_last(local->selects[i]->state->elements, (char*)line->childend->before);
										local->selects[i]->n = elem->Count(elem) - 1;
										local->selects[(i + 1) % 2] = local->selects[i];
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
								}
								else {
									set_last(local->selects[i]->state->elements, (char*)elem);
									local->selects[i]->n = elem->Count(elem) - 1;
									set_last(e->state->elements, (char*)self->next);
									return 0;
								}
							}
							else {
								local->selects[0]->n--;
								local->selects[1]->n = local->selects[0]->n;
								set_last(e->state->elements, (char*)self->next);
								return 0;
							}
						}
						else {
							local->selects[(seln + 1) % 2] = local->selects[seln];
							set_last(e->state->elements, (char*)self->next);
							return 0;
						}
					}
					else if (e->key == SDLK_RIGHT) {
						local->window->invalidate = true;
						if (n1 == n2) {
							if (n1 == letter->value->text->size) {
								Element* elem = self->next;
								Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
								Element* body = NULL;
								if (local->selects[seln]->state->elements->size >= 3) body = *(Element**)get_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3);
								if (body != NULL && elem->type == _ElemEnd && body->type == _CloneElem) {
									if (line == body->childend) {
										Element* elem2 = body->next;
										if (elem2->type == _Line) {
											local->selects[seln]->state->elements->size --;
											set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2);
											set_last(local->selects[seln]->state->elements, (char*)elem2->childend->next);
											local->selects[seln]->n = 0;
											local->selects[(seln + 1) % 2] = local->selects[seln];
											set_last(e->state->elements, (char*)self->next);
											return 0;
										}
										else if (elem2->type == _CloneElem) {
											CloneElement* clone = (CloneElement*)elem2;
											set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3, (char*)clone);
											set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)clone->childstart);
											set_last(local->selects[seln]->state->elements, (char*)clone->childstart->childend->next);
											local->selects[seln]->n = 0;
											local->selects[(seln + 1) % 2] = local->selects[seln];
											set_last(e->state->elements, (char*)self);
											return 0;
										}
										else {
											set_last(e->state->elements, (char*)self->next);
											return 1;
										}
									}
								}
								line = line->next;
								if (elem->type == _End) {
									return 0;
								}
								else if (elem->type == _Kaigyou || elem->type == _ElemEnd) {
									if (line->type == _Line) {
										local->selects[seln]->state->elements->size--;
										set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)line);
										set_last(e->state->elements, (char*)line->childend->next);
										local->selects[seln]->n = 0;
										local->selects[(seln + 1) % 2] = local->selects[seln];
										return 0;
									}
									else if (line->type == _CloneElem) {
										CloneElement* clone = (CloneElement*)line;
										set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)clone);
										set_last(local->selects[seln]->state->elements, (char*)clone->childstart);
										add_list(thgc, local->selects[seln]->state->elements, (char*)clone->childstart->childend->next);
										local->selects[seln]->n = 0;
										local->selects[(seln + 1) % 2] = local->selects[seln];
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
									else {
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
								}
								else {
									set_last(local->selects[seln]->state->elements, (char*)elem);
									local->selects[seln]->n = 1;
									local->selects[(seln + 1) % 2] = local->selects[seln];
									set_last(e->state->elements, (char*)self->next);
									return 0;
									
								}
							}
							else {
								local->selects[0]->n++;
								local->selects[1]->n = local->selects[0]->n;
								set_last(e->state->elements, (char*)self->next);
								return 0;
							}
						}
						else {
							local->selects[seln] = local->selects[(seln + 1) % 2];
							set_last(e->state->elements, (char*)self->next);
							return 0;
						}
					}
					else if (e->key == SDLK_UP) {
						local->window->invalidate = true;
						if (n1 == n2) {
							Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
							Element* line2 = line->before;
							Element* body = NULL;
							if (local->selects[seln]->state->elements->size >= 3) body = *(Element**)get_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3);
							int n = 0;
							if (local->countn == -1) {
								for (Element* elem = line->childend->next; elem->type != _ElemEnd; elem = elem->next) {
									if (elem == self) {
										n += local->selects[seln]->n;
										break;
									}
									n += elem->Count(elem);
								}
							}
							else n = local->countn;
							if (body != NULL && body->type == _CloneElem) {
								CloneElement* clone = (CloneElement*)body;
								if (line == clone->childstart) {
									Element* elem2 = body->before;
									if (elem2->type == _Line) {
										local->selects[seln]->state->elements->size--;
										set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2);
										Element* elem3 = Counting(elem2, &n);
										local->selects[seln]->n = n;
										set_last(local->selects[seln]->state->elements, (char*)elem3);
										local->selects[(seln + 1) % 2] = local->selects[seln];
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
									else if (elem2->type == _CloneElem) {
										set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3, (char*)elem2);
										set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2->childend);
										Element* elem3 = Counting(elem2->childend, &n);
										local->selects[seln]->n = n;
										set_last(local->selects[seln]->state->elements, (char*)elem3);
										local->selects[(seln + 1) % 2] = local->selects[seln];
										set_last(e->state->elements, (char*)self);
										return 0;
									}
									else {
										set_last(e->state->elements, (char*)self->next);
										return 1;
									}
								}
							}
							if (line2->type == _ElemEnd) {
								set_last(local->selects[seln]->state->elements, (char*)line2->childend->next);
								local->selects[seln]->n = 0;
								local->selects[(seln + 1) % 2] = local->selects[seln];
								set_last(e->state->elements, (char*)self->next);
								return 1;
							}
							set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)line2);
							Element* elem4 = Counting(line2, &n);
							set_last(local->selects[seln]->state->elements, (char*)elem4);
							local->selects[seln]->n = n;
							set_last(e->state->elements, (char*)self->next);
							return 0;
						}
						else {
							local->selects[(seln + 1) % 2] = local->selects[seln];
						}
					}
					else if (e->key == SDLK_DOWN) {
						local->window->invalidate = true;
						if (n1 == n2) {
							Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
							Element* line2 = line->next;
							Element* body = NULL;
							if (local->selects[seln]->state->elements->size >= 3) body = *(Element**)get_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3);
							int n = 0;
							if (local->countn == -1) {
								for (Element* elem = line->childend->next; elem->type != _ElemEnd; elem = elem->next) {
									if (elem == self) {
										n += local->selects[seln]->n;
										break;
									}
									n += elem->Count(elem);
								}
							}
							else n = local->countn;
							if (body != NULL && body->type == _CloneElem) {
								CloneElement* clone = (CloneElement*)body;
								if (line == clone->childstart) {
									Element* elem2 = body->next;
									if (elem2->type == _Line) {
										local->selects[seln]->state->elements->size--;
										set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2);
										Element* elem3 = Counting(elem2, &n);
										local->selects[seln]->n = n;
										set_last(local->selects[seln]->state->elements, (char*)elem3);
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
									else if (elem2->type == _CloneElem) {
										set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3, (char*)elem2);
										set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2->childend);
										Element* elem3 = Counting(elem2->childend, &n);
										local->selects[seln]->n = n;
										set_last(local->selects[seln]->state->elements, (char*)elem3);
										local->selects[(seln + 1) % 2] = local->selects[seln];
										set_last(e->state->elements, (char*)self->next);
										return 0;
									}
									else {
										set_last(e->state->elements, (char*)self->next);
										return 1;
									}
								}
							}
							if (line2->type == _ElemEnd) {
								set_last(local->selects[seln]->state->elements, (char*)line2->childend->before);
								local->selects[seln]->n = 0;
								local->selects[(seln + 1) % 2] = local->selects[seln];
								set_last(e->state->elements, (char*)self->next);
								return 1;
							}
							set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)line2);
							Element* elem4 = Counting(line2, &n);
							set_last(local->selects[seln]->state->elements, (char*)elem4);
							local->selects[seln]->n = n;
							set_last(e->state->elements, (char*)self->next);
							return 1;
						}
						else {
							local->selects[(seln + 1) % 2] = local->selects[seln];
							set_last(e->state->elements, (char*)self->next);
							return 1;
						}
					}
					local->window->update = true;
					letter->value->text = StringAdd2(thgc, StringAdd2(thgc, SubString(thgc, letter->value->text, 0, n1), e->text), SubString(thgc, letter->value->text, n2, letter->value->text->size - n2));
					local->selects[seln]->n = n1 + e->text->size;
					local->selects[(seln + 1) % 2] = local->selects[seln];
					StateUpdate(e->state);
					set_last(e->state->elements, (char*)self->next);
					return 1;
				}
				else {
					if (e->key == SDLK_KP_ENTER || e->key == SDLK_RETURN) {
						local->window->update = true;
						Line* line = (Line*)GC_malloc(thgc, _LineC);
						initLine(thgc, line);
						Line* parent = *(Line**)get_list(e->state->elements, e->state->elements->size - 2);
						AddRange((Element*)line, self->next);
						self->next = parent->base.childend;
						parent->base.childend->before = self;
						letter->value->text = SubString(thgc, letter->value->text, 0, local->selects[i]->n);
						Letter* kaigyou = (Letter*)GC_malloc(thgc, _LetterC);
						initKaigyou(thgc, kaigyou, _Kaigyou);
						String* str = (String*)GC_malloc(thgc, _String);
						str->data = (char*)"\n"; str->size = 1;
						kaigyou->value->text = (String*)GC_malloc(thgc, _String);
						Next(self, (Element*)kaigyou);
						Next((Element*)parent, (Element*)line);
						set_last(e->state->elements, (char*)parent->base.childend);
						StateUpdate(e->state);
						*select = true;
						return 1;
					}
					else if (e->key == SDLK_LEFT || e->key == SDLK_UP) {
						local->window->invalidate = true;
						local->selects[(i + 1) % 2] = local->selects[i];
						local->seln = 2;
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
					else if (e->key == SDLK_RIGHT || e->key == SDLK_DOWN) {
						local->window->invalidate = true;
						local->selects[i] = local->selects[(i + 1) % 2];
						local->seln = 2;
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
					else {
						local->window->update = true;
						letter->value->text = StringAdd2(thgc, SubString(thgc, letter->value->text, 0, local->selects[i]->n), e->text);
						*select = true;
						StateUpdate(e->state);
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
				}
			}
		}
	}
	set_last(e->state->elements, (char*)self->next);
	return -1;
}
void LetterCheck(ThreadGC* thgc, char* self) {
	Letter* letter = (Letter*)self;
	letter->base.before = (Element*)copy_object(thgc, (char*)letter->base.before);
	letter->base.next = (Element*)copy_object(thgc, (char*)letter->base.next);
	letter->base.parent = (Element*)copy_object(thgc, (char*)letter->base.parent);
	letter->base.childend = (Element*)copy_object(thgc, (char*)letter->base.childend);
	letter->base.groups = (List*)copy_object(thgc, (char*)letter->base.groups);
	letter->value = (LetterPart*)copy_object(thgc, (char*)letter->value);
}
int LetterCount(Element* self) {
	Letter* letter = (Letter*)self;
	return letter->value->text->size;
}
void initLetter(ThreadGC* thgc, Letter* self, enum LetterType type) {
	initElement(thgc, (Element*)self);
	self->base.single = true;
	self->base.Count = LetterCount;
	self->base.Measure = LetterMeasure;
	self->base.Draw = LetterDraw;
	self->base.Mouse = LetterMouse;
	self->base.Key = LetterKey;
	self->base.single = true;
	self->base.type = type;
	self->value = (LetterPart*)GC_malloc(thgc, _LetterPart);
}
Element* KaigyouMeasure(ThreadGC* thgc, Element* self, Measure* m, Local* local, int* order) {
	if (self->atlas != NULL) m->atlas = self->atlas;
	self->pos2.x = m->x + m->px; self->pos2.y = 0;
	m->px += 1;
	self->size2.x = 1;
	self->size2.y = TTF_GetFontHeight(getFont(m->atlas));
	if (m->h < self->size2.y) m->h = self->size2.y;
	Letter* let = (Letter*)self;
	let->value->atlas = m->atlas;
	return NULL;
}
void KaigyouDraw(ThreadGC* thgc, Element* self, Graphic* g, Local* local, bool* select, RenderCommandQueue* q) {
	Letter* letter = (Letter*)self;
	if (*select) {
		Select* sel2 = local->selects[(local->seln + 1) % 2];
		if (*(Element**)get_last(sel2->state->elements)== self) *select = false;
	}
	else {
		if (*(Element**)get_last(local->selects[0]->state->elements) == self && *(Element**)get_last(local->selects[1]->state->elements) == self) {
			//drawRect(thgc->commandQueue, *letter->value->atlas, self->pos2.x, self->pos2.y, 2, self->size2.y, 0, 0xFFCCCCCC);
		}
		else if (*(Element**)get_last(local->selects[0]->state->elements) == self || *(Element**)get_last(local->selects[1]->state->elements) == self) {
			*select = true;
			//drawRect(thgc->commandQueue, *letter->value->atlas, self->pos2.x, self->pos2.y, 2, self->size2.y, 0, 0xFFCCCCCC);
		}
	}
}
int KaigyouMouse(ThreadGC* thgc, Element* self, MouseEvent* e, Local* local) {
	return -1;
}
int KaigyouKey(ThreadGC* thgc, Element* self, KeyEvent* e, Local* local, bool* select) {
	Letter* kaigyou = (Letter*)self;
	if (local->seln == 2) {
		set_last(e->state->elements, (char*)self);
		return 1;
	}
	else if (*select) {
		Select* sel2 = local->selects[(local->seln + 1) % 2];
		if (*(Element**)get_list(sel2->state->elements, sel2->n) == self) {
			if (self->BackKeyDown != NULL) {
				MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackKeyDown->obj);
				MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
				res->table = (Map*)create_mapy(thgc, true);
				MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
				mo->req = NULL; mo->res = res;
				auto rn = GC_AddRoot(thgc);
				auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackKeyDown->func, (char*)mo)));
				h.promise().state = -2;
				thgc->queue->push(h);
			}
			local->seln = 2;
			*select = false;
			sel2->n = 0;
			local->selects[local->seln] = sel2;
			set_last(e->state->elements, (char*)self->next);
			return 1;
		}
		else {
			Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
			Element* line2 = line->next;
			if (line2->type == _ElemEnd) {
				set_last(e->state->elements, (char*)self->next);
				return 0;
			}
			RemoveBefore(self->next);
			AddRange(line, line2->childend->next);
			RemoveBefore(line2->next);
			set_last(e->state->elements, (char*)line2->childend->next);
			return 0;
		}
	}
	else {
		for (int i = 0; i < 2; i++) {
			if (*(Element**)get_list(local->selects[i]->state->elements, local->selects[i]->state->n) == self) {
				if (self->GoKeyDown != NULL) {
					MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->GoKeyDown->obj);
					MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
					res->table = (Map*)create_mapy(thgc, true);
					MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
					mo->req = NULL; mo->res = res;
					auto rn = GC_AddRoot(thgc);
					auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoKeyDown->func, (char*)mo)));
					h.promise().state = -2;
					thgc->queue->push(h);
				}
				local->seln = i;
				int seln = i;
				if (*(Element**)get_list(local->selects[(i + 1) % 2]->state->elements, local->selects[(i + 1) % 2]->state->elements->size - 1) == self) {
					if (self->BackKeyDown != NULL) {
						MemObj* mo = (MemObj*)clone_object(thgc, (char*)self->BackKeyDown->obj);
						MemTable* res = (MemTable*)GC_malloc(thgc, _MemTable);
						res->table = (Map*)create_mapy(thgc, true);
						MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
						mo->req = NULL; mo->res = res;
						auto rn = GC_AddRoot(thgc);
						auto h = MakeFrame(rn, self->BackKeyDown->func, (char*)mo);
						thgc->queue->push(std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(h)));
					}
					local->seln = 2;
					if (e->key == SDLK_KP_ENTER || e->key == SDLK_RETURN) {
						local->window->update = true;
						Line* line = (Line*)GC_malloc(thgc, _LineC);
						initLine(thgc, line);
						Letter* kaigyou = (Letter*)GC_malloc(thgc, _LetterC);
						initKaigyou(thgc, kaigyou, _Kaigyou);
						kaigyou->base.atlas = kaigyou->base.atlas;
						kaigyou->value->text = createString(thgc, (char*)"\n", 1, 1);
						Element* line2 = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
						Element* kaigyou2 = line2->childend->before;
						RemoveBefore(line2->childend);
						Next(line->base.childend, (Element*)kaigyou2);
						StateUpdate(e->state);
						set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)line);
						set_last(local->selects[i]->state->elements, (char*)kaigyou2);
						local->selects[(i + 1) % 2] = local->selects[i];
						Before(line2->childend, (Element*)kaigyou);
						Next(line2, (Element*)line);
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
					else if (e->key == SDLK_BACKSPACE) {
						local->window->update = true;
						Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
						if (line->childend->next == self) {
							Element* line2 = line->before;
							if (line2->type == _ElemEnd) {
								set_last(e->state->elements, (char*)self->next);
								return 0;
							}
							else {
								Element* start = line->childend->next;
								RemoveBefore(line2->childend);
								FirstRange(line, line2->childend->before);
								RemoveBefore(line);
								StateUpdate(e->state);
								local->selects[(i + 1) % 2] = local->selects[i];
								set_last(e->state->elements, (char*)self->next);
								return 0;
							}
						}
						else {
							if (((Object*)((char*)self->before - sizeof(Object)))->type == _LetterC) {
								Letter* letter = (Letter*)self->before;
								letter->value->text->size--;
								local->selects[(i + 1) % 2] = local->selects[i];
								StateUpdate(e->state);
								set_last(e->state->elements, (char*)self->next);
								return 0;
							}
						}
						return 1;
					}
					else if (e->key == SDLK_DELETE) {
						local->window->update = true;
						Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
						Element* line2 = line->next;
						if (line2->type == _ElemEnd) {
							set_last(e->state->elements, (char*)self->next);
							return 0;
						}
						RemoveBefore(self->next);
						Element* start = line2->childend->next;
						AddRange(line, line2->childend->next);
						RemoveBefore(line2->next);
						StateUpdate(e->state);
						set_last(local->selects[i]->state->elements, (char*)start);
						local->selects[(i + 1) % 2] = local->selects[i];
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
					else if (e->key == SDLK_LEFT) {
						local->window->invalidate = true;
						Element* elem = *(Element**)get_last(local->selects[i]->state->elements);
						elem = elem->before;
						Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
						Element* body = NULL;
						if (local->selects[i]->state->elements->size >= 3) body = *(Element**)get_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 3);
						if (body != NULL && elem->type == _ElemEnd && body->type == _CloneElem) {
							CloneElement* clone = (CloneElement*)body;
							if (line == clone->childstart) {
								body = body->before;
								if (body->type == _Line) {
									local->selects[i]->state->elements->size--;
									set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)body);
									set_last(local->selects[i]->state->elements, (char*)body->childend->before);
									elem = body->childend->before;
									local->selects[i]->n = elem->Count(elem) - 1;
									local->selects[(i + 1) % 2] = local->selects[i];
									set_last(e->state->elements, (char*)self->next);
									return 1;
								}
								else if (body->type == _CloneElem) {
									set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 3, (char*)body);
									set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)body->childend);
									set_last(local->selects[i]->state->elements, (char*)body->childend->childend->before);
									local->selects[i]->n = 0;
									local->selects[(i + 1) % 2] = local->selects[i];
									set_last(e->state->elements, (char*)self->next);
									return 1;
								}
								else {
									set_last(e->state->elements, (char*)self);
									return 1;
								}
							}
						}
						Element* line2 = line->before;
						if (elem->type == _ElemEnd || elem == line->childend) {
							if (line2->type == _ElemEnd) {
								set_last(e->state->elements, (char*)self);
								return 1;
							}
							else if (line2->type == _CloneElem) {
								elem = line2->childend->childend->before;
								set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)line2);
								set_last(local->selects[i]->state->elements, (char*)line2->childend);
								add_list(thgc, local->selects[i]->state->elements, (char*)elem);
								local->selects[i]->n = elem->Count(elem) - 1;
								local->selects[(i + 1) % 2] = local->selects[i];
								set_last(e->state->elements, (char*)self->next);
								return 1;
							}
						}
						else {
							set_last(local->selects[i]->state->elements, (char*)elem);
							local->selects[i]->n = elem->Count(elem) - 1;
							local->selects[(i + 1) % 2] = local->selects[i];
							set_last(e->state->elements, (char*)self->next);
							return 1;
						}
					}
					else if (e->key == SDLK_RIGHT) {
						local->window->invalidate = true;
						if (self->type == _End) {
							set_last(e->state->elements, (char*)self);
							return 1;
						}
						Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
						Element* body = NULL;
						if (local->selects[i]->state->elements->size >= 3) body = *(Element**)get_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 3);
						if (body != NULL && body->type == _CloneElem) {
							if (line == body->childend) {
								body = body->next;
								if (body->type == _Line) {
									local->selects[i]->state->elements->size--;
									set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)body);
									set_last(local->selects[i]->state->elements, (char*)body->childend->next);
									local->selects[i]->n = 0;
									local->selects[(i + 1) % 2] = local->selects[i];
									set_last(e->state->elements, (char*)self);
									return 1;
								}
								else if (body->type == _CloneElem) {
									CloneElement* clone = (CloneElement*)body;
									set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 3, (char*)clone);
									set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)clone->childstart);
									set_last(local->selects[i]->state->elements, (char*)clone->childstart->childend->next);
									local->selects[i]->n = 0;
									local->selects[(i + 1) % 2] = local->selects[i];
									set_last(e->state->elements, (char*)self);
									return 1;
								}
								else {
									set_last(e->state->elements, (char*)self);
									return 1;
								}
							}
						}
						Element* line2 = line->next;
						if (line2->type == _CloneElem) {
							CloneElement* clone = (CloneElement*)line2;
							Element* elem = clone->childstart->childend->next;
							set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)clone);
							set_last(local->selects[i]->state->elements, (char*)clone->childstart);
							add_list(thgc, local->selects[i]->state->elements, (char*)elem);
							local->selects[i]->n = 0;
							local->selects[(i + 1) % 2] = local->selects[i];
							set_last(e->state->elements, (char*)self);
							return 1;
						}
						else if (line2->type == _ElemEnd) {
							set_last(e->state->elements, (char*)self);
							return 1;
						}
						else {
							set_list(local->selects[i]->state->elements, local->selects[i]->state->elements->size - 2, (char*)line2);
							set_last(local->selects[i]->state->elements, (char*)line2->childend->next);
							local->selects[i]->n = 0;
							local->selects[(i + 1) % 2] = local->selects[i];
							set_last(e->state->elements, (char*)self);
							return 1;
						}
					}
					else if (e->key == SDLK_UP) {
						local->window->invalidate = true;
						Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
						Element* line2 = line->before;
						Element* body = NULL;
						if (local->selects[seln]->state->elements->size >= 3) body = *(Element**)get_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3);
						int n = 0;
						if (local->countn == -1) {
							for (Element* elem = line->childend->next; elem->type != _ElemEnd; elem = elem->next) {
								if (elem == self) {
									n += local->selects[seln]->n;
									break;
								}
								n += elem->Count(elem);
							}
						}
						else n = local->countn;
						if (body != NULL && body->type == _CloneElem) {
							CloneElement* clone = (CloneElement*)body;
							if (line == clone->childstart) {
								Element* elem2 = body->before;
								if (elem2->type == _Line) {
									local->selects[seln]->state->elements->size--;
									set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2);
									Element* elem3 = Counting(elem2, &n);
									local->selects[seln]->n = n;
									set_last(local->selects[seln]->state->elements, (char*)elem3);
									local->selects[(seln + 1) % 2] = local->selects[seln];
									set_last(e->state->elements, (char*)self->next);
									return 0;
								}
								else if (elem2->type == _CloneElem) {
									set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3, (char*)elem2);
									set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2->childend);
									Element* elem3 = Counting(elem2->childend, &n);
									local->selects[seln]->n = n;
									set_last(local->selects[seln]->state->elements, (char*)elem3);
									local->selects[(seln + 1) % 2] = local->selects[seln];
									set_last(e->state->elements, (char*)self);
									return 0;
								}
								else {
									set_last(e->state->elements, (char*)self->next);
									return 1;
								}
							}
						}
						if (line2->type == _ElemEnd) {
							set_last(local->selects[seln]->state->elements, (char*)line2->childend->next);
							local->selects[seln]->n = 0;
							local->selects[(seln + 1) % 2] = local->selects[seln];
							set_last(e->state->elements, (char*)self->next);
							return 1;
						}
						set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)line2);
						Element* elem4 = Counting(line2, &n);
						set_last(local->selects[seln]->state->elements, (char*)elem4);
						local->selects[seln]->n = n;
						set_last(e->state->elements, (char*)self->next);
						return 0;
					}
					else if (e->key == SDLK_DOWN) {
						local->window->invalidate = true;
						Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
						Element* line2 = line->next;
						Element* body = NULL;
						if (local->selects[seln]->state->elements->size >= 3) body = *(Element**)get_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3);
						int n = 0;
						if (local->countn == -1) {
							for (Element* elem = line->childend->next; elem->type != _ElemEnd; elem = elem->next) {
								if (elem == self) {
									n += local->selects[seln]->n;
									break;
								}
								n += elem->Count(elem);
							}
						}
						else n = local->countn;
						if (body != NULL && body->type == _CloneElem) {
							CloneElement* clone = (CloneElement*)body;
							if (line == clone->childstart) {
								Element* elem2 = body->next;
								if (elem2->type == _Line) {
									local->selects[seln]->state->elements->size--;
									set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2);
									Element* elem3 = Counting(elem2, &n);
									local->selects[seln]->n = n;
									set_last(local->selects[seln]->state->elements, (char*)elem3);
									set_last(e->state->elements, (char*)self->next);
									return 0;
								}
								else if (elem2->type == _CloneElem) {
									set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 3, (char*)elem2);
									set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)elem2->childend);
									Element* elem3 = Counting(elem2->childend, &n);
									local->selects[seln]->n = n;
									set_last(local->selects[seln]->state->elements, (char*)elem3);
									local->selects[(seln + 1) % 2] = local->selects[seln];
									set_last(e->state->elements, (char*)self->next);
									return 0;
								}
								else {
									set_last(e->state->elements, (char*)self->next);
									return 1;
								}
							}
						}
						if (line2->type == _ElemEnd) {
							set_last(local->selects[seln]->state->elements, (char*)line->childend->before);
							local->selects[seln]->n = 0;
							local->selects[(seln + 1) % 2] = local->selects[seln];
							set_last(e->state->elements, (char*)self->next);
							return 1;
						}
						set_list(local->selects[seln]->state->elements, local->selects[seln]->state->elements->size - 2, (char*)line2);
						Element* elem4 = Counting(line2, &n);
						set_last(local->selects[seln]->state->elements, (char*)elem4);
						local->selects[seln]->n = n;
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
					else {
						local->window->update = true;
						Letter* letter = (Letter*)GC_malloc(thgc, _LetterC);
						initLetter(thgc, letter, _Letter);
						letter->value->text = e->text;
						letter->base.atlas = kaigyou->base.atlas;
						Before(self, (Element*)letter);
						StateUpdate(e->state);
						local->selects[(i + 1) % 2] = local->selects[i];
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
				}
				else {
					if (e->key == SDLK_LEFT || e->key == SDLK_UP) {
						local->window->invalidate = true;
						local->seln = 2;
						local->selects[(local->seln + 1) % 2] = local->selects[i];
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
					else if (e->key == SDLK_RIGHT || e->key == SDLK_DOWN) {
						local->window->invalidate = true;
						local->seln = 2;
						local->selects[i] = local->selects[(local->seln + 1) % 2];
						set_last(e->state->elements, (char*)self->next);
						return 1;
					}
					else {
						local->window->update = true;
						Element* line = *(Element**)get_list(e->state->elements, e->state->elements->size - 2);
						Element* line2 = line->next;
						if (line2->type == _ElemEnd) {
							set_last(e->state->elements, (char*)self->next);
							return 0;
						}
						set_last(e->state->elements, (char*)self->next);
						RemoveBefore(self->next);
						AddRange(line, line2->childend->next);
						RemoveBefore(line2->next);
						StateUpdate(e->state);
						*select = true;
						return 0;
					}
				}
			}
		}
	}
	set_last( e->state->elements, (char*)self->next);
	return -1;
}*/
#include "addelem.h"
