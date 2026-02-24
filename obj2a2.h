// ============================================================
// obj2a2.h - A解析パス (vtable dispatch版) 第2ブロック
// Comment, TagBlock, Signal, ServerClient等
// ============================================================

// 前方宣言
Obj* exeA_Comment(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepA_Comment(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeA_Comment2(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepA_Comment2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepA_Dolor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepA_HtmObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeA_TagBlock(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepA_TagBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepA_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* opeA_ElemObj(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2);
Obj* exepA_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeA_SignalFunction(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryA_SignalFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeA_ServerFunction(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryA_ServerFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_Connect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_ConnectStock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_Address(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Mountain(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Sum(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Question(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_LinearFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);

// ============================================================
// Comment::exeA
// ============================================================
Obj* exeA_Comment(ThreadGC* thgc, Obj* self, Master* local) {
	Block* block = (Block*)*(Obj**)get_list(self->children, 0);
	ObjBlock* objblk = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
	objblk->objtype = LetterType::OT_ObjBlock;
	objblk->vt = NULL;
	objblk->obj = self;
	objblk->n = -1;
	block->obj = (Obj*)objblk;
	// block.vmapA にelement型を追加 (div, br, sheet, cell, span)
	// TODO: ElemType構造体が必要
	// block.vmapA["linear"] = new LinearFunction()
	// TODO: LinearFunction構造体が必要
	add_list(thgc, local->blocks, (char*)block);
	// local.comments.Add(comelet)
	// TODO: CommentLet処理
	vt_exeA(thgc, (Obj*)block, local);
	if (local->blocks->size > 0) local->blocks->size--;
	if (local->comments->size > 0) local->comments->size--;
	return self;
}

// ============================================================
// Comment::exepA
// ============================================================
Obj* exepA_Comment(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return exeA_Comment(thgc, self, local);
}

// ============================================================
// Comment2::exeA
// ============================================================
Obj* exeA_Comment2(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// Comment2::exepA
// ============================================================
Obj* exepA_Comment2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Dolor::exepA ($type, $func, $(expr))
// ============================================================
Obj* exepA_Dolor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		// $type / $func → Clones
		Obj* clones = make_cobj(thgc, CType::_CObj, LetterType::OT_Dolor, self->letter);
		return clones;
	} else if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		Block* blk = (Block*)val;
		if (blk->rets->size != 1) return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		Obj* ret0 = *(Obj**)get_list(blk->rets, 0);
		if (ret0->objtype == LetterType::OT_Number || ret0->objtype == LetterType::OT_StrObj ||
			ret0->objtype == LetterType::OT_FloatVal || ret0->objtype == LetterType::OT_BoolVal) {
			if (local->comments->size > 0) {
				// return HtmObj
				Obj* htm = make_cobj(thgc, CType::_CObj, LetterType::OT_Htm, ret0->letter);
				return htm;
			} else {
				return ErrorA(thgc, LetterType::OT_Error, ret0->letter);
			}
		}
		return ErrorA(thgc, LetterType::OT_Error, ret0->letter);
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// HtmObj::exepA
// ============================================================
Obj* exepA_HtmObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (local->comments->size > 0) {
		return NULL;
	}
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}

// ============================================================
// TagBlock::exepA
// ============================================================
Obj* exepA_TagBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (local->comments->size > 0) {
		Block* blk1 = (Block*)vt_GetterA(thgc, vt_exeA(thgc, *(Obj**)get_list(self->children, 0), local), local);
		ObjBlock* objblk1 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
		objblk1->objtype = LetterType::OT_ObjBlock;
		objblk1->vt = NULL;
		objblk1->obj = self;
		objblk1->n = 0;
		blk1->obj = (Obj*)objblk1;
		add_list(thgc, local->blocks, (char*)blk1);
		vt_exeA(thgc, (Obj*)blk1, local);

		Block* blk2 = (Block*)vt_GetterA(thgc, vt_exeA(thgc, *(Obj**)get_list(self->children, 1), local), local);
		ObjBlock* objblk2 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
		objblk2->objtype = LetterType::OT_ObjBlock;
		objblk2->vt = NULL;
		objblk2->obj = self;
		objblk2->n = 1;
		blk2->obj = (Obj*)objblk2;
		add_list(thgc, local->blocks, (char*)blk2);
		vt_exeA(thgc, (Obj*)blk2, local);

		if (local->blocks->size > 0) local->blocks->size--;
		if (local->blocks->size > 0) local->blocks->size--;
		return NULL;
	}
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}

// ============================================================
// TagBlock::exeA
// ============================================================
Obj* exeA_TagBlock(ThreadGC* thgc, Obj* self, Master* local) {
	return exepA_TagBlock(thgc, self, NULL, local, NULL);
}

// ============================================================
// ElemType::exepA
// ============================================================
Obj* exepA_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Obj* val2 = *(Obj**)get_list(primary->children, *n + 1);
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* elemobj = make_cobj(thgc, CType::_CObj, LetterType::OT_Htm, val2->letter);
		declareA(thgc, local, val2->letter->text, elemobj);
		return elemobj;
	}
	return self;
}

// ============================================================
// ElemType::PrimaryA
// ============================================================
Obj* PrimaryA_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* elemobj = make_cobj(thgc, CType::_CObj, LetterType::OT_Htm, val2->letter);
		declareA(thgc, local, val2->letter->text, elemobj);
		return elemobj;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ElemObj::opeA
// ============================================================
Obj* opeA_ElemObj(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) {
	if (val2 == NULL) {
		// unary operators +, !, *
		if (key != NULL && key->size == 1) {
			char c = key->data[0];
			if (c == '+' || c == '!' || c == '*') {
				return self;
			}
		}
	}
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}

// ============================================================
// Signal::exepA
// ============================================================
Obj* exepA_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Signal::PrimaryA
// ============================================================
Obj* PrimaryA_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		Obj* word = val2;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_CallBlock) {
			if (local->comments->size > 0) return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			(*n)++;
			// SignalFunction作成
			Obj* func = make_cobj(thgc, CType::_CObj, LetterType::OT_Signal, word->letter);
			declareA(thgc, local, word->letter->text, func);
			Block* blk1 = (Block*)*(Obj**)get_list(val2->children, 0);
			ObjBlock* objblk1 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
			objblk1->objtype = LetterType::OT_ObjBlock;
			objblk1->vt = NULL;
			objblk1->obj = func;
			objblk1->n = 0;
			blk1->obj = (Obj*)objblk1;
			add_list(thgc, local->blocks, (char*)blk1);
			vt_exeA(thgc, (Obj*)blk1, local);

			Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
			ObjBlock* objblk2 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
			objblk2->objtype = LetterType::OT_ObjBlock;
			objblk2->vt = NULL;
			objblk2->obj = func;
			objblk2->n = 1;
			blk2->obj = (Obj*)objblk2;
			add_list(thgc, local->blocks, (char*)blk2);
			vt_exeA(thgc, (Obj*)blk2, local);

			if (local->blocks->size > 0) local->blocks->size--;
			if (local->blocks->size > 0) local->blocks->size--;
			return func;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// SignalFunction::exeA
// ============================================================
Obj* exeA_SignalFunction(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// SignalFunction::PrimaryA
// ============================================================
Obj* PrimaryA_SignalFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ServerClient::exepA
// ============================================================
Obj* exepA_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// ServerClient::PrimaryA
// ============================================================
Obj* PrimaryA_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::_None, val2->letter);
			return ret;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	} else if (val2->objtype == LetterType::OT_CallBlock) {
		if (local->comments->size > 0) return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		(*n)++;
		// ServerFunction作成
		Obj* sf = make_cobj(thgc, CType::_CObj, LetterType::OT_Server, self->letter);
		declareA(thgc, local, self->letter->text, sf);

		Block* blk1 = (Block*)*(Obj**)get_list(val2->children, 0);
		ObjBlock* objblk1 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
		objblk1->objtype = LetterType::OT_ObjBlock;
		objblk1->vt = NULL;
		objblk1->obj = sf;
		objblk1->n = 0;
		blk1->obj = (Obj*)objblk1;
		add_list(thgc, local->blocks, (char*)blk1);
		vt_exeA(thgc, (Obj*)blk1, local);

		Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
		ObjBlock* objblk2 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
		objblk2->objtype = LetterType::OT_ObjBlock;
		objblk2->vt = NULL;
		objblk2->obj = sf;
		objblk2->n = 1;
		blk2->obj = (Obj*)objblk2;
		add_list(thgc, local->blocks, (char*)blk2);
		vt_exeA(thgc, (Obj*)blk2, local);

		if (local->blocks->size > 0) local->blocks->size--;
		if (local->blocks->size > 0) local->blocks->size--;
		return sf;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// ServerFunction::exeA
// ============================================================
Obj* exeA_ServerFunction(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// ServerFunction::PrimaryA
// ============================================================
Obj* PrimaryA_ServerFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, self->letter);
}

// ============================================================
// Connect::PrimaryA
// ============================================================
Obj* PrimaryA_Connect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			// connect.new(...)
			if (val2->objtype == LetterType::OT_Bracket) {
				Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
				if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
				Block* blk1 = (Block*)val;
				if (blk1->rets->size == 1) {
					Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::_None, (*(Obj**)get_list(blk1->rets, 0))->letter);
					return ret;
				}
				return ErrorA(thgc, LetterType::OT_Error, ((Block*)val)->letter);
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// ConnectStock::PrimaryA
// ============================================================
Obj* PrimaryA_ConnectStock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			// .back / .Store(...)
			if (val2->objtype == LetterType::OT_Bracket) {
				Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
				if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
				(*n)++;
				return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
			}
			return make_cobj(thgc, CType::_CObj, LetterType::_None, val2->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	} else if (val2->objtype == LetterType::OT_Block) {
		Obj* blk01 = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (blk01->objtype == LetterType::OT_Wait || blk01->objtype == LetterType::OT_Error || blk01->objtype == LetterType::OT_NG) return blk01;
		Block* blk = (Block*)blk01;
		if (blk->rets->size != 1) return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Dot) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				if (val2->objtype == LetterType::OT_Bracket) {
					Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
					if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
					(*n)++;
					return make_cobj(thgc, CType::_CObj, LetterType::_None, val2->letter);
				}
				return ErrorA(thgc, LetterType::OT_NG, val2->letter);
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return ErrorA(thgc, LetterType::OT_NG, val2->letter);
	}
	return make_cobj(thgc, CType::_CObj, LetterType::_None, val2->letter);
}

// ============================================================
// Address::PrimaryA
// ============================================================
Obj* PrimaryA_Address(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			// address += "/" + word.name
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	} else if (val2->objtype == LetterType::OT_Bracket) {
		Obj* block = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (block->objtype == LetterType::OT_Wait || block->objtype == LetterType::OT_Error || block->objtype == LetterType::OT_NG) return block;
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Mountain::exepA (^ range expression)
// ============================================================
Obj* exepA_Mountain(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);
	if (val2->objtype == LetterType::OT_Word) {
		Block* arr = make_cblock(thgc, CType::_Block, LetterType::OT_Array);
		return (Obj*)arr;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Sum::PrimaryA
// ============================================================
Obj* PrimaryA_Sum(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		(*n)++;
		return make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Question::exepA (?)
// ============================================================
Obj* exepA_Question(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Block* blk = make_cblock(thgc, CType::_Block, LetterType::OT_Array);
	Block* blk1 = make_cblock(thgc, CType::_Block, LetterType::OT_Array);
	add_list(thgc, blk->rets, (char*)blk1);
	add_list(thgc, blk->rets, (char*)blk1);
	for (int i = 0; i < 2; i++) {
		Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
		add_list(thgc, blk1->rets, (char*)num);
	}
	return (Obj*)blk1;
}

// ============================================================
// LinearFunction::PrimaryA
// ============================================================
Obj* PrimaryA_LinearFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		(*n)++;
		Obj* blk = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (blk->objtype == LetterType::OT_Wait || blk->objtype == LetterType::OT_Error || blk->objtype == LetterType::OT_NG) return blk;
		// return AnimationFunction
		return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// initVTables_A2 - obj2a2.h用VTable初期化
// ============================================================
void initVTables_A2() {
	// --- Comment ---
	vt_Comment = vt_Obj_default;
	vt_Comment.exeA = exeA_Comment;
	vt_Comment.exepA = exepA_Comment;
	vt_lookup[(int)LetterType::OT_Comment] = &vt_Comment;

	// --- Comment2 ---
	vt_Comment2 = vt_Obj_default;
	vt_Comment2.exeA = exeA_Comment2;
	vt_Comment2.exepA = exepA_Comment2;
	vt_lookup[(int)LetterType::OT_Comment2] = &vt_Comment2;

	// --- Dolor ---
	vt_Dolor = vt_Obj_default;
	vt_Dolor.exepA = exepA_Dolor;
	vt_lookup[(int)LetterType::OT_Dolor] = &vt_Dolor;

	// --- HtmObj ---
	vt_HtmObj = vt_Obj_default;
	vt_HtmObj.exepA = exepA_HtmObj;
	vt_lookup[(int)LetterType::OT_Htm] = &vt_HtmObj;

	// --- TagBlock ---
	vt_TagBlock = vt_Obj_default;
	vt_TagBlock.exeA = exeA_TagBlock;
	vt_TagBlock.exepA = exepA_TagBlock;
	vt_lookup[(int)LetterType::OT_TagBlock] = &vt_TagBlock;

	// --- Signal ---
	vt_Signal = vt_Obj_default;
	vt_Signal.exepA = exepA_Signal;
	vt_Signal.PrimaryA = PrimaryA_Signal;
	vt_lookup[(int)LetterType::OT_Signal] = &vt_Signal;

	// --- ServerClient ---
	vt_ServerClient = vt_Obj_default;
	vt_ServerClient.exepA = exepA_ServerClient;
	vt_ServerClient.PrimaryA = PrimaryA_ServerClient;
	vt_lookup[(int)LetterType::OT_ServerClient] = &vt_ServerClient;

	// --- Mountain ---
	vt_Mountain = vt_Obj_default;
	vt_Mountain.exepA = exepA_Mountain;
	vt_lookup[(int)LetterType::OT_Mountain] = &vt_Mountain;
}
