// ============================================================
// obj1c.h - C解析パス (LLVM IR code generation) - Obj1C.cs converted
// ============================================================

// ============================================================
// 前方宣言
// ============================================================
Obj* exeC_Operator(ThreadGC* thgc, Obj* self, Master* local);
Obj* NextC_Primary(ThreadGC* thgc, Primary* primary, Master* local, int* i, Obj** val1);
Obj* exeC_Primary(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeC_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* exeRangeC_Block(ThreadGC* thgc, Block* self, int start, int end, Master* local, bool id);
Obj* PrimaryC_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* SelfC_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* GetterC_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* EqualC_Block(ThreadGC* thgc, Block* self, String* op, Master* local, Obj* val2);
Obj* exepC_CallBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeC_CallBlock(ThreadGC* thgc, Obj* self, Master* local);
Obj* declareC(ThreadGC* thgc, Master* local, String* name);
Obj* getC(ThreadGC* thgc, Master* local, String* name);

// ============================================================
// ヘルパー: "%v" + llvm_n++ のLVari生成
// ============================================================
static inline LVari* make_vn(ThreadGC* thgc, const char* type) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%%v%d", llvm_n++);
	return make_lvari_c(thgc, type, buf);
}

// ============================================================
// ヘルパー: snprintf+createString
// ============================================================
static inline String* fmt_str(ThreadGC* thgc, const char* fmt, ...) {
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	int len = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	return createString(thgc, buf, len, 1);
}

// ============================================================
// ヘルパー: LGeteにインデックスを追加
// ============================================================
static inline LGete* make_lgete_idx2(ThreadGC* thgc, const char* type, LVari* y, LVari* x, LVari* idx0, LVari* idx1) {
	String* ts = createString(thgc, (char*)type, (int)strlen(type), 1);
	LGete* g = make_lgete(thgc, ts, y, x);
	add_list(thgc, ((LComp*)g)->comps, (char*)idx0);
	add_list(thgc, ((LComp*)g)->comps, (char*)idx1);
	return g;
}

// ============================================================
// ヘルパー: LCallに引数を追加
// ============================================================
static inline void lcall_add(ThreadGC* thgc, LCall* c, LVari* arg) {
	add_list(thgc, ((LComp*)c)->comps, (char*)arg);
}

// ============================================================
// ヘルパー: エラーObj生成
// ============================================================
static inline Obj* ErrorC(ThreadGC* thgc, LetterType type, ATSSpan* letter, const char* msg) {
	Obj* err = make_cobj(thgc, CType::_CObj, type, letter);
	if (letter) letter->error = createString(thgc, (char*)msg, (int)strlen(msg), 1);
	return err;
}

// ============================================================
// Operator::exeC
// ============================================================
Obj* exeC_Operator(ThreadGC* thgc, Obj* self, Master* local) {
	ATSSpan* letter = self->letter;
	if (letter != NULL && letter->text != NULL) {
		int len = letter->text->size;
		char* data = letter->text->data;
		bool is_assign = (len == 1 && data[0] == '=');
		bool is_colon = (len == 1 && data[0] == ':');
		bool is_in = (len == 2 && data[0] == 'i' && data[1] == 'n');
		if (is_assign || is_colon || is_in) {
			Obj* val2 = vt_GetterC(thgc, vt_exeC(thgc, *(Obj**)get_list(self->children, 1), local), local);
			Obj* val1 = vt_SelfC(thgc, vt_exeC(thgc, *(Obj**)get_list(self->children, 0), local), local);
			return vt_opeC(thgc, val1, letter->text, local, val2);
		}
	}
	{
		Obj* val1 = vt_GetterC(thgc, vt_exeC(thgc, *(Obj**)get_list(self->children, 0), local), local);
		if (val1->objtype == LetterType::OT_Wait || val1->objtype == LetterType::OT_Error || val1->objtype == LetterType::OT_NG) return val1;
		Obj* val2 = vt_GetterC(thgc, vt_exeC(thgc, *(Obj**)get_list(self->children, 1), local), local);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
		return vt_opeC(thgc, val1, letter->text, local, val2);
	}
}

