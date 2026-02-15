
ATSSpan* make_span(ThreadGC* thgc, FontId font, LetterType t, int start, int end, uint32_t color, String* text) {
	ATSSpan* s = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
	s->color = color;
	s->font = font;
	s->t = t;
	s->start = start;
	s->end = end;
	s->text = text;
	s->error = NULL;
	return s;
}

List* Compile(ThreadGC* thgc, String* str, FontId font) {
	List* list = create_list(thgc, sizeof(List*), _List);
	List* list0 = create_list(thgc, sizeof(ATSSpan*), _List);
	add_list(thgc, list, (char*)list0);
	int len = str->size;
	for (int i = 0; i < len; i++) {
		wchar_t c = GetChar(str, i);
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_') {
			int j = i + 1;
			for (; j < len; j++) {
				c = GetChar(str, j);
				if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_') continue;
				break;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font, _Name, i, j, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if ('0' <= c && c <= '9') {
			int j = i + 1;
			bool flv = false;
			for (; j < len; j++) {
				c = GetChar(str, j);
				if ('0' <= c && c <= '9') continue;
				else if (c == '.') {
					if (flv) break;
					j++;
					if (j < len && '0' <= GetChar(str, j) && GetChar(str, j) <= '9') {
						flv = true;
						continue;
					}
					else {
						j--;
						break;
					}
				}
				break;
			}
			if (flv) add_list(thgc, list0, (char*)make_span(thgc, font, _Decimal, i, j, 0x000000FF, SubString(thgc, str, i, j - i)));
			else add_list(thgc, list0, (char*)make_span(thgc, font, _Number, i, j, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if (c == '"') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					add_list(thgc, list0, (char*)make_span(thgc, font, _Str, i, j, 0x8B4513FF, SubString(thgc, str, i + 1, j - i - 1)));
					break;
				}
				c = GetChar(str, j);
				if (c == '"') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _Str, i, j + 1, 0x8B4513FF, SubString(thgc, str, i + 1, j - i - 1)));
					j++;
					break;
				}
				else if (c == '\n' || c == '\0') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _Str, i, j, 0x8B4513FF, SubString(thgc, str, i + 1, j - i - 1)));
					break;
				}
			}
			i = j - 1;
		}
		else if (c == '&') {
			int j = i + 1;
			for (; j < len; j++) {
				c = GetChar(str, j);
				if (c == '\n' || c == '\0') break;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font, _And, i, j, 0x8B4513FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if (c == '^') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Mountain, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '`') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					add_list(thgc, list0, (char*)make_span(thgc, font, _HLetter, i, j, 0x000000FF, SubString(thgc, str, i + 1, j - i - 1)));
					break;
				}
				c = GetChar(str, j);
				if (c == '`') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _HLetter, i, j + 1, 0x000000FF, SubString(thgc, str, i + 1, j - i - 1)));
					j++;
					break;
				}
				else if (c == '<' || c == '>' || c == '~' || c == '$') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _HLetter, i, j, 0x000000FF, SubString(thgc, str, i + 1, j - i - 1)));
					break;
				}
				else if (c == '\n' || c == '\0') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _HLetter, i, j, 0x000000FF, SubString(thgc, str, i + 1, j - i - 1)));
					break;
				}
			}
			i = j - 1;
		}
		else if (c == ' ') {
			int j = i + 1;
			for (; j < len; j++) {
				if (GetChar(str, j) != ' ') break;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font, _Space, i, j, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if (c == '\t') {
			int j = i + 1;
			for (; j < len; j++) {
				if (GetChar(str, j) != '\t') break;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font, _Space, i, j, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if (c == ':') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Colon, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == ';') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Semicolon, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '$') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Dolor, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == ',') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Comma, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '|') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Bou, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '#') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Sharp, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '%') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Percent, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '@') {
			if (i + 1 < len) {
				wchar_t c2 = GetChar(str, i + 1);
				if (('a' <= c2 && c2 <= 'z') || ('A' <= c2 && c2 <= 'Z')) {
					int j = i + 2;
					for (; j < len; j++) {
						c2 = GetChar(str, j);
						if (('a' <= c2 && c2 <= 'z') || ('A' <= c2 && c2 <= 'Z')) continue;
						break;
					}
					add_list(thgc, list0, (char*)make_span(thgc, font, _AtLetter, i, j, 0x000000FF, SubString(thgc, str, i, j - i)));
					i = j - 1;
				}
			}
		}
		else if (c == '\n') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Kaigyou, i, i + 1, 0x000000FF, NULL));
			int j = i + 1;
			for (; j < len; j++) {
				wchar_t c2 = GetChar(str, j);
				if (c2 == ' ') {
					int si = j;
					j++;
					for (; j < len; j++) {
						if (GetChar(str, j) != ' ') break;
					}
					add_list(thgc, list0, (char*)make_span(thgc, font, _Space, si, j, 0x000000FF, SubString(thgc, str, si, j - si)));
					j--;
				}
				else if (c2 == '\n') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _Kaigyou, j, j + 1, 0x000000FF, NULL));
				}
				else break;
			}
			i = j - 1;
		}
		else if (c == '=') {
			if (i + 1 < len && GetChar(str, i + 1) == '=') {
				add_list(thgc, list0, (char*)make_span(thgc, font, _EqualEqual, i, i + 2, 0x000000FF, NULL));
				i++;
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, _Equal, i, i + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '!') {
			if (i + 1 < len && GetChar(str, i + 1) == '=') {
				add_list(thgc, list0, (char*)make_span(thgc, font, _NotEqual, i, i + 2, 0x000000FF, NULL));
				i++;
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, _Not, i, i + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '+') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Plus, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '-') {
			if (i + 1 < len) {
				wchar_t c2 = GetChar(str, i + 1);
				if (c2 == '>') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _Right, i, i + 2, 0x000000FF, NULL));
					i++;
				}
				else if (c2 == '-') {
					i++;
					int j = i + 1;
					for (; j < len; j++) {
						c2 = GetChar(str, j);
						if (c2 == '\n' || c2 == '\0') break;
					}
					add_list(thgc, list0, (char*)make_span(thgc, font, _CommentSingle, i - 1, j, 0x008000FF, SubString(thgc, str, i + 1, j - i - 1)));
					i = j - 1;
				}
				else {
					add_list(thgc, list0, (char*)make_span(thgc, font, _Minus, i, i + 1, 0x000000FF, NULL));
				}
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, _Minus, i, i + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '<') {
			if (i + 1 < len) {
				wchar_t c2 = GetChar(str, i + 1);
				if (c2 == '-') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _Left, i, i + 2, 0x000000FF, NULL));
					i++;
				}
				else if (c2 == '=') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _LessEqual, i, i + 2, 0x000000FF, NULL));
					i++;
				}
				else if (c2 == '&') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _StringTag, i, i + 2, 0x000000FF, NULL));
					i++;
				}
				else {
					add_list(thgc, list0, (char*)make_span(thgc, font, _LessThan, i, i + 1, 0x000000FF, NULL));
				}
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, _LessThan, i, i + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '>') {
			if (i + 1 < len) {
				wchar_t c2 = GetChar(str, i + 1);
				if (c2 == '=') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _MoreEqual, i, i + 2, 0x000000FF, NULL));
					i++;
				}
				else if (c2 == '>') {
					add_list(thgc, list0, (char*)make_span(thgc, font, _RightRight, i, i + 2, 0x000000FF, NULL));
					i++;
				}
				else {
					add_list(thgc, list0, (char*)make_span(thgc, font, _MoreThan, i, i + 1, 0x000000FF, NULL));
				}
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, _MoreThan, i, i + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '*') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Mul, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '/') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Div, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '~') {
			if (i + 1 < len && GetChar(str, i + 1) == '~') {
				i++;
				if (i + 1 < len && GetChar(str, i + 1) == '~') {
					i++;
					add_list(thgc, list0, (char*)make_span(thgc, font, _NyoroNyoroNyoro, i - 2, i + 1, 0x000000FF, NULL));
				}
				else {
					add_list(thgc, list0, (char*)make_span(thgc, font, _NyoroNyoro, i - 1, i + 1, 0x000000FF, NULL));
				}
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, _Nyoro, i, i + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '.') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Dot, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '(') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _BracketL, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == ')') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _BracketR, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '[') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _BlockL, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == ']') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _BlockR, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '{') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _BraceL, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '}') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _BraceR, i, i + 1, 0x000000FF, NULL));
		}
		else if (c == '\0') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _End, i, i + 1, 0x000000FF, NULL));
			break;
		}
		else if (c == '?') {
			add_list(thgc, list0, (char*)make_span(thgc, font, _Question, i, i + 1, 0x000000FF, NULL));
		}
		else if (c >= 256) {
			int j = i + 1;
			for (; j < len; j++) {
				if (GetChar(str, j) < 256) break;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font, _CommentSingle, i, j, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
	}
	return list0;
}

