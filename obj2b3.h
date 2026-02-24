// ============================================================
// obj2b3.h - B解析パス (vtable dispatch版) - Block 3
// Comment, LinearFunction, AnimationFunction, Comment2,
// Clones, Dolor, HtmObj, TagBlock, ElemType, ElemObj,
// Signal, SignalFunction, ServerClient, ServerFunction,
// Connect, ConnectStock, Address, Mountain, Sum, Question
// ============================================================

// 前方宣言
Obj* exeB_Comment(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_Comment(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_LinearFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_Comment2(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_Comment2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepB_Dolor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepB_HtmObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepB_TagBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeB_TagBlock(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* opeB_ElemObj(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2);
Obj* exepB_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_SignalFunction(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryB_SignalFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_ServerFunction(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryB_ServerFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_Connect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_ConnectStock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_Address(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Mountain(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Sum(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Question(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);

// ============================================================
// Comment::exeB, exepB
// ============================================================
Obj* exeB_Comment(ThreadGC* thgc, Obj* self, Master* local) {
	Block* block = (Block*)(*(Obj**)get_list(self->children, 0));
	add_list(thgc, local->blocks, (char*)&block);
	vt_exeB(thgc, (Obj*)block, local);
	local->blocks->size--;
	return self;
}
Obj* exepB_Comment(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Block* block = (Block*)(*(Obj**)get_list(self->children, 0));
	add_list(thgc, local->blocks, (char*)&block);
	vt_exeB(thgc, (Obj*)block, local);
	local->blocks->size--;
	return self;
}

// ============================================================
// LinearFunction::PrimaryB
// ============================================================
Obj* PrimaryB_LinearFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		(*n)++;
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		// AnimationFunction creation
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Comment2::exeB, exepB
// ============================================================
Obj* exeB_Comment2(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* exepB_Comment2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }

// ============================================================
// Dolor::exepB
// ============================================================
Obj* exepB_Dolor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		// $type, $func handling
		return self;
	} else if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// HtmObj::exepB
// ============================================================
Obj* exepB_HtmObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	// if (local->comment) return null
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}

// ============================================================
// TagBlock::exepB, exeB
// ============================================================
Obj* exepB_TagBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* exeB_TagBlock(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// ElemType::exepB, PrimaryB
// ============================================================
Obj* exepB_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (*n + 1 < primary->children->size) {
		Obj* val2 = *(Obj**)get_list(primary->children, *n + 1);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			// declareB(word.name, new ElemObj)
			return self;
		}
	}
	return self;
}
Obj* PrimaryB_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		// declareB(word.name, new ElemObj)
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ElemObj::opeB
// ============================================================
Obj* opeB_ElemObj(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) {
	if (val2 == NULL) {
		if (str_matchA(key, "+", 1) || str_matchA(key, "!", 1) || str_matchA(key, "*", 1)) {
			return self;
		}
	}
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}

// ============================================================
// Signal::exepB, PrimaryB
// ============================================================
Obj* exepB_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_CallBlock) {
			(*n)++;
			return getB(thgc, local, val2->letter->text, val2->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// SignalFunction::exeB, PrimaryB
// ============================================================
Obj* exeB_SignalFunction(ThreadGC* thgc, Obj* self, Master* local) {
	// basicexeB: clone block1, set up variables, execute block2
	return self;
}
Obj* PrimaryB_SignalFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ServerClient::exepB, PrimaryB
// ============================================================
Obj* exepB_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			// sigmapA lookup
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	} else if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// ServerFunction::exeB, PrimaryB
// ============================================================
Obj* exeB_ServerFunction(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* PrimaryB_ServerFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, self->letter);
}

// ============================================================
// Connect::PrimaryB
// ============================================================
Obj* PrimaryB_Connect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			Obj* ch2 = *(Obj**)get_list(primary->children, *n);
			// .new handling
			if (ch2->objtype == LetterType::OT_Bracket) {
				Obj* val = vt_GetterB(thgc, vt_exeB(thgc, ch2, local), local);
				if (IS_WEG(val)) return val;
				// ConnectStock creation
				return self;
			}
			return ErrorA(thgc, LetterType::OT_Error, ch2->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// ConnectStock::PrimaryB
// ============================================================
Obj* PrimaryB_ConnectStock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			Obj* ch2 = *(Obj**)get_list(primary->children, *n);
			// back, Store, send handling
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	} else if (val2->objtype == LetterType::OT_Block) {
		// Model/Gene lookup, Output/Select/First
		(*n)++;
		return self;
	} else if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Address::PrimaryB
// ============================================================
Obj* PrimaryB_Address(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			// address.name.ServerClient lookup
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Mountain::exepB
// ============================================================
Obj* exepB_Mountain(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);
	if (val2->objtype == LetterType::OT_Word) {
		// parse cell reference (e.g. A1, B2C3)
		(*n)++;
		// return Block(Array) with parsed coordinates
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Sum::PrimaryB
// ============================================================
Obj* PrimaryB_Sum(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		(*n)++;
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		// Sum calculation
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Question::exepB
// ============================================================
Obj* exepB_Question(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	// return Block(Array) with Number(0) rets
	return self;
}

// ============================================================
// initVTables_B3 - B-pass vtable初期化 (Block 3)
// ============================================================
void initVTables_B3() {
	// --- Comment ---
	vt_Comment.exeB = exeB_Comment;
	vt_Comment.exepB = exepB_Comment;

	// --- Comment2 ---
	vt_Comment2.exeB = exeB_Comment2;
	vt_Comment2.exepB = exepB_Comment2;

	// --- Dolor ---
	vt_Dolor.exepB = exepB_Dolor;

	// --- HtmObj ---
	vt_HtmObj.exepB = exepB_HtmObj;

	// --- TagBlock ---
	vt_TagBlock.exepB = exepB_TagBlock;
	vt_TagBlock.exeB = exeB_TagBlock;

	// --- ElemType ---
	vt_ElemType.exepB = exepB_ElemType;
	vt_ElemType.PrimaryB = PrimaryB_ElemType;

	// --- ElemObj ---
	vt_ElemObj.opeB = opeB_ElemObj;

	// --- Signal ---
	vt_Signal.exepB = exepB_Signal;
	vt_Signal.PrimaryB = PrimaryB_Signal;

	// --- SignalFunction ---
	vt_SignalFunction.exeB = exeB_SignalFunction;
	vt_SignalFunction.PrimaryB = PrimaryB_SignalFunction;

	// --- ServerClient ---
	vt_ServerClient.exepB = exepB_ServerClient;
	vt_ServerClient.PrimaryB = PrimaryB_ServerClient;

	// --- ServerFunction ---
	vt_ServerFunction.exeB = exeB_ServerFunction;
	vt_ServerFunction.PrimaryB = PrimaryB_ServerFunction;

	// --- Connect ---
	vt_Connect.PrimaryB = PrimaryB_Connect;

	// --- ConnectStock ---
	vt_ConnectStock.PrimaryB = PrimaryB_ConnectStock;

	// --- Address ---
	vt_Address.PrimaryB = PrimaryB_Address;

	// --- LinearFunction ---
	vt_LinearFunction.PrimaryB = PrimaryB_LinearFunction;

	// --- Mountain ---
	vt_Mountain.exepB = exepB_Mountain;

	// --- Sum ---
	vt_Sum.PrimaryB = PrimaryB_Sum;

	// --- Question ---
	vt_Question.exepB = exepB_Question;
}
