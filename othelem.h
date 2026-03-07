#pragma once
struct NewImage : NewElement {

};
void NewImageMeasure(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	return;
}
void NewImageDraw(ThreadGC* thgc, NewElement* elem, NewGraphic* g, NewLocal* local, RenderCommandQueue* q) {
	bgfx::TextureHandle* tex1 = &nulltex;
	bgfx::TextureHandle* tex2 = &nulltex;
	if (isValidImageId(elem->background->tex1)) {
		ResolvedTexture rt = myResolveForDraw(thgc, elem->background->tex1);
		if (rt.isValid()) {
			tex1 = rt.texture;
			elem->background->angle = rt.u0;
			elem->background->curl = rt.v0;
			elem->background->scrollX = rt.u1;
			elem->background->scrollY = rt.v1;
		}
	}
	if (isValidImageId(elem->background->tex2)) {
		ResolvedTexture rt2 = myResolveForDraw(thgc, elem->background->tex2);
		if (rt2.isValid()) tex2 = rt2.texture;
	}
	g->layer->pushBackground(elem->background, g->pos.x, g->pos.y, elem->size.x, elem->size.y, elem->zIndex + 0.1,
		tex1, tex2, g->fb, g->fbsize, g->viewId, 0.0f);
}
void ImageDrawSelect(ThreadGC* thgc, NewLocal* local, NewElement* self, int m, int n, NewGraphic* g, PointF pos, RenderCommandQueue* q) {
	if (m == n) return;
	g->layer->pushFill(pos.x + self->pos.x + self->pos2.x, self->pos.y + self->pos2.y, pos.y + self->size.x + 1, self->size.y, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x4477ff66, 0, 0.0f, 0.0f, 1.0f, 0, 12000.0f, g->fb, g->fbsize, g->viewId);
}
int ImageMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, PointF pos, NewLocal* local) {
	NewImage* letter = (NewImage*)self;
	if (self->pos2.x + self->pos.x <= e->x - pos.x && e->x - pos.x < self->pos2.x + self->pos.x + self->size.x) {
		if (self->pos2.y + self->pos.y <= e->y - pos.y && e->y - pos.y < self->pos2.y + self->pos.y + self->size.y) {
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
			if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				local->select.from = local->select.to = self;
				local->select.fromid = local->select.toid = self->id;
				local->select.m = local->select.n = 0;
				Offscreen* seloff = FindOffscreen(self);
				local->select.window = seloff ? seloff->window : nullptr;
			}
			else if (e->action == SDL_EVENT_MOUSE_BUTTON_UP || e->click) {
				Offscreen* tooff = FindOffscreen(self);
				NativeWindow* toWin = tooff ? tooff->window : nullptr;
				if (toWin == local->select.window) {
					local->select.to = self;
					local->select.toid = self->id;
					local->select.n = 0;
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
			return 0;
		}
	}
	return -1;
}
void initNewImage(ThreadGC* thgc, NewImage* im1) {
	im1->type = LetterType::_Image;
	im1->next = im1;
	im1->before = im1;
	im1->parent = NULL;
	im1->childend = NULL;
	im1->Measure = NewImageMeasure;
	im1->Draw = NewImageDraw;
	im1->Mouse = ImageMouse;
	im1->Key = ElementKey;
	im1->DrawSelection = ImageDrawSelect;
	im1->len = len1;
}
struct PopupWindow : NewElement {
	bool visible;
	int cornerRadius;
};
struct NewDown;
struct NewDrop : NewElement {
	PopupWindow* pw;
	NewDown* select;
};
struct NewDown : NewLine {
	NewDrop* drop;
};
int DropMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, PointF pos, NewLocal* local) {
	NewDrop* drop = (NewDrop*)self;
	if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		drop->pw->visible = true;
		myShowWindow(thgc, drop->pw->offscreen->window);
	}
	return 0;
}
int DropKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local) {
	return 0;
}
void NewDropMeasure(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	measure->pos.x += elem->size.x;
	return;
}
// シェブロン（V字・45度）を描画
void drawChevron(LayerInfo* layer, float cx, float cy, float size, float thickness,
	float zIndex, uint32_t color, bgfx::FrameBufferHandle* fb, PointI* fbsize, uint64_t viewId) {
	int steps = (int)(size * 0.5f);
	if (steps < 3) steps = 3;
	float half = size * 0.5f;  // 45度: 幅と高さが同じ
	Background dot{};
	dot.type = DrawCommandType::Fill;
	dot.fillcolor = color;
	dot.borderRadiusTL = dot.borderRadiusTR = dot.borderRadiusBR = dot.borderRadiusBL = thickness * 0.5f;
	dot.borderTop = dot.borderRight = dot.borderBottom = dot.borderLeft = 0;
	dot.aa = 1.0f;
	for (int i = 0; i <= steps; i++) {
		float t = (float)i / (float)steps;
		// 左腕: 左上 → 中央下（cyを中心にする）
		float lx = cx - half + t * half;
		float ly = (cy - half * 0.5f) + t * half;
		layer->pushBackground(&dot, lx - thickness * 0.5f, ly - thickness * 0.5f,
			thickness, thickness, zIndex, &nulltex, &nulltex, fb, fbsize, viewId, 0.0f);
		// 右腕: 右上 → 中央下（cyを中心にする）
		float rx = cx + half - t * half;
		float ry = (cy - half * 0.5f) + t * half;
		layer->pushBackground(&dot, rx - thickness * 0.5f, ry - thickness * 0.5f,
			thickness, thickness, zIndex, &nulltex, &nulltex, fb, fbsize, viewId, 0.0f);
	}
}
void NewDropDraw(ThreadGC* thgc, NewElement* elem, NewGraphic* g, NewLocal* local, RenderCommandQueue* q) {
	NewDrop* drop = (NewDrop*)elem;
	bgfx::TextureHandle* tex1 = &nulltex;
	bgfx::TextureHandle* tex2 = &nulltex;
	if (isValidImageId(elem->background->tex1)) {
		ResolvedTexture rt = myResolveForDraw(thgc, elem->background->tex1);
		if (rt.isValid()) {
			tex1 = rt.texture;
			elem->background->angle = rt.u0;
			elem->background->curl = rt.v0;
			elem->background->scrollX = rt.u1;
			elem->background->scrollY = rt.v1;
		}
	}
	if (isValidImageId(elem->background->tex2)) {
		ResolvedTexture rt2 = myResolveForDraw(thgc, elem->background->tex2);
		if (rt2.isValid()) tex2 = rt2.texture;
	}
	g->layer->pushBackground(elem->background, g->pos.x + elem->pos.x, g->pos.y + elem->pos.y, elem->size.x, elem->size.y, elem->zIndex + 0.1,
		tex1, tex2, g->fb, g->fbsize, g->viewId, 0.0f);
	if (drop->select != NULL) drop->select->Draw(thgc, drop->select, g, local, q);
	// 右端にシェブロン描画
	float dx = g->pos.x + elem->pos.x;
	float dy = g->pos.y + elem->pos.y;
	float chevronSize = elem->size.y * 0.4f;
	float chevronX = dx + elem->size.x - elem->size.y * 0.5f;
	float chevronY = dy + elem->size.y * 0.5f;
	drawChevron((LayerInfo*)g->layer, chevronX, chevronY, chevronSize, 1.5f,
		elem->zIndex + 0.9f, 0x808080FF, g->fb, g->fbsize, g->viewId);
	g->pos.x += elem->size.x;
}
void DropValueChange0(NewElement* elem, int n) {

}
void initNewDrop(ThreadGC* thgc, NewDrop* drop, PopupWindow* pw) {
	drop->type = LetterType::_Drop;
	drop->next = drop->before = drop;
	drop->childend = NULL;
	drop->offscreened = true;
	drop->Measure = NewDropMeasure;
	drop->Draw = NewDropDraw;
	drop->Mouse = DropMouse;
	drop->Key = ElementKey;
	drop->DrawSelection = ImageDrawSelect;
	drop->len = len1;
	drop->values = (SIValues*)GC_alloc(thgc, CType::_SIValues);
	drop->values->values[0].type = SIType::StringValue;
	drop->values->valueCallbacks[0] = DropValueChange0;
	drop->background = (Background*)GC_alloc(thgc, CType::_Background);
	drop->background->fillcolor = 0xFFFFFFFF;
	drop->background->type = DrawCommandType::Fill;
	drop->background->borderRadiusTL = drop->background->borderRadiusTR = drop->background->borderRadiusBR = drop->background->borderRadiusBL = 4;
	drop->background->borderTop = drop->background->borderRight = drop->background->borderBottom = drop->background->borderLeft = 1;
	drop->background->borderColor = 0x8080a0ff;
	drop->pw = pw;
}
int DownMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, PointF pos, NewLocal* local) {
	NewDown* down = (NewDown*)self;
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
	if (e->action == SDL_EVENT_MOUSE_BUTTON_DOWN && down->parent->type == LetterType::_Popup) {
		PopupWindow* popup = (PopupWindow*)down->parent;
		NewDrop* drop = (NewDrop*)popup->offscreen->window->anchorElement;
		drop->select = down;
		FindOffscreen(drop)->markLayout(local);
		myHideWindow(thgc, popup->offscreen->window);
	}
	for (NewElement* elem = self->childend->next; elem->type != _ElemEnd; elem = elem->next) {
		if (self->orient) {
			if (elem->pos.x <= e->x - pos.x && e->x - pos.x < elem->pos.x + elem->size.x) {
				elem->Mouse(thgc, elem, e, { pos.x + elem->pos.x, pos.y }, local);
				pos.x += elem->size.x;
			}
		}
		else {
			if (elem->pos.y <= e->y - pos.y && e->y - pos.y < elem->pos.y + elem->size.y) {
				elem->Mouse(thgc, elem, e, { pos.x, pos.y + elem->pos.y }, local);
				pos.y += elem->size.y;
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
int DownKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local) {
	return 0;
}
void initNewDown(ThreadGC* thgc, NewDown* down) {
	down->type = LetterType::_Down;
	down->next = down->before = down;
	down->childend = (NewElement*)GC_alloc(thgc, CType::_EndC);
	initNewEndElement(thgc, down->childend, down);
	down->orient = true;
	down->Measure = NewMeasureCall;
	down->Draw = NewDrawCall;
	down->Mouse = DownMouse;
	down->Key = ElementKey;
	down->DrawSelection = ElementDrawSelect;
	down->len = len1;
}
void reinitNewDown(ThreadGC* thgc, NewDown* down, String* text) {
	if (down->parent->type == LetterType::_Popup) {
		PopupWindow* popup = (PopupWindow*)down->parent;
		NewDrop* drop = (NewDrop*)popup->offscreen->window->anchorElement;
		drop->select = down;
	}
}
int PopupMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local) {
	return 0;
}
int PopupKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local) {
	return 0;
}
void PopupMeasure(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	if (elem->offscreen != NULL) {
		measure = new NewMeasure();
		measure->base = elem;
		measure->pos = { 0, 0 };
		measure->size = { elem->size.x, elem->size.y };
	}
	NewMeasure newmeasure;
	newmeasure.pos.x = measure->pos.x + elem->margins[3] + elem->borderRadius + elem->paddings[3];
	newmeasure.pos.y = measure->pos.y + elem->margins[0] + elem->borderRadius + elem->paddings[0];
	newmeasure.start = measure->pos; newmeasure.group = measure->group;
	float sizex = 0, sizey = 0;
	for (NewElement* child = elem->childend->next; child->type != LetterType::_ElemEnd; ) {
		if (child->offscreen == NULL) {
			child->Measure(thgc, child, &newmeasure, local, n);
		} else if (child->offscreen->layout) {
			// オフスクリーン子: layoutフラグが立っていれば独立座標系でMeasure
			NewMeasure offMeasure;
			offMeasure.pos = { 0, 0 };
			offMeasure.size = { child->size.x, child->size.y };
			offMeasure.start = { 0, 0 };
			offMeasure.group = child->offscreen->group;
			child->Measure(thgc, child, &offMeasure, local, n);
			child->offscreen->layout = false;
		}
		// layout==false のオフスクリーン子は既存のsizeをそのまま使う
		if (elem->orient) {
			if (sizey < child->size.y) sizey = child->size.y;
			newmeasure.pos.x += child->size.x;
			newmeasure.pos.y = newmeasure.start.y;
			child->pos2.x = sizex;
			child->pos2.y = 0;
			sizex += child->size.x;
		}
		else
		{
			if (sizex < child->size.x) sizex = child->size.x;
			newmeasure.pos.y += child->size.y;
			newmeasure.pos.x = newmeasure.start.x;
			child->pos2.x = 0;
			child->pos2.y = sizey;
			sizey += child->size.y;
		}
		child = child->next;
	}
	elem->size2.x = sizex + elem->margins[1] + elem->margins[3] + elem->paddings[1] + elem->paddings[3] + elem->borderRadius * 2;
	elem->size2.y = sizey + elem->margins[0] + elem->margins[2] + elem->paddings[0] + elem->paddings[2] + elem->borderRadius * 2;
	if (elem->xtype == SizeType::Auto) { elem->size.x = elem->size2.x; }
	if (elem->ytype == SizeType::Auto) { elem->size.y = elem->size2.y; }
	myResizePopupWindow(thgc, elem->offscreen->window, elem->size.x, elem->size.y);
	return;
}
void initPopup(ThreadGC* thgc, NewLocal* local, PopupWindow* popup, PopupAnchor pa, NewElement* elem, int cornerRaidus, int x = 0, int y = 0) {
	popup->type = LetterType::_Popup;
	popup->next = popup->before = popup;
	popup->childend = (NewElement*)GC_alloc(thgc, CType::_EndC);
	initNewEndElement(thgc, popup->childend, popup);
	popup->offscreen = (Offscreen*)GC_alloc(thgc, CType::_Offscreen);
	popup->offscreen->group = &createGroup(thgc);
	popup->offscreen->markLayout(local);
	popup->offscreen->elem = popup;
	popup->offscreen->imPing = queueOffscreenNew(thgc, 1, 1);
	popup->offscreen->imPong = queueOffscreenNew(thgc, 1, 1);
	popup->offscreen->next = popup->offscreen->before = popup->offscreen;
	popup->offscreen->child = (Offscreen*)GC_alloc(thgc, CType::_OffscreenEnd);
	popup->offscreen->child->next = popup->offscreen->child->before = popup->offscreen->child;
	popup->offscreen->child->parent = popup->offscreen;
	popup->offscreened = true;
	popup->background = (Background*)GC_alloc(thgc, CType::_Background);
	popup->background->tex1 = myloadTexture2D(thgc, "123.png", ImageUsage::Background);
	popup->background->fillcolor = 0xffffffff;
	popup->background->borderColor = 0x00000000;
	popup->background->borderTop = popup->background->borderRight = popup->background->borderBottom = popup->background->borderLeft = 0.0f;
	popup->background->shadowBlur = 1.0f;
	popup->background->shadowColor = 0x00000000;
	popup->Measure = PopupMeasure;
	popup->Draw = NewDrawCall;
	popup->Mouse = ElementMouse;
	popup->Key = PopupKey;
	popup->DrawSelection = ElementDrawSelect;
	popup->len = len1;
	NativeWindow* p = myCreatePopupWindow(thgc, NativeWindowType::WindowType_Popup, pa, x, y, 200, 400, cornerRaidus, elem, false);
	if (p) {
		p->local = popup;
		popup->offscreen->window = p;
	}
}
// ポップアップをツリーに単騎挿入し、Offscreen の NativeWindow を設定する
// 子要素はこの後に追加すること（子は popup->nw を継承する）
void addPopupToTree(ThreadGC* thgc, NewLocal* local, NewElement* parent, PopupWindow* popup) {
	NewElementAddLast(thgc, local, parent, popup);
	if (popup->offscreen && popup->offscreen->window) {
		popup->offscreen->window = popup->offscreen->window;
	}
}
struct NewTab : NewElement{
	List* strs;
};
void NewTabMeasure(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	return;
}
void NewTabDraw(ThreadGC* thgc, NewElement* elem, NewGraphic* g, NewLocal* local, RenderCommandQueue* q) {
	NewTab* tab = (NewTab*)elem;
	bgfx::TextureHandle* tex1 = &nulltex;
	bgfx::TextureHandle* tex2 = &nulltex;
	if (isValidImageId(elem->background->tex1)) {
		ResolvedTexture rt = myResolveForDraw(thgc, elem->background->tex1);
		if (rt.isValid()) {
			tex1 = rt.texture;
			elem->background->angle = rt.u0;
			elem->background->curl = rt.v0;
			elem->background->scrollX = rt.u1;
			elem->background->scrollY = rt.v1;
		}
	}
	if (isValidImageId(elem->background->tex2)) {
		ResolvedTexture rt2 = myResolveForDraw(thgc, elem->background->tex2);
		if (rt2.isValid()) tex2 = rt2.texture;
	}
	g->layer->pushBackground(elem->background, g->pos.x, g->pos.y, elem->size.x, elem->size.y, elem->zIndex + 0.1,
		tex1, tex2, g->fb, g->fbsize, g->viewId, 0.0f);
	g->layer->pushFill(g->pos.x + 80, g->pos.y, 100, elem->size.y - 2, 6.0f, 6.0f, 6.0f, 6.0f, 0.1f, 1.0f, 3.0f, 9.0f, 3.0f, 0xc0c0ffff, 0x202040ff, 0, 0, 0, 0, elem->zIndex + 0.3, g->fb, g->fbsize, g->viewId, 0.0, 0.0f);
	drawString((LayerInfo*)g->layer, *getAtlas(thgc), elem->font, *(String**)get_list(tab->strs, 0), g->pos.x + elem->pos.x, g->pos.y + elem->pos.y, std::floor(elem->zIndex) + 0.9,
		0x000000FF, g->group, g->fb, g->fbsize, g->viewId);
	drawString((LayerInfo*)g->layer, *getAtlas(thgc), elem->font, *(String**)get_list(tab->strs, 1), g->pos.x + elem->pos.x + 100, g->pos.y + elem->pos.y, std::floor(elem->zIndex) + 0.9,
		0x000000FF, g->group, g->fb, g->fbsize, g->viewId);
	drawString((LayerInfo*)g->layer, *getAtlas(thgc), elem->font, *(String**)get_list(tab->strs, 2), g->pos.x + elem->pos.x + 200, g->pos.y + elem->pos.y, std::floor(elem->zIndex) + 0.9,
		0x000000FF, g->group, g->fb, g->fbsize, g->viewId);
}
void initNewTab(ThreadGC* thgc, NewTab* tab) {
	tab->type = LetterType::_Drop;
	tab->next = tab->before = tab;
	tab->childend = NULL;
	tab->offscreened = true;
	tab->Measure = NewTabMeasure;
	tab->Draw = NewTabDraw;
	tab->Mouse = DropMouse;
	tab->Key = ElementKey;
	tab->DrawSelection = ImageDrawSelect;
	tab->len = len1;
	tab->values = (SIValues*)GC_alloc(thgc, CType::_SIValues);
	tab->values->values[0].type = SIType::StringValue;
	tab->values->valueCallbacks[0] = DropValueChange0;
	tab->strs = create_list(thgc, sizeof(String*), CType::_String);
	tab->size = { 400, 24 };
	String* test1 = createString(thgc, "test1", 5, 1);
	String* test2 = createString(thgc, "test2", 5, 1);
	String* test3 = createString(thgc, "test3", 5, 1);
	add_list(thgc, tab->strs, (char*)test1);
	add_list(thgc, tab->strs, (char*)test2);
	add_list(thgc, tab->strs, (char*)test3);
	tab->background = (Background*)GC_alloc(thgc, CType::_Background);
	tab->background->fillcolor = 0x8080aaff;
	tab->background->type = DrawCommandType::Fill;
	tab->background->borderRadiusTL = tab->background->borderRadiusTR = tab->background->borderRadiusBR = tab->background->borderRadiusBL = 4;
	tab->background->borderTop = tab->background->borderRight = tab->background->borderBottom = tab->background->borderLeft = 1;
}