// ============================================================
// Primary::NextC
// ============================================================
Obj* NextC_Primary(ThreadGC* thgc, Primary* primary, Master* local, int* i, Obj** val1) {
	Obj* child = *(Obj**)get_list(primary->children, *i);
	switch (child->objtype) {
		case LetterType::OT_Comment2:
			(*i)++;
			break;
		case LetterType::OT_Htm:
		case LetterType::OT_TagBlock:
			vt_exepC(thgc, child, i, local, primary);
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
				*val1 = vt_exepC(thgc, child, i, local, primary);
				(*i)++;
			} else {
				*val1 = vt_GetterC(thgc, *val1, local);
				if ((*val1)->objtype == LetterType::OT_Wait || (*val1)->objtype == LetterType::OT_Error || (*val1)->objtype == LetterType::OT_NG) return *val1;
				*val1 = vt_PrimaryC(thgc, *val1, i, local, primary, child);
			}
			break;
		case LetterType::OT_Dot:
		case LetterType::OT_Left:
		case LetterType::OT_Right:
			*val1 = vt_GetterC(thgc, *val1, local);
			if ((*val1)->objtype == LetterType::OT_Wait || (*val1)->objtype == LetterType::OT_Error || (*val1)->objtype == LetterType::OT_NG) return *val1;
			*val1 = vt_PrimaryC(thgc, *val1, i, local, primary, child);
			break;
		case LetterType::OT_CallBlock:
		case LetterType::OT_Block:
			if (*val1 == NULL) {
				*val1 = vt_exepC(thgc, child, i, local, primary);
				(*i)++;
			} else {
				*val1 = vt_GetterC(thgc, *val1, local);
				if ((*val1)->objtype == LetterType::OT_Wait || (*val1)->objtype == LetterType::OT_Error || (*val1)->objtype == LetterType::OT_NG) return *val1;
				*val1 = vt_PrimaryC(thgc, *val1, i, local, primary, child);
			}
			break;
		default:
			break;
	}
	return *val1;
}

// ============================================================
// Primary::exeC
// ============================================================
Obj* exeC_Primary(ThreadGC* thgc, Obj* self, Master* local) {
	Primary* pself = (Primary*)self;
	Obj* val1 = NULL;
	for (int i = 0; i < pself->children->size - 1;) {
		NextC_Primary(thgc, pself, local, &i, &val1);
		if (val1 != NULL && (val1->objtype == LetterType::OT_Wait || val1->objtype == LetterType::OT_Error || val1->objtype == LetterType::OT_NG)) return val1;
	}
	if (val1 == NULL) val1 = *(Obj**)get_list(pself->children, pself->children->size - 1);

	// singleops処理
	for (int i = pself->singleops->size - 1; i >= 0; i--) {
		SingleOp* sop = *(SingleOp**)get_list(pself->singleops, i);
		if (sop->letter != NULL && sop->letter->text != NULL) {
			val1 = vt_GetterC(thgc, val1, local);
			val1 = vt_opeC(thgc, val1, sop->letter->text, local, NULL);
			break;
		}
	}
	return val1;
}

// ============================================================
// Block::exeC
// ============================================================
Obj* exeC_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	if (blk->objtype == LetterType::OT_Array) return (Obj*)blk;
	return exeRangeC_Block(thgc, blk, 0, blk->children->size, local, true);
}

// ============================================================
// Block::exeRangeC
// ============================================================
Obj* exeRangeC_Block(ThreadGC* thgc, Block* self, int start, int end, Master* local, bool id) {
	self->rets = create_list(thgc, sizeof(Obj*), CType::_List);
	LFunc* func = llvm_get_func(local->llvm);
	for (int i = start; i < end; i++) {
		Obj* child = *(Obj**)get_list(self->children, i);
		add_list(thgc, func->comps, (char*)make_llocmarker(thgc, i, i, func->debugId));
		Obj* val = vt_exeC(thgc, child, local);
		add_list(thgc, self->rets, (char*)val);
	}
	if (start == end) {
		add_list(thgc, func->comps, (char*)make_llocmarker(thgc, 1, 0, func->debugId));
	}
	return (Obj*)self;
}

// ============================================================
// Block::exepC
// ============================================================
Obj* exepC_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Block* blk = (Block*)self;

	if (blk->objtype == LetterType::OT_Block) {
		// Hash/Array creation path
		LFunc* func = llvm_get_func(local->llvm);
		LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
		LVari* vcl = make_vn(thgc, "%CreateHashType");
		LLoad* ld1 = make_lload(thgc, vcl, make_lvari_c(thgc, "%CreateHashType*", "@CreateHash"), false);
		add_list(thgc, func->comps, (char*)ld1);
		LVari* voj = make_vn(thgc, "%HashType*");
		LVari* objected_v = make_lvari_c(thgc, "i1", "true");
		LCall* call1 = make_lcall(thgc, voj, vcl);
		lcall_add(thgc, call1, thgc4);
		lcall_add(thgc, call1, objected_v);
		add_list(thgc, func->comps, (char*)call1);
		Obj* blk2 = exeRangeC_Block(thgc, blk, 0, blk->children->size, local, false);
		LVari* varih = make_vn(thgc, "%AddHashType");
		add_list(thgc, func->comps, (char*)make_lload(thgc, varih, make_lvari_c(thgc, "%AddHashType*", "@AddHash"), false));
		for (int i = 0; i < blk->rets->size; i++) {
			Obj* ret_i = *(Obj**)get_list(blk->rets, i);
			LCall* addcall = make_lcall(thgc, NULL, varih);
			lcall_add(thgc, addcall, thgc4);
			lcall_add(thgc, addcall, voj);
			lcall_add(thgc, addcall, make_lvari_c(thgc, "%StringType*", "null"));
			if (ret_i->vari_c) lcall_add(thgc, addcall, ret_i->vari_c);
			add_list(thgc, func->comps, (char*)addcall);
		}
		return (Obj*)blk;
	}
	return exeRangeC_Block(thgc, blk, 0, blk->children->size, local, true);
}

