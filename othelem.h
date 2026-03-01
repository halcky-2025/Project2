#pragma once
struct NewImage : NewElement {

};
void NewImageMeasure(ThreadGC* thgc, NewElement* elem, NewMeasure* measure, NewLocal* local, int* n) {
	elem->pos2.x = measure->pos.x;
	elem->pos2.y = measure->pos.y;
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
	g->layer->pushBackground(elem->background, elem->pos2.x, elem->pos2.y, elem->size.x, elem->size.y, elem->zIndex + 0.1,
		tex1, tex2, g->fb, g->fbsize, g->viewId, 0.0f);
}
void ImageDrawSelect(ThreadGC* thgc, NewLocal* local, NewElement* self, int m, int n, NewGraphic* g, RenderCommandQueue* q) {
	if (m == n) return;
	g->layer->pushFill(self->pos.x + self->pos2.x, self->pos.y + self->pos2.y, self->size.x + 1, self->size.y, 0.0f, 0.0f, 0.0f, 0x4477ff66, 0, 0.0f, 0.0f, 1.0f, 0, 12000.0f, g->fb, g->fbsize, g->viewId);
}
int ImageMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local) {
	NewImage* letter = (NewImage*)self;
	if (self->pos2.x + self->pos.x <= e->x && e->x < self->pos2.x + self->pos.x + self->size.x) {
		if (self->pos2.y + self->pos.y <= e->y && e->y < self->pos2.y + self->pos.y + self->size.y) {
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
};
struct NewDrop : NewElement {
	PopupWindow* pw;
};
struct NewDown : NewElement {
	NewDrop* drop;
};
int DropMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local) {
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
	elem->pos2.x = measure->pos.x;
	elem->pos2.y = measure->pos.y;
	return;
}
void NewDropDraw(ThreadGC* thgc, NewElement* elem, NewGraphic* g, NewLocal* local, RenderCommandQueue* q) {
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
	g->layer->pushBackground(elem->background, elem->pos2.x, elem->pos2.y, elem->size.x, elem->size.y, elem->zIndex + 0.1,
		tex1, tex2, g->fb, g->fbsize, g->viewId, 0.0f);
	drawString((LayerInfo*)g->layer, *getAtlas(thgc), getFont("sans", 16), elem->values->values[0].s, g->pos.x + elem->pos2.x, g->pos.y + elem->pos2.y, std::floor(elem->zIndex) + 0.9,
		0x000000FF, g->group, g->fb, g->fbsize, g->viewId);
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
	drop->background->borderRadius = 4;
	drop->background->borderWidth = 1;
	drop->background->borderColor = 0x8080a0ff;
	drop->pw = pw;
}
int DownMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local) {
	return 0;
}
int DownKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local) {
	return 0;
}
void initNewDown(ThreadGC* thgc, NewDown* down) {
	down->type = LetterType::_Down;
	down->next = down->before = down;
	down->childend = (NewElement*)GC_alloc(thgc, CType::_EndC);
	initNewEndElement(thgc, down->childend, down);
	down->offscreened = true;
	down->Measure = NewMeasureCall;
	down->Draw = NewDrawCall;
	down->Mouse = ElementMouse;
	down->Key = ElementKey;
	down->DrawSelection = ElementDrawSelect;
	down->len = len1;
}
void reinitNewDown(ThreadGC* thgc, NewDown* down, String* text) {

	if (down->parent->parent->type == LetterType::_Popup) {
		PopupWindow* popup = (PopupWindow*)down->parent->parent;
		NewElement* drop = popup->offscreen->window->anchorElement;
		SIVal val;
		val.s = text;
		val.type = SIType::StringValue;
		SetSIValue(drop, drop->values, 0, val);
	}
}
int PopupMouse(ThreadGC* thgc, NewElement* self, MouseEvent* e, NewLocal* local) {
	return 0;
}
int PopupKey(ThreadGC* thgc, NewElement* self, int m, int n, KeyEvent* e, NewLocal* local) {
	return 0;
}
void initPopup(ThreadGC* thgc, NewLocal* local, PopupWindow* popup, PopupAnchor pa, NewElement* elem, int x = 0, int y = 0) {
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
	popup->background->borderWidth = 0.0f;
	popup->background->shadowBlur = 1.0f;
	popup->background->shadowColor = 0x00000000;
	popup->Measure = NewMeasureCall;
	popup->Draw = NewDrawCall;
	popup->Mouse = ElementMouse;
	popup->Key = PopupKey;
	popup->DrawSelection = ElementDrawSelect;
	popup->len = len1;
	NativeWindow* p = myCreatePopupWindow(thgc, NativeWindowType::WindowType_Popup, pa, x, y, 200, 400, elem, false);
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