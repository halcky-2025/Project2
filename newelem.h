#pragma once
#include <map>
#include <string>
#include <vector>
#include <bgfx/bgfx.h>
#include <limits>
enum DrawCommandType {
	DelFBO = -2, FBO = -1, Fill = 0, Gradient = 1, Stripe = 2, Checker = 3, GradientChecker = 4,
	Image = 5, PageCurl = 6, RawImage = 7, End = 8
};
typedef struct NewElement;
typedef struct DrawCommand;
struct DrawCommand;
struct RectDrawCommand;
struct Background;
struct BorderDrawCommand;
struct ShadowOnlyDrawCommand;
struct BorderShadowDrawCommand;
struct ImageDrawCommand;
struct ImageBorderDrawCommand;
struct ImageShadowDrawCommand;
struct ImageBorderShadowDrawCommand;
struct PatternDrawCommand;
struct PatternBorderDrawCommand;
struct PatternShadowDrawCommand;
struct PatternBorderShadowDrawCommand;
struct FBODrawCommand;
struct CopyDrawCommand;
void MeasureString(FontAtlas& atlas, FontId font, String* text, int n, float width, float* retwid, float* rethei, size_t* len, ExtendedRenderGroup* group);
struct LayerInfo {
	~LayerInfo() {
		for (auto* cmd : cmds) {
			delete cmd;
		}
	}

	// �R�s�[���֎~�i�_�u�� delete �h�~�j
	LayerInfo(const LayerInfo&) = delete;
	LayerInfo& operator=(const LayerInfo&) = delete;

	// ���[�u�͋���
	LayerInfo(LayerInfo&& other) noexcept
		: cmds(std::move(other.cmds))
		// ... ���̃����o�[�����[�u ...
	{
	}

	LayerInfo& operator=(LayerInfo&& other) noexcept {
		if (this != &other) {
			// �����̃R�}���h�����
			for (auto* cmd : cmds) {
				delete cmd;
			}
			cmds = std::move(other.cmds);
			// ... ���̃����o�[�����[�u ...
		}
		return *this;
	}

	LayerInfo() = default;
	std::map<std::string, std::vector<float>> defaultUniforms;
	std::map<std::string, bgfx::TextureHandle> defaultTextures;

	float opacity = 1.0f;
	bool visible = true;
	bool needsUpdate = true;
	int width = 800;
	int height = 600;
	std::vector<DrawCommand*> cmds;

	void push(DrawCommand* cmd);
void pushFill(float x, float y, float width, float height,
	float radius, float borderWidth, float aaPixels,
	uint32_t fillColor, uint32_t borderColor,
	float shadowX, float shadowY, float shadowBlur,
	uint32_t shadowColor, float zIndex,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId,
	float blendMode = 0.0f);
void  pushImage(float x, float y, float width, float height,
	float atlasX, float atlasY, float atlasW, float atlasH,
	float radius, float aaPixels, float borderWidth,
	uint32_t borderColor,
	float shadowX, float shadowY, float shadowBlur,
	uint32_t shadowColor, uint32_t modulate,
	float zIndex, bgfx::TextureHandle* tex1,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId,
	float blendMode = 0.0f);
void  pushPattern(enum DrawCommandType patternMode,
	float x, float y, float width, float height,
	float colorCount, float angle,
	float scrollX, float scrollY,
	float radius, float aaPixels,
	float borderWidth, uint32_t borderColor,
	float shadowX, float shadowY, float shadowBlur,
	uint32_t shadowColor,
	int dataOffset, float zIndex,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId,
	float blendMode = 0.0f);
void  pushText(float x, float y, float width, float height,
	float atlasX, float atlasY, float atlasW, float atlasH,
	uint32_t color, float zIndex, bgfx::TextureHandle* tex1,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId,
	float blendMode = 0.0f);
void pushPageCurl(float x, float y, float width, float height,
	float progress, float curlRadius, float curlAngle,
	uint32_t fillColor,
	float uvMinX, float uvMinY, float uvSizeX, float uvSizeY,
	float backUVMinX, float backUVMinY, float borderWidth,
	uint32_t borderColor,
	float shadowX, float shadowY, float shadowBlur,
	uint32_t shadowColor,
	bgfx::TextureHandle* tex1, bgfx::TextureHandle* tex2, float zIndex,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId,
	float blendMode = 0.0f);
void pushRawImage(float x, float y, float width, float height,
	float atlasX, float atlasY, float atlasW, float atlasH,
	uint32_t modulate, float zIndex, bgfx::TextureHandle* tex1,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId,
	float blendMode = 0.0f);
void pushBackground(Background* back, float x, float y, float width, float height,
	float zIndex, bgfx::TextureHandle* tex1, bgfx::TextureHandle* tex2,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId,
	float blendMode);

	LayerInfo& setUniform(const std::string& name, const std::vector<float>& data);
	LayerInfo& setTexture(const std::string& uniformName, bgfx::TextureHandle texture);
	void pushFBO(NewElement* elem, int fboWidth, int fboHeight,
		bgfx::TextureHandle* texture, bgfx::FrameBufferHandle* fbo, PointI* fbsize, bool resized);
	void pushDelFBO(bgfx::FrameBufferHandle fbo);
	void clear();
};
enum BgType {
	BgFill, BgImage, BgGradient, BgStripe, BgCheck, BgGradientChecker
};
struct Background {
	enum DrawCommandType type;
	uint32_t fillcolor;
	ImageId tex1, tex2;
	float aa;
	float curl;
	float borderRadius, borderWidth;
	uint32_t borderColor;
	float shadowX, shadowY, shadowBlur;
	uint32_t shadowColor;
	float count, offset, angle;
	float scrollX, scrollY;
};
struct Offscreen;
struct NewLocal;
struct HoppyWindow;
struct PointF {
	float x;
	float y;
};
struct MouseEvent {
	char* blk;
	int id;
	int x, y;
	Point basepos;
	int action;
	enum MouseCall call;
	HoppyWindow* window;
	bool click = false;
	ExtendedRenderGroup* group;
};
struct Capture {
	MouseEvent* down;
	bool (*Capture)(struct Capture*, MouseEvent*);
};
struct KeyEvent {
	char* blk;
	int id;
	String* text;
	int key;
	Uint8* keys;
	enum KeyCall call;
	bool shift, ctrl, alt;
};
struct NewMeasure {
	NewElement* base;
	PointF pos, size;
	PointF start;
	ExtendedRenderGroup* group;
};
struct NewGraphic {
	LayerInfo* layer;
	NewElement* offscreen;
	NewElement* orient;
	PointF pos, size;
	PointF start, end;
	ImageId im;
	bgfx::FrameBufferHandle* fb;
	PointI* fbsize;
	int viewId = 0;
	ExtendedRenderGroup* group;
};
struct NewEndElement {
	struct NewElement* next, * before, * parent, * childend;
	enum LetterType type;
	int (*Mouse)(ThreadGC*, NewElement*, MouseEvent*, NewLocal*);
	int (*Key)(ThreadGC*, NewElement*, int, int, KeyEvent*, NewLocal*);
	void (*DrawSelection) (ThreadGC*, NewLocal*, NewElement*, int, int, NewGraphic*, RenderCommandQueue* q);
	MemFunc* GoMouseDown; MemFunc* BackMouseDown; MemFunc* GoKeyDown; MemFunc* BackKeyDown;
	int (*len)(NewElement* elem);
	uint64_t id;
};
int len1(NewElement* elem) {
	return 1;
}
struct NewElement;
struct NewLocal;
struct NewElement : NewEndElement {
	float margins[4];
	float mbefores[4];
	float paddings[4];
	float pbefores[4];
	float borderWidth, borderRadius;
	float opacity;
	PointF pos, pos2, global, size, size2, scroll;
	float zIndex;
	enum SizeType xtype, ytype;
	enum Position position;
	enum Layout layout;
	Background* background;
	bool orient;
	void (*Measure)(ThreadGC*, struct NewElement*, NewMeasure*, NewLocal*, int*);
	void (*Draw)(ThreadGC*, struct NewElement*, NewGraphic*, NewLocal*, RenderCommandQueue* q);
	FontId font;
	bool offscreened = false;
	Offscreen* offscreen;
	bool editable;
};
enum DirtyType {
	None,
	Partial = 1,
	OffscreenLayout = 2,
	Rebuild = 4,
	RebuildValue = 7,
};
struct NewSelect {
	NewElement* from, * to;
	uint64_t fromid, toid;
	int m, n;
	NewElement* start, * end;
	int s, e;
	int count;
};
struct NewLocal : NewElement {
	bool resetid;
	HoppyWindow* window;
	DirtyType dirty;
	List* screens;
	Map* temap;
	Map* selects;
	NewSelect select;
};
struct RenderGroup;
struct OffscreenEnd {
	Offscreen* next, * before;
	Offscreen* child, * parent;
	int id;
};
struct Offscreen : OffscreenEnd {
	ImageId imPing;
	ImageId imPong;
	PointI fbsize;
	bool ping = true;
	bool layout = true;
	bool paint = true;
	NewElement* elem;
	void markLayout(NewLocal* local) { layout = true; paint = true; local->dirty = (DirtyType)(local->dirty | DirtyType::Partial); }
	void markPaint(NewLocal* local) { paint = true; local->dirty = (DirtyType)(local->dirty | DirtyType::Partial); }
	int viewId = 0;
	ExtendedRenderGroup *group;
	bool dirty = false;
};
static int viewId = 0;