// ============================================================
// Block::PrimaryC
// ============================================================
Obj* PrimaryC_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Block* blk = (Block*)self;
	if (blk->objtype == LetterType::OT_Bracket) {
		if (val2->objtype == LetterType::OT_Block) {
			if (blk->rets->size == 1) {
				Block* v2blk = (Block*)val2;
				if (v2blk->children->size == 0) {
					Obj* cls = *(Obj**)get_list(blk->rets, 0);
					if (cls != NULL) {
						ArrType* typearray = (ArrType*)GC_alloc(thgc, CType::_ArrType);
						typearray->objtype = LetterType::OT_ArrType;
						typearray->base = cls;
						return (Obj*)typearray;
					}
					return ErrorC(thgc, LetterType::OT_Error, val2->letter, "array type needs base type");
				}
				return ErrorC(thgc, LetterType::OT_Error, val2->letter, "array type [] must have 0 args");
			}
			return ErrorC(thgc, LetterType::OT_Error, self->letter, "array type needs 1 type");
		}
		else if (val2->objtype == LetterType::OT_Bracket) {
			Obj* block = vt_exeC(thgc, val2, local);
			Block* blk2 = (Block*)block;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			for (int i = 0; i < blk2->rets->size; i++) {
				Obj* ri = *(Obj**)get_list(blk2->rets, i);
				if (ri->objtype == LetterType::OT_Word) {
					Obj* variable = declareC(thgc, local, ri->letter->text);
					*(Obj**)get_list(blk2->rets, i) = variable;
				}
				else return ErrorC(thgc, LetterType::OT_Error, ri->letter, "variable decl needs name");
			}
			return block;
		}
		else if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			Obj* variable = declareC(thgc, local, val2->letter->text);
			return variable;
		}
		else if (val2->objtype == LetterType::OT_Left) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (blk->rets->size == 1) {
				Obj* r0 = *(Obj**)get_list(blk->rets, 0);
				return vt_PrimaryC(thgc, r0, n, local, primary, val2);
			}
			return ErrorC(thgc, LetterType::OT_Error, self->letter, "() can only hold 1 item");
		}
		else if (val2->objtype == LetterType::OT_Dot) {
			if (blk->rets->size == 1) {
				Obj* r0 = *(Obj**)get_list(blk->rets, 0);
				return vt_PrimaryC(thgc, r0, n, local, primary, val2);
			}
			return ErrorC(thgc, LetterType::OT_Error, self->letter, "() can only hold 1 item");
		}
		return ErrorC(thgc, LetterType::OT_NG, val2->letter, "need something after ()");
	}
	else if (blk->objtype == LetterType::OT_Block || blk->objtype == LetterType::OT_Array) {
		if (val2->objtype == LetterType::OT_Dot) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				(*n)++;
				if (val2->letter && val2->letter->text) {
					if (str_matchA(val2->letter->text, "length", 6)) {
						Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
						return num;
					}
					else if (str_matchA(val2->letter->text, "Sort", 4)) {
						Obj* v2next = *(Obj**)get_list(primary->children, *n);
						if (v2next->objtype == LetterType::OT_Bracket) {
							(*n)++;
							Obj* val = vt_GetterC(thgc, vt_exeC(thgc, v2next, local), local);
							if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
							return (Obj*)blk;
						}
						return ErrorC(thgc, LetterType::OT_Error, v2next->letter, "Sort needs ()");
					}
					else if (str_matchA(val2->letter->text, "Range", 5)) {
						Obj* v2next = *(Obj**)get_list(primary->children, *n);
						if (v2next->objtype == LetterType::OT_Bracket) {
							vt_GetterC(thgc, vt_exeC(thgc, v2next, local), local);
							(*n)++;
							return (Obj*)blk;
						}
						return ErrorC(thgc, LetterType::OT_Error, v2next->letter, "Range needs ()");
					}
				}
				return ErrorC(thgc, LetterType::OT_NG, val2->letter, "unknown array method");
			}
			return ErrorC(thgc, LetterType::OT_Error, val2->letter, "need name after .");
		}
		return ErrorC(thgc, LetterType::OT_NG, val2->letter, "need something after []");
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "invalid error");
}

