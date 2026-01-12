
#pragma once
#include <map>
#include <string>
#include <vector>
#include <bgfx/bgfx.h>
#include <limits>
enum DrawCommandType {
	DelFBO = -2, FBO = -1, Fill = 0, Gradient = 1, Stripe = 2, Checker = 3, GradientChecker = 4,
	Image = 5, End = 7, PageCurl = 6
};
typedef struct NewElement;
typedef struct DrawCommand;
struct DrawCommand;
struct RectDrawCommand;
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

	// コピーを禁止（ダブル delete 防止）
	LayerInfo(const LayerInfo&) = delete;
	LayerInfo& operator=(const LayerInfo&) = delete;

	// ムーブは許可
	LayerInfo(LayerInfo&& other) noexcept
		: cmds(std::move(other.cmds))
		// ... 他のメンバーもムーブ ...
	{
	}

	LayerInfo& operator=(LayerInfo&& other) noexcept {
		if (this != &other) {
			// 既存のコマンドを解放
			for (auto* cmd : cmds) {
				delete cmd;
			}
			cmds = std::move(other.cmds);
			// ... 他のメンバーもムーブ ...
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
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);
void  pushImage(float x, float y, float width, float height,
	float atlasX, float atlasY, float atlasW, float atlasH,
	float radius, float aaPixels, float borderWidth,
	uint32_t borderColor,
	float shadowX, float shadowY, float shadowBlur,
	uint32_t shadowColor, uint32_t modulate,
	float zIndex, bgfx::TextureHandle* tex1,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);
void  pushPattern(enum DrawCommandType patternMode,
	float x, float y, float width, float height,
	float colorCount, float angle,
	float scrollX, float scrollY,
	float radius, float aaPixels,
	float borderWidth, uint32_t borderColor,
	float shadowX, float shadowY, float shadowBlur,
	uint32_t shadowColor,
	int dataOffset, float zIndex,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);
void  pushText(float x, float y, float width, float height,
	float atlasX, float atlasY, float atlasW, float atlasH,
	uint32_t color, float zIndex, bgfx::TextureHandle* tex1,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);
void pushPageCurl(float x, float y, float width, float height,
	float progress, float curlRadius, float curlAngle,
	uint32_t fillColor,
	float uvMinX, float uvMinY, float uvSizeX, float uvSizeY,
	float backUVMinX, float backUVMinY, float borderWidth,
	uint32_t borderColor,
	float shadowX, float shadowY, float shadowBlur,
	uint32_t shadowColor,
	bgfx::TextureHandle* tex1, bgfx::TextureHandle* tex2, float zIndex,
	bgfx::FrameBufferHandle* targetFBO, PointI* fbsize, uint8_t viewId);

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
typedef struct Background {
	enum DrawCommandType type;
	uint32_t fillcolor;
	ImageId tex1;
	float aa;
	float borderRadius, borderWidth;
	uint32_t borderColor;
	float shadowX, shadowY, shadowBlur;
	uint32_t shadowColor;
	float count, offset, angle;
	float scrollX, scrollY;
};
typedef struct Offscreen;
typedef struct NewEndElement {
	struct NewElement* next, * before, * parent, * childend;
	enum LetterType type;
	unsigned long id;
};
void initNewEndElement(ThreadGC* thgc, NewEndElement* end, NewElement* parent) {
	end->next = end->before = (NewElement*)end;
	end->parent = parent;
	end->type = LetterType::_ElemEnd;
	end->id = 0;
}
typedef struct PointF {
	float x;
	float y;
} PointF;
typedef struct NewElement;
typedef struct NewMeasure {
	NewElement* base;
	PointF pos, size;
	PointF start;
	ExtendedRenderGroup* group;
} NewMeasure;
typedef struct NewGraphic {
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
} NewGraphic;
typedef struct NewLocal;
typedef struct NewElement : NewEndElement {
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
	int (*Mouse)(ThreadGC*, struct NewElement*, MouseEvent*, Local*);
	int (*Key)(ThreadGC*, struct NewElement*, KeyEvent*, Local*, bool*);
	bool offscreened = false;
	Offscreen* offscreen;
} NewElement;
typedef struct HoppyWindow HoppyWindow;
enum DirtyType {
	None,
	Partial = 1,
	Rebuild = 2,
	RebuildValue = 3,
};
typedef struct NewLocal : NewElement {
	HoppyWindow* window;
	Select* selects[2];
	int seln;
	int countn;
	DirtyType dirty;
	List* screens;
	Map* temap;
};
struct RenderGroup;
typedef struct Offscreen {
	ImageId imPing;
	ImageId imPong;
	bgfx::FrameBufferHandle fbPing;
	bgfx::FrameBufferHandle fbPong;
	PointI fbsize;
	bool ping = true;
	bool layout = true;
	bool paint = true;
	NewElement* elem;
	void markLayout(NewLocal* local) { layout = true; paint = true; local->dirty = (DirtyType)(local->dirty | DirtyType::Partial); }
	void markPaint(NewLocal* local) { paint = true; local->dirty = (DirtyType)(local->dirty | DirtyType::Partial); }
	int viewId = 0;
	ExtendedRenderGroup *group;
};
static int viewId = 0;

typedef struct NewLine : NewElement {
	bool recompile;
};
typedef struct NewLetter : NewElement {
	String* text;
	FontId font;
	uint32_t color;
};
typedef struct NewSelect {
	NewElement* start, * end;
	int m, n;
	bool* select;
} NewSelect;
void ResetId(NewElement* elem, int* n) {
	for (NewElement* child = elem->childend->next; child->type != LetterType::_ElemEnd; child = child->next) {
		child->id = (*n)++ * 65536 * 65536 * 65536;
		ResetId(child, n);
	}
	elem->childend->id = (*n)++ * 65536 * 65536 * 65536;
}
void SetChildIds(NewElement* elem) {

	uint64_t startId = elem->id;
	uint64_t endId = elem->childend->id;

	// 子要素の数を数える
	int childCount = 0;
	for (auto el = elem->childend->next; el->type != LetterType::_ElemEnd; el = el->next) {
		childCount++;
		if (el->childend != NULL) {
			childCount++;  // childend分も
		}
	}

	if (childCount == 0) return;

	// 各子要素にIDを割り当て
	uint64_t step = (endId - startId) / (childCount + 1);
	uint64_t currentId = startId;

	for (auto el = elem->childend->next; el->type != LetterType::_ElemEnd; el = el->next) {
		currentId += step;
		el->id = currentId;

		if (el->childend != NULL) {
			currentId += step;
			el->childend->id = currentId;
			SetChildIds(el);  // 再帰
		}
	}
}
bool CheckOffscreen(NewElement* elem) {
	if (elem->offscreen || elem->xtype == SizeType::Page || elem->xtype == SizeType::Scroll || elem->ytype == SizeType::Page || elem->ytype == SizeType::Scroll) {
		return true;
	}
	return false;

}
void NewBefore(NewLocal* local, NewElement* self, NewElement* elem) {
	elem->next = self;
	elem->before = self->before;
	self->before->next = elem;
	self->before = elem;
	elem->parent = self->parent;
	NewElement* before;
	if (self->before->type == LetterType::_ElemEnd) {
		before = self->parent;
	}
	else {
		before = self->before;
		if (before->childend != NULL) before = before->childend;
	}
	if (elem->childend != NULL) {
		if (self->id - before->id <= 2) {
			int n = 1;
			ResetId(local, &n);
		}
		else {
			elem->id = self->id + (before->id - self->id) / 3;
			elem->childend->id = self->id + (self->next->id - before->id) / 3 * 2;
		}
		SetChildIds(elem);
	}
	else elem->id = (self->id + before->id) / 2;
}
void NewNext(NewLocal* local, NewElement* self, NewElement* elem) {
	elem->before = self;
	elem->next = self->next;
	self->next->before = elem;
	self->next = elem;
	elem->parent = self->parent;
	NewElement* before;
	if (self->type == LetterType::_ElemEnd) {
		before = self->parent;
	}
	else {
		before = self;
		if (before->childend != NULL) before = before->childend;
	}
	if (elem->childend != NULL) {
		if (self->next->id - before->id <= 2) {
			int n = 1;
			ResetId(local, &n);
		}
		else {
			elem->id = (self->next->id - before->id) / 3;
			elem->childend->id = (self->next->id - before->id) / 3 * 2;
		}
		SetChildIds(elem);
	}
	else elem->id = (self->id + before->id) / 2;
}
Offscreen* FindOffscreen(NewElement* elem) {
	for (; elem != NULL; elem = elem->parent) {
		if (elem->offscreen != NULL) return elem->offscreen;
	}
	return NULL;
}
void NewRemoveElement(NewElement* elem) {
	elem->before->next = elem->next;
	elem->next->before = elem->before;
}
void RebuildOffscreen(ThreadGC* thgc, List* screens, NewElement* elem) {
	for(NewElement* child = elem->childend->next; child->type != LetterType::_ElemEnd; child = child->next) {
		if (child->offscreen != NULL) add_list(thgc, screens, (char *)child->offscreen);
		if (child->childend != NULL) RebuildOffscreen(thgc, screens, child);
	}
}
void NewNextElement(ThreadGC* thgc, NewLocal* local, NewElement* before, NewElement* elem) {
	if (elem->parent != NULL) {
		NewNext(local, before, elem);
		local->dirty = DirtyType::RebuildValue;
	}
	else {
		NewNext(local, before, elem);
		if (CheckOffscreen(elem)) {
			elem->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
			elem->offscreen->group = &createGroup(thgc);
			elem->offscreen->elem = elem;
		}
		FindOffscreen(elem->parent)->markLayout(local);
	}

}
void DeleteNewElement(NewLocal* local, NewElement* elem) {
	FindOffscreen(elem->parent)->markLayout(local);
	NewRemoveElement(elem);
}
void changeOrient(NewLocal* local, NewElement* elem, bool orient) {
	elem->orient = orient;
	FindOffscreen(elem)->markLayout(local);
}
void changeXtype(ThreadGC* thgc, NewLocal* local, NewElement* elem, SizeType xtype) {
	if (elem->xtype == xtype) return;
	FindOffscreen(elem)->markLayout(local);
	elem->xtype = xtype;
	if (elem->offscreen == NULL) {
		if (CheckOffscreen(elem)) {
			elem->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
			elem->offscreen->group = &createGroup(thgc);
			elem->offscreen->elem = elem;
		}
	}
	else {
		if (!CheckOffscreen(elem)) {
			elem->offscreen = NULL;
		}
	}
}
void changeYType(ThreadGC* thgc, NewLocal* local, NewElement* elem, SizeType ytype) {
	if (elem->ytype == ytype) return;
	elem->ytype = ytype;
	FindOffscreen(elem)->markLayout(local);
	if (elem->offscreen == NULL) {
		if (CheckOffscreen(elem)) {
			elem->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
			elem->offscreen->group = &createGroup(thgc);
			elem->offscreen->elem = elem;
		}
	}
	else {
		if (!CheckOffscreen(elem)) {
			elem->offscreen = NULL;
		}
	}
}
void changeOffscreen(ThreadGC* thgc, NewLocal* local, NewElement* elem, bool offscreened) {
	if (elem->offscreened == offscreened) return;
	elem->offscreened = offscreened;
	if (offscreened) {
		if (elem->offscreen == NULL) {
			FindOffscreen(elem)->markLayout(local);
			elem->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
			elem->offscreen->group = &createGroup(thgc);
			elem->offscreen->elem = elem;
		}
	}
	else {
		if (elem->offscreen != NULL) {
			FindOffscreen(elem)->markLayout(local);
			elem->offscreen = NULL;
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
}// ========== 合成パラメータ（Dirtyにしない） ========== 
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
		NewBefore(local, parent->childend, line);
	}
	NewBefore(local, parent->childend->before->childend, child);
	FindOffscreen(parent)->markLayout(local);
}
void NewLineAddLast(ThreadGC* thgc, NewLocal* local, NewElement* parent, NewLine* line) {
	NewBefore(local, parent->childend, line);
	FindOffscreen(parent)->markLayout(local);
}


void NewMeasureCall(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	if (elem->offscreen != NULL) {
		measure = new NewMeasure();
		measure->base = elem;
		measure->pos = { 0, 0 };
		measure->size = { elem->size.x, elem->size.y };
	}
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
		drawUnderPagingBar(g->layer, *getAtlas(thgc), getFont("sans", 16), elem->pos2.x, sizey + 110.0f, sizex, 5.0f, 3.0, 16.0, 0.0f, g->group, g->fb, g->fbsize, g->viewId);
		drawRightPagingBar(g->layer, *getAtlas(thgc), getFont("sans", 16), sizex + 10.0f, elem->pos2.y, 5.0f, 100.0f, 3.0, 16.0, 0.0f, g->group, g->fb, g->fbsize, g->viewId);
		drawUnderScrollBar(g->layer, elem->pos2.x, sizey + 100.0f, sizex, 5.0f, 100.0f, 50.0f, 300.0f, 0.0f, g->fb, g->fbsize, g->viewId);
		drawRightScrollBar(g->layer, sizex, elem->pos2.y, 5.0f, 100.0f, 100.0f, 50.0f, 300.0f, 0.0f, g->fb, g->fbsize, g->viewId);
		pro += 0.004f;
		pro = fmod(pro, 1.0f);
		auto info2 = mygetStandaloneTextureInfo(thgc, elem->background->tex1);
		g->layer->pushPageCurl(elem->pos2.x + 10, elem->pos2.y + 150.f, sizex, sizey,
			pro, -0.35f, 0.44f, 0xFFFFFFFF, 0.0f, 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0x000000FF, 1.0f, 1.0f, 1.0f, 0x000000FF, &info->handle, &info2->handle, 0.0f, g->fb, g->fbsize, g->viewId);
		g = new NewGraphic{g->layer, elem, elem, {0,0}, {elem->size2.x, elem->size2.y}, {0,0}, {0,0},
			elem->offscreen->ping ? elem->offscreen->imPong : elem->offscreen->imPing,  &info->fbo,  &info->size, elem->offscreen->viewId = viewId--, elem->offscreen->group };
		elem->offscreen->ping = !elem->offscreen->ping;
		std::vector<float>* colors = new std::vector<float>{
			1.0f, 0.0f, 0.0f, 1.0f,  // 赤
			0.0f, 1.0f, 0.0f, 1.0f,  // 緑
			0.0f, 0.0f, 1.0f, 1.0f,  // 青
		};
		std::vector<float>* widths = new std::vector<float>{ 10.0f, 10.0f, 10.0f };
		int size = widths->size();
		int n = addPattern(thgc, *colors, *widths);
		//g->layer->pushFill(elem->pos.x, elem->pos.y, elem->size2.x / 3 * 2, elem->size2.y / 3 * 2, 6.0f, 1.0f, 1.0, 0xffffffff, 0x000000ff, 3.0f, 3.0f, 12.0f, 0x00ff00ff, 0, g->fb, g->fbsize, g->viewId);
		g->layer->pushPattern(DrawCommandType::GradientChecker, elem->pos.x, elem->pos.y, elem->size2.x / 3 * 2, elem->size2.y / 3 * 2, size, 0.0f, 0.0f, 0.0f, 6.0f, 1.0f, 1.0, 0x000000FF, 3.0f, 3.0f, 2.0f, 0x00ff00FF, n, 0, g->fb, g->fbsize, g->viewId);
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
	end->id = std::numeric_limits<unsigned long>::max();
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
	local->Mouse = NULL;
	local->Key = NULL;
	local->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
	local->offscreen->group = &createGroup(thgc);
	local->offscreen->markLayout(local);
	local->offscreen->elem = local;
	local->offscreen->imPing = queueOffscreenNew(thgc, 1, 1);
	local->offscreen->imPong = queueOffscreenNew(thgc, 1, 1);
	local->offscreened = true;
	local->background = new Background();
	local->background->tex1 = myloadTexture2D(thgc, "123.png");
	local->dirty = DirtyType::RebuildValue;
	local->screens = create_list(thgc, sizeof(Offscreen*), true);
}
#include "Compile.h"
void RecompileLine(ThreadGC* thgc, NewLocal* local, NewLine* line) {
	if (!line->recompile) return;
	NewElement* start = line->childend;
	String* text = (String*)GC_alloc(thgc, CType::_String);
	FontId* font = NULL;
	for (NewElement* child = line->childend->next; ;) {
		if (child->type == LetterType::_Letter) {
			NewLetter* letter = (NewLetter*)child;
			font = &letter->font;
			text = StringAdd2(thgc, text, letter->text);
			NewRemoveElement(child);
		}
		else {
			if (start->next == child) continue;
			List* l = Compile(thgc, text, *font);
			for (auto i = l->size - 1; i >= 0; i--) {
				NewElement* elem = (NewElement*)get_list(l, i);
				NewNext(local, start, elem);
			}
			if (child->type == LetterType::_ElemEnd) {
				break;
			}
			else start = child;
		}
	}
}
void NewLineMeasureCall(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	RecompileLine(thgc, local, (NewLine*)elem);
	bool offscreened = false;
	if (elem->xtype == SizeType::Scroll) { measure->size.y -= 5; offscreened = true; }
	else if (elem->xtype == SizeType::Range) { offscreened = true; }
	if (elem->ytype == SizeType::Scroll) { measure->size.x -= 5; offscreened = true; }
	else if (elem->ytype == SizeType::Range) { offscreened = true; }
	else if (elem->ytype == SizeType::Break) { measure->size.y -= 10; }
	if (elem->type == LetterType::_Main) { offscreened = true; }
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
void initNewLine(ThreadGC* thgc, NewLine* line) {
	line->type = LetterType::_Line;
	line->next = line;
	line->before = line;
	line->parent = NULL;
	line->childend = (NewElement*)GC_alloc(thgc, CType::_EndC);
	initNewEndElement(thgc, (NewEndElement*)line->childend, line);
	line->orient = true;
	line->id = 0;
	line->Measure = NewLineMeasureCall;
	line->Draw = NewDrawCall;
}
void NewLetterMeasureCall(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	NewLetter* letter = (NewLetter*)elem;
	String* text = letter->text;
	FontId font = letter->font;
	float width, height;
	size_t nn;
	MeasureString(*getAtlas(thgc), font, text, text->size, 10000.0f, &width, &height, &nn, measure->group);
	measure->pos.x += width;
	elem->size.x = width;
	elem->size.y = height;
	if (measure->size.y < height) measure->size.y = height;
}
bgfx::FrameBufferHandle nullfb = BGFX_INVALID_HANDLE;
void NewLetterDrawCall(ThreadGC* thgc, NewElement* elem, NewGraphic* g, NewLocal* local, RenderCommandQueue* q) {
	NewLetter* letter = (NewLetter*)elem;
	String* text = letter->text;
	FontId font = letter->font;
	uint32_t color = letter->color;
	drawString((LayerInfo*)g->layer, *getAtlas(thgc), font, text, g->pos.x, g->pos.y, std::floor(elem->zIndex) + 0.9,
		color, g->group, g->fb, g->fbsize, g->viewId);
}
void initNewLetter(ThreadGC* thgc, NewLetter* letter, FontId font, enum LetterType type) {
	letter->type = type;
	letter->next = NULL;
	letter->before = NULL;
	letter->parent = NULL;
	letter->childend = NULL;
	letter->id = 0;
	letter->Measure = NewLetterMeasureCall;
	letter->Draw = NewLetterDrawCall;
	letter->text = NULL;
	letter->font = font;
}