// ============================================================
// パーサー構造体定義
// ============================================================

struct Obj {
	LetterType objtype;
	ATSSpan* letter;
	List* children;
};

struct Block {
	LetterType objtype;
	ATSSpan* letter;
	List* children;
	Map* branchmap;
	Map* labelmap;
	Map* labelmapn;
	ATSSpan* letter2;
};

struct TagBlock {
	LetterType objtype;
	ATSSpan* letter;
	List* children;
	Map* branchmap;
	Map* labelmap;
	Map* labelmapn;
	ATSSpan* letter2;
	ATSSpan* tagletter;
};

struct Primary {
	LetterType objtype;
	ATSSpan* letter;
	List* children;
	List* singleops;
};

struct Label {
	ATSSpan* letter;
	String* name;
	int n;
	Map* labelmap;
};

struct PrimOp {
	LetterType objtype;
	ATSSpan* letter;
};

struct SingleOp {
	ATSSpan* letter;
};

struct Operator {
	List* types;
};

struct Master {
	NewElement* top;
	NewElement* elem;
	LetterType lasttype;
	ATSSpan* last;
	int n;
	void init(NewElement* _top) {
		elem = top = _top; n = 0;
	}
	void Next() {
	head:
		if (elem->type == LetterType::_Letter) {
			NewLetter* letter = (NewLetter*)elem;
			for (; n < letter->atsspans->size; n++) {
				auto as = *(ATSSpan**)get_list(letter->atsspans, n);
				if (as->t == LetterType::_Space) continue;
				else {
					last = as;
					lasttype = as->t;
					n++;
					return;
				}
			}
			n = 0;
		}
		if (elem->childend != NULL) {
			elem = elem->childend->next;
			goto head;
		}
		if (elem->type == _ElemEnd) {
			elem = elem->parent->next;
			if (elem->before == top) {
				lasttype = LetterType::_End;
				last = NULL;
				return;
			}
			else if (lasttype == LetterType::_Kaigyou) goto head;
			else {
				lasttype = LetterType::_Kaigyou;
				last = NULL;
				return;
			}
		}
		elem = elem->next;
		goto head;
	}
	List* operators;
	List* analblocks;
	Map* labelmap;
	int IntCls;
	int FloatCls;
	int StrCls;
};

