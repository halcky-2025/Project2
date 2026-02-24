// ============================================================
// obj2b.h - B解析パス (vtable dispatch版) - Block 1
// Operator, Primary, Block, CallBlock + TypeCheck
// ============================================================

// ヘルパーマクロ
#define IS_WEG(obj) ((obj)->objtype == LetterType::OT_Wait || (obj)->objtype == LetterType::OT_Error || (obj)->objtype == LetterType::OT_NG)

// 前方宣言
Obj* exeB_Operator(ThreadGC* thgc, Obj* self, Master* local);
Obj* exeB_Primary(ThreadGC* thgc, Obj* self, Master* local);
Obj* NextB_Primary(ThreadGC* thgc, Primary* primary, Master* local, int* i, Obj** val1);
Obj* exeB_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeRangeB_Block(ThreadGC* thgc, Block* self, int start, int end, Master* local, bool id, bool typecheck);
Obj* PrimaryB_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* SelfB_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* GetterB_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_CallBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeB_CallBlock(ThreadGC* thgc, Obj* self, Master* local);

// ============================================================
// declareB
// ============================================================
Obj* declareB(ThreadGC* thgc, Master* local, String* name, Obj* obj) {
	// B-pass: 宣言を記録 (A-passと同じパターン)
	Block* blk = *(Block**)get_list(local->blocks, local->blocks->size - 1);
	add_mapy(thgc, blk->vmapA, name, (char*)obj);
	return obj;
}

// ============================================================
// getB
// ============================================================
Obj* getB(ThreadGC* thgc, Master* local, String* name, ATSSpan* letter) {
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* blk = *(Block**)get_list(local->blocks, i);
		char* val = get_mapy(blk->vmapA, name);
		if (val != NULL) return (Obj*)val;
	}
	// sigmapA check
	// TODO: local->sigmapA
	return ErrorA(thgc, LetterType::OT_Error, letter);
}

// ============================================================
// Operator::exeB
// ============================================================
Obj* exeB_Operator(ThreadGC* thgc, Obj* self, Master* local) {
	String* name = self->rename; // operator name
	Obj* ch0 = *(Obj**)get_list(self->children, 0);
	Obj* ch1 = *(Obj**)get_list(self->children, 1);

	if (str_matchA(name, "=", 1) || str_matchA(name, ":", 1) || str_matchA(name, "in", 2)) {
		Obj* val2 = vt_GetterB(thgc, vt_exeB(thgc, ch1, local), local);
		// local->calls pop
		if (IS_WEG(val2)) return val2;
		// local->calls push KouhoSet2
		Obj* val1 = vt_SelfB(thgc, vt_exeB(thgc, ch0, local), local);
		if (IS_WEG(val1)) return val1;
		return vt_opeB(thgc, val1, name, local, val2);
	} else {
		// local->calls push KouhoSet2
		Obj* val1 = vt_GetterB(thgc, vt_exeB(thgc, ch0, local), local);
		// local->calls pop
		if (IS_WEG(val1)) return val1;
		// local->calls push KouhoSet2
		Obj* val2 = vt_GetterB(thgc, vt_exeB(thgc, ch1, local), local);
		if (IS_WEG(val2)) return val2;
		// local->calls pop
		return vt_opeB(thgc, val1, name, local, val2);
	}
}

// ============================================================
// Primary::NextB
// ============================================================
Obj* NextB_Primary(ThreadGC* thgc, Primary* primary, Master* local, int* i, Obj** val1) {
	Obj* ch = *(Obj**)get_list(primary->children, *i);
	LetterType t = ch->objtype;

	switch (t) {
	case LetterType::OT_Comment2:
		(*i)++;
		break;
	case LetterType::OT_Htm:
	case LetterType::OT_TagBlock:
		vt_exepB(thgc, ch, i, local, primary);
		(*i)++;
		*val1 = NULL;
		break;
	case LetterType::OT_Gene:
	case LetterType::OT_Model:
	case LetterType::OT_Class:
	case LetterType::OT_Word:
	case LetterType::OT_Number:
	case LetterType::OT_FloatVal:
	case LetterType::OT_BoolVal:
	case LetterType::OT_StrObj:
	case LetterType::OT_Bracket:
	case LetterType::OT_Print:
	case LetterType::OT_Return:
	case LetterType::OT_Goto:
	case LetterType::OT_Continue:
	case LetterType::OT_Var:
	case LetterType::OT_If:
	case LetterType::OT_While:
	case LetterType::OT_For:
	case LetterType::OT_Switch:
	case LetterType::OT_GeneLabel:
	case LetterType::OT_Comment:
	case LetterType::OT_Client:
	case LetterType::OT_Server:
	case LetterType::OT_Signal:
	case LetterType::OT_ServerClient:
	case LetterType::OT_Dolor:
	case LetterType::OT_Mountain:
		if (*val1 == NULL) {
			*val1 = vt_exepB(thgc, ch, i, local, primary);
			(*i)++;
		} else {
			*val1 = vt_GetterB(thgc, *val1, local);
			if (IS_WEG(*val1)) return *val1;
			*val1 = vt_PrimaryB(thgc, *val1, i, local, primary, vt_exepB(thgc, ch, i, local, primary));
		}
		break;
	case LetterType::OT_Dot:
	case LetterType::OT_Left:
	case LetterType::OT_Right:
		*val1 = vt_GetterB(thgc, *val1, local);
		if (IS_WEG(*val1)) return *val1;
		*val1 = vt_PrimaryB(thgc, *val1, i, local, primary, ch);
		break;
	case LetterType::OT_CallBlock:
	case LetterType::OT_Block:
		if (*val1 == NULL) {
			*val1 = vt_exepB(thgc, ch, i, local, primary);
			(*i)++;
		} else {
			*val1 = vt_GetterB(thgc, *val1, local);
			if (IS_WEG(*val1)) return *val1;
			*val1 = vt_PrimaryB(thgc, *val1, i, local, primary, ch);
		}
		break;
	default:
		break;
	}
	return *val1;
}

