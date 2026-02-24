
ATSSpan* make_span(ThreadGC* thgc, FontId font, LetterType t, int start, int end, uint32_t color, String* text) {
	ATSSpan* s = (ATSSpan*)GC_alloc(thgc, CType::_ATSSpan);
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
	List* list = create_list(thgc, sizeof(List*), CType::_List);
	List* list0 = create_list(thgc, sizeof(ATSSpan*), CType::_List);
	add_list(thgc, list, (char*)list0);
	int len = str->size;
	int n = 0;
	for (int i = 0; i < len; i++) {
		wchar_t c = GetChar(str, i);
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_') {
			int j = i + 1;
			for (; j < len; j++) {
				c = GetChar(str, j);
				if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_') continue;
				break;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Name, i - n, j - n, 0x000000FF, SubString(thgc, str, i, j - i)));
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
			if (flv) add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Decimal, i - n, j - n, 0x000000FF, SubString(thgc, str, i, j - i)));
			else add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Number, i - n, j - n, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if (c == '"') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Str, i - n, j - n, 0x8B4513FF, SubString(thgc, str, i + 1, j - i - 1)));
					break;
				}
				c = GetChar(str, j);
				if (c == '"') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Str, i - n, j - n + 1, 0x8B4513FF, SubString(thgc, str, i + 1, j - i - 1)));
					j++;
					break;
				}
				else if (c == '\n' || c == '\0') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Str, i - n, j - n, 0x8B4513FF, SubString(thgc, str, i + 1, j - i - 1)));
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
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_And, i - n, j - n, 0x8B4513FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if (c == '^') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Mountain, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '`') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_HLetter, i - n, j - n, 0x000000FF, SubString(thgc, str, i + 1, j - i - 1)));
					break;
				}
				c = GetChar(str, j);
				if (c == '`') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_HLetter, i - n, j - n + 1, 0x000000FF, SubString(thgc, str, i + 1, j - i - 1)));
					j++;
					break;
				}
				else if (c == '<' || c == '>' || c == '~' || c == '$') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_HLetter, i - n, j - n, 0x000000FF, SubString(thgc, str, i + 1, j - i - 1)));
					break;
				}
				else if (c == '\n' || c == '\0') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_HLetter, i - n, j - n, 0x000000FF, SubString(thgc, str, i + 1, j - i - 1)));
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
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Space, i - n, j - n, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if (c == '\t') {
			int j = i + 1;
			for (; j < len; j++) {
				if (GetChar(str, j) != '\t') break;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Space, i - n, j - n, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
		else if (c == ':') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Colon, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == ';') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Semicolon, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '$') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Dolor, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == ',') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Comma, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '|') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Bou, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '#') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Sharp, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '%') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Percent, i - n, i - n + 1, 0x000000FF, NULL));
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
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_AtLetter, i, j, 0x000000FF, SubString(thgc, str, i, j - i)));
					i = j - 1;
				}
			}
		}
		else if (c == '\n') {
			list0 = create_list(thgc, sizeof(ATSSpan*), CType::_List);
			add_list(thgc, list, (char*)list0);
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Kaigyou, i, i + 1, 0x000000FF, NULL));
			n = i + 1;
		}
		else if (c == '=') {
			if (i + 1 < len && GetChar(str, i + 1) == '=') {
				add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_EqualEqual, i - n, i - n + 2, 0x000000FF, NULL));
				i++;
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Equal, i - n, i - n + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '!') {
			if (i + 1 < len && GetChar(str, i + 1) == '=') {
				add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_NotEqual, i - n, i - n + 2, 0x000000FF, NULL));
				i++;
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Not, i - n, i - n + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '+') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Plus, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '-') {
			if (i + 1 < len) {
				wchar_t c2 = GetChar(str, i + 1);
				if (c2 == '>') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Right, i - n, i -n + 2, 0x000000FF, NULL));
					i++;
				}
				else if (c2 == '-') {
					i++;
					int j = i + 1;
					for (; j < len; j++) {
						c2 = GetChar(str, j);
						if (c2 == '\n' || c2 == '\0') break;
					}
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_CommentSingle, i - n - 1, j - n, 0x008000FF, SubString(thgc, str, i + 1, j - i - 1)));
					i = j - 1;
				}
				else {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Minus, i - n, i - n + 1, 0x000000FF, NULL));
				}
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Minus, i - n, i - n + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '<') {
			if (i + 1 < len) {
				wchar_t c2 = GetChar(str, i + 1);
				if (c2 == '-') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Left, i - n, i - n + 2, 0x000000FF, NULL));
					i++;
				}
				else if (c2 == '=') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_LessEqual, i - n, i - n + 2, 0x000000FF, NULL));
					i++;
				}
				else if (c2 == '&') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_StringTag, i - n, i - n + 2, 0x000000FF, NULL));
					i++;
				}
				else {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_LessThan, i - n, i - n + 1, 0x000000FF, NULL));
				}
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_LessThan, i - n, i - n + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '>') {
			if (i + 1 < len) {
				wchar_t c2 = GetChar(str, i + 1);
				if (c2 == '=') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_MoreEqual, i- n, i - n + 2, 0x000000FF, NULL));
					i++;
				}
				else if (c2 == '>') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_RightRight, i - n, i - n + 2, 0x000000FF, NULL));
					i++;
				}
				else {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_MoreThan, i - n, i - n + 1, 0x000000FF, NULL));
				}
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_MoreThan, i - n, i - n + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '*') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Mul, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '/') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Div, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '~') {
			if (i + 1 < len && GetChar(str, i + 1) == '~') {
				i++;
				if (i + 1 < len && GetChar(str, i + 1) == '~') {
					i++;
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_NyoroNyoroNyoro, i - n - 2, i - n + 1, 0x000000FF, NULL));
				}
				else {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_NyoroNyoro, i - n - 1, i - n + 1, 0x000000FF, NULL));
				}
			}
			else {
				add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Nyoro, i - n, i - n + 1, 0x000000FF, NULL));
			}
		}
		else if (c == '.') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Dot, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '(') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_BracketL, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == ')') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_BracketR, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '[') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_BlockL, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == ']') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_BlockR, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '{') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_BraceL, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '}') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_BraceR, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c == '\0') {
			break;
		}
		else if (c == '?') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Question, i - n, i - n + 1, 0x000000FF, NULL));
		}
		else if (c >= 256) {
			int j = i + 1;
			for (; j < len; j++) {
				if (GetChar(str, j) < 256) break;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_CommentSingle, i - n, j - n, 0x000000FF, SubString(thgc, str, i, j - i)));
			i = j - 1;
		}
	}
	return list;
}

