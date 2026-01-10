

void TreeElementCheck(ThreadGC* thgc, char* self) {
	TreeElement* tree = (TreeElement*)self;
	GC_UPDATE_PTR_C(thgc, tree->id, String*);
	GC_UPDATE_PTR_C(thgc, tree->elem, NewElement*);
	GC_UPDATE_PTR_C(thgc, tree->parent, TreeElement*);
	GC_UPDATE_PTR_C(thgc, tree->children, List*);
}
void createLetter(ThreadGC* thgc, NewElement* parent, String* str, FontId font) {
	NewLetter* letter = (NewLetter*)GC_alloc(thgc, _LetterC);
	initNewLetter(thgc, letter, font, _Letter);
	letter->text = str;
	if (parent != NULL) {
		NewElementAddLast(thgc, NULL, parent, (NewElement*)letter);
	}
}
void AddTE(ThreadGC* thgc, TreeElement* te, TreeElement* parentt) {
	te->parent = parentt;
	add_list(thgc, parentt->children, (char*)te);
}
void RemoveChildTE(ThreadGC* thgc, List* deletes, TreeElement* te) {
	for (int i = 0; i < te->children->size; i++) {
		TreeElement* child = (TreeElement*)get_list(te->children, i);
		RemoveChildTE(thgc, deletes, child);
		child->del = true;
		add_list(thgc, deletes, (char*)child);
	}
}
void RemoveTE(ThreadGC* thgc, List* deletes, TreeElement* te) {
	remove_list_val(thgc, te->parent->children, (char*)te);
	te->parent = NULL;
	te->del = true;
	RemoveChildTE(thgc, deletes, te);
}
void CancelRemoveChildTE(TreeElement* te) {
	for (int i = 0; i < te->children->size; i++) {
		TreeElement* child = (TreeElement*)get_list(te->children, i);
		child->del = false;
		CancelRemoveChildTE(child);
	}
}
enum OpCode {
	Plus = 1,
	Minus = 2,
	Mul = 4,
	Divi = 8
};
//+*-
TreeElement* checkTreeElement(ThreadGC* thgc, TreeElement* parentt, NewElement* parent, List* deletes, String* id, enum LetterType type, enum OpCode op, NewElement* (*func)(ThreadGC*)) {
	TreeElement* te = (TreeElement*)get_mapy(thgc->map, id);
	if (te == NULL) {
		NewElement* elem = func(thgc);
		te = (TreeElement*)GC_alloc(thgc, _TreeElement);
		te->elem = elem;
		te->id = id;
		te->children = create_list(thgc, sizeof(TreeElement*), true);
		if ((op & Minus) == Minus) {
			return NULL;
		}
		if (parent == NULL) return NULL;
		AddTE(thgc, te, parentt);
		NewElementAddLast(thgc, NULL, parent, elem);
		return te;
	}
	else if (te->elem->type == type) {
		te->del = false;
		if ((op & Minus) == Minus) {
			remove_mapy(thgc, thgc->map, te->id);
			remove_list_val(thgc, deletes, (char*)te);
			RemoveTE(thgc, deletes, te);
			NewRemoveElement(te->elem);
			return NULL;
		}
		if ((op & Plus) == Plus) {
		}
		else {
			te->elem->pos.x = 0; te->elem->pos.y = 0;
			te->elem->size.x = 0; te->elem->size.y = 0;
			CancelRemoveChildTE(te);
		}
		if (((op & Mul) == Mul) || ((op & Divi) == Divi)) {
		}
		else {
			for (int i = 0; i < te->children->size; i++) {
				TreeElement* child = (TreeElement*)get_list(te->children, i);
				RemoveTE(thgc, deletes, child);
			}
			for (NewElement* elem = te->elem->childend; elem->next != te->elem->childend; ) {
				NewRemoveElement (elem->next);
			}
		}
		if (parent == NULL) return te;
		else {
			NewRemoveElement(te->elem);
			te->elem->parent = parent;
			NewElementAddLast(thgc, NULL, parent, te->elem);
			remove_list_val(thgc, te->parent->children, (char*)te);
			AddTE(thgc, te, parentt);
			return te;
		}
	}
	else {
		te->del = false;
		NewElement* elem = func(thgc);
		if ((op & Minus) == Minus) {
			remove_mapy(thgc, thgc->map, te->id);
			remove_list_val(thgc, deletes, (char*)te);
			RemoveChildTE(thgc, deletes, te);
			NewRemoveElement(te->elem);
			return NULL;
		}
		if ((op & Plus) == Plus) {
			elem->pos = te->elem->pos; elem->size = te->elem->size;
			elem->xtype = te->elem->xtype; elem->ytype = te->elem->ytype;
		}
		if (((op & Mul) == Mul) || ((op & Divi) == Divi)) {
			elem->childend = te->elem->childend;
			elem->childend->parent = elem;
			for (NewElement* child = elem->childend->next; child != elem->childend; child = child->next) {
				child->parent = elem;
			}
			CancelRemoveChildTE(te);
		}
		else {
			for (int i = 0; i < te->children->size; i++) {
				TreeElement* child = (TreeElement*)get_list(te->children, i);
				RemoveTE(thgc, deletes, child);
			}
			for (NewElement* elem = te->elem->childend; elem->next != te->elem->childend; ) {
				NewRemoveElement(elem->next);
			}
		}
		if (parent == NULL) {
			elem->next = te->elem->next;
			elem->before = te->elem->before;
			elem->next->before = elem;
			elem->before->next = elem;
			elem->parent = te->elem->parent;
			te->elem = elem;
			return te;
		}
		else {
			NewRemoveElement(te->elem);
			NewElementAddLast(thgc, NULL, parent, elem);
			te->elem = elem;
			remove_list_val(thgc, te->parent->children, (char*)te);
			AddTE(thgc, te, parentt);
			return te;
		}
	}
}
void TreeElementLast(ThreadGC* thgc, List* deletes) {
	for (int i = 0; i < deletes->size; i++) {
		TreeElement* te = (TreeElement*)get_list(deletes, i);
		remove_mapy(thgc, thgc->map, te->id);
	}
	deletes->size = 0;
}