// ============================================================
// ヘルパー関数
// ============================================================

Obj* make_cobj(ThreadGC* thgc, int ctype, LetterType ot, ATSSpan* letter) {
	Obj* o = (Obj*)GC_alloc(thgc, ctype);
	o->objtype = ot;
	o->letter = letter;
	o->children = create_list(thgc, sizeof(Obj*), _List);
	return o;
}

Block* make_cblock(ThreadGC* thgc, int ctype, LetterType ot) {
	Block* b = (Block*)GC_alloc(thgc, ctype);
	b->objtype = ot;
	b->letter = NULL;
	b->children = create_list(thgc, sizeof(Obj*), _List);
	b->branchmap = create_mapy(thgc, _List);
	b->labelmap = create_mapy(thgc, _List);
	b->labelmapn = create_mapy(thgc, _List);
	b->letter2 = NULL;
	return b;
}

Obj* make_word_change(ThreadGC* thgc, ATSSpan* letter, Master* local) {
	Obj* o = make_cobj(thgc, _Word, OT_None, letter);
	return o;
}

// ============================================================
// 前方宣言
// ============================================================
Obj* ParseBlock(ThreadGC* thgc, Master* local, LetterType end, int comments, bool* error);
Block* ParseBlock2(ThreadGC* thgc, Master* local, LetterType finish, int comments, bool* error);
Block* ParseLines(ThreadGC* thgc, Master* local, LetterType sub, LetterType sub2, LetterType sub3, LetterType finish, LetterType type, int comments, bool* error, ATSSpan** outLetter);
Obj* ParseOpe1(ThreadGC* thgc, Master* local, int n, int comments, bool tag, LetterType type, bool* error);
Obj* ParseOperator(ThreadGC* thgc, Master* local, int n, int comments, bool tag, LetterType type, bool* error);
Obj* ParsePrimary(ThreadGC* thgc, Master* local, int comments, LetterType type, bool* error);

