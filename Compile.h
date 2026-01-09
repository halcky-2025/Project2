
void initNewLetter(ThreadGC* thgc, NewLetter* letter, FontId font, enum LetterType type);
List* Compile(ThreadGC* thgc, String* str, FontId font) {
	List* list = create_list(thgc, sizeof(List*), true);
	List* list0 = create_list(thgc, sizeof(NewElement*), true);
	add_list(thgc, list, (char*)list0);
	int len = str->size / str->esize;
	for (int i = 0; i < len; i++) {
		wchar_t c = GetChar(str, i);
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
					initNewLetter(thgc, letter, font, _Letter);
					letter->text = SubString(thgc, str, i, j - i);
					add_list(thgc, list0, (char*)letter);
					break;
				}
				c = GetChar(str, j);
				if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_') {
					continue;
				}
				else {
					NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
					initNewLetter(thgc, letter, font, _Letter);
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
					NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
					initNewLetter(thgc, letter, font, _Number);
					letter->text = SubString(thgc, str, i, j - i);
					add_list(thgc, list0, (char*)letter);
					break;
				}
				c = GetChar(str, j);
				if ('0' <= c && c <= '9') {
					continue;
				}
				else {
					NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
					initNewLetter(thgc, letter, font, _Number);
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
					NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
					initNewLetter(thgc, letter,font, _Space);
					letter->text = SubString(thgc, str, i, j - i);
					add_list(thgc, list0, (char*)letter);
					break;
				}
				c = GetChar(str, j);
				if (c == ' ') {
					continue;
				}
				else {
					NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
					initNewLetter(thgc, letter, font, _Space);
					add_list(thgc, list0, (char*)letter);
					break;
				}
			}
			i = j - 1;
		}
		else if (c == '(') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _BracketL);
			letter->text = createString(thgc, (char*)"(", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == ')') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _BracketR);
			letter->text = createString(thgc, (char*)")", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '[') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _BlockL);
			letter->text = createString(thgc, (char*)"[", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == ']') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _BlockR);
			letter->text = createString(thgc, (char*)"]", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '{') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _BraceL);
			letter->text = createString(thgc, (char*)"{", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '}') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _BraceR);
			letter->text = createString(thgc, (char*)"}", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == ';') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _Semicolon);
			letter->text = createString(thgc, (char*)";", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == ',') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _Comma);
			letter->text = createString(thgc, (char*)",", 1, 1);
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
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _Plus);
			letter->text = createString(thgc, (char*)"+", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '-') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _Minus);
			letter->text = createString(thgc, (char*)"-", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '*') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _Mul);
			letter->text = createString(thgc, (char*)"*", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '/') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _Div);
			letter->text = createString(thgc, (char*)"/", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '%') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _Mod);
			letter->text = createString(thgc, (char*)"%", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
		else if (c == '=') {
			NewLetter* letter = (NewLetter*)GC_malloc(thgc, _LetterC);
			initNewLetter(thgc, letter, font, _Equal);
			letter->text = createString(thgc, (char*)"=", 1, 1);
			add_list(thgc, list0, (char*)letter);
		}
	}
	return list;
}