// ============================================================
// Block::SelfC
// ============================================================
Obj* SelfC_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	for (int i = 0; i < blk->rets->size; i++) {
		Obj* ri = *(Obj**)get_list(blk->rets, i);
		ri = vt_SelfC(thgc, ri, local);
		*(Obj**)get_list(blk->rets, i) = ri;
		if (ri->objtype == LetterType::OT_Wait || ri->objtype == LetterType::OT_Error || ri->objtype == LetterType::OT_NG) return ri;
	}
	return self;
}

// ============================================================
// Block::GetterC
// ============================================================
Obj* GetterC_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	for (int i = 0; i < blk->rets->size; i++) {
		Obj* ri = *(Obj**)get_list(blk->rets, i);
		ri = vt_GetterC(thgc, ri, local);
		*(Obj**)get_list(blk->rets, i) = ri;
		if (ri->objtype == LetterType::OT_Wait || ri->objtype == LetterType::OT_Error || ri->objtype == LetterType::OT_NG) return ri;
	}
	return self;
}

// ============================================================
// CallBlock::exepC
// ============================================================
Obj* exepC_CallBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// CallBlock::exeC
// ============================================================
Obj* exeC_CallBlock(ThreadGC* thgc, Obj* self, Master* local) {
	Block* cblk = (Block*)self;
	// local.llvm = new LLVM(local)
	local->llvm = make_lllvm(thgc, local);

	// func = new Func(local.llvm, "void @main", "i32 %cn")
	LVari* mainY = make_lvari_c(thgc, "void", "@main");
	LFunc* mainFunc = make_lfunc(thgc, local->llvm, mainY);
	LVari* cnParam = make_lvari_c(thgc, "i32", "%cn");
	add_list(thgc, mainFunc->draws, (char*)cnParam);
	add_list(thgc, local->llvm->comps, (char*)mainFunc);
	local->llvm->main_func = mainFunc;

	// main.comps.Add(Store(@cnp, %cn))
	add_list(thgc, mainFunc->comps, (char*)make_lstore(thgc, make_lvari_c(thgc, "i32*", "@cnp"), make_lvari_c(thgc, "i32", "%cn")));

	// typedec = new TypeDec("%mainType")
	LTypeDec* typedec = make_ltypedec(thgc, createString(thgc, (char*)"%mainType", 9, 1), NULL, false);
	add_list(thgc, local->llvm->types, (char*)typedec);

	// funccheck = new Func(local.llvm, "void @mainCheck", thgc2, i8p)
	LVari* thgc3 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* i8p = make_lvari_c(thgc, "i8*", "%self");
	LVari* thgc2 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LFunc* funccheck = make_lfunc(thgc, local->llvm, make_lvari_c(thgc, "void", "@mainCheck"));
	add_list(thgc, funccheck->draws, (char*)thgc2);
	add_list(thgc, funccheck->draws, (char*)i8p);
	add_list(thgc, local->llvm->comps, (char*)funccheck);

	// typedec.comps.Add(new TypeVal("i8*", "blk"))
	add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i8*", 3, 1), createString(thgc, (char*)"blk", 3, 1)));

	// GC check function body
	LVari* vari = make_vn(thgc, "i8*");
	LGete* gete = make_lgete_idx2(thgc, "%mainType", vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0"));
	add_list(thgc, funccheck->comps, (char*)gete);
	LVari* co_val = make_lvari_c(thgc, "%CopyObjectType", "%co");
	add_list(thgc, funccheck->comps, (char*)make_lload(thgc, co_val, make_lvari_c(thgc, "%CopyObjectType*", "@CopyObject"), false));
	LVari* vari2 = make_vn(thgc, "i8*");
	add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
	LVari* co_cval = make_vn(thgc, "i8*");
	LCall* co_call = make_lcall(thgc, co_cval, co_val);
	lcall_add(thgc, co_call, thgc2);
	lcall_add(thgc, co_call, vari);
	add_list(thgc, funccheck->comps, (char*)co_call);
	add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));

	// main body: load thgc, addclass
	add_list(thgc, mainFunc->comps, (char*)make_lload(thgc, thgc3, make_lvari_c(thgc, "%ThreadGCType**", "@thgcp"), false));
	LVari* ac_val = make_lvari_c(thgc, "%GC_AddClassType", "%addclass");
	add_list(thgc, mainFunc->comps, (char*)make_lload(thgc, ac_val, make_lvari_c(thgc, "%GC_AddClassType*", "@GC_AddClass"), false));
	LVari* countv = make_lvari_c(thgc, "i32", "");
	int cn_val = local->llvm->cn++;
	cblk->obj = (Obj*)GC_alloc(thgc, CType::_ObjBlock); // ensure obj exists
	// ((ObjBlock*)cblk->obj)->n_c = cn_val; // store class number

	char cnbuf[32]; snprintf(cnbuf, sizeof(cnbuf), "%d", cn_val);
	LVari* typ = make_lvari_c(thgc, "i32", cnbuf);
	LVari* tnp = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tnp%d", llvm_n++)->data);
	add_list(thgc, mainFunc->comps, (char*)make_lload(thgc, tnp, make_lvari_c(thgc, "i32*", "@cnp"), false));
	LVari* tv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%cv%d", llvm_n++)->data);
	add_list(thgc, mainFunc->comps, (char*)make_lbinop(thgc, LBOP_Add, tv, tnp, typ));

	LStrV* strv = make_lstrv(thgc, createString(thgc, (char*)"@m", 2, 1), createString(thgc, (char*)"m", 1, 1), 1);
	add_list(thgc, local->llvm->strs, (char*)strv);
	LCall* ac_call = make_lcall(thgc, NULL, ac_val);
	lcall_add(thgc, ac_call, thgc3);
	lcall_add(thgc, ac_call, tv);
	lcall_add(thgc, ac_call, countv);
	lcall_add(thgc, ac_call, (LVari*)strv);
	lcall_add(thgc, ac_call, make_lvari_c(thgc, "%GCCheckFuncType", "@mainCheck"));
	lcall_add(thgc, ac_call, make_lvari_c(thgc, "%GCFinalizeFuncType", "null"));
	add_list(thgc, mainFunc->comps, (char*)ac_call);

	LFunc* curFunc = llvm_get_func(local->llvm);
	LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	add_list(thgc, curFunc->dbinits, (char*)make_lload(thgc, thgc4, make_lvari_c(thgc, "%ThreadGCType**", "@thgcp"), false));

	LVari* rngv = make_lvari_c(thgc, "%RootNodeType**", "%rngv");
	LGete* rngGete = make_lgete_idx2(thgc, "%ThreadGCType", rngv, thgc4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1"));
	add_list(thgc, curFunc->dbinits, (char*)rngGete);
	LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	add_list(thgc, curFunc->dbinits, (char*)make_lload(thgc, rn4, rngv, false));

	// GC_malloc
	LVari* go_val = make_lvari_c(thgc, "%GC_mallocType", "%gcobject");
	add_list(thgc, curFunc->comps, (char*)make_lload(thgc, go_val, make_lvari_c(thgc, "%GC_mallocType*", "@GC_malloc"), false));
	LVari* go_v = make_lvari_c(thgc, "%GCObjectPtr", "%obj");
	LVari* tmp = make_lvari_c(thgc, "i32", cnbuf);
	LVari* tmi = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tmi%d", llvm_n++)->data);
	add_list(thgc, curFunc->comps, (char*)make_lload(thgc, tmi, make_lvari_c(thgc, "i32*", "@cnp"), false));
	LVari* tmv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tmv%d", llvm_n++)->data);
	add_list(thgc, curFunc->comps, (char*)make_lbinop(thgc, LBOP_Add, tmv, tmi, tmp));
	LCall* go_call = make_lcall(thgc, go_v, go_val);
	lcall_add(thgc, go_call, thgc4);
	lcall_add(thgc, go_call, tmv);
	add_list(thgc, curFunc->comps, (char*)go_call);
	LVari* go_c = make_vn(thgc, "%mainType*");
	add_list(thgc, curFunc->comps, (char*)make_llocmarker(thgc, 1, 0, curFunc->debugId));
	LCast* go_cast = make_lcast(thgc, LCAST_Bitcast, go_c, go_v);
	add_list(thgc, curFunc->comps, (char*)go_cast);

	// Alloca + Store
	LVari* objptr = make_lvari_c(thgc, "%mainType**", "%objptr");
	add_list(thgc, curFunc->comps, (char*)make_lalloca(thgc, objptr));
	add_list(thgc, curFunc->comps, (char*)make_lstore(thgc, objptr, go_c));

	// GC_SetRoot
	LVari* srv = make_vn(thgc, "%GC_SetRootType");
	add_list(thgc, curFunc->comps, (char*)make_lload(thgc, srv, make_lvari_c(thgc, "%GC_SetRootType*", "@GC_SetRoot"), false));
	LCall* srcall = make_lcall(thgc, NULL, srv);
	lcall_add(thgc, srcall, rn4);
	lcall_add(thgc, srcall, objptr);
	add_list(thgc, curFunc->comps, (char*)srcall);

	// Process vmapA of second child (block)
	int order = 1;
	int count = 8;
	Block* blk = (Block*)*(Obj**)get_list(cblk->children, 1);

	// Set model on the block's obj
	if (blk->obj) {
		((Obj*)blk->obj)->model = createString(thgc, (char*)"%mainType", 9, 1);
		((Obj*)blk->obj)->bas = objptr;
	}

	local->blocks = create_list(thgc, sizeof(Block*), CType::_List);
	add_list(thgc, local->blocks, (char*)blk);
	List* decs = create_list(thgc, sizeof(Obj*), CType::_List);

	// Iterate vmapA
	for (int ki = 0; ki < blk->vmapA->kvs->size; ki++) {
		KV* kv = *(KV**)get_list(blk->vmapA->kvs, ki);
		Obj* v = (Obj*)kv->value;
		if (v->objtype == LetterType::OT_Variable) {
			Variable* variable = (Variable*)v;
			variable->objtype = LetterType::OT_Variable;
			v->order = order++;
			add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i8*", 3, 1), kv->key));
			count += 8;

			// GC check for object types
			vari = make_vn(thgc, "i8*");
			char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", v->order);
			gete = make_lgete_idx2(thgc, "%mainType", vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf));
			add_list(thgc, funccheck->comps, (char*)gete);
			vari2 = make_vn(thgc, "i8*");
			add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
			co_cval = make_vn(thgc, "i8*");
			co_call = make_lcall(thgc, co_cval, co_val);
			lcall_add(thgc, co_call, thgc2);
			lcall_add(thgc, co_call, vari);
			add_list(thgc, funccheck->comps, (char*)co_call);
			add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
		}
		else if (v->objtype == LetterType::OT_Function) {
			Function* f = (Function*)v;
			if (f->identity == 0) f->identity = llvm_n++;
			v->order = order++;
			add_list(thgc, decs, (char*)f);
			add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));

			// GC check
			vari = make_vn(thgc, "i8*");
			char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", v->order);
			gete = make_lgete_idx2(thgc, "%mainType", vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf));
			add_list(thgc, funccheck->comps, (char*)gete);
			vari2 = make_vn(thgc, "i8*");
			add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
			co_cval = make_vn(thgc, "i8*");
			co_call = make_lcall(thgc, co_cval, co_val);
			lcall_add(thgc, co_call, thgc2);
			lcall_add(thgc, co_call, vari);
			add_list(thgc, funccheck->comps, (char*)co_call);
			add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
			count += 8;

			// Function init call
			LVari* va0 = make_vn(thgc, "%FuncType*");
			LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "@%sdec%d", f->letter ? f->letter->text->data : "f", f->identity)->data));
			lcall_add(thgc, fu_call, rn4);
			lcall_add(thgc, fu_call, go_c);
			lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", "!"));
			lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", "?"));
			add_list(thgc, curFunc->comps, (char*)fu_call);
			LVari* va2 = make_vn(thgc, "%mainType*");
			add_list(thgc, curFunc->comps, (char*)make_lload(thgc, va2, objptr, false));
			LVari* va = make_vn(thgc, "%FuncType*");
			gete = make_lgete_idx2(thgc, "%mainType", va, va2, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf));
			add_list(thgc, curFunc->comps, (char*)gete);
			add_list(thgc, curFunc->comps, (char*)make_lstore(thgc, va, va0));
		}
		else if (v->objtype == LetterType::OT_ClassObj) {
			ClassObj* f = (ClassObj*)v;
			if (f->identity == 0) f->identity = llvm_n++;
			v->order = order++;
			add_list(thgc, decs, (char*)f);
			add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));
			count += 8;
		}
		else if (v->objtype == LetterType::OT_ModelObj) {
			ModelObj* f = (ModelObj*)v;
			if (f->identity == 0) f->identity = llvm_n++;
			v->order = order++;
			add_list(thgc, decs, (char*)f);
			add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));
			count += 8;
		}
	}

	// countv.name = count
	char countbuf[32]; snprintf(countbuf, sizeof(countbuf), "%d", count);
	countv->name = createString(thgc, countbuf, (int)strlen(countbuf), 1);
	add_list(thgc, funccheck->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));

	// Execute block
	Obj* obj = exeRangeC_Block(thgc, blk, 0, blk->children->size, local, true);

	// Epilogue
	LVari* rnpv = make_lvari_c(thgc, "i32*", "%ptr");
	add_list(thgc, curFunc->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	LVari* rnpv12 = make_lvari_c(thgc, "i32", "%v");
	add_list(thgc, curFunc->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
	LVari* rnpv2 = make_lvari_c(thgc, "i32", "%dec");
	add_list(thgc, curFunc->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12, make_lvari_c(thgc, "i32", "1")));
	add_list(thgc, curFunc->comps, (char*)make_lstore(thgc, rnpv, rnpv2));

	if (blk->rets == NULL || blk->rets->size == 0) {
		add_list(thgc, curFunc->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "i32", "0")));
	} else {
		Obj* valobj = vt_GetterC(thgc, *(Obj**)get_list(blk->rets, blk->rets->size - 1), local);
		add_list(thgc, curFunc->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "i32", "0")));
	}

	local->blocks->size--;
	add_list(thgc, mainFunc->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));

	// DB init strings
	LVari* csvari = make_vn(thgc, "%CreateStringType");
	add_list(thgc, curFunc->dbinits, (char*)make_lload(thgc, csvari, make_lvari_c(thgc, "%CreateStringType*", "@CreateString"), false));
	int len2 = 2; // "db"
	LStrV* sd2 = make_lstrv(thgc, createString(thgc, (char*)"@db", 3, 1), createString(thgc, (char*)"db", 2, 1), len2);
	add_list(thgc, local->llvm->strs, (char*)sd2);
	LVari* vd2 = make_vn(thgc, "i8*");
	add_list(thgc, curFunc->dbinits, (char*)make_lgete_idx2(thgc, "[3 x i8]", vd2, (LVari*)sd2, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	LVari* vdd2 = make_vn(thgc, "%StringType*");
	LCall* cscall = make_lcall(thgc, vdd2, csvari);
	lcall_add(thgc, cscall, thgc4);
	lcall_add(thgc, cscall, vd2);
	lcall_add(thgc, cscall, make_lvari_c(thgc, "i32", "2"));
	lcall_add(thgc, cscall, make_lvari_c(thgc, "i32", "1"));
	add_list(thgc, curFunc->dbinits, (char*)cscall);

	LVari* dbinit = make_vn(thgc, "%DbInitType");
	add_list(thgc, curFunc->dbinits, (char*)make_lload(thgc, dbinit, make_lvari_c(thgc, "%DbInitType*", "@DbInit"), false));
	LVari* sqlp = make_lvari_c(thgc, "ptr", "%sqlp");
	LCall* dbcall = make_lcall(thgc, sqlp, dbinit);
	lcall_add(thgc, dbcall, thgc4);
	lcall_add(thgc, dbcall, vdd2);
	lcall_add(thgc, dbcall, make_lvari_c(thgc, "i32", "0"));
	lcall_add(thgc, dbcall, make_lvari_c(thgc, "i32", "0"));
	lcall_add(thgc, dbcall, make_lvari_c(thgc, "i32", "0"));
	lcall_add(thgc, dbcall, make_lvari_c(thgc, "ptr", "null"));
	add_list(thgc, curFunc->dbinits, (char*)dbcall);
	add_list(thgc, curFunc->dbinits, (char*)make_lstore(thgc, make_lvari_c(thgc, "ptr", "@sqlp"), sqlp));

	return self;
}

// ============================================================
// declareC
// ============================================================
Obj* declareC(ThreadGC* thgc, Master* local, String* name) {
	return getC(thgc, local, name);
}

// ============================================================
// getC - variable lookup with LLVM getelementptr chain
// ============================================================
Obj* getC(ThreadGC* thgc, Master* local, String* name) {
	int count = 0;
	bool outside = false;
	LVari* vari = NULL;
	Block* oldblk = NULL;
	local->compsC = create_list(thgc, sizeof(LComp*), CType::_List);
	LFunc* func = llvm_get_func(local->llvm);

	for (int i = 0; i < local->blocks->size; i++) {
		LComp* loaded = NULL;
		Block* blk = *(Block**)get_list(local->blocks, local->blocks->size - i - 1);
		Obj* blkobj = blk->obj ? (Obj*)blk->obj : NULL;

		if (blkobj && blkobj->objtype == LetterType::OT_Comment) {
			if (func->async_flag) {
				LVari* obj = make_lvari_c(thgc, "%CoroFrameType*", "%frame");
				LVari* ogv = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
				add_list(thgc, local->compsC, (char*)make_lgete_idx2(thgc, "%CoroFrameType", ogv, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
				vari = make_vn(thgc, "%GCObjectPtr");
				add_list(thgc, local->compsC, (char*)make_lload(thgc, vari, ogv, false));
				outside = true;
			} else {
				vari = make_vn(thgc, "i8*");
				if (blkobj->bas) {
					add_list(thgc, local->compsC, (char*)make_lload(thgc, vari, blkobj->bas, false));
				}
				outside = true;
			}
		}
		else if (i == 0 || (blk->obj && ((ObjBlock*)blk->obj)->n == 1) || (blk->obj && ((ObjBlock*)blk->obj)->n == -1)) {
			if (blkobj && (blkobj->objtype == LetterType::OT_While || blkobj->objtype == LetterType::OT_For || blkobj->objtype == LetterType::OT_Model)) {
				if (outside) {
					LVari* newvari = make_vn(thgc, "i8**");
					if (oldblk && ((Obj*)oldblk->obj)->model) {
						add_list(thgc, local->compsC, (char*)make_lgete_idx2(thgc, (char*)((Obj*)oldblk->obj)->model->data, newvari, vari, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
					}
					LVari* newvari2 = make_vn(thgc, "i8*");
					add_list(thgc, local->compsC, (char*)make_lload(thgc, newvari2, newvari, false));
					vari = newvari2;
				} else {
					if (func->async_flag) {
						LVari* obj = make_lvari_c(thgc, "%CoroFrameType*", "%frame");
						LVari* ogv = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
						add_list(thgc, local->compsC, (char*)make_lgete_idx2(thgc, "%CoroFrameType", ogv, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
						vari = make_vn(thgc, "%GCObjectPtr");
						add_list(thgc, local->compsC, (char*)make_lload(thgc, vari, ogv, false));
						outside = true;
					} else {
						vari = make_vn(thgc, "i8*");
						if (blkobj->bas) {
							loaded = (LComp*)make_lload(thgc, vari, blkobj->bas, false);
						}
					}
				}
			}
			else if (blkobj && (blkobj->objtype == LetterType::OT_None || blkobj->objtype == LetterType::OT_Function || blkobj->objtype == LetterType::OT_ClassObj || blkobj->objtype == LetterType::OT_ModelObj)) {
				if (outside) {
					LVari* newvari = make_vn(thgc, "i8**");
					if (oldblk && ((Obj*)oldblk->obj)->model) {
						add_list(thgc, local->compsC, (char*)make_lgete_idx2(thgc, (char*)((Obj*)oldblk->obj)->model->data, newvari, vari, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
					}
					LVari* newvari2 = make_vn(thgc, "i8*");
					add_list(thgc, local->compsC, (char*)make_lload(thgc, newvari2, newvari, false));
					vari = newvari2;
				} else {
					vari = make_vn(thgc, "i8*");
					if (blkobj->bas) {
						add_list(thgc, local->compsC, (char*)make_lload(thgc, vari, blkobj->bas, false));
					}
				}
				outside = true;
			}
		}
		else {
			if (blkobj && (blkobj->objtype == LetterType::OT_While || blkobj->objtype == LetterType::OT_For)) {
				if (!outside) {
					vari = make_vn(thgc, "i8*");
					if (blkobj->bas) {
						loaded = (LComp*)make_lload(thgc, vari, blkobj->bas, false);
					}
				}
			}
		}

		// Check if name exists in vmapA
		char* found = get_mapy(blk->vmapA, name);
		if (found != NULL) {
			Obj* obj = (Obj*)found;
			if (loaded != NULL) add_list(thgc, local->compsC, (char*)loaded);
			LVari* newvari = make_vn(thgc, "i8*");
			char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", obj->order);
			if (blkobj && blkobj->model) {
				add_list(thgc, local->compsC, (char*)make_lgete_idx2(thgc, (char*)blkobj->model->data, newvari, vari, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
			}
			func->vari = newvari;

			// Emit all accumulated comps
			for (int ci = 0; ci < local->compsC->size; ci++) {
				LComp* comp = *(LComp**)get_list(local->compsC, ci);
				add_list(thgc, func->comps, (char*)comp);
			}

			if (obj->objtype == LetterType::OT_Variable) {
				return vt_SelfC(thgc, obj, local);
			}
			return obj;
		}
		oldblk = blk;
		count++;
	}

	// Check sigmapA
	if (local->sigmapA) {
		char* sig = get_mapy(local->sigmapA, name);
		if (sig != NULL) return (Obj*)sig;
	}

	return ErrorC(thgc, LetterType::OT_Error, NULL, "variable not found");
}