// ============================================================
// ParseStart
// ============================================================
Obj* ParseStart(ThreadGC* thgc, Master* local, bool* error) {
	local->analblocks = create_list(thgc, sizeof(Block*), _List);
	local->Next();
	Block* item = make_cblock(thgc, _CallBlock, OT_Call1);
	add_list(thgc, item->children, (char*)make_cobj(thgc, _CObj, OT_Call1, NULL));
	ATSSpan* outLetter = NULL;
	Block* item2 = ParseLines(thgc, local, _Kaigyou, _Semicolon, _Comma, _End, OT_Call2, 0, error, &outLetter);
	add_list(thgc, item->children, (char*)item2);
	add_list(thgc, local->analblocks, (char*)item2);
	return (Obj*)item;
}

// ============================================================
// ParseBlock
// ============================================================
Obj* ParseBlock(ThreadGC* thgc, Master* local, LetterType end, int comments, bool* error) {
	Block* item;
	if (end == _MoreThan) {
		item = (Block*)GC_alloc(thgc, _TagBlock);
		item->objtype = OT_None;
		item->letter = NULL;
		item->children = create_list(thgc, sizeof(Obj*), _List);
		item->branchmap = create_mapy(thgc, _List);
		item->labelmap = create_mapy(thgc, _List);
		item->labelmapn = create_mapy(thgc, _List);
		item->letter2 = NULL;
	} else {
		item = make_cblock(thgc, _CallBlock, OT_None);
	}
head:
	if (local->lasttype == _Kaigyou) local->Next();
	ATSSpan* outLetter = NULL;
	Block* item2 = ParseLines(thgc, local, _Kaigyou, _Comma, _Semicolon, _Bou, OT_Call1, comments, error, &outLetter);
	add_list(thgc, local->analblocks, (char*)item2);
	add_list(thgc, item->children, (char*)item2);
	Block* item3 = ParseBlock2(thgc, local, end, comments, error);
	add_list(thgc, item->children, (char*)item3);
	add_list(thgc, local->analblocks, (char*)item3);
	return (Obj*)item;
}

// ============================================================
// ParseBlock2
// ============================================================
Block* ParseBlock2(ThreadGC* thgc, Master* local, LetterType finish, int comments, bool* error) {
	ATSSpan* outLetter = NULL;
	Block* item = ParseLines(thgc, local, _Kaigyou, _Semicolon, _Comma, finish, OT_Call2, comments, error, &outLetter);
	return item;
}