struct NewLine : NewElement {
};
struct ATSSpan {
	int start, end;
	FontId font;
	uint32_t color;
	String* text;
	LetterType t;
	String* error;
};
struct StyleSpan {
	int start, end;
	FontId font;
	uint32_t color;
};
struct RenderSpan {
	int start, end;
	FontId font;
	uint32_t color;
	float x, y, width, height;
};
struct NewLetter : NewElement {
	String* text;
	uint32_t color;
	List* atsspans;
	List* stylespans;
	List* renderspans;
	bool recompile;
};
void initNewLetter(ThreadGC* thgc, NewLetter* letter, FontId font, enum LetterType type);
void ElementDrawSelect(ThreadGC* thgc, NewLocal* local, NewElement* self, int m, int n, NewGraphic* g, RenderCommandQueue* q);
void LetterDrawSelect(ThreadGC* thgc, NewLocal* local, NewElement* self, int m, int n, NewGraphic* g, RenderCommandQueue* q);
int ElementKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local);
int EndKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local);
int ElementMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local);
int LetterMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local);
int LetterKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local);
void initNewEndElement(ThreadGC* thgc, NewEndElement* end, NewElement* parent) {
	end->next = end->before = (NewElement*)end;
	end->parent = parent;
	end->type = LetterType::_ElemEnd;
	end->id = 0;
	end->len = len1;
	end->Mouse = ElementMouse;
	end->DrawSelection = ElementDrawSelect;
	end->Key = EndKey;
	end->len = len1;
}
void RootResetId(NewElement* elem, int* n) {
	for (NewElement* child = elem->childend->next; child->type != LetterType::_ElemEnd; child = child->next) {
		child->id = (uint64_t)++(*n) * 65536ULL * 65536ULL * 65536ULL;
		if (child->childend != NULL) RootResetId(child, n);
	}
	elem->childend->id = (uint64_t)++(*n) * 65536ULL * 65536ULL * 65536ULL;
}
void ResetId(NewLocal* local) {
	int n = 0;
	local->resetid = false;
	RootResetId(local, &n);
}
void MarkResetId(NewLocal* local) {
	local->resetid = true;
}
void SetChildIds(NewElement* elem) {

	uint64_t startId = elem->id;
	uint64_t endId = elem->childend->id;

	// �q�v�f�̐��𐔂���
	int childCount = 0;
	for (auto el = elem->childend->next; el->type != LetterType::_ElemEnd; el = el->next) {
		childCount++;
		if (el->childend != NULL) {
			childCount++;  // childend����
		}
	}

	if (childCount == 0) return;

	// �e�q�v�f��ID�����蓖��
	uint64_t step = (endId - startId) / (childCount + 1);
	uint64_t currentId = startId;

	for (auto el = elem->childend->next; el->type != LetterType::_ElemEnd; el = el->next) {
		currentId += step;
		el->id = currentId;

		if (el->childend != NULL) {
			currentId += step;
			el->childend->id = currentId;
			SetChildIds(el);  // �ċA
		}
	}
}
bool CheckOffscreen(NewElement* elem) {
	if (elem->offscreen || elem->xtype == SizeType::Page || elem->xtype == SizeType::Scroll || elem->ytype == SizeType::Page || elem->ytype == SizeType::Scroll) {
		return true;
	}
	return false;

}
//elem->id
//elem->child-id
//elem->childend->id
//next->id
void NewBefore(ThreadGC* thgc, NewLocal* local, NewElement* self, NewElement* elem) {
	NewElement* before;
	if (self->before->type == LetterType::_ElemEnd) {
		before = self->parent;
	}
	else {
		before = self->before;
		if (before->childend != NULL) before = before->childend;
	}
	elem->before->next = elem->next;
	elem->next->before = elem->before;
	elem->next = self;
	elem->before = self->before;
	self->before->next = elem;
	self->before = elem;
	elem->parent = self->parent;
	if (elem->childend != NULL) {
		if (self->id - before->id <= 2) MarkResetId(local);
		else {
			elem->id = self->id - (self->id - before->id) * 2 / 3;
			elem->childend->id = self->id - (self ->id - before->id) / 3;
		}
		SetChildIds(elem);
	}
	else {
		if (self->id - before->id <= 1) MarkResetId(local);
		else elem->id = (self->id + before->id) / 2;
	}
}
void NewNext(ThreadGC* thgc, NewLocal* local, NewElement* self, NewElement* elem) {
	NewElement* next = self->next;
	NewElement* before;
	if (self->type == LetterType::_ElemEnd) {
		before = self->parent;
	}
	else {
		before = self;
		if (next->childend != NULL) next = next->childend->next;
	}
	elem->before->next = elem->next;
	elem->next->before = elem->before;
	elem->before = self;
	elem->next = self->next;
	self->next->before = elem;
	self->next = elem;
	elem->parent = self->parent;
	if (elem->childend != NULL) {
		if (next->id - before->id <= 2) MarkResetId(local);
		else {
			elem->id = before->id + (next->id - before->id) / 3;
			elem->childend->id = before->id + (next->id - before->id) / 3 * 2;
		}
		SetChildIds(elem);
	}
	else {
		if (next->id - before->id <= 1) MarkResetId(local);
		else elem->id = (before->id + next->id) / 2;
	}
}
Offscreen* FindOffscreen(NewElement* elem) {
	for (; elem != NULL; elem = elem->parent) {
		if (elem->type == LetterType::_ElemEnd) continue;
		if (elem->offscreen != NULL) return elem->offscreen;
	}
	return NULL;
}
void NewRemove(ThreadGC* thgc, NewLocal* local, NewElement* elem) {
	elem->before->next = elem->next;
	elem->next->before = elem->before;
	elem->parent = NULL;
}
void RebuildOffscreen(ThreadGC* thgc, Offscreen* off, NewElement* elem, int *n) {
	for(NewElement* child = elem->childend->next; child->type != LetterType::_ElemEnd; child = child->next) {
		child->id = ++(*n) * 65535ULL * 65535ULL * 65535ULL;
		if (child->offscreen != NULL) {
			child->offscreen->next = off;
			child->offscreen->before = off;
			off->before->next = child->offscreen;
			off->before = child->offscreen;
			child->offscreen->parent = off->parent;
			child->offscreen->child->next = child->offscreen->child->before = child->offscreen->child;
			child->offscreen->child->parent = child->offscreen;
			if (child->childend != NULL) RebuildOffscreen(thgc, child->offscreen->child, child, n);
		}
		else if (child->childend != NULL) {
			RebuildOffscreen(thgc, off, child, n);
		}
	}
	elem->childend->id = ++(*n) * 65535ULL * 65535ULL * 65535ULL;
}
void BeforeOffscreen(NewLocal* local, Offscreen* next, Offscreen* screen) {
	screen->next->before = screen->before;
	screen->before->next = screen->next;
	screen->before = next->before;
	screen->next = next;
	next->before->next = screen;
	next->before = screen;
	screen->paint = next->parent;
}
void NextOffscreen(NewLocal* local, Offscreen* before, Offscreen* screen) {
	screen->next->before = screen->before;
	screen->before->next = screen->next;
	screen->next = before->next;
	screen->before = before;
	before->next->before = screen;
	before->next = screen;
	screen->parent = before->parent;
}
void RemoveOffscreen(Offscreen* screen) {
	screen->next->before = screen->before;
	screen->before->next = screen->next;
	screen->next->before = screen;
	screen->parent = NULL;
}
Offscreen* FindNextOffscreen(NewElement* elem) {
	for (; elem->type != _ElemEnd; elem = elem->next) {
		if (elem->childend != NULL) {
			Offscreen* ret = FindNextOffscreen(elem->childend->next);
			if (ret != NULL) return ret;
		}
	}
	return NULL;
}
void RootBeforeOffscreen(ThreadGC* thgc, NewLocal* local, NewElement* elem, Offscreen* offscreen) {
	if (elem->offscreen != NULL) {
		BeforeOffscreen(local, offscreen, elem->offscreen);
		return;
	}
	if (elem->childend != NULL) {
		for (NewElement* e = elem->childend->next; e->type != _ElemEnd; e = e->next) {
			RootBeforeOffscreen(thgc, local, e, offscreen);
		}
	}
}
void RootBeforeOffscreen2(ThreadGC* thgc, NewLocal* local, NewElement* elem, Offscreen* offscreen) {
	if (CheckOffscreen(elem)) {
		elem->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
		elem->offscreen->group = &createGroup(thgc);
		elem->offscreen->elem = elem;
		elem->offscreen->dirty = true;
		elem->offscreen->child = (Offscreen*)GC_alloc(thgc, CType::_OffscreenEnd);
		elem->offscreen->child->next = elem->offscreen->child->before = elem->offscreen->child;
		BeforeOffscreen(local, offscreen, elem->offscreen);
		offscreen = elem->offscreen->child;
	}
	if (elem->childend != NULL) {
		for (NewElement* e = elem->childend->next; e->type != _ElemEnd; e = e->next) {
			RootBeforeOffscreen2(thgc, local, e, offscreen);
		}
	}
}
void RootBeforeOffscreen3(ThreadGC* thgc, NewLocal* local, NewElement* elem, Offscreen* offscreen) {
	if (elem->childend != NULL) {
		for (NewElement* e = elem->childend->next; e->type != _ElemEnd; e = e->next) {
			RootBeforeOffscreen(thgc, local, e, offscreen);
		}
	}
}
void RootDeleteOffscreen(ThreadGC* thgc, NewLocal* local, NewElement* elem) {
	if (elem->offscreen != NULL) {
		elem->offscreen = NULL;
		RemoveOffscreen(elem->offscreen);
	}
	if (elem->childend != NULL) {
		for (NewElement* e = elem->childend->next; e->type != _ElemEnd; e = e->next) {
			RootDeleteOffscreen(thgc, local, e);
		}
	}
}
void NewNextElement(ThreadGC* thgc, NewLocal* local, NewElement* before, NewElement* elem) {
	Offscreen* bscreen = FindOffscreen(elem);
	NewNext(thgc, local, before, elem);
	if (bscreen != NULL) {
		bscreen->markLayout(local);
		Offscreen* ascreen = FindOffscreen(elem);
		if (ascreen != NULL) {
			ascreen->markLayout(local);
			Offscreen* screen = FindNextOffscreen(elem->next);
			if (screen == NULL) screen = ascreen->child;
			RootBeforeOffscreen(thgc, local, elem, screen);
		}
		else {
			RootDeleteOffscreen(thgc, local, elem);
		}
	}
	else {
		Offscreen* ascreen = FindOffscreen(elem);
		if (ascreen != NULL) {
			ascreen->markLayout(local);
			Offscreen* screen = FindNextOffscreen(elem->next);
			if (screen == NULL) screen = ascreen->child;
			RootBeforeOffscreen2(thgc, local, elem, screen);
		}
	}
}
void NewNextMoveElement(ThreadGC* thgc, NewLocal* local, NewElement* before, NewElement* elem) {
	NewRemoveElement(thgc, local, elem);
	NewNextElement(thgc, local, before, elem);
}
void NewRemoveElement(ThreadGC* thgc, NewLocal* local, NewElement* elem) {
	Offscreen* screen = FindOffscreen(elem);
	if (screen != NULL) {
		RootDeleteOffscreen(thgc, local, elem);
		screen->markLayout(local);
	}
	NewRemove(thgc, local, elem);
}
void changeOrient(NewLocal* local, NewElement* elem, bool orient) {
	elem->orient = orient;
	Offscreen* screen = FindOffscreen(elem);
	if (screen != NULL) screen->markLayout(local);
}
void changeXtype(ThreadGC* thgc, NewLocal* local, NewElement* elem, SizeType xtype) {
	if (elem->xtype == xtype) return;
	elem->xtype = xtype;
	Offscreen* off = FindOffscreen(elem);
	if (off != NULL) {
		off->markLayout(local);
		if (elem->offscreen == NULL) {
			if (CheckOffscreen(elem)) {
				elem->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
				elem->offscreen->group = &createGroup(thgc);
				elem->offscreen->elem = elem;
				elem->offscreen->dirty = true;
				elem->offscreen->child = (Offscreen*)GC_alloc(thgc, CType::_OffscreenEnd);
				elem->offscreen->child->next = elem->offscreen->child->before = elem->offscreen->child;
				elem->offscreen->child->parent = elem->offscreen;
				RootBeforeOffscreen3(thgc, local, elem, elem->offscreen->child);
				Offscreen* next = FindNextOffscreen(elem->next);
				if (next == NULL) next = off->child;
				BeforeOffscreen(local, next, elem->offscreen);

			}
		}
		else {
			if (!CheckOffscreen(elem)) {
				for (Offscreen* offscreen = elem->offscreen->parent->child->before;offscreen != offscreen->parent->child; ) {
					Offscreen* before = offscreen->before;
					NextOffscreen(local, elem->offscreen, offscreen);
					offscreen = before;
				}
				RemoveOffscreen(elem->offscreen);
				elem->offscreen = NULL;
			}
		}
	}
}
void changeYType(ThreadGC* thgc, NewLocal* local, NewElement* elem, SizeType ytype) {
	if (elem->ytype == ytype) return;
	elem->ytype = ytype;
	Offscreen* off = FindOffscreen(elem);
	if (off != NULL) {
		off->markLayout(local);
		if (elem->offscreen == NULL) {
			if (CheckOffscreen(elem)) {
				elem->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
				elem->offscreen->group = &createGroup(thgc);
				elem->offscreen->elem = elem;
				elem->offscreen->dirty = true;
				elem->offscreen->child = (Offscreen*)GC_alloc(thgc, CType::_OffscreenEnd);
				elem->offscreen->child->next = elem->offscreen->child->before = elem->offscreen->child;
				elem->offscreen->child->parent = elem->offscreen;
				RootBeforeOffscreen3(thgc, local, elem, elem->offscreen->child);
				Offscreen* next = FindNextOffscreen(elem->next);
				if (next == NULL) next = off->child;
				BeforeOffscreen(local, next, elem->offscreen);

			}
		}
		else {
			if (!CheckOffscreen(elem)) {
				for (Offscreen* offscreen = elem->offscreen->parent->child->before; offscreen != offscreen->parent->child; ) {
					Offscreen* before = offscreen->before;
					NextOffscreen(local, elem->offscreen, offscreen);
					offscreen = before;
				}
				RemoveOffscreen(elem->offscreen);
				elem->offscreen = NULL;
			}
		}
	}
}
void changeOffscreen(ThreadGC* thgc, NewLocal* local, NewElement* elem, bool offscreened) {
	if (elem->offscreened == offscreened) return;
	elem->offscreened = offscreened;
	Offscreen* off = FindOffscreen(elem);
	if (off != NULL) {
		off->markLayout(local);
		if (elem->offscreen == NULL) {
			if (CheckOffscreen(elem)) {
				elem->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
				elem->offscreen->group = &createGroup(thgc);
				elem->offscreen->elem = elem;
				elem->offscreen->dirty = true;
				elem->offscreen->child = (Offscreen*)GC_alloc(thgc, CType::_OffscreenEnd);
				elem->offscreen->child->next = elem->offscreen->child->before = elem->offscreen->child;
				elem->offscreen->child->parent = elem->offscreen;
				RootBeforeOffscreen3(thgc, local, elem, elem->offscreen->child);
				Offscreen* next = FindNextOffscreen(elem->next);
				if (next == NULL) next = off->child;
				BeforeOffscreen(local, next, elem->offscreen);

			}
		}
		else {
			if (!CheckOffscreen(elem)) {
				for (Offscreen* offscreen = elem->offscreen->parent->child->before; offscreen != offscreen->parent->child; ) {
					Offscreen* before = offscreen->before;
					NextOffscreen(local, elem->offscreen, offscreen);
					offscreen = before;
				}
				RemoveOffscreen(elem->offscreen);
				elem->offscreen = NULL;
			}
		}
	}
}
void changeBackground(NewLocal* local, NewElement* elem, Background* back) {
	elem->background = back;
	FindOffscreen(elem)->markPaint(local);
}
void changeMargin(NewLocal* local, NewElement* elem, float top, float right, float down, float left) {
	elem->margins[0] = top;
	elem->margins[1] = right;
	elem->margins[2] = down;
	elem->margins[3] = left;
	FindOffscreen(elem)->markLayout(local);
}
void changePadding(NewLocal* local, NewElement* elem, float top, float right, float down, float left) {
	elem->paddings[0] = top;
	elem->paddings[1] = right;
	elem->paddings[2] = down;
	elem->paddings[3] = left;
	FindOffscreen(elem)->markLayout(local);
}
void changeWidth(NewLocal* local, NewElement* elem, float width) {
	elem->size.x = width;
	FindOffscreen(elem)->markLayout(local);
}
void changeHeight(NewLocal* local, NewElement* elem, float height) {
	elem->size.y = height;
	FindOffscreen(elem)->markLayout(local);
}
void setSize(NewLocal* local, NewElement* elem, float width, float height) {
	elem->size.x = width;
	elem->size.y = height;
	FindOffscreen(elem)->markLayout(local);
}
/*void setText(NewElement* elem, String* text) {
	if (node && node->text != text) { node->text = text; markLayoutDirty(node); }
}
void setFontSize(NewElement* elem, float size) {
	if (node && node->fontSize != size) {
		node->fontSize = size; markLayoutDirty(node);
	}
}
void setColor(NewElement* elem, uint32_t color) {
	if (node && node->color != color) {
		node->color = color; markPaintDirty(node);
	}
}// ========== �����p�����[�^�iDirty�ɂ��Ȃ��j ========== 
void setScroll(NewElement* elem, float x, float y) {
	elem->scroll.x = x;
	elem->scroll.y = y;
	//main dirty
}
void setOpacity(NewElement* elem, float opacity) {
	elem->opacity = opacity;
	//main dirty
}
void setTransform(NewElement* elem, float tx, float ty, float scale, float rot) {
	Surface* surface = findSurfaceFor(node);
	if (surface) { surface->composite.translateX = tx; surface->composite.translateY = ty; surface->composite.scaleX = scale; surface->composite.scaleY = scale; surface->composite.rotation = rot;
	}
}*/
void initNewLine(ThreadGC* thgc, NewLine* line);
void NewElementAddLast(ThreadGC* thgc, NewLocal* local, NewElement* parent, NewElement* child) {
	if (parent->childend->before->type != LetterType::_Line) {
		auto line = (NewLine*)GC_alloc(thgc, CType::_LineC);
		initNewLine(thgc, line);
		NewBefore(thgc, local, parent->childend, line);
	}
	NewBefore(thgc, local, parent->childend->before->childend, child);
	FindOffscreen(parent)->markLayout(local);
}
void NewLineAddLast(ThreadGC* thgc, NewLocal* local, NewElement* parent, NewLine* line) {
	NewBefore(thgc, local, parent->childend, line);
	FindOffscreen(parent)->markLayout(local);
}