// ============================================================
// パーサー構造体定義
// ============================================================

// 前方宣言
struct Obj;
struct Block;
struct Primary;
struct Master;

// 仮想関数テーブル (C#のvirtual/overrideに対応)
struct ObjVT {
	Obj* (*exeZ)(ThreadGC*, Obj*, Master*);
	Obj* (*exepZ)(ThreadGC*, Obj*, int*, Master*, Primary*);
	Obj* (*GetterZ)(ThreadGC*, Obj*, Master*);
	Obj* (*SelfZ)(ThreadGC*, Obj*, Master*);
	Obj* (*opeZ)(ThreadGC*, Obj*, String*, Master*, Obj*);
	Obj* (*PrimaryZ)(ThreadGC*, Obj*, int*, Master*, Primary*, Obj*);
	Obj* (*exeA)(ThreadGC*, Obj*, Master*);
	Obj* (*exepA)(ThreadGC*, Obj*, int*, Master*, Primary*);
	Obj* (*GetterA)(ThreadGC*, Obj*, Master*);
	Obj* (*SelfA)(ThreadGC*, Obj*, Master*);
	Obj* (*opeA)(ThreadGC*, Obj*, String*, Master*, Obj*);
	Obj* (*PrimaryA)(ThreadGC*, Obj*, int*, Master*, Primary*, Obj*);
	Obj* (*exeB)(ThreadGC*, Obj*, Master*);
	Obj* (*exepB)(ThreadGC*, Obj*, int*, Master*, Primary*);
	Obj* (*GetterB)(ThreadGC*, Obj*, Master*);
	Obj* (*SelfB)(ThreadGC*, Obj*, Master*);
	Obj* (*opeB)(ThreadGC*, Obj*, String*, Master*, Obj*);
	Obj* (*PrimaryB)(ThreadGC*, Obj*, int*, Master*, Primary*, Obj*);
};

// Primary用VTableの前方宣言 (objvt.hで定義)
extern ObjVT vt_Primary;

struct Obj {
	LetterType objtype;
	ATSSpan* letter;
	List* children;
	ObjVT* vt;
	String* rename;
	String* version;
};

struct Block {
	LetterType objtype;
	ATSSpan* letter;
	List* children;
	ObjVT* vt;
	Map* branchmap;
	Map* labelmap;
	Map* labelmapn;
	ATSSpan* letter2;
	Map* vmapA;
	List* rets;
	Obj* obj;
};

struct TagBlock {
	LetterType objtype;
	ATSSpan* letter;
	List* children;
	ObjVT* vt;
	Map* branchmap;
	Map* labelmap;
	Map* labelmapn;
	ATSSpan* letter2;
	ATSSpan* tagletter;
	Map* vmapA;
	List* rets;
	Obj* obj;
};