// ============================================================
// ParseLines
// ============================================================
Block* ParseLines(ThreadGC* thgc, Master* local, LetterType sub, LetterType sub2, LetterType sub3, LetterType finish, LetterType type, int comments, bool* error, ATSSpan** outLetter) {
	Block* item = make_cblock(thgc, _Block, type);
	bool tag = false;
	if (finish == _MoreThan) tag = true;
head:
	if (local->lasttype == _Sharp) {
		local->last->error = NULL;
		local->Next();
		if (local->lasttype == _Sharp) {
			local->last->error = NULL;
			local->Next();
			if (local->lasttype == _Name || local->lasttype == _Number || local->lasttype == _Str) {
				String* name = local->last->text;
				ATSSpan* let = local->last;
				local->last->error = NULL;
				local->Next();
				Label* label0 = (Label*)get_mapy(local->labelmap, name);
				if (label0 == NULL) {
					label0 = (Label*)GC_alloc(thgc, _Label);
					label0->letter = let; label0->name = name; label0->n = 0;
					label0->labelmap = create_mapy(thgc, _List);
					add_mapy(thgc, local->labelmap, name, (char*)label0);
				}
				Label* label = (Label*)get_mapy(item->branchmap, name);
				if (label == NULL) {
					label = (Label*)GC_alloc(thgc, _Label);
					label->letter = let; label->name = name; label->n = item->children->size;
					label->labelmap = create_mapy(thgc, _List);
					add_mapy(thgc, item->branchmap, name, (char*)label);
				}
				if (local->lasttype == _Dot) {
					local->last->error = NULL;
					local->Next();
					if (local->lasttype == _Name || local->lasttype == _Number || local->lasttype == _Str) {
						String* name2 = local->last->text;
						Label* label00 = (Label*)get_mapy(label0->labelmap, name2);
						if (label00 == NULL) {
							label00 = (Label*)GC_alloc(thgc, _Label);
							label00->letter = local->last; label00->name = name2; label00->n = 0;
							label00->labelmap = create_mapy(thgc, _List);
							add_mapy(thgc, label0->labelmap, name2, (char*)label00);
						}
						Label* label2 = (Label*)get_mapy(label->labelmap, name2);
						if (label2 == NULL) {
							label2 = (Label*)GC_alloc(thgc, _Label);
							label2->letter = local->last; label2->name = name2; label2->n = item->children->size;
							label2->labelmap = create_mapy(thgc, _List);
							add_mapy(thgc, label->labelmap, name2, (char*)label2);
						}
						local->last->error = NULL;
						local->Next();
						if (local->lasttype == _Dot) {
							local->Next();
							if (local->lasttype == _Name || local->lasttype == _Number || local->lasttype == _Str) {
								String* name3 = local->last->text;
								Label* label000 = (Label*)get_mapy(label00->labelmap, name);
								if (label000 == NULL) {
									label000 = (Label*)GC_alloc(thgc, _Label);
									label000->letter = local->last; label000->name = name3; label000->n = 0;
									label000->labelmap = create_mapy(thgc, _List);
									add_mapy(thgc, label00->labelmap, name, (char*)label000);
								}
								Label* label3 = (Label*)get_mapy(label2->labelmap, name3);
								if (label3 == NULL) {
									label3 = (Label*)GC_alloc(thgc, _Label);
									label3->letter = local->last; label3->name = name3; label3->n = item->children->size;
									label3->labelmap = create_mapy(thgc, _List);
									add_mapy(thgc, label2->labelmap, name3, (char*)label3);
								}
								local->last->error = NULL;
								local->Next();
							} else {
								local->last->error = createString(thgc, (char*)"Label syntax error", 18, 1);
								*error = true;
							}
						}
					} else {
						local->last->error = createString(thgc, (char*)"Label syntax error", 18, 1);
						*error = true;
					}
				}
			}
		}
		else if (local->lasttype == _Name || local->lasttype == _Number || local->lasttype == _Str) {
			String* name = local->last->text;
			ATSSpan* let = local->last;
			let->error = NULL;
			local->Next();
			if (local->lasttype == _Dot) {
				local->last->error = NULL;
				local->Next();
				if (local->lasttype == _Name || local->lasttype == _Number || local->lasttype == _Str) {
					Label* label = (Label*)get_mapy(item->labelmap, name);
					if (label == NULL) {
						label = (Label*)GC_alloc(thgc, _Label);
						label->letter = local->last; label->name = local->last->text; label->n = item->children->size;
						label->labelmap = create_mapy(thgc, _List);
						add_mapy(thgc, item->labelmap, name, (char*)label);
						add_mapyn(thgc, item->labelmapn, (char*)(intptr_t)item->children->size, (char*)label);
					}
					if (get_mapy(label->labelmap, local->last->text) != NULL) {
						local->last->error = createString(thgc, (char*)"Label syntax error", 18, 1);
						*error = true;
					} else {
						Label* label2 = (Label*)GC_alloc(thgc, _Label);
						label2->letter = local->last; label2->name = local->last->text; label2->n = item->children->size;
						label2->labelmap = create_mapy(thgc, _List);
						add_mapy(thgc, label->labelmap, local->last->text, (char*)label2);
						add_mapyn(thgc, item->labelmapn, (char*)(intptr_t)item->children->size, (char*)label2);
					}
					local->Next();
				} else {
					*error = true;
				}
			} else {
				if (get_mapy(item->labelmap, name) != NULL) {
					local->last->error = createString(thgc, (char*)"Label syntax error", 18, 1);
					*error = true;
				} else {
					Label* label = (Label*)GC_alloc(thgc, _Label);
					label->letter = let; label->name = name; label->n = item->children->size;
					label->labelmap = create_mapy(thgc, _List);
					add_mapy(thgc, item->labelmap, name, (char*)label);
					add_mapyn(thgc, item->labelmapn, (char*)(intptr_t)item->children->size, (char*)label);
				}
			}
		}
		else if (local->lasttype == _Str) {
			Label* label = (Label*)GC_alloc(thgc, _Label);
			label->letter = local->last; label->name = local->last->text; label->n = item->children->size;
			label->labelmap = create_mapy(thgc, _List);
			add_mapy(thgc, item->labelmap, local->last->text, (char*)label);
			add_mapyn(thgc, item->labelmapn, (char*)(intptr_t)item->children->size, (char*)label);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _Number) {
			Label* label = (Label*)GC_alloc(thgc, _Label);
			label->letter = local->last; label->name = local->last->text; label->n = item->children->size;
			label->labelmap = create_mapy(thgc, _List);
			add_mapy(thgc, item->labelmap, local->last->text, (char*)label);
			add_mapyn(thgc, item->labelmapn, (char*)(intptr_t)item->children->size, (char*)label);
			local->last->error = NULL;
			local->Next();
		}
		else {
			local->last->error = createString(thgc, (char*)"Label syntax error", 18, 1);
			*error = true;
		}
	}
	if (local->lasttype == sub) {
		local->Next();
		goto head;
	}
	else if (local->lasttype == sub2) {
		local->Next();
		goto head;
	}
	else if (local->lasttype == sub3) {
		local->Next();
		goto head;
	}
	else if (local->lasttype == finish) {
		ATSSpan* letter = local->last;
		if (letter) letter->error = NULL;
		item->letter2 = letter;
		local->Next();
		if (outLetter) *outLetter = letter;
		return item;
	}
	add_list(thgc, item->children, (char*)ParseOperator(thgc, local, 0, comments, tag, type, error));
	if (local->lasttype == sub) {
		local->Next();
		goto head;
	}
	else if (local->lasttype == sub2) {
		local->Next();
		goto head;
	}
	else if (local->lasttype == sub3) {
		local->Next();
		goto head;
	}
	else if (local->lasttype == finish) {
		ATSSpan* letter = local->last;
		if (letter) letter->error = NULL;
		item->letter2 = letter;
		local->Next();
		if (outLetter) *outLetter = letter;
		return item;
	}
	else if (local->lasttype == _End) {
		if (local->last) local->last->error = createString(thgc, (char*)"Parse error", 11, 1);
		if (outLetter) *outLetter = local->last;
		return item;
	}
	else {
		*error = true;
		if (local->last) local->last->error = createString(thgc, (char*)"Parse error", 11, 1);
		local->Next();
		goto head;
	}
}

