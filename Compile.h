
void initNewLetter(ThreadGC* thgc, NewLetter* letter, FontId font, enum LetterType type);
List* Compile(ThreadGC* thgc, String* str, FontId font) {
	List* list = create_list(thgc, sizeof(List*), true);
	List* list0 = create_list(thgc, sizeof(ATSSpan*), true);
	add_list(thgc, list, (char*)list0);
	int len = str->size / str->esize;
	for (int i = 0; i < len; i++) {
		wchar_t c = GetChar(str, i);
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
					letter->color = 0x000000FF;
					letter->font = font;
					letter->t = LetterType::_Name;
					letter->start = i; letter->end = j;
					letter->text = SubString(thgc, str, i, j - i);
					add_list(thgc, list0, (char*)letter);
					break;
				}
				c = GetChar(str, j);
				if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_') {
					continue;
				}
				else {
					ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
					letter->color = 0x000000FF;
					letter->font = font;
					letter->t = LetterType::_Name;
					letter->start = i; letter->end = j;
					letter->text = SubString(thgc, str, i, j - i);
					add_list(thgc, list0, (char*)letter);
					break;
				}
			}
			i = j - 1;
		}
		else if ('0' <= c && c <= '9') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
					letter->color = 0x000000FF;
					letter->font = font;
					letter->t = LetterType::_Number;
					letter->start = i; letter->end = j;
					letter->text = SubString(thgc, str, i, j - i);
					add_list(thgc, list0, (char*)letter);
					break;
				}
				c = GetChar(str, j);
				if ('0' <= c && c <= '9') {
					continue;
				}
				else {
					ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
					letter->color = 0x000000FF;
					letter->font = font;
					letter->t = LetterType::_Number;
					letter->start = i; letter->end = j;
					letter->text = SubString(thgc, str, i, j - i);
					add_list(thgc, list0, (char*)letter);
					break;
				}
			}
			i = j - 1;
		}
		else if (c == ' ') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
					letter->color = 0x000000FF;
					letter->font = font;
					letter->t = LetterType::_Space;
					letter->start = i; letter->end = j;
					add_list(thgc, list0, (char*)letter);
					break;
				}
				c = GetChar(str, j);
				if (c == ' ') {
					continue;
				}
				else {
					ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
					letter->color = 0x000000FF;
					letter->font = font;
					letter->t = LetterType::_Space;
					letter->start = i; letter->end = j;
					add_list(thgc, list0, (char*)letter);
					break;
				}
			}
			i = j - 1;
		}
		else if (c == '(') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_BracketL;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == ')') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_BracketR;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '[') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_BlockL;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == ']') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_BlockR;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '{') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_BraceL;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '}') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_BraceR;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == ';') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_Semicolon;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == ',') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_Comma;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '\0') {
			break;
		}
		else if (c == '\n') {
			list0 = create_list(thgc, sizeof(NewElement*), true);
			add_list(thgc, list, (char*)list0);
		}
		else if (c == '+') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_Plus;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '-') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_Minus;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '*') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_Mul;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '/') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_Div;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '%') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_Mod;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '=') {
			ATSSpan* letter = (ATSSpan*)GC_alloc(thgc, _ATSSpan);
			letter->color = 0x000000FF;
			letter->font = font;
			letter->t = LetterType::_Equal;
			letter->start = i; letter->end = i + 1;
			add_list(thgc, list0, (char*)letter);
		}
	}
	return list0;
}