struct Primary {
	LetterType objtype;
	ATSSpan* letter;
	List* children;
	ObjVT* vt;
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

struct Operator : Obj {
};

struct Master {
	NewElement* top;
	NewElement* elem;
	LetterType lasttype;
	ATSSpan* last;
	int n;
	void init(ThreadGC* thgc, NewElement* _top) {
		elem = top = _top; n = 0;
		operators = create_list(thgc, sizeof(List*), CType::_List);
		// = : in
		List* op0 = create_list(thgc, sizeof(LetterType), CType::_Struct);
		add_list(thgc, op0, (char*)LetterType::_Equal);
		add_list(thgc, op0, (char*)LetterType::_Colon);
		add_list(thgc, operators, (char*)op0);
		// < <= > >= == !=
		List* op1 = create_list(thgc, sizeof(LetterType), CType::_Struct);
		add_list(thgc, op1, (char*)LetterType::_LessThan);
		add_list(thgc, op1, (char*)LetterType::_LessEqual);
		add_list(thgc, op1, (char*)LetterType::_MoreThan);
		add_list(thgc, op1, (char*)LetterType::_MoreEqual);
		add_list(thgc, op1, (char*)LetterType::_EqualEqual);
		add_list(thgc, op1, (char*)LetterType::_NotEqual);
		add_list(thgc, operators, (char*)op1);
		// + -
		List* op2 = (List*)create_list(thgc, sizeof(LetterType), CType::_List);
		add_list(thgc, op2, (char*)LetterType::_Plus);
		add_list(thgc, op2, (char*)LetterType::_Minus);
		add_list(thgc, operators, (char*)op2);
		// * /
		List* op3 = create_list(thgc, sizeof(LetterType), CType::_List);
		add_list(thgc, op3, (char*)LetterType::_Mul);
		add_list(thgc, op3, (char*)LetterType::_Div);
		add_list(thgc, operators, (char*)op3);
		labelmap = create_mapy(thgc, CType::_List);
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
		if (elem->type == LetterType::_ElemEnd) {
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
	List* blocks;
	List* blockslist;
	List* comments;
	Obj* gene;
	Obj* Object;
	Obj* Void;
	Obj* Int;
	Obj* Short;
	Obj* StrT;
	Obj* Bool;
	Obj* FloatT;
	Obj* MouseEventT;
	Obj* KeyEventT;
};

// ============================================================
// Z解析用の型構造体
// ============================================================

struct VariClass : Obj {
	int n;
};

struct ArrType : Obj {
	Obj* base;
};

struct FuncType2 : Obj {
	Obj* rettype;
	List* draws;
};

struct Variable : Obj {
	Obj* vartype;
};

struct Function : Obj {
	Obj* rettype;
	Block* draw;
	List* blocks;
};

struct GenericFunction : Obj {
	Obj* rettype;
	Block* draw;
	List* blocks;
	Map* vmap;
};

struct ClassObj : Obj {
};

struct ModelObj : Obj {
	ATSSpan* letter2;
	Block* draw;
};

struct GeneObj : Obj {
	ATSSpan* letter2;
	String* name;
	Block* call;
	List* blocks;
	Map* vmap;
};

struct Generic : Obj {
	Map* vmap;
};

struct GenericObj : Obj {
	Obj* generic;
	List* draws;
};

struct ObjBlock : Obj {
	Obj* obj;
	int n;
};

// ============================================================
// ヘルパー関数
// ============================================================

Obj* make_cobj(ThreadGC* thgc, int ctype, LetterType ot, ATSSpan* letter) {
	Obj* o = (Obj*)GC_alloc(thgc, ctype);
	o->objtype = ot;
	o->letter = letter;
	o->children = create_list(thgc, sizeof(Obj*), CType::_List);
	o->vt = NULL;
	o->rename = NULL;
	o->version = NULL;
	return o;
}

Block* make_cblock(ThreadGC* thgc, int ctype, LetterType ot) {
	Block* b = (Block*)GC_alloc(thgc, ctype);
	b->objtype = ot;
	b->letter = NULL;
	b->children = create_list(thgc, sizeof(Obj*), CType::_List);
	b->vt = NULL;
	b->branchmap = create_mapy(thgc, CType::_List);
	b->labelmap = create_mapy(thgc, CType::_List);
	b->labelmapn = create_mapy(thgc, CType::_List);
	b->letter2 = NULL;
	b->vmapA = create_mapy(thgc, CType::_List);
	b->rets = create_list(thgc, sizeof(Obj*), CType::_List);
	b->obj = NULL;
	return b;
}

Obj* make_word_change(ThreadGC* thgc, ATSSpan* letter, Master* local) {
	Obj* o = make_cobj(thgc, CType::_Word, LetterType::OT_Word, letter);
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
	local->analblocks = create_list(thgc, sizeof(Block*), CType::_List);
	local->Next();
	Block* item = make_cblock(thgc, CType::_CallBlock, LetterType::OT_Call1);
	add_list(thgc, item->children, (char*)make_cobj(thgc, CType::_CObj, LetterType::OT_Call1, NULL));
	ATSSpan* outLetter = NULL;
	Block* item2 = ParseLines(thgc, local, LetterType::_Kaigyou, LetterType::_Semicolon, LetterType::_Comma, LetterType::_End, LetterType::OT_Call2, 0, error, &outLetter);
	add_list(thgc, item->children, (char*)item2);
	add_list(thgc, local->analblocks, (char*)item2);
	return (Obj*)item;
}

// ============================================================
// ParseBlock
// ============================================================
Obj* ParseBlock(ThreadGC* thgc, Master* local, LetterType end, int comments, bool* error) {
	Block* item;
	if (end == LetterType::_MoreThan) {
		item = (Block*)GC_alloc(thgc, CType::_TagBlock);
		item->objtype = LetterType::OT_TagBlock;
		item->letter = NULL;
		item->children = create_list(thgc, sizeof(Obj*), CType::_List);
		item->vt = NULL;
		item->branchmap = create_mapy(thgc, CType::_List);
		item->labelmap = create_mapy(thgc, CType::_List);
		item->labelmapn = create_mapy(thgc, CType::_List);
		item->letter2 = NULL;
	} else {
		item = make_cblock(thgc, CType::_CallBlock, LetterType::OT_CallBlock);
	}
head:
	if (local->lasttype == LetterType::_Kaigyou) local->Next();
	ATSSpan* outLetter = NULL;
	Block* item2 = ParseLines(thgc, local, LetterType::_Kaigyou, LetterType::_Comma, LetterType::_Semicolon, LetterType::_Bou, LetterType::OT_Call1, comments, error, &outLetter);
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
	Block* item = ParseLines(thgc, local, LetterType::_Kaigyou, LetterType::_Semicolon, LetterType::_Comma, finish, LetterType::OT_Call2, comments, error, &outLetter);
	return item;
}

// ============================================================
// ParseLines
// ============================================================
Block* ParseLines(ThreadGC* thgc, Master* local, LetterType sub, LetterType sub2, LetterType sub3, LetterType finish, LetterType type, int comments, bool* error, ATSSpan** outLetter) {
	Block* item = make_cblock(thgc, CType::_Block, type);
	bool tag = false;
	if (finish == LetterType::_MoreThan) tag = true;
head:
	if (local->lasttype == LetterType::_Sharp) {
		local->last->error = NULL;
		local->Next();
		if (local->lasttype == LetterType::_Sharp) {
			local->last->error = NULL;
			local->Next();
			if (local->lasttype == LetterType::_Name || local->lasttype == LetterType::_Number || local->lasttype == LetterType::_Str) {
				String* name = local->last->text;
				ATSSpan* let = local->last;
				local->last->error = NULL;
				local->Next();
				Label* label0 = (Label*)get_mapy(local->labelmap, name);
				if (label0 == NULL) {
					label0 = (Label*)GC_alloc(thgc, CType::_Label);
					label0->letter = let; label0->name = name; label0->n = 0;
					label0->labelmap = create_mapy(thgc, CType::_List);
					add_mapy(thgc, local->labelmap, name, (char*)label0);
				}
				Label* label = (Label*)get_mapy(item->branchmap, name);
				if (label == NULL) {
					label = (Label*)GC_alloc(thgc, CType::_Label);
					label->letter = let; label->name = name; label->n = item->children->size;
					label->labelmap = create_mapy(thgc, CType::_List);
					add_mapy(thgc, item->branchmap, name, (char*)label);
				}
				if (local->lasttype == LetterType::_Dot) {
					local->last->error = NULL;
					local->Next();
					if (local->lasttype == LetterType::_Name || local->lasttype == LetterType::_Number || local->lasttype == LetterType::_Str) {
						String* name2 = local->last->text;
						Label* label00 = (Label*)get_mapy(label0->labelmap, name2);
						if (label00 == NULL) {
							label00 = (Label*)GC_alloc(thgc, CType::_Label);
							label00->letter = local->last; label00->name = name2; label00->n = 0;
							label00->labelmap = create_mapy(thgc, CType::_List);
							add_mapy(thgc, label0->labelmap, name2, (char*)label00);
						}
						Label* label2 = (Label*)get_mapy(label->labelmap, name2);
						if (label2 == NULL) {
							label2 = (Label*)GC_alloc(thgc, CType::_Label);
							label2->letter = local->last; label2->name = name2; label2->n = item->children->size;
							label2->labelmap = create_mapy(thgc, CType::_List);
							add_mapy(thgc, label->labelmap, name2, (char*)label2);
						}
						local->last->error = NULL;
						local->Next();
						if (local->lasttype == LetterType::_Dot) {
							local->Next();
							if (local->lasttype == LetterType::_Name || local->lasttype == LetterType::_Number || local->lasttype == LetterType::_Str) {
								String* name3 = local->last->text;
								Label* label000 = (Label*)get_mapy(label00->labelmap, name);
								if (label000 == NULL) {
									label000 = (Label*)GC_alloc(thgc, CType::_Label);
									label000->letter = local->last; label000->name = name3; label000->n = 0;
									label000->labelmap = create_mapy(thgc, CType::_List);
									add_mapy(thgc, label00->labelmap, name, (char*)label000);
								}
								Label* label3 = (Label*)get_mapy(label2->labelmap, name3);
								if (label3 == NULL) {
									label3 = (Label*)GC_alloc(thgc, CType::_Label);
									label3->letter = local->last; label3->name = name3; label3->n = item->children->size;
									label3->labelmap = create_mapy(thgc, CType::_List);
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
		else if (local->lasttype == LetterType::_Name || local->lasttype == LetterType::_Number || local->lasttype == LetterType::_Str) {
			String* name = local->last->text;
			ATSSpan* let = local->last;
			let->error = NULL;
			local->Next();
			if (local->lasttype == LetterType::_Dot) {
				local->last->error = NULL;
				local->Next();
				if (local->lasttype == LetterType::_Name || local->lasttype == LetterType::_Number || local->lasttype == LetterType::_Str) {
					Label* label = (Label*)get_mapy(item->labelmap, name);
					if (label == NULL) {
						label = (Label*)GC_alloc(thgc, CType::_Label);
						label->letter = local->last; label->name = local->last->text; label->n = item->children->size;
						label->labelmap = create_mapy(thgc, CType::_List);
						add_mapy(thgc, item->labelmap, name, (char*)label);
						add_mapyn(thgc, item->labelmapn, (char*)(intptr_t)item->children->size, (char*)label);
					}
					if (get_mapy(label->labelmap, local->last->text) != NULL) {
						local->last->error = createString(thgc, (char*)"Label syntax error", 18, 1);
						*error = true;
					} else {
						Label* label2 = (Label*)GC_alloc(thgc, CType::_Label);
						label2->letter = local->last; label2->name = local->last->text; label2->n = item->children->size;
						label2->labelmap = create_mapy(thgc, CType::_List);
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
					Label* label = (Label*)GC_alloc(thgc, CType::_Label);
					label->letter = let; label->name = name; label->n = item->children->size;
					label->labelmap = create_mapy(thgc, CType::_List);
					add_mapy(thgc, item->labelmap, name, (char*)label);
					add_mapyn(thgc, item->labelmapn, (char*)(intptr_t)item->children->size, (char*)label);
				}
			}
		}
		else if (local->lasttype == LetterType::_Str) {
			Label* label = (Label*)GC_alloc(thgc, CType::_Label);
			label->letter = local->last; label->name = local->last->text; label->n = item->children->size;
			label->labelmap = create_mapy(thgc, CType::_List);
			add_mapy(thgc, item->labelmap, local->last->text, (char*)label);
			add_mapyn(thgc, item->labelmapn, (char*)(intptr_t)item->children->size, (char*)label);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_Number) {
			Label* label = (Label*)GC_alloc(thgc, CType::_Label);
			label->letter = local->last; label->name = local->last->text; label->n = item->children->size;
			label->labelmap = create_mapy(thgc, CType::_List);
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
	else if (local->lasttype == LetterType::_End) {
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
	List* opdef = *(List**)get_list(local->operators, n);
	for (int i = 0; i < opdef->size; i++) {
		LetterType op = *(LetterType*)get_list(opdef, i);
		if (local->lasttype == op) {
			if (local->lasttype == LetterType::_MoreThan && tag) {
				return item;
			}
			Obj* item2 = make_cobj(thgc, CType::_COperator, op, local->last);
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
		if (local->lasttype == LetterType::_StringTag) {
			ATSSpan* letter = local->last;
			local->Next();
			Obj* tagblock = ParseBlock(thgc, local, LetterType::_MoreThan, 1, error);
			tagblock->letter = letter;
			letter->error = NULL;
		}
		else if (local->lasttype == LetterType::_End || local->lasttype == LetterType::_Kaigyou) {
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
	Primary* item = (Primary*)GC_alloc(thgc, CType::_Primary);
	item->objtype = LetterType::OT_None;
	item->letter = local->last;
	item->children = create_list(thgc, sizeof(Obj*), CType::_List);
	item->vt = &vt_Primary;
	item->singleops = create_list(thgc, sizeof(SingleOp*), CType::_List);

	if (comments > 0) {
		if (local->lasttype == LetterType::_Dot || local->lasttype == LetterType::_Mul || local->lasttype == LetterType::_RightRight) {
			SingleOp* sop = (SingleOp*)GC_alloc(thgc, CType::_SingleOp);
			sop->letter = local->last;
			add_list(thgc, item->singleops, (char*)sop);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_Plus || local->lasttype == LetterType::_Minus || local->lasttype == LetterType::_Mul || local->lasttype == LetterType::_Div) {
			SingleOp* sop = (SingleOp*)GC_alloc(thgc, CType::_SingleOp);
			sop->letter = local->last;
			add_list(thgc, item->singleops, (char*)sop);
			local->last->error = NULL;
			local->Next();
		}
	}
	else if (local->lasttype == LetterType::_Plus || local->lasttype == LetterType::_Minus || local->lasttype == LetterType::_Not) {
		SingleOp* sop = (SingleOp*)GC_alloc(thgc, CType::_SingleOp);
		sop->letter = local->last;
		add_list(thgc, item->singleops, (char*)sop);
		local->last->error = NULL;
		local->Next();
	}

	bool first = true;
	for (;;) {
		if (comments > 0 && type != LetterType::OT_Call1 && (local->lasttype == LetterType::_LessThan || local->lasttype == LetterType::_StringTag)) {
			ATSSpan* letter = local->last;
			letter->error = NULL;
			local->Next();
			Obj* tagblock = ParseBlock(thgc, local, LetterType::_MoreThan, comments, error);
			tagblock->letter = letter;
			add_list(thgc, item->children, (char*)tagblock);
		}
		else if (comments > 0 && local->lasttype == LetterType::_Dolor) {
			Obj* dolor = make_cobj(thgc, CType::_CDolor, LetterType::OT_Dolor, local->last);
			add_list(thgc, item->children, (char*)dolor);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_Nyoro) {
			Obj* comment = make_cobj(thgc, CType::_Comment, LetterType::OT_Comment, local->last);
			local->last->error = NULL;
			add_list(thgc, item->children, (char*)comment);
			local->Next();
			ATSSpan* outL = NULL;
			Block* ret = ParseLines(thgc, local, LetterType::_Kaigyou, LetterType::_Semicolon, LetterType::_Comma, LetterType::_NyoroNyoro, LetterType::OT_Comment, comments + 1, error, &outL);
			add_list(thgc, comment->children, (char*)ret);
		}
		else if (local->lasttype == LetterType::_NyoroNyoroNyoro) {
			Obj* comment2 = make_cobj(thgc, CType::_Comment2, LetterType::OT_Comment2, local->last);
			local->last->error = NULL;
			add_list(thgc, item->children, (char*)comment2);
			local->Next();
			ATSSpan* outL = NULL;
			Block* ret = ParseLines(thgc, local, LetterType::_Kaigyou, LetterType::_Semicolon, LetterType::_Comma, LetterType::_NyoroNyoro, LetterType::OT_Comment, comments + 1, error, &outL);
			add_list(thgc, comment2->children, (char*)ret);
		}
		else if (comments > 0 && local->lasttype == LetterType::_HLetter) {
			Obj* htm = make_cobj(thgc, CType::_HtmObj, LetterType::OT_Htm, local->last);
			add_list(thgc, item->children, (char*)htm);
			local->Next();
		}
		else if (local->lasttype == LetterType::_Mountain) {
			Obj* mtn = make_cobj(thgc, CType::_CMountain, LetterType::OT_Mountain, local->last);
			add_list(thgc, item->children, (char*)mtn);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_Question) {
			Obj* q = make_cobj(thgc, CType::_CQuestion, LetterType::_Question, local->last);
			add_list(thgc, item->children, (char*)q);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_Name) {
			Obj* word = make_word_change(thgc, local->last, local);
			add_list(thgc, item->children, (char*)word);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_AtLetter) {
			Obj* addr = make_cobj(thgc, CType::_Address, LetterType::_AtLetter, local->last);
			add_list(thgc, item->children, (char*)addr);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_Number) {
			Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, local->last);
			add_list(thgc, item->children, (char*)num);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_Decimal) {
			Obj* fv = make_cobj(thgc, CType::_FloatVal, LetterType::OT_FloatVal, local->last);
			add_list(thgc, item->children, (char*)fv);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_Str) {
			Obj* so = make_cobj(thgc, CType::_StrObj, LetterType::OT_StrObj, local->last);
			add_list(thgc, item->children, (char*)so);
			local->last->error = NULL;
			local->Next();
		}
		else if (local->lasttype == LetterType::_BracketL) {
			ATSSpan* letter = local->last;
			letter->error = NULL;
			local->Next();
			ATSSpan* outL = NULL;
			Block* block = ParseLines(thgc, local, LetterType::_Semicolon, LetterType::_Comma, LetterType::_Kaigyou, LetterType::_BracketR, LetterType::OT_Bracket, comments, error, &outL);
			block->letter = letter;
			add_list(thgc, item->children, (char*)block);
		}
		else if (local->lasttype == LetterType::_BlockL) {
			ATSSpan* letter = local->last;
			letter->error = NULL;
			local->Next();
			ATSSpan* outL = NULL;
			Block* block = ParseLines(thgc, local, LetterType::_Semicolon, LetterType::_Comma, LetterType::_Kaigyou, LetterType::_BlockR, LetterType::OT_Block, comments, error, &outL);
			block->letter = letter;
			add_list(thgc, item->children, (char*)block);
		}
		else if (local->lasttype == LetterType::_BraceL) {
			ATSSpan* letter = local->last;
			letter->error = NULL;
			local->Next();
			Obj* callblock = ParseBlock(thgc, local, LetterType::_BraceR, comments, error);
			callblock->letter = letter;
			add_list(thgc, item->children, (char*)callblock);
		}
		else if (!first) {
			if (local->lasttype == LetterType::_Dot || local->lasttype == LetterType::_Left || local->lasttype == LetterType::_Right) {
				PrimOp* pop = (PrimOp*)GC_alloc(thgc, CType::_PrimOp);
				pop->letter = local->last;
				pop->objtype = local->lasttype;
				add_list(thgc, item->children, (char*)pop);
				local->last->error = NULL;
				local->Next();
				first = true;
				continue;
			}
			else {
				add_list(thgc, item->children, (char*)make_cobj(thgc, CType::_CObj, LetterType::OT_None, local->last));
				return (Obj*)item;
			}
		}
		else {
			add_list(thgc, item->children, (char*)make_cobj(thgc, CType::_CObj, LetterType::OT_None, local->last));
			return (Obj*)item;
		}
		first = false;
	}
}

// ============================================================
// CompileJapanese - split text into words
// ============================================================
// char types: kanji, katakana, hiragana, ascii
// particles after kanji/katakana are included in the word
// hiragana splits at particle boundaries

static inline int jpCharType(wchar_t c) {
	// 0=other, 1=hiragana, 2=katakana, 3=kanji, 4=ascii/digit
	if (c >= 0x3040 && c <= 0x309F) return 1; // hiragana
	if (c >= 0x30A0 && c <= 0x30FF) return 2; // katakana
	if (c >= 0x4E00 && c <= 0x9FFF) return 3; // CJK unified
	if (c >= 0x3400 && c <= 0x4DBF) return 3; // CJK ext-A
	if (c >= 0xF900 && c <= 0xFAFF) return 3; // CJK compat
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
	    (c >= '0' && c <= '9') || c == '_') return 4; // ascii/digit
	return 0;
}

static inline bool isJoshi(wchar_t c) {
	// Japanese particles (ha,ga,wo,ni,de,no,to,mo,he,ka,ya,yo,ne,na,wa,zo,ze,ba)
	if (c == 0x306F || c == 0x304C || c == 0x3092 || c == 0x306B ||
	    c == 0x3067 || c == 0x306E || c == 0x3068 || c == 0x3082 ||
	    c == 0x3078 || c == 0x304B || c == 0x3084 || c == 0x3088 ||
	    c == 0x306D || c == 0x306A || c == 0x308F || c == 0x305E ||
	    c == 0x305C || c == 0x3070) return true;
	// Punctuation (half-width)
	if (c == ' ' || c == '.' || c == ',' || c == '!' || c == '?' ||
	    c == ';' || c == ':' || c == '\t') return true;
	// Punctuation (full-width)
	if (c == 0x3002 || c == 0x3001 || c == 0xFF0E || c == 0xFF0C ||
	    c == 0xFF01 || c == 0xFF1F || c == 0xFF1B || c == 0xFF1A ||
	    c == 0x30FB || c == 0x3000) return true;
	return false;
}

// 2-char particles: kara,made,yori,nado,dake,shite,tte,node,noni,kedo,demo,daga,deha,nimo,toha,heha,kamo
static inline bool isJoshi2(wchar_t c1, wchar_t c2) {
	return (c1 == 0x304B && c2 == 0x3089) ||  // kara
	       (c1 == 0x307E && c2 == 0x3067) ||  // made
	       (c1 == 0x3088 && c2 == 0x308A) ||  // yori
	       (c1 == 0x306A && c2 == 0x3069) ||  // nado
	       (c1 == 0x3060 && c2 == 0x3051) ||  // dake
	       (c1 == 0x3057 && c2 == 0x3066) ||  // shite
	       (c1 == 0x3063 && c2 == 0x3066) ||  // tte
	       (c1 == 0x306E && c2 == 0x3067) ||  // node
	       (c1 == 0x306E && c2 == 0x306B) ||  // noni
	       (c1 == 0x3051 && c2 == 0x3069) ||  // kedo
	       (c1 == 0x3067 && c2 == 0x3082) ||  // demo
	       (c1 == 0x3060 && c2 == 0x304C) ||  // daga
	       (c1 == 0x3067 && c2 == 0x306F) ||  // deha
	       (c1 == 0x306B && c2 == 0x3082) ||  // nimo
	       (c1 == 0x3068 && c2 == 0x306F) ||  // toha
	       (c1 == 0x3078 && c2 == 0x306F) ||  // heha
	       (c1 == 0x304B && c2 == 0x3082);    // kamo
}

List* CompileJapanese(ThreadGC* thgc, String* str, FontId font) {
	List* list0 = create_list(thgc, sizeof(ATSSpan*), CType::_List);
	int len = str->size;
	int i = 0;
	while (i < len) {
		wchar_t c = GetChar(str, i);

		// newline
		if (c == '\n') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Kaigyou, i, i + 1, 0x000000FF, NULL));
			int j = i + 1;
			for (; j < len; j++) {
				wchar_t c2 = GetChar(str, j);
				if (c2 == ' ') {
					int si = j;
					j++;
					for (; j < len; j++) {
						if (GetChar(str, j) != ' ') break;
					}
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Space, si, j, 0x000000FF, SubString(thgc, str, si, j - si)));
					j--;
				}
				else if (c2 == '\n') {
					add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_Kaigyou, j, j + 1, 0x000000FF, NULL));
				}
				else break;
			}
			i = j;
			continue;
		}
		// null terminator
		if (c == '\0') {
			add_list(thgc, list0, (char*)make_span(thgc, font, LetterType::_End, i, i + 1, 0x000000FF, NULL));
			break;
		}

		int ct = jpCharType(c);

		if (ct == 0) {
			int ep = i + 1;
			if (isJoshi(c)) {
				while (ep < len && isJoshi(GetChar(str, ep))) ep++;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font,
				LetterType::_CommentSingle, i, ep, 0x000000FF,
				SubString(thgc, str, i, ep - i)));
			i = ep;
		}
		else if (ct == 4) {
			int j = i + 1;
			while (j < len && jpCharType(GetChar(str, j)) == 4) j++;
			int ep = j;
			while (ep < len && isJoshi(GetChar(str, ep))) ep++;
			add_list(thgc, list0, (char*)make_span(thgc, font,
				LetterType::_CommentSingle, i, ep, 0x000000FF,
				SubString(thgc, str, i, ep - i)));
			i = ep;
		}
		else if (ct == 2 || ct == 3) {
			int j = i + 1;
			while (j < len && jpCharType(GetChar(str, j)) == ct) j++;
			int ep = j;
			if (j < len) {
				wchar_t cj = GetChar(str, j);
				int cjt = jpCharType(cj);
				if (cjt == 1 || cjt == 0) {
					if (j + 1 < len && isJoshi2(cj, GetChar(str, j + 1))) {
						ep = j + 2;
					}
					else if (isJoshi(cj)) {
						ep = j + 1;
					}
				}
			}
			add_list(thgc, list0, (char*)make_span(thgc, font,
				LetterType::_CommentSingle, i, ep, 0x000000FF,
				SubString(thgc, str, i, ep - i)));
			i = ep;
		}
		else {
			// hiragana: split at particle boundary (include particle)
			int j = i + 1;
			while (j < len) {
				wchar_t cj = GetChar(str, j);
				if (jpCharType(cj) != 1) break;
				// 2-char particle check
				if (j + 1 < len && isJoshi2(cj, GetChar(str, j + 1))) {
					j += 2;
					break;
				}
				if (isJoshi(cj)) {
					j++;
					break;
				}
				j++;
			}
			add_list(thgc, list0, (char*)make_span(thgc, font,
				LetterType::_CommentSingle, i, j, 0x000000FF,
				SubString(thgc, str, i, j - i)));
			i = j;
		}
	}
	return list0;
}

#include "objz.h"
#include "objvt.h"
#include "obj2a.h"
#include "obj2a2.h"
#include "obj2a3.h"
#include "obj2b.h"
#include "obj2b2.h"
#include "obj2b3.h"