// ============================================================
// ParseOpe1
// ============================================================
Obj* ParseOpe1(ThreadGC* thgc, Master* local, int n, int comments, bool tag, LetterType type, bool* error) {
	if (n < local->operators->size) return ParseOperator(thgc, local, n, comments, tag, type, error);
	else return ParsePrimary(thgc, local, comments, type, error);
}

// ============================================================
// ParseOperator
// ============================================================
Obj* ParseOperator(ThreadGC* thgc, Master* local, int n, int comments, bool tag, LetterType type, bool* error) {
	Obj* item = ParseOpe1(thgc, local, n + 1, comments, tag, type, error);
	Operator* opdef = *(Operator**)get_list(local->operators, n);
	for (int i = 0; i < opdef->types->size; i++) {
		LetterType op = (LetterType)(intptr_t)*get_list(opdef->types, i);
		if (local->lasttype == op) {
			if (local->lasttype == _MoreThan && tag) {
				return item;
			}
			Obj* item2 = make_cobj(thgc, _COperator, OT_None, local->last);
			local->last->error = NULL;
			local->Next();
			add_list(thgc, item2->children, (char*)item);
			add_list(thgc, item2->children, (char*)ParseOpe1(thgc, local, n + 1, comments, tag, type, error));
			return item2;
		}
	}
	return item;
}

