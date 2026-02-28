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
			}
			else if (e->action == SDL_EVENT_MOUSE_BUTTON_UP || e->click) {
				local->select.to = self;
				local->select.toid = self->id;
				local->select.n = 0;
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
	im1->type = LetterType::_Line;
	im1->next = im1;
	im1->before = im1;
	im1->parent = NULL;
	im1->childend = NULL;
	im1->orient = true;
	im1->id = 0;
	im1->Measure = NewImageMeasure;
	im1->Draw = NewImageDraw;
	im1->Mouse = ImageMouse;
	im1->Key = ElementKey;
	im1->DrawSelection = ImageDrawSelect;
	im1->len = len1;
}