// ============================================================
// Primary::exeB
// ============================================================
Obj* exeB_Primary(ThreadGC* thgc, Obj* self, Master* local) {
	Primary* primary = (Primary*)self;
	// if (local->comments.Count > 0) local->comments.Last().Start();
	Obj* val1 = NULL;
	int count = primary->children->size;
	for (int i = 0; i < count - 1;) {
		NextB_Primary(thgc, primary, local, &i, &val1);
		if (val1 != NULL && IS_WEG(val1)) return val1;
	}
	if (val1 == NULL) val1 = *(Obj**)get_list(primary->children, count - 1);

	// singleops processing
	if (primary->singleops != NULL) {
		for (int i = primary->singleops->size - 1; i >= 0; i--) {
			Obj* sop = *(Obj**)get_list(primary->singleops, i);
			String* str = sop->rename;
			if (str_matchA(str, ".", 1)) {
				// op = singleops[i]
			} else if (str_matchA(str, "*", 1) || str_matchA(str, ">>", 2)) {
				// op = singleops[i]
				Obj* last = *(Obj**)get_list(primary->children, count - 1);
				if (val1 != last) val1 = vt_opeB(thgc, val1, str, local, NULL);
			} else {
				val1 = vt_GetterB(thgc, val1, local);
				val1 = vt_opeB(thgc, val1, str, local, NULL);
			}
		}
	}
	return val1;
}

// ============================================================
// Block::exeRangeB
// ============================================================
Obj* exeRangeB_Block(ThreadGC* thgc, Block* self, int start, int end, Master* local, bool id, bool typecheck) {
	self->rets = create_list(thgc, sizeof(Obj*), CType::_List);
	for (int i = 0; i < self->children->size; i++) {
		Obj* ch = *(Obj**)get_list(self->children, i);
		Obj* val = vt_exeB(thgc, ch, local);
		add_list(thgc, self->rets, (char*)&val);
		if (IS_WEG(val)) return val;
	}
	// typecheck (Block cls check) - simplified
	return (Obj*)self;
}

// ============================================================
// Block::exepB
// ============================================================
Obj* exepB_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Block* blk = (Block*)self;
	if (self->objtype == LetterType::OT_Block) {
		// T-type declaration handling (simplified)
		int m = 0;
		Obj* ch0 = *(Obj**)get_list(blk->children, m);
		if (ch0->objtype == LetterType::OT_Word) {
			if (str_matchA(ch0->letter->text, "T", 1)) {
				// GenericFunction / Function declaration
				// ... complex T-type handling (simplified for now)
			}
		}
	}
	Block* blk2 = (Block*)exeRangeB_Block(thgc, blk, 0, blk->children->size, local, true, true);
	if (self->objtype == LetterType::OT_Bracket) {
		if (blk2->rets != NULL && blk2->rets->size == 1) {
			return *(Obj**)get_list(blk2->rets, 0);
		}
	}
	return (Obj*)blk2;
}

// ============================================================
// Block::exeB
// ============================================================
Obj* exeB_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	if (self->objtype == LetterType::OT_Array)
		return ErrorA(thgc, LetterType::OT_Error, self->letter);
	return exeRangeB_Block(thgc, blk, 0, blk->children->size, local, true, true);
}