// ============================================================
// ParseTagString
// ============================================================
Obj* ParseTagString(ThreadGC* thgc, Master* local, LetterType type, bool* error) {
	for (;;) {
		if (local->lasttype == _StringTag) {
			ATSSpan* letter = local->last;
			local->Next();
			Obj* tagblock = ParseBlock(thgc, local, _MoreThan, 1, error);
			tagblock->letter = letter;
			letter->error = NULL;
		}
		else if (local->lasttype == _End || local->lasttype == _Kaigyou) {
			return NULL;
		}
		else {
			local->Next();
		}
	}
	return NULL;
}

// ============================================================
// ParsePrimary
// ============================================================
Obj* ParsePrimary(ThreadGC* thgc, Master* local, int comments, LetterType type, bool* error) {
	Primary* item = (Primary*)GC_alloc(thgc, _Primary);
	item->objtype = OT_None;
	item->letter = local->last;
	item->children = create_list(thgc, sizeof(Obj*), _List);
	item->singleops = create_list(thgc, sizeof(SingleOp*), _List);

	if (comments > 0) {
		if (local->lasttype == _Dot || local->lasttype == _Mul || local->lasttype == _RightRight) {
			SingleOp* sop = (SingleOp*)GC_alloc(thgc, _SingleOp);
			sop->letter = local->last;
			add_list(thgc, item->singleops, (char*)sop);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _Plus || local->lasttype == _Minus || local->lasttype == _Mul || local->lasttype == _Div) {
			SingleOp* sop = (SingleOp*)GC_alloc(thgc, _SingleOp);
			sop->letter = local->last;
			add_list(thgc, item->singleops, (char*)sop);
			local->last->error = NULL;
			local->Next();
		}
	}
	else if (local->lasttype == _Plus || local->lasttype == _Minus || local->lasttype == _Not) {
		SingleOp* sop = (SingleOp*)GC_alloc(thgc, _SingleOp);
		sop->letter = local->last;
		add_list(thgc, item->singleops, (char*)sop);
		local->last->error = NULL;
		local->Next();
	}

	bool first = true;
	for (;;) {
		if (comments > 0 && type != OT_Call1 && (local->lasttype == _LessThan || local->lasttype == _StringTag)) {
			ATSSpan* letter = local->last;
			letter->error = NULL;
			local->Next();
			Obj* tagblock = ParseBlock(thgc, local, _MoreThan, comments, error);
			tagblock->letter = letter;
			add_list(thgc, item->children, (char*)tagblock);
		}
		else if (comments > 0 && local->lasttype == _Dolor) {
			Obj* dolor = make_cobj(thgc, _CDolor, OT_None, local->last);
			add_list(thgc, item->children, (char*)dolor);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _Nyoro) {
			Obj* comment = make_cobj(thgc, _Comment, OT_None, local->last);
			local->last->error = NULL;
			add_list(thgc, item->children, (char*)comment);
			local->Next();
			ATSSpan* outL = NULL;
			Block* ret = ParseLines(thgc, local, _Kaigyou, _Semicolon, _Comma, _NyoroNyoro, OT_Comment, comments + 1, error, &outL);
			add_list(thgc, comment->children, (char*)ret);
		}
		else if (local->lasttype == _NyoroNyoroNyoro) {
			Obj* comment2 = make_cobj(thgc, _Comment2, OT_None, local->last);
			local->last->error = NULL;
			add_list(thgc, item->children, (char*)comment2);
			local->Next();
			ATSSpan* outL = NULL;
			Block* ret = ParseLines(thgc, local, _Kaigyou, _Semicolon, _Comma, _NyoroNyoro, OT_Comment, comments + 1, error, &outL);
			add_list(thgc, comment2->children, (char*)ret);
		}
		else if (comments > 0 && local->lasttype == _HLetter) {
			Obj* htm = make_cobj(thgc, _HtmObj, OT_None, local->last);
			add_list(thgc, item->children, (char*)htm);
			local->Next();
		}
		else if (local->lasttype == _Mountain) {
			Obj* mtn = make_cobj(thgc, _CMountain, OT_None, local->last);
			add_list(thgc, item->children, (char*)mtn);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _Question) {
			Obj* q = make_cobj(thgc, _CQuestion, OT_None, local->last);
			add_list(thgc, item->children, (char*)q);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _Name) {
			Obj* word = make_word_change(thgc, local->last, local);
			add_list(thgc, item->children, (char*)word);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _AtLetter) {
			Obj* addr = make_cobj(thgc, _Address, OT_None, local->last);
			add_list(thgc, item->children, (char*)addr);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _Number) {
			Obj* num = make_cobj(thgc, _CNumber, OT_None, local->last);
			add_list(thgc, item->children, (char*)num);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _Decimal) {
			Obj* fv = make_cobj(thgc, _FloatVal, OT_None, local->last);
			add_list(thgc, item->children, (char*)fv);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _Str) {
			Obj* so = make_cobj(thgc, _StrObj, OT_None, local->last);
			add_list(thgc, item->children, (char*)so);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == _BracketL) {
			ATSSpan* letter = local->last;
			letter->error = NULL;
			local->Next();
			ATSSpan* outL = NULL;
			Block* block = ParseLines(thgc, local, _Semicolon, _Comma, _Kaigyou, _BracketR, OT_Bracket, comments, error, &outL);
			block->letter = letter;
			add_list(thgc, item->children, (char*)block);
		}
		else if (local->lasttype == _BlockL) {
			ATSSpan* letter = local->last;
			letter->error = NULL;
			local->Next();
			ATSSpan* outL = NULL;
			Block* block = ParseLines(thgc, local, _Semicolon, _Comma, _Kaigyou, _BlockR, OT_Block, comments, error, &outL);
			block->letter = letter;
			add_list(thgc, item->children, (char*)block);
		}
		else if (local->lasttype == _BraceL) {
			ATSSpan* letter = local->last;
			letter->error = NULL;
			local->Next();
			Obj* callblock = ParseBlock(thgc, local, _BraceR, comments, error);
			callblock->letter = letter;
			add_list(thgc, item->children, (char*)callblock);
		}
		else if (!first) {
			if (local->lasttype == _Dot || local->lasttype == _Left || local->lasttype == _Right) {
				PrimOp* pop = (PrimOp*)GC_alloc(thgc, _PrimOp);
				pop->letter = local->last;
				pop->objtype = local->lasttype;
				add_list(thgc, item->children, (char*)pop);
				local->last->error = NULL;
				local->Next();
				first = true;
				continue;
			}
			else {
				add_list(thgc, item->children, (char*)make_cobj(thgc, _CObj, OT_None, local->last));
				return (Obj*)item;
			}
		}
		else {
			add_list(thgc, item->children, (char*)make_cobj(thgc, _CObj, OT_None, local->last));
			return (Obj*)item;
		}
		first = false;
	}
}