void NewMeasureCall(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	if (elem->offscreen != NULL) {
		measure = new NewMeasure();
		measure->base = elem;
		measure->pos = { 0, 0 };
		measure->size = { elem->size.x, elem->size.y };
	}
	elem->pos2.x = measure->pos.x;
	elem->pos2.y = measure->pos.y;
	measure->pos.x += elem->margins[3] + elem->borderRadius + elem->paddings[3];
	measure->pos.y += elem->margins[0] + elem->borderRadius + elem->paddings[0];
	measure->start = measure->pos;
	float sizex = 0, sizey = 0;
	for (NewElement* child = elem->childend->next; child->type != LetterType::_ElemEnd; ) {
		child->Measure(thgc, child, measure, local, n);
		if (elem->orient) {
			if (sizey < child->size.y) sizey = child->size.y;
			measure->pos.x += child->size.x;
			measure->pos.y = measure->start.y;
			sizex += child->size.x;
		}
		else
		{
			if (sizex < child->size.x) sizex = child->size.x;
			measure->pos.y += child->size.y;
			measure->pos.x = measure->start.x;
			sizey += child->size.y;
		}
		child = child->next;
	}
	elem->size2.x = sizex + elem->margins[1] + elem->margins[3] + elem->paddings[1] + elem->paddings[3] + elem->borderRadius * 2;
	elem->size2.y = sizey + elem->margins[0] + elem->margins[2] + elem->paddings[0] + elem->paddings[2] + elem->borderRadius * 2;
	if (elem->xtype == SizeType::Auto) { elem->size.x = elem->size2.x; }
	if (elem->ytype == SizeType::Auto) { elem->size.y = elem->size2.y; }
	return;
}
float pro = 0;
int addPattern(ThreadGC* thgc, std::vector<float>& colors, std::vector<float>& widthes);
void NewDrawCall(ThreadGC* thgc, NewElement* elem, NewGraphic* g, NewLocal* local, RenderCommandQueue* q) {
	bool offscreened = false;
	float sbarx = 0, sbary = 0;
	//x Scroll xchanged
	if (elem->xtype == SizeType::Scroll) {
		sbary += 5; offscreened = true;
	}
	else if (elem->ytype == SizeType::Page) {
		offscreened = true;
		sbarx += 10;
	}
	if (elem->ytype == SizeType::Scroll) {
		sbarx += 5;
		offscreened = true;
	}
	else if (elem->ytype == SizeType::Page) {
		offscreened = true;
		sbary += 10;
	}
	if (elem->type == LetterType::_Main) {
		offscreened = true;
	}
	if (elem->offscreen != NULL) {
		float sizex = elem->size.x - sbarx; float sizey = elem->size.y - sbary;
		if (elem->size2.x <= elem->size.x) sizex = elem->size2.x;
		if (elem->size2.y <= elem->size.y) sizey = elem->size2.y;
		float size3x = elem->offscreen->fbsize.x, size3y = elem->offscreen->fbsize.y;
		bool resized = false;
		if (elem->offscreen->fbsize.x < elem->size2.x) {
			size3x = elem->size2.x * 1.3;
			resized = true;
		}
		else if (elem->size2.x < elem->offscreen->fbsize.x / 3) {
			size3x = elem->size2.x * 1.3;
			resized = true;
		}
		if (elem->offscreen->fbsize.y < elem->size2.y) {
			size3y = elem->size2.y * 1.3;
			resized = true;
		}
		else if (elem->size2.y < elem->offscreen->fbsize.y / 3) {
			size3y = elem->size2.y * 1.3;
			resized = true;
		}
		if (resized) {
			queueOffscreenResize(thgc, elem->offscreen->imPing, size3x, size3y);
			queueOffscreenResize(thgc, elem->offscreen->imPong, size3x, size3y);
		}
		auto info = mygetStandaloneTextureInfo(thgc, elem->offscreen->ping ? elem->offscreen->imPing : elem->offscreen->imPong);
		g->layer->pushText(elem->pos2.x, elem->pos2.y, sizex, sizey,
			elem->scroll.x / size3x, elem->scroll.y / size3y, sizex / size3x, sizey / size3y, 0xFFFFFFFF, 10000, &info->handle, g->fb, g->fbsize, g->viewId);
		/*drawUnderPagingBar(g->layer, *getAtlas(thgc), getFont("sans", 16), elem->pos2.x, sizey + 110.0f, sizex, 5.0f, 3.0, 16.0, 0.0f, g->group, g->fb, g->fbsize, g->viewId);
		drawRightPagingBar(g->layer, *getAtlas(thgc), getFont("sans", 16), sizex + 10.0f, elem->pos2.y, 5.0f, 100.0f, 3.0, 16.0, 0.0f, g->group, g->fb, g->fbsize, g->viewId);
		drawUnderScrollBar(g->layer, elem->pos2.x, sizey + 100.0f, sizex, 5.0f, 100.0f, 50.0f, 300.0f, 0.0f, g->fb, g->fbsize, g->viewId);
		drawRightScrollBar(g->layer, sizex, elem->pos2.y, 5.0f, 100.0f, 100.0f, 50.0f, 300.0f, 0.0f, g->fb, g->fbsize, g->viewId);
		pro += 0.004f;
		pro = fmod(pro, 1.0f);
		auto info2 = mygetStandaloneTextureInfo(thgc, elem->background->tex1);*/
		g = new NewGraphic{g->layer, elem, elem, {0,0}, {elem->size2.x, elem->size2.y}, {0,0}, {0,0},
			elem->offscreen->ping ? elem->offscreen->imPong : elem->offscreen->imPing,  &info->fbo,  &info->size, elem->offscreen->viewId = --viewId, elem->offscreen->group };
		elem->offscreen->ping = !elem->offscreen->ping;
		static bgfx::TextureHandle s_nulltex = BGFX_INVALID_HANDLE;
		bgfx::TextureHandle* tex1, * tex2;
		if (isValidImageId(elem->background->tex1)) {
			auto info = mygetStandaloneTextureInfo(thgc, elem->background->tex1);
			tex1 = &info->handle;
		}
		else tex1 = &s_nulltex;
		if (isValidImageId(elem->background->tex2)) {
			auto info = mygetStandaloneTextureInfo(thgc, elem->background->tex2);
			tex2 = &info->handle;
		}
		else tex2 = &s_nulltex;
		g->layer->pushBackground(elem->background, elem->pos2.x, elem->pos2.y, sizex, sizey, std::floor(elem->zIndex),
			tex1, tex2, g->fb, g->fbsize, g->viewId, 1.0f);
		std::vector<float>* colors = new std::vector<float>{
			1.0f, 0.0f, 0.0f, 1.0f,  // ��
			0.0f, 1.0f, 0.0f, 1.0f,  // ��
			0.0f, 0.0f, 1.0f, 1.0f,  // ��
		};
		std::vector<float>* widths = new std::vector<float>{ 10.0f, 10.0f, 10.0f };
		int size = widths->size();
		int n = addPattern(thgc, *colors, *widths);
		//g->layer->pushPattern(DrawCommandType::GradientChecker, elem->pos.x, elem->pos.y, elem->size2.x / 3 * 2, elem->size2.y / 3 * 2, size, 0.0f, 0.0f, 0.0f, 6.0f, 1.0f, 1.0, 0x000000FF, 3.0f, 3.0f, 2.0f, 0x00ff00FF, n, 0, g->fb, g->fbsize, g->viewId);
	}
	g->pos.x += elem->margins[3] + elem->borderRadius + elem->paddings[3];
	g->pos.y += elem->margins[0] + elem->borderRadius + elem->paddings[0];
	g->start = g->pos;
	if (elem->background != NULL) {
		//g->layer->push(elem->background->cmd);
	}
	float sizex, sizey;
	NewElement* start = elem->childend->next;
	//background.draw
	for (NewElement* child = elem->childend->next; child->type != LetterType::_ElemEnd; ) {
		child->Draw(thgc, child, g, local, q);
		child = child->next;
	}
}
void initNewLocal(ThreadGC* thgc, NewLocal* local) {
	local->next = local->before = (NewElement*)local;
	local->parent = NULL;
	NewEndElement* end = (NewEndElement*)GC_alloc(thgc, CType::_EndC);
	initNewEndElement(thgc, end, (NewElement*)local);
	end->id = 65536; std::numeric_limits<uint64_t>::max();
	local->childend = (NewElement*)end;
	local->margins[0] = local->margins[1] = local->margins[2] = local->margins[3] = 0;
	local->paddings[0] = local->paddings[1] = local->paddings[2] = local->paddings[3] = 0;
	local->pos.x = local->pos.y = 0;
	local->offscreen = new Offscreen();
	local->scroll.x = local->scroll.y = 0;
	local->size.x = 800;
	local->size.y = 600;
	local->size2.x = local->size2.y = 0;
	local->xtype = local->ytype = SizeType::Scroll;
	local->position = Relative;
	local->layout = ocupay;
	local->type = LetterType::_Main;
	local->Measure = NewMeasureCall;
	local->Draw = NewDrawCall;
	local->Mouse = ElementMouse;
	local->Key = ElementKey;
	local->DrawSelection = ElementDrawSelect;
	local->len = len1;
	local->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
	local->offscreen->group = &createGroup(thgc);
	local->offscreen->markLayout(local);
	local->offscreen->elem = local;
	local->offscreen->imPing = queueOffscreenNew(thgc, 1, 1);
	local->offscreen->imPong = queueOffscreenNew(thgc, 1, 1);
	local->offscreened = true;
	local->background = new Background();
	local->background->tex1 = myloadTexture2D(thgc, "123.png");
	local->background->fillcolor = 0xffffffff;
	local->background->borderColor = 0x00000000;
	local->background->borderWidth = 0.0f;
	local->background->shadowBlur = 1.0f;
	local->background->shadowColor = 0x00000000;
	local->dirty = DirtyType::RebuildValue;
	local->screens = create_list(thgc, sizeof(Offscreen*), _List);
	local->editable = true;
	local->select.from = NULL;
}
#include "Compile.h"
void initNewLine(ThreadGC* thgc, NewLine* line) {
	line->type = LetterType::_Line;
	line->next = line;
	line->before = line;
	line->parent = NULL;
	line->childend = (NewElement*)GC_alloc(thgc, CType::_EndC);
	initNewEndElement(thgc, (NewEndElement*)line->childend, line);
	line->orient = true;
	line->id = 0;
	line->Measure = NewMeasureCall;
	line->Draw = NewDrawCall;
	line->Mouse = ElementMouse;
	line->Key = ElementKey;
	line->DrawSelection = ElementDrawSelect;
	line->len = len1;
}
void NewLetterMeasureCall(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	NewLetter* letter = (NewLetter*)elem;
	if (letter->recompile) {
		List* list = Compile(thgc, letter->text, letter->font);
		letter->atsspans = *(List**)get_list(list, 0);
		if (list->size > 1) {
			String* text = letter->text;
			NewElement* nowline = elem->parent;
			NewLetter* let = letter;
			int m = 0;
			for (int i = 1; ; i++) {
				if (i == list->size) {
					let->text = SubString(thgc, text, m, text->size - m);
					break;
				}
				List* list2 = *(List**)get_list(list, i);
				ATSSpan* atsspan = *(ATSSpan**)get_list(list2, 0);
				NewLine* line = (NewLine*)GC_alloc(thgc, CType::_LineC);
				let->text = SubString(thgc, text, m, atsspan->start - m);
				m = atsspan->end;
				initNewLine(thgc, line);
				NewNext(thgc, local, nowline, line);
				let = (NewLetter*)GC_alloc(thgc, CType::_LetterC);
				initNewLetter(thgc, let, letter->font, LetterType::_Letter);
				remove_list(thgc, list2, 0);
				let->atsspans = list2;
				NewNext(thgc, local, line->childend, let);
				nowline = line;
			}
		}
		letter->recompile = false;
	}
	std::vector<int> boundaries;
	for (int i = 0; i < letter->atsspans->size; i++) {
		ATSSpan* a = *(ATSSpan**)get_list(letter->atsspans, i);
		boundaries.push_back(a->start);
		boundaries.push_back(a->end);
	}


	for (int i = 0; i < letter->stylespans->size; i++) {
		StyleSpan* s = *(StyleSpan**)get_list(letter->stylespans, i);
		boundaries.push_back(s->start);
		boundaries.push_back(s->end);
	}

	std::sort(boundaries.begin(), boundaries.end());
	boundaries.erase(std::unique(boundaries.begin(), boundaries.end()), boundaries.end());

	letter->renderspans = create_list(thgc, sizeof(RenderSpan*), CType::_RenderSpan);
	letter->size2 = { 0.0f, 0.0f };
	for (size_t i = 0; i + 1 < boundaries.size(); ++i) {
		int segStart = boundaries[i];
		int segEnd = boundaries[i + 1];

		RenderSpan *r = (RenderSpan*)GC_alloc(thgc, CType::_RenderSpan);
		r->start = segStart;
		r->end = segEnd;

		bool applied = false;
		// �܂� StyleSpan ��T���i�D��j
		for (int i = 0; i < letter->stylespans->size; i++) {
			StyleSpan* s = *(StyleSpan**)get_list(letter->stylespans, i);
			r->color = s->color;
			r->font = s->font;
			applied = true;
			break;
		}
		// �Ȃ���� ATSspan
		if (!applied) {
			for (int i = 0; i < letter->atsspans->size; i++) {
				ATSSpan* a = *(ATSSpan**)get_list(letter->atsspans, i);
				if (a->start <= segStart && a->end >= segEnd) {
					r->color = a->color;
					r->font = a->font;
					break;
				}
			}
		}
		float w, h;
		size_t n;
		MeasureString(*getAtlas(thgc), r->font, SubString(thgc, letter->text, r->start, r->end - r->start), r->end - r->start, 10000, &w, &h, &n, measure->group);
		r->width = w;
		r->height = h;
		r->x = letter->size2.x;
		if (letter->size2.y < h) letter->size2.y = h;
		letter->size2.x += w;
		add_list(thgc, letter->renderspans, (char *)r);
	}
	letter->size = letter->size2;
	letter->pos2.x = measure->pos.x;
	letter->pos2.y = measure->pos.y;
}
void SelectDraw(ThreadGC* thgc, NewLocal* local, NewGraphic* g, RenderCommandQueue* q) {
	if (local->select.m == local->select.from->len(local->select.from)) {
		local->select.from = local->select.from->next;
		local->select.fromid = local->select.from->id;
		local->select.m = 0;
	}
	if (local->select.n == local->select.to->len(local->select.to)) {
		local->select.to = local->select.to->next;
		local->select.toid = local->select.to->id;
		local->select.n = 0;
	}
	if (local->select.fromid == local->select.toid) {
		if (local->select.m <= local->select.n) {
			local->select.start = local->select.from;
			local->select.end = local->select.to;
			local->select.s = local->select.m;
			local->select.e = local->select.n;
		}
		else {
			local->select.start = local->select.to;
			local->select.end = local->select.from;
			local->select.s = local->select.n;
			local->select.e = local->select.m;
		}
	}
	else if (local->select.fromid < local->select.toid) {
		local->select.start = local->select.from;
		local->select.end = local->select.to;
		local->select.s = local->select.m;
		local->select.e = local->select.n;
	}
	else {
		local->select.start = local->select.to;
		local->select.end = local->select.from;
		local->select.s = local->select.n;
		local->select.e = local->select.m;
	}
	NewElement* start = local->select.start;
	int s = local->select.s;
	for (;;) {
		if (start == local->select.end) {
			start->DrawSelection(thgc, local, start, s, local->select.e, g, q);
			break;
		}
		start->DrawSelection(thgc, local, start, s, start->len(start), g, q);
		s = 0;
		if (start->childend != NULL) {
			start = start->childend->next;
			continue;
		}
		else if (start->type == _ElemEnd) {
			start = start->parent->next;
			continue;
		}
		start = start->next;
	}
}
void ElementDrawSelect(ThreadGC* thgc, NewLocal* local, NewElement* self, int m, int n, NewGraphic* g, RenderCommandQueue* q) {
	return;
}
void LetterDrawSelect(ThreadGC* thgc, NewLocal* local, NewElement* self, int m, int n, NewGraphic* g, RenderCommandQueue* q) {
	NewLetter* letter = (NewLetter*)self;
	int l = 0, r = letter->renderspans->size - 1;
	while (l <= r) {
		int m2 = (l + r) / 2;
		auto s = *(RenderSpan**)get_list(letter->renderspans, m2);
		if (m < s->start)
			r = m2 - 1;
		else if (m >= s->end)
			l = m2 + 1;
		else{
			String* str = SubString(thgc, letter->text, s->start, s->end);
			float w0, h0;
			size_t n2;
			MeasureString(*getAtlas(thgc), s->font, str, m - s->start, 10000, &w0, &h0, &n2, NULL);
			for (;;) {
				if (n <= s->end) {
					float w, h;
					str = SubString(thgc, letter->text, s->start, s->end);
					MeasureString(*getAtlas(thgc), s->font, str, n - s->start, 10000, &w, &h, &n2, NULL);
					g->layer->pushFill(self->pos.x + self->pos2.x + s->x + w0, self->pos.y + self->pos2.y + s->y, w - w0 + 1, s->height, 0.0f, 0.0f, 0.0f, 0x4477ff66, 0, 0.0f, 0.0f, 1.0f, 0, 12000.0f, g->fb, g->fbsize, g->viewId);
					break;
				}
				else {
					g->layer->pushFill(self->pos.x + self->pos2.x + s->x + w0, self->pos.y + self->pos2.y + s->y, s->width - w0 + 1, s->height, 0.0f, 0.0f, 0.0f, 0x4477ff66, 0, 0.0f, 0.0f, 1.0f, 0, 12000.0f, g->fb, g->fbsize, g->viewId);
				}
				w0 = 0.0f; h0 = 0.0f;
				m2++;
				s = *(RenderSpan**)get_list(letter->renderspans, m2);
			}
			break;
		}
	}
	return;
}
bgfx::FrameBufferHandle nullfb = BGFX_INVALID_HANDLE;
void NewLetterDrawCall(ThreadGC* thgc, NewElement* elem, NewGraphic* g, NewLocal* local, RenderCommandQueue* q) {
	NewLetter* letter = (NewLetter*)elem;
	for (int i = 0; i < letter->renderspans->size; i++) {
		auto rs = *(RenderSpan**)get_list(letter->renderspans, i);
		drawString((LayerInfo*)g->layer, *getAtlas(thgc), rs->font, SubString(thgc, letter->text, rs->start, rs->end - rs->start), g->pos.x + letter->pos2.x + rs->x, g->pos.y + letter->pos2.y + rs->y, std::floor(elem->zIndex) + 0.9,
			rs->color, g->group, g->fb, g->fbsize, g->viewId);
	}
}
int LetterLen(NewElement* elem) {
	NewLetter* letter = (NewLetter*)elem;
	return letter->text->size;
}
void initNewLetter(ThreadGC* thgc, NewLetter* letter, FontId font, enum LetterType type) {
	letter->type = type;
	letter->next = letter->before = letter;
	letter->parent = NULL;
	letter->childend = NULL;
	letter->id = 0;
	letter->Measure = NewLetterMeasureCall;
	letter->Draw = NewLetterDrawCall;
	letter->Mouse = LetterMouse;
	letter->Key = LetterKey;
	letter->DrawSelection = LetterDrawSelect;
	letter->len = LetterLen;
	letter->text = NULL;
	letter->font = font;
	letter->atsspans = create_list(thgc, sizeof(ATSSpan*), CType::_ATSSpan);
	letter->stylespans = create_list(thgc, sizeof(StyleSpan*), CType::_StyleSpan);
	letter->renderspans = create_list(thgc, sizeof(RenderSpan*), CType::_RenderSpan);
	letter->recompile = true;
}
int ElementMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local) {
	if (self->offscreened) e->group = self->offscreen->group;
	if (self->GoMouseDown != NULL) {
		MemObj* mo = (MemObj*)GC_clone(thgc, (char*)self->GoMouseDown->obj);
		MemTable* res = (MemTable*)GC_alloc(thgc, _MemTable);
		res->table = (Map*)create_mapy(thgc, _List);
		MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
		mo->req = NULL; mo->res = res;
		auto rn = GC_add_root_node(thgc);
		auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoMouseDown->func, (char*)mo)));
		h.promise().state = -2;
		thgc->queue->push(h);
	}
	for (NewElement* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
		if (elem->orient) {
			if (elem->pos.y + elem->pos2.y <= e->y && e->y < elem->pos.y + elem->pos2.y + elem->size.y) {
				elem->Mouse(thgc, elem, e, local);
			}
		}
		else {
			if (elem->pos.x + elem->pos2.x <= e->x && e->x < elem->pos.x + elem->pos2.x + elem->size.x) {
				elem->Mouse(thgc, elem, e, local);
			}

		}
	}

	if (self->BackMouseDown != NULL) {
		MemObj* mo = (MemObj*)GC_clone(thgc, (char*)self->BackMouseDown->obj);
		MemTable* res = (MemTable*)GC_alloc(thgc, _MemTable);
		res->table = (Map*)create_mapy(thgc, _List);
		MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
		mo->req = NULL; mo->res = res;
		auto rn = GC_add_root_node(thgc);
		auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackMouseDown->func, (char*)mo)));
		h.promise().state = -2;
		thgc->queue->push(h);
	}
	return -1;
}
void NewMoveElement(ThreadGC* thgc, NewLocal* local, NewElement* before, NewElement* from, NewElement* to) {
	if (from->type == LetterType::_ElemEnd || to->type == LetterType::_ElemEnd) return;
	NewElement* b, *a = before->next;
	if (before->type == _ElemEnd) {
		b = before->parent;
	}
	else b = before;
	from->before->next = to->next;
	to->next->before = from->before;
	to->next = before->next;
	before->next->before = to;
	before->next = from;
	from->before = before;
	int n = 2;
	for (NewElement* elem = from; elem != to; elem = elem->next) n++;
	int c = 1;
	for (; ; from = from->next) {
		from->id = b->id + (a->id - b->id) * c / n;
		from->parent = before->parent;
		if (from == to) break;
	}
	
}
void SelectKey(ThreadGC* thgc, NewLocal* local, KeyEvent* e) {
	if (local->select.m == local->select.from->len(local->select.from)) {
		local->select.from = local->select.from->next;
		local->select.fromid = local->select.from->id;
		local->select.m = 0;
	}
	if (local->select.n == local->select.to->len(local->select.to)) {
		local->select.to = local->select.to->next;
		local->select.toid = local->select.to->id;
		local->select.n = 0;
	}
	if (local->select.fromid == local->select.toid) {
		if (local->select.m <= local->select.n) {
			local->select.start = local->select.from;
			local->select.end = local->select.to;
			local->select.s = local->select.m;
			local->select.e = local->select.n;
		}
		else {
			local->select.start = local->select.to;
			local->select.end = local->select.from;
			local->select.s = local->select.n;
			local->select.e = local->select.m;
		}
	}
	else if (local->select.fromid < local->select.toid) {
		local->select.start = local->select.from;
		local->select.end = local->select.to;
		local->select.s = local->select.m;
		local->select.e = local->select.n;
	}
	else {
		local->select.start = local->select.to;
		local->select.end = local->select.from;
		local->select.s = local->select.n;
		local->select.e = local->select.m;
	}
	NewElement* start = local->select.start;
	std::vector<NewElement*> vec;
	for (; start != NULL; start = start->parent) vec.push_back(start);
	for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
		NewElement* e = *it;
		if (e->GoKeyDown != NULL) {
			MemObj* mo = (MemObj*)GC_clone(thgc, (char*)e->GoKeyDown->obj);
			MemTable* res = (MemTable*)GC_alloc(thgc, _MemTable);
			res->table = (Map*)create_mapy(thgc, _List);
			MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
			mo->req = NULL; mo->res = res;
			auto rn = GC_add_root_node(thgc);
			auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, e->GoKeyDown->func, (char*)mo)));
			h.promise().state = -2;
			thgc->queue->push(h);
		}
	}
	start = local->select.start;
	int s = local->select.s;
	for (;;) {
		if (start == local->select.end) {
			start->Key(thgc, start, s, local->select.e, e, local);
			break;
		}
		NewElement* next = start->next;
		int n = start->Key(thgc, start, s, start->len(start), e, local);
		s = 0;
		if (n == 1) break;
		else if (n == -1) {
			start = next;
			break;
		}
		else if (n == -2) {
			start = start->before->next;
			continue;
		}
		if (start->childend != NULL) {
			start = start->childend->next;
			continue;
		}
		else if (start->type == _ElemEnd) {
			start = start->parent->next;
			continue;
		}
		start = start->next;
	}
	for (; start != NULL; start = start->parent) {
		if (start->BackKeyDown != NULL) {
			MemObj* mo = (MemObj*)GC_clone(thgc, (char*)start->BackKeyDown->obj);
			MemTable* res = (MemTable*)GC_alloc(thgc, _MemTable);
			res->table = (Map*)create_mapy(thgc, _List);
			MemInsert(thgc, res, _KeyEvent, (ModelVal*)e);
			mo->req = NULL; mo->res = res;
			auto rn = GC_add_root_node(thgc);
			auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, start->BackKeyDown->func, (char*)mo)));
			h.promise().state = -2;
			thgc->queue->push(h);
		}
	}
}
int ElementKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local) {
	return 0;
}
int AddText(ThreadGC* thgc, NewLocal* local, NewLetter* letter, int m, int n, String* text) {
	if (m == 0 && n == letter->text->size && (text == NULL || text->size == 0)) {
		NewRemove(thgc, local, letter);
		return -1;
	}

	int deleteLen = n - m;
	int insertLen = (text != NULL) ? text->size : 0;
	int delta = insertLen - deleteLen;

	// stylespans を更新
	for (int i = letter->stylespans->size - 1; i >= 0; i--) {
		StyleSpan* s = *(StyleSpan**)get_list(letter->stylespans, i);

		// 削除範囲に完全に含まれるスパンは除去
		if (s->start >= m && s->end <= n) {
			remove_list(thgc, letter->stylespans, i);
			continue;
		}

		// 削除範囲と重なるスパンをクリップ
		if (s->start < n && s->end > m) {
			if (s->start < m) {
				// スパンの左側が残る
				if (s->end <= n) {
					s->end = m;
				}
				else {
					// スパンが削除範囲をまたぐ → 縮める
					s->end += delta;
				}
			}
			else {
				// スパンの右側が残る
				s->start = m + insertLen;
				s->end += delta;
			}
		}
		// 削除範囲より後のスパンはシフト
		else if (s->start >= n) {
			s->start += delta;
			s->end += delta;
		}
	}

	// テキスト更新
	if (text == NULL)
		letter->text = StringAdd2(thgc, SubString(thgc, letter->text, 0, m), SubString(thgc, letter->text, n, letter->text->size - n));
	else
		letter->text = StringAdd2(thgc, StringAdd2(thgc, SubString(thgc, letter->text, 0, m), text), SubString(thgc, letter->text, n, letter->text->size - n));

	letter->recompile = true;
	return 0;
}
void UniteText(ThreadGC* thgc, NewLocal* local, NewElement* before, NewElement* next) {
	if (before->type == LetterType::_Letter && next->type == LetterType::_Letter) {
		NewLetter* bef = (NewLetter*)before, * nex = (NewLetter*)next;
		AddText(thgc, local, bef, bef->len(bef), bef->len(bef), nex->text);
		NewRemove(thgc, local, nex);
	}

}
void UniteLine(ThreadGC* thgc, NewLocal* local, NewLine* before, NewLine* next) {
	for (NewElement* elem = before->childend->before; elem->type != _ElemEnd; ) {
		NewElement* bef = elem->before;
		NewNext(thgc, local, next->childend, elem);
		elem = bef;
	}
	if (local->select.from == before->childend) {
		local->select.from = next->childend;
	}
	else if (local->select.to == before->childend) {
		local->select.to = next->childend;
	}
	NewRemove(thgc, local, before);
}
int EndKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local) {
	if (!self->parent->parent->editable) return 1;
	if (m == 1) return 0;
	if (e->key == SDLK_LEFT) {
		local->select.count = -1;
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (m == 0) {
				if (self->before->type == _ElemEnd) {
					if (self->parent->before->type == _ElemEnd) return 0;
					local->select.to = self->parent->before->childend;
					local->select.toid = local->select.to->id;
					local->select.n = 0;
				}
				else {
					local->select.to = self->before;
					local->select.toid = local->select.to->id;
					local->select.n = self->before->len(self->before) - 1;
				}
				if (!e->shift) {
					local->select.from = local->select.to;
					local->select.fromid = local->select.to->id;
					local->select.m = local->select.n;
				}
			}
			else {
				local->select.m = local->select.n = m - 1;
				if (!e->shift) {
					local->select.m = local->select.n;
				}
			}
			FindOffscreen(self)->markPaint(local);
			return 0;
		}
		else {
			if (e->shift) {
				self = local->select.to;
				if (m == 0) {
					if (self->before->type == _ElemEnd) {
						if (self->parent->before->type == _ElemEnd) return 1;
						local->select.to = self->parent->before->childend;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
					}
					else {
						local->select.to = self->before;
						local->select.toid = local->select.to->id;
						local->select.n = self->before->len(self->before) - 1;
					}
				}
				else {
					local->select.n--;
				}
				FindOffscreen(self)->markPaint(local);
				return 1;
			}
			else {
				local->select.from = local->select.to = local->select.start;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.s;
			}
			FindOffscreen(self)->markPaint(local);
			return 1;
		}
	}
	else if (e->key == SDLK_RIGHT) {
		local->select.count = -1;
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (self->parent->next->type == _ElemEnd) return 0;
			local->select.to = self->parent->next->childend->next;
			local->select.toid = local->select.to->id;
			local->select.n = 0;
			if (!e->shift) {
				local->select.from = local->select.to;
				local->select.fromid = local->select.from->id;
				local->select.m = local->select.n;
			}
			FindOffscreen(self)->markPaint(local);
			return 1;
		}
		else {
			if (e->shift) {
				self = local->select.to;
				if (m == self->len(self)) {
					if (self->next->type == _ElemEnd) {
						if (self->parent->next->type == _ElemEnd) return 0;
						local->select.to = self->parent->next->childend->next;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
					}
					else {
						local->select.to = self->next;
						local->select.toid = local->select.to->id;
						local->select.n = 1;
					}
				}
				else {
					if (self->type == _ElemEnd) {
						if (self->parent->next->type == _ElemEnd) return 0;
						local->select.to = self->parent->next->childend->next;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
					}
					local->select.n++;
				}
				FindOffscreen(self)->markPaint(local);
				return 0;
			}
			else {
				local->select.from = local->select.to = local->select.end;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.e;
			}
			FindOffscreen(self)->markPaint(local);
			return 1;
		}
	}
	else if (e->key == SDLK_UP) {
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (self->parent->before->type == _ElemEnd) {
				local->select.to = self->parent->childend->next;
				local->select.toid = local->select.to->id;
				local->select.n = 0;
			}
			else {
				if (local->select.count < 0) {
					local->select.count = m;
					for (NewElement* elem = self->before; elem->type != _ElemEnd; elem = elem->before) {
						local->select.count += elem->len(elem);
					}
				}
				for (NewElement* elem = self->parent->before->childend->next; ; elem = elem->next) {
					if (elem->type == _ElemEnd) {
						local->select.to = elem;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
						break;
					}
					else if (local->select.count <= elem->len(elem)) {
						local->select.to = elem;
						local->select.toid = local->select.toid;
						local->select.n = local->select.count;
						break;
					}
				}
			}
			if (!e->shift) {
				local->select.from = local->select.to;
				local->select.fromid = local->select.from->id;
				local->select.m = local->select.n;
			}
			FindOffscreen(self)->markPaint(local);
			return 1;
		}
		else {
			if (e->shift) {
				self = local->select.to;
				if (self->parent->before->type == _ElemEnd) {
					local->select.to = self->parent->childend->next;
					local->select.toid = local->select.to->id;
					local->select.n = 0;
				}
				else {
					if (local->select.count < 0) {
						local->select.count = local->select.n;
						for (NewElement* elem = self->before; elem->type != _ElemEnd; elem = elem->before) {
							local->select.count += elem->len(elem);
						}
					}
					for (NewElement* elem = self->parent->before->childend->next; ; elem = elem->next) {
						if (elem->type == _ElemEnd) {
							local->select.to = elem;
							local->select.toid = local->select.to->id;
							local->select.n = 0;
							break;
						}
						else if (local->select.count <= elem->len(elem)) {
							local->select.to = elem;
							local->select.toid = local->select.to->id;
							local->select.n = local->select.count;
							break;
						}
					}
				}
				FindOffscreen(self)->markPaint(local);
				return 1;
			}
			else {
				local->select.from = local->select.to = local->select.start;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.s;
			}
			FindOffscreen(self)->markPaint(local);
			return 1;
		}
	}
	else if (e->key == SDLK_DOWN) {
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (self->parent->next->type == _ElemEnd) {
				local->select.to = self->parent->childend;
				local->select.toid = local->select.to->id;
				local->select.n = 0;
			}
			else {
				if (local->select.count < 0) {
					local->select.count = m;
					for (NewElement* elem = self->before; elem->type != _ElemEnd; elem = elem->before) {
						local->select.count += elem->len(elem);
					}
				}
				for (NewElement* elem = self->parent->next->childend->next; ; elem = elem->next) {
					if (elem->type == _ElemEnd) {
						local->select.to = elem;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
						break;
					}
					else if (local->select.count <= elem->len(elem)) {
						local->select.to = elem;
						local->select.toid = local->select.to->id;
						local->select.n = local->select.count;
						break;
					}
				}
			}
			if (!e->shift) {
				local->select.from = local->select.to;
				local->select.fromid = local->select.from->id;
				local->select.m = local->select.n;
			}
			FindOffscreen(self)->markPaint(local);
			return 1;
		}
		else {
			if (e->shift) {
				self = local->select.to;
				if (self->parent->next->type == _ElemEnd) {
					local->select.to = self->parent->childend;
					local->select.toid = local->select.to->id;
					local->select.n = 0;
				}
				else {
					if (local->select.count < 0) {
						local->select.count = local->select.n;
						for (NewElement* elem = self->before; elem->type != _ElemEnd; elem = elem->before) {
							local->select.count += elem->len(elem);
						}
					}
					for (NewElement* elem = self->parent->next->childend->next; ; elem = elem->next) {
						if (elem->type == _ElemEnd) {
							local->select.to = elem;
							local->select.toid = local->select.to->id;
							local->select.n = 0;
							break;
						}
						else if (local->select.count <= elem->len(elem)) {
							local->select.to = elem;
							local->select.toid = local->select.to->id;
							local->select.n = local->select.count;
							break;
						}
					}
				}
				FindOffscreen(self)->markPaint(local);
				return 1;
			}
			else {
				local->select.from = local->select.to = local->select.end;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.e;
				FindOffscreen(self)->markPaint(local);
			}
			return 1;
		}
	}
	local->select.count = -1;
	if (e->key == SDLK_UNKNOWN) {
		FindOffscreen(self)->markLayout(local);
		UniteLine(thgc, local, (NewLine*)self->parent, (NewLine*)self->parent->next);
		return -2;
	}
	if (e->key == SDLK_KP_ENTER || e->key == SDLK_RETURN) {
		NewLine* newline = (NewLine*)GC_alloc(thgc, _LineC);
		initNewLine(thgc, newline);
		NewBefore(thgc, local, self->parent, newline);
		NewMoveElement(thgc, local, newline->childend, self->parent->childend->next, self->before);
		e->key = SDLK_UNKNOWN;
		FindOffscreen(self)->markLayout(local);
		return -1;
	}
	else if (e->key == SDLK_BACKSPACE) {
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (self->before->type == _ElemEnd) {
				if (self->parent->before->type == _ElemEnd) return 0;
				NewElement* el = self->parent->before->childend->before;
				local->select.from = local->select.to = el;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.from->len(local->select.from);
				UniteLine(thgc, local, (NewLine*)self->parent->before, (NewLine*)self->parent);
				UniteText(thgc, local, el, el->next);
			}
			else {
				if (self->before->type == LetterType::_Letter) {
					if (self->parent->parent->editable) {
						NewLetter* let = (NewLetter*)self->before;
						NewElement* before = let->before;
						if (AddText(thgc, local, let, let->text->size - 1, let->text->size, NULL) == -1) {
							local->select.from = local->select.to = before;
							local->select.fromid = local->select.toid = before->id;
							local->select.m = local->select.n = before->len(before);
						}
						else {
							local->select.from = local->select.to = let;
							local->select.fromid = local->select.toid = local->select.from->id;
							local->select.m = local->select.n = let->text->size - 1;
						}
					}
				}
			}
		}
		else {
			local->select.from = local->select.to = self->before;
			local->select.fromid = local->select.toid = local->select.from->id;
			local->select.m = local->select.n = local->select.from->len(local->select.from);
			UniteLine(thgc, local, (NewLine*)self->parent->before, (NewLine*)self->parent);
			e->key = SDLK_UNKNOWN;
			FindOffscreen(self)->markLayout(local);
			return -2;
		}
		FindOffscreen(self)->markLayout(local);
	}
	else if (e->key == SDLK_DELETE) {
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			NewElement* el = self->parent->childend->before;
			local->select.from = local->select.to = el;
			local->select.fromid = local->select.toid = local->select.from->id;
			local->select.m = local->select.n = local->select.from->len(local->select.from);
			UniteLine(thgc, local, (NewLine*)self->parent, (NewLine*)self->parent->next);
			UniteText(thgc, local, el, el->next);
		}
		else {
			local->select.from = local->select.to = self->parent->childend->before;
			local->select.fromid = local->select.toid = local->select.from->id;
			local->select.m = local->select.n = local->select.from->len(local->select.from);
			UniteLine(thgc, local, (NewLine*)self->parent->before, (NewLine*)self->parent);
			e->key = SDLK_UNKNOWN;
			FindOffscreen(self)->markLayout(local);
			return -2;
		}
		FindOffscreen(self)->markLayout(local);
	}
	else {
		if (self->before->type == LetterType::_Letter) {
			NewLetter* let = (NewLetter*)self->before;
			NewElement* before = let->before;
			if (AddText(thgc, local, let, let->text->size, let->text->size, e->text) == -1) {
				local->select.from = local->select.to = before;
				local->select.fromid = local->select.toid = before->id;
				local->select.m = local->select.n = before->len(before);
			}
			else {
				local->select.from = local->select.to = let;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = let->text->size;
			}
		}
		else {
			NewLetter* let = (NewLetter*)GC_alloc(thgc, CType::_LetterC);
			initNewLetter(thgc, let, self->parent->parent->font, LetterType::_Letter);
			let->text = e->text;
			NewBefore(thgc, local, self, let);
		}
		if (n == 1) {
			local->select.from = local->select.to = self->parent->before->childend->before;
			local->select.fromid = local->select.toid = local->select.from->id;
			local->select.m = local->select.n = local->select.from->len(local->select.from);
			UniteLine(thgc, local, (NewLine*)self->parent->before, (NewLine*)self->parent);
			e->key = SDLK_UNKNOWN;
			FindOffscreen(self)->markLayout(local);
			return -2;
		}
		FindOffscreen(self)->markLayout(local);
	}
	return -1;
}
int LetterMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local) {
	NewLetter* letter = (NewLetter*)self;
	for (int i = 0; i < letter->renderspans->size; i++) {
		auto rs = *(RenderSpan**)get_list(letter->renderspans, i);
		if (self->pos2.x + self->pos.x + rs->x <= e->x && e->x < self->pos2.x + self->pos.x + rs->x + rs->width) {
			if (self->pos2.y + self->pos.y + rs->y <= e->y && e->y < self->pos2.y + self->pos.y + rs->y + rs->height) {
				if (self->GoMouseDown != NULL) {
					MemObj* mo = (MemObj*)GC_clone(thgc, (char*)self->GoMouseDown->obj);
					MemTable* res = (MemTable*)GC_alloc(thgc, _MemTable);
					res->table = (Map*)create_mapy(thgc, _List);
					MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
					mo->req = NULL; mo->res = res;
					auto rn = GC_add_root_node(thgc);
					auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->GoMouseDown->func, (char*)mo)));
					h.promise().state = -2;
					thgc->queue->push(h);
				}
				float width;
				String* str = SubString(thgc, letter->text, rs->start, rs->end - rs->start);
				size_t n;
				float hei;
				MeasureString(*getAtlas(thgc), rs->font, str, str->size, e->x - self->pos2.x - self->pos.x - rs->x, &width, &hei, &n, e->group);
				if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN) {
					local->select.from = local->select.to = self;
					local->select.fromid = local->select.toid = self->id;
					local->select.m = local->select.n = rs->start + n;
				}
				else if (e->action == SDL_EVENT_MOUSE_BUTTON_UP || e->click) {
					local->select.to = self;
					local->select.toid = self->id;
					local->select.n = rs->start + n;
				}

				if (self->BackMouseDown != NULL) {
					MemObj* mo = (MemObj*)GC_clone(thgc, (char*)self->BackMouseDown->obj);
					MemTable* res = (MemTable*)GC_alloc(thgc, _MemTable);
					res->table = (Map*)create_mapy(thgc, _List);
					MemInsert(thgc, res, _MouseEvent, (ModelVal*)e);
					mo->req = NULL; mo->res = res;
					auto rn = GC_add_root_node(thgc);
					auto h = std::coroutine_handle<Generator::promise_type>::from_address(static_cast<void*>(MakeFrame(rn, self->BackMouseDown->func, (char*)mo)));
					h.promise().state = -2;
					thgc->queue->push(h);
				}
				return n;
			}
		}
	}
	return -1;
}
int LetterKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local) {
	if (!self->parent->parent->editable) return 0;
	NewLetter* letter = (NewLetter*)self;
	if (e->key == SDLK_LEFT) {
		local->select.count = -1;
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (m == 0) {
				if (self->before->type == _ElemEnd) {
					if (self->parent->before->type == _ElemEnd) return 1;
					local->select.to = self->parent->before->childend;
					local->select.toid = local->select.to->id;
					local->select.n = 0;
				}
				else {
					local->select.to = self->before;
					local->select.toid = local->select.to->id;
					local->select.n = self->before->len(self->before) - 1;
				}
				if (!e->shift) {
					local->select.from = local->select.to;
					local->select.fromid = local->select.to->id;
					local->select.m = local->select.n;
				}
			}
			else {
				local->select.n = m - 1;
				if (!e->shift) {
					local->select.m = local->select.n;
				}
			}
			FindOffscreen(letter)->markPaint(local);
			return 1;
		}
		else {
			if (e->shift) {
				self = local->select.to;
				if (local->select.n == 0) {
					if (self->before->type == _ElemEnd) {
						if (self->parent->before->type == _ElemEnd) return 1;
						local->select.to = self->parent->before->childend;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
					}
					else {
						local->select.to = self->before;
						local->select.toid = local->select.to->id;
						local->select.n = self->before->len(self->before) - 1;
					}
				}
				else {
					local->select.n--;
				}
				FindOffscreen(letter)->markPaint(local);
				return 1;
			}
			else {
				local->select.from = local->select.to = local->select.start;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.s;
			}
			FindOffscreen(letter)->markPaint(local);
			return 1;
		}
	}
	else if (e->key == SDLK_RIGHT) {
		local->select.count = -1;
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (m == letter->text->size) {
				if (self->next->type == _ElemEnd) {
					if (self->parent->before->type == _ElemEnd) return 1;
					local->select.to = self->parent->next->childend->next;
					local->select.toid = local->select.to->id;
					local->select.n = 0;
				}
				else {
					local->select.to = self->next;
					local->select.toid = local->select.to->id;
					local->select.n = 1;
				}
				if (!e->shift) {
					local->select.from = local->select.to;
					local->select.fromid = local->select.to->id;
					local->select.m = local->select.n;
				}
			}
			else {
				local->select.n = m + 1;
				if (!e->shift) {
					local->select.m = local->select.n;
				}
			}
			FindOffscreen(letter)->markPaint(local);
			return 1;
		}
		else {
			if (e->shift) {
				self = local->select.to;
				if (local->select.n == self->len(self)) {
					if (self->next->type == _ElemEnd) {
						if (self->parent->next->type == _ElemEnd) return 0;
						local->select.to = self->parent->next->childend->next;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
					}
					else {
						local->select.to = self->next;
						local->select.toid = local->select.to->id;
						local->select.n = 1;
					}
				}
				else {
					if (self->type == _ElemEnd) {
						if (self->parent->next->type == _ElemEnd) return 0;
						local->select.to = self->parent->next->childend->next;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
					}
					local->select.n++;
				}
				FindOffscreen(letter)->markPaint(local);
				return 1;
			}
			else {
				local->select.from = local->select.to = local->select.end;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.e;
			}
			FindOffscreen(letter)->markPaint(local);
			return 1;
		}
	}
	else if (e->key == SDLK_UP) {
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (self->parent->before->type == _ElemEnd) {
				local->select.to = self->parent->childend->next;
				local->select.toid = local->select.to->id;
				local->select.n = 0;
			}
			else {
				if (local->select.count < 0) {
					local->select.count = m;
					for (NewElement* elem = self->before; elem->type != _ElemEnd; elem = elem->before) {
						local->select.count += elem->len(elem);
					}
				}
				for (NewElement* elem = self->parent->before->childend->next; ; elem = elem->next) {
					if (elem->type == _ElemEnd) {
						local->select.to = elem;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
						break;
					}
					else if (local->select.count <= elem->len(elem)) {
						local->select.to = elem;
						local->select.toid = local->select.toid;;
						local->select.n = local->select.count;
						break;
					}
				}
			}
			if (!e->shift) {
				local->select.from = local->select.to;
				local->select.fromid = local->select.from->id;
				local->select.m = local->select.n;
			}
			FindOffscreen(letter)->markPaint(local);
			return 1;
		}
		else {
			if (e->shift) {
				self = local->select.to;
				if (self->parent->before->type == _ElemEnd) {
					local->select.to = self->parent->childend->next;
					local->select.toid = local->select.to->id;
					local->select.n = 0;
				}
				else {
					if (local->select.count < 0) {
						local->select.count = local->select.n;
						for (NewElement* elem = self->before; elem->type != _ElemEnd; elem = elem->before) {
							local->select.count += elem->len(elem);
						}
					}
					for (NewElement* elem = self->parent->before->childend->next; ; elem = elem->next) {
						if (elem->type == _ElemEnd) {
							local->select.to = elem;
							local->select.toid = local->select.to->id;
							local->select.n = 0;
							break;
						}
						else if (local->select.count <= elem->len(elem)) {
							local->select.to = elem;
							local->select.toid = local->select.to->id;
							local->select.n = local->select.count;
							break;
						}
					}
				}
				FindOffscreen(letter)->markPaint(local);
				return 1;
			}
			else {
				local->select.from = local->select.to = local->select.start;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.s;
			}
			FindOffscreen(letter)->markPaint(local);
			return 1;
		}
	}
	else if (e->key == SDLK_DOWN) {
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (self->parent->next->type == _ElemEnd) {
				local->select.to = self->parent->childend;
				local->select.toid = local->select.to->id;
				local->select.n = 0;
			}
			else {
				if (local->select.count < 0) {
					local->select.count = m;
					for (NewElement* elem = self->before; elem->type != _ElemEnd; elem = elem->before) {
						local->select.count += elem->len(elem);
					}
				}
				for (NewElement* elem = self->parent->next->childend->next; ; elem = elem->next) {
					if (elem->type == _ElemEnd) {
						local->select.to = elem;
						local->select.toid = local->select.to->id;
						local->select.n = 0;
						break;
					}
					else if (local->select.count <= elem->len(elem)) {
						local->select.to = elem;
						local->select.toid = local->select.to->id;
						local->select.n = local->select.count;
						break;
					}
				}
			}
			if (!e->shift) {
				local->select.from = local->select.to;
				local->select.fromid = local->select.from->id;
				local->select.m = local->select.n;
			}
			FindOffscreen(letter)->markPaint(local);
			return 1;
		}
		else {
			if (e->shift) {
				self = local->select.to;
				if (self->parent->next->type == _ElemEnd) {
					local->select.to = self->parent->childend;
					local->select.toid = local->select.to->id;
					local->select.n = 0;
				}
				else {
					if (local->select.count < 0) {
						local->select.count = local->select.n;
						for (NewElement* elem = self->before; elem->type != _ElemEnd; elem = elem->before) {
							local->select.count += elem->len(elem);
						}
					}
					for (NewElement* elem = self->parent->next->childend->next; ; elem = elem->next) {
						if (elem->type == _ElemEnd) {
							local->select.to = elem;
							local->select.toid = local->select.to->id;
							local->select.n = 0;
							break;
						}
						else if (local->select.count <= elem->len(elem)) {
							local->select.to = elem;
							local->select.toid = local->select.to->id;
							local->select.n = local->select.count;
							break;
						}
					}
				}
				FindOffscreen(letter)->markPaint(local);
				return 1;
			}
			else {
				local->select.from = local->select.to = local->select.end;
				local->select.fromid = local->select.toid = local->select.from->id;
				local->select.m = local->select.n = local->select.e;
				FindOffscreen(letter)->markPaint(local);
			}
			return 1;
		}
	}
	local->select.count = -1;
	if (e->key == SDLK_UNKNOWN) {
		FindOffscreen(letter)->markLayout(local);
		int n2 = AddText(thgc, local, letter, m, n, NULL);
		if (n2 != -1) {
			UniteText(thgc, local, letter->before, letter);
		}
		return n2;
	}
	else if (e->key == SDLK_KP_ENTER || e->key == SDLK_RETURN) {
		String* head = SubString(thgc, letter->text, 0, m);
		String* tail = SubString(thgc, letter->text, n, letter->text->size - n);
		NewElement* nowline = self->parent;
		NewLine* newline = (NewLine*)GC_alloc(thgc, _LineC);
		initNewLine(thgc, newline);
		NewBefore(thgc, local, self->parent, newline);
		NewMoveElement(thgc, local, newline->childend, self->parent->childend->next, letter);
		if (head->size == 0) {
			NewRemove(thgc, local, self);
		}
		else {
			letter->text = head;
			letter->recompile = true;
		}
		if (tail->size != 0) {
			NewLetter* let2 = (NewLetter*)GC_alloc(thgc, CType::_LetterC);
			initNewLetter(thgc, let2, letter->font, LetterType::_Letter);
			NewNext(thgc, local, nowline->childend, let2);
			let2->text = tail;
			local->select.from = local->select.to = let2;
			local->select.fromid = local->select.toid = let2->id;
			local->select.m = local->select.n = 0;
		}
		else {
			local->select.from = local->select.to = nowline->childend;
			local->select.fromid = local->select.toid = nowline->id;
			local->select.m = local->select.n = 1;
		}
		e->key = SDLK_UNKNOWN;
		FindOffscreen(letter)->markLayout(local);
		return -1;
	}
	else if (e->key == SDLK_BACKSPACE) {
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (m == 0) {
				if (letter->before->type == _ElemEnd) {
					if (letter->parent->before->type == _ElemEnd) return 0;
					NewElement* el = self->parent->before->childend->before;
					local->select.from = local->select.to = el;
					local->select.fromid = local->select.toid = local->select.from->id;
					local->select.m = local->select.n = local->select.from->len(local->select.from);
					UniteLine(thgc, local, (NewLine*)letter->parent->before, (NewLine*)letter->parent);
					UniteText(thgc, local, el, el->next);
				}
				else {
					if (letter->before->type == LetterType::_Letter) {
						if (letter->parent->parent->editable) {
							NewLetter* let = (NewLetter*)letter->before;
							NewElement* before = let->before;
							if (AddText(thgc, local, let, let->text->size - 1, let->text->size, NULL) == -1) {
								local->select.from = local->select.to = before;
								local->select.fromid = local->select.toid = before->id;
								local->select.m = local->select.n = before->len(before);
							}
							else {
								local->select.from = local->select.to = let;
								local->select.fromid = local->select.toid = local->select.from->id;
								local->select.m = local->select.n = let->text->size - 1;
							}
						}
					}
				}
			}
			else {
				NewElement* before = self->before;
				if (AddText(thgc, local, letter, m - 1, m, NULL) == -1) {
					local->select.from = local->select.to = before;
					local->select.fromid = local->select.toid = before->id;
					local->select.m = local->select.n = before->len(before);
				}
				else local->select.m = local->select.n = m - 1;
			}
		}
		else {
			NewElement* before = self->before;
			if (AddText(thgc, local, letter, m - 1, m, NULL) == -1) {
				local->select.from = local->select.to = before;
				local->select.fromid = local->select.toid = before->id;
				local->select.m = local->select.n = before->len(before);
			}
			else local->select.m = local->select.n = m;
			e->key = SDLK_UNKNOWN;
		}
		FindOffscreen(letter)->markLayout(local);
	}
	else if (e->key == SDLK_DELETE) {
		if (local->select.from == local->select.to && local->select.m == local->select.n) {
			if (m == letter->text->size) {
				if (letter->next->type == _ElemEnd) {
					if (letter->parent->next->type == _ElemEnd) return 0;
					NewElement* el = self->parent->childend->before;
					local->select.from = local->select.to = el;
					local->select.fromid = local->select.toid = local->select.from->id;
					local->select.m = local->select.n = local->select.from->len(local->select.from);
					UniteLine(thgc, local, (NewLine*)letter->parent, (NewLine*)letter->parent->next);
					UniteText(thgc, local, el, el->next);
				}
				else {
					if (letter->next->type == LetterType::_Letter) {
						if (letter->parent->parent->editable) {
							NewLetter* let = (NewLetter*)letter->next;
							NewElement* before = let->before;
							if (AddText(thgc, local, letter, m - 1, m, NULL) == -1) {
								local->select.from = local->select.to = before;
								local->select.fromid = local->select.toid = before->id;
								local->select.m = local->select.n = before->len(before);
								return -1;
							}
							else {
								local->select.from = local->select.to = let;
								local->select.fromid = local->select.toid = local->select.from->id;
								local->select.m = local->select.n = 0;
							}
						
						}
					}
				}
			}
			else {
				NewElement* before = self->before;
				if (AddText(thgc, local, letter, m - 1, m, NULL) == -1) {
					local->select.from = local->select.to = before;
					local->select.fromid = local->select.toid = before->id;
					local->select.m = local->select.n = before->len(before);
					return -1;
				}
			}
		}
		else {
			AddText(thgc, local, letter, m, n, NULL);

			NewElement* before = self->before;
			if (AddText(thgc, local, letter, m - 1, m, NULL) == -1) {
				local->select.from = local->select.to = before;
				local->select.fromid = local->select.toid = before->id;
				local->select.m = local->select.n = before->len(before);
				return -1;
			}
			else local->select.m = local->select.n = m;
			e->key = SDLK_UNKNOWN;
		}
		FindOffscreen(letter)->markLayout(local);
	}
	else {
		AddText(thgc, local, letter, m, n, e->text);
		local->select.from = local->select.to = self;
		local->select.fromid = local->select.toid = self->id;
		local->select.m = m + e->text->size;
		local->select.n = local->select.m;
		FindOffscreen(letter)->markLayout(local);
		e->key = SDLK_UNKNOWN;
		return 0;
	}
	return -1;
}