// ============================================================
// Block::PrimaryB
// ============================================================
Obj* PrimaryB_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Block* blk = (Block*)self;
	if (self->objtype == LetterType::OT_Bracket) {
		if (val2->objtype == LetterType::OT_Block) {
			// 配列型宣言: (Type)[]
			if (blk->rets != NULL && blk->rets->size == 1) {
				// ArrType作成
			}
			return ErrorA(thgc, LetterType::OT_NG, val2->letter);
		} else if (val2->objtype == LetterType::OT_Bracket) {
			// 複数変数宣言
			Block* block = (Block*)vt_exeB(thgc, val2, local);
			(*n)++;
			return (Obj*)block;
		} else if (val2->objtype == LetterType::OT_Word) {
			// 変数宣言: (Type) name
			(*n)++;
			// Variable作成 + declareB
			return (Obj*)blk;
		} else if (val2->objtype == LetterType::OT_Left) {
			(*n)++;
			Obj* ch = *(Obj**)get_list(primary->children, *n);
			if (blk->rets != NULL && blk->rets->size == 1) {
				return vt_PrimaryB(thgc, *(Obj**)get_list(blk->rets, 0), n, local, primary, ch);
			}
			return ErrorA(thgc, LetterType::OT_Error, self->letter);
		} else if (val2->objtype == LetterType::OT_Dot) {
			if (blk->rets != NULL && blk->rets->size == 1) {
				return vt_PrimaryB(thgc, *(Obj**)get_list(blk->rets, 0), n, local, primary, val2);
			}
			return ErrorA(thgc, LetterType::OT_Error, self->letter);
		}
		return ErrorA(thgc, LetterType::OT_NG, val2->letter);
	} else if (self->objtype == LetterType::OT_Block || self->objtype == LetterType::OT_Array) {
		if (val2->objtype == LetterType::OT_Block) {
			// 配列アクセス: arr[index]
			Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
			if (IS_WEG(val)) return val;
			Block* block = (Block*)val;
			if (block->rets == NULL || block->rets->size != 1)
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			Obj* idx = *(Obj**)get_list(block->rets, 0);
			Obj* me = self;
			if (idx->objtype == LetterType::OT_Number) {
				// Number* num = (Number*)idx; me = blk->rets[num->value]
			} else if (idx->objtype == LetterType::OT_StrObj) {
				// me = blk->rets[blk->labelmap[str]]
			} else {
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			}
			(*n)++;
			return me;
		} else if (val2->objtype == LetterType::OT_Dot) {
			(*n)++;
			Obj* ch = *(Obj**)get_list(primary->children, *n);
			if (ch->objtype == LetterType::OT_Word) {
				(*n)++;
				String* wname = ch->letter->text;
				if (str_matchA(wname, "length", 6)) {
					// return new Number(rets.Count) { cls = local.Int }
					Obj* num = make_cobj(thgc, CType::_CObj, LetterType::OT_Number, ch->letter);
					return num;
				} else if (str_matchA(wname, "Sort", 4)) {
					// Sort handling
					(*n)++;
					return (Obj*)blk;
				} else if (str_matchA(wname, "Range", 5)) {
					// Range handling
					(*n)++;
					return (Obj*)blk;
				}
				return ErrorA(thgc, LetterType::OT_NG, ch->letter);
			}
			return ErrorA(thgc, LetterType::OT_Error, ch->letter);
		}
		return ErrorA(thgc, LetterType::OT_NG, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Block::SelfB
// ============================================================
Obj* SelfB_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	if (blk->rets == NULL) return self;
	for (int i = 0; i < blk->rets->size; i++) {
		Obj** pp = (Obj**)get_list(blk->rets, i);
		*pp = vt_SelfB(thgc, *pp, local);
		if (IS_WEG(*pp)) return *pp;
	}
	return self;
}

// ============================================================
// Block::GetterB
// ============================================================
Obj* GetterB_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	if (blk->rets == NULL) return self;
	for (int i = 0; i < blk->rets->size; i++) {
		Obj** pp = (Obj**)get_list(blk->rets, i);
		*pp = vt_GetterB(thgc, vt_GetterB(thgc, *pp, local), local);
		if (IS_WEG(*pp)) return *pp;
	}
	return self;
}

// ============================================================
// CallBlock::exepB
// ============================================================
Obj* exepB_CallBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// CallBlock::exeB
// ============================================================
Obj* exeB_CallBlock(ThreadGC* thgc, Obj* self, Master* local) {
	// B-pass CallBlock: analyse loop
	// analblocks の各 block を analyse する
	// 簡略化: 各analblockのstatusを初期化し、analyseを呼ぶ
	return self;
}

// ============================================================
// initVTables_B - B-pass vtable初期化 (Block 1)
// ============================================================
void initVTables_B() {
	// --- Operator ---
	vt_Operator.exeB = exeB_Operator;

	// --- Primary ---
	vt_Primary.exeB = exeB_Primary;

	// --- Block / Bracket / Call1 / Call2 ---
	vt_Block.exeB = exeB_Block;
	vt_Block.exepB = exepB_Block;
	vt_Block.PrimaryB = PrimaryB_Block;
	vt_Block.SelfB = SelfB_Block;
	vt_Block.GetterB = GetterB_Block;
	// Block_Array uses same functions
	vt_Block_Array.exeB = exeB_Block;
	vt_Block_Array.exepB = exepB_Block;
	vt_Block_Array.PrimaryB = PrimaryB_Block;
	vt_Block_Array.SelfB = SelfB_Block;
	vt_Block_Array.GetterB = GetterB_Block;

	// --- CallBlock ---
	vt_CallBlock.exepB = exepB_CallBlock;
	vt_CallBlock.exeB = exeB_CallBlock;
}
