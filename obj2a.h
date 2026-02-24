// ============================================================
// obj2a.h - A解析パス (vtable dispatch版)
// ============================================================

// 前方宣言 - vtable実装関数
Obj* exeA_Operator(ThreadGC* thgc, Obj* self, Master* local);
Obj* exeA_Primary(ThreadGC* thgc, Obj* self, Master* local);
Obj* NextA_Primary(ThreadGC* thgc, Primary* primary, Master* local, int* i, Obj** val1);
Obj* exeA_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepA_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeRangeA_Block(ThreadGC* thgc, Block* self, int start, int end, Master* local, bool id);
Obj* PrimaryA_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* SelfA_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* GetterA_Block(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepA_CallBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeA_CallBlock(ThreadGC* thgc, Obj* self, Master* local);
Obj* GetterA_Word(ThreadGC* thgc, Obj* self, Master* local);
Obj* SelfA_Word(ThreadGC* thgc, Obj* self, Master* local);
Obj* GetterA_Variable(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryA_Variable(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_Function(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_Number(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);

// ============================================================
// ヘルパー関数
// ============================================================

bool is_operator_typeA(LetterType t) {
	return t == LetterType::_Equal || t == LetterType::_Colon ||
		t == LetterType::_LessThan || t == LetterType::_LessEqual ||
		t == LetterType::_MoreThan || t == LetterType::_MoreEqual ||
		t == LetterType::_EqualEqual || t == LetterType::_NotEqual ||
		t == LetterType::_Plus || t == LetterType::_Minus ||
		t == LetterType::_Mul || t == LetterType::_Div ||
		t == LetterType::_Mod || t == LetterType::_And || t == LetterType::_Percent;
}

Obj* ErrorA(ThreadGC* thgc, LetterType type, ATSSpan* letter) {
	Obj* err = make_cobj(thgc, CType::_CObj, type, letter);
	return err;
}

bool str_matchA(String* s, const char* lit, int len) {
	if (s == NULL) return false;
	if (s->size != len) return false;
	if (s->esize == 1) return memcmp(s->data, lit, len) == 0;
	return false;
}

// ============================================================
// declareA
// ============================================================
Obj* declareA(ThreadGC* thgc, Master* local, String* name, Obj* obj) {
	Block* blk = *(Block**)get_list(local->blocks, local->blocks->size - 1);
	char* existing = get_mapy(blk->vmapA, name);
	if (existing != NULL) {
		return ErrorA(thgc, LetterType::OT_Error, ((Obj*)existing)->letter);
	}
	add_mapy(thgc, blk->vmapA, name, (char*)obj);
	return obj;
}

// ============================================================
// declareRA
// ============================================================
Obj* declareRA(ThreadGC* thgc, Master* local, String* name, Obj* obj) {
	Block* blk = *(Block**)get_list(local->blocks, local->blocks->size - 1);
	add_mapy(thgc, blk->vmapA, name, (char*)obj);
	return obj;
}

// ============================================================
// getA
// ============================================================
Obj* getA(ThreadGC* thgc, Master* local, String* name) {
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* blk = *(Block**)get_list(local->blocks, i);
		char* val = get_mapy(blk->vmapA, name);
		if (val != NULL) return (Obj*)val;
	}
	return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
}

// ============================================================
// Word::GetterA
// ============================================================
Obj* GetterA_Word(ThreadGC* thgc, Obj* self, Master* local) {
	return vt_GetterA(thgc, getA(thgc, local, self->letter->text), local);
}

// ============================================================
// Word::SelfA
// ============================================================
Obj* SelfA_Word(ThreadGC* thgc, Obj* self, Master* local) {
	return getA(thgc, local, self->letter->text);
}

// ============================================================
// Variable::GetterA
// ============================================================
Obj* GetterA_Variable(ThreadGC* thgc, Obj* self, Master* local) {
	Obj* val = make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
	return val;
}

// ============================================================
// Variable::PrimaryA
// ============================================================
Obj* PrimaryA_Variable(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Obj* val = make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
	return vt_PrimaryA(thgc, val, n, local, primary, val2);
}

// ============================================================
// Function::PrimaryA
// ============================================================
Obj* PrimaryA_Function(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* v = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (v->objtype == LetterType::OT_Wait || v->objtype == LetterType::OT_Error || v->objtype == LetterType::OT_NG) return v;
		Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
		return ret;
	}
	(*n)++; return self;
}

// ============================================================
// Number::PrimaryA
// ============================================================
Obj* PrimaryA_Number(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* v2 = *(Obj**)get_list(primary->children, *n);
		if (v2->objtype == LetterType::OT_Word) {
			(*n)++;
			Obj* v3 = *(Obj**)get_list(primary->children, *n);
			if (str_matchA(v2->letter->text, "random", 6)) {
				if (v3->objtype == LetterType::OT_Bracket) {
					(*n)++;
					Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
					return num;
				}
			}
		}
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Operator::exeA
// ============================================================
Obj* exeA_Operator(ThreadGC* thgc, Obj* self, Master* local) {
	ATSSpan* letter = self->letter;
	if (letter != NULL && letter->text != NULL) {
		int len = letter->text->size;
		char* data = letter->text->data;
		bool is_assign = (len == 1 && data[0] == '=');
		bool is_colon = (len == 1 && data[0] == ':');
		bool is_in = (len == 2 && data[0] == 'i' && data[1] == 'n');
		if (is_assign || is_colon || is_in) {
			Obj* val2 = vt_GetterA(thgc, vt_exeA(thgc, *(Obj**)get_list(self->children, 1), local), local);
			if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
			Obj* val1 = vt_SelfA(thgc, vt_exeA(thgc, *(Obj**)get_list(self->children, 0), local), local);
			if (val1->objtype == LetterType::OT_Wait || val1->objtype == LetterType::OT_Error || val1->objtype == LetterType::OT_NG) return val1;
			return val1;
		}
	}
	{
		Obj* val1 = vt_GetterA(thgc, vt_exeA(thgc, *(Obj**)get_list(self->children, 0), local), local);
		if (val1->objtype == LetterType::OT_Wait || val1->objtype == LetterType::OT_Error || val1->objtype == LetterType::OT_NG) return val1;
		Obj* val2 = vt_GetterA(thgc, vt_exeA(thgc, *(Obj**)get_list(self->children, 1), local), local);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
		return val1;
	}
}

// ============================================================
// Primary::NextA
// ============================================================
Obj* NextA_Primary(ThreadGC* thgc, Primary* primary, Master* local, int* i, Obj** val1) {
	Obj* child = *(Obj**)get_list(primary->children, *i);
	switch (child->objtype) {
		case LetterType::OT_Comment2:
			(*i)++;
			break;
		case LetterType::OT_Htm:
		case LetterType::OT_TagBlock:
			vt_exepA(thgc, child, i, local, primary);
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
				*val1 = vt_exepA(thgc, child, i, local, primary);
				(*i)++;
			} else {
				*val1 = vt_GetterA(thgc, *val1, local);
				if ((*val1)->objtype == LetterType::OT_Wait || (*val1)->objtype == LetterType::OT_Error || (*val1)->objtype == LetterType::OT_NG) return *val1;
				*val1 = vt_PrimaryA(thgc, *val1, i, local, primary, vt_exepA(thgc, child, i, local, primary));
			}
			break;
		case LetterType::OT_Dot:
		case LetterType::OT_Left:
		case LetterType::OT_Right:
			*val1 = vt_GetterA(thgc, *val1, local);
			if ((*val1)->objtype == LetterType::OT_Wait || (*val1)->objtype == LetterType::OT_Error || (*val1)->objtype == LetterType::OT_NG) return *val1;
			*val1 = vt_PrimaryA(thgc, *val1, i, local, primary, child);
			break;
		case LetterType::OT_CallBlock:
		case LetterType::OT_Block:
			if (*val1 == NULL) {
				*val1 = vt_exepA(thgc, child, i, local, primary);
				(*i)++;
			} else {
				*val1 = vt_GetterA(thgc, *val1, local);
				if ((*val1)->objtype == LetterType::OT_Wait || (*val1)->objtype == LetterType::OT_Error || (*val1)->objtype == LetterType::OT_NG) return *val1;
				*val1 = vt_PrimaryA(thgc, *val1, i, local, primary, child);
			}
			break;
		default:
			break;
	}
	return *val1;
}

// ============================================================
// Primary::exeA (vtable signature: Obj* self)
// ============================================================
Obj* exeA_Primary(ThreadGC* thgc, Obj* self, Master* local) {
	Primary* pself = (Primary*)self;
	Obj* val1 = NULL;
	for (int i = 0; i < pself->children->size - 1;) {
		NextA_Primary(thgc, pself, local, &i, &val1);
		if (val1 != NULL && (val1->objtype == LetterType::OT_Wait || val1->objtype == LetterType::OT_Error || val1->objtype == LetterType::OT_NG)) return val1;
	}
	if (val1 == NULL) val1 = *(Obj**)get_list(pself->children, pself->children->size - 1);

	// singleops処理
	for (int i = pself->singleops->size - 1; i >= 0; i--) {
		SingleOp* sop = *(SingleOp**)get_list(pself->singleops, i);
		if (sop->letter != NULL && sop->letter->text != NULL) {
			String* str = sop->letter->text;
			if (str_matchA(str, ".", 1)) {
				// op = sop; (dot)
			} else if (str_matchA(str, "*", 1) || str_matchA(str, "/", 1) || str_matchA(str, ">>", 2)) {
				Obj* last = *(Obj**)get_list(pself->children, pself->children->size - 1);
				if (val1 != last) val1 = vt_opeA(thgc, val1, str, local, NULL);
			} else {
				val1 = vt_GetterA(thgc, val1, local);
				val1 = vt_opeA(thgc, val1, str, local, NULL);
			}
		}
	}
	return val1;
}

// ============================================================
// Block::exeA (vtable signature: Obj* self)
// ============================================================
Obj* exeA_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	if (blk->objtype == LetterType::OT_Array) return (Obj*)blk;
	return exeRangeA_Block(thgc, blk, 0, blk->children->size, local, true);
}

// ============================================================
// Block::exeRangeA (vtable dispatch で子を実行)
// ============================================================
Obj* exeRangeA_Block(ThreadGC* thgc, Block* self, int start, int end, Master* local, bool id) {
	self->rets = create_list(thgc, sizeof(Obj*), CType::_List);
	if (self->objtype == LetterType::OT_Call1 || self->objtype == LetterType::OT_Call2) {
		for (int i = 0; i < self->children->size; i++) {
			Obj* child = *(Obj**)get_list(self->children, i);
			Obj* val = vt_exeA(thgc, child, local);
			add_list(thgc, self->rets, (char*)val);
		}
	} else {
		for (int i = 0; i < self->children->size; i++) {
			Obj* child = *(Obj**)get_list(self->children, i);
			Obj* val = vt_exeA(thgc, child, local);
			if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
			add_list(thgc, self->rets, (char*)val);
		}
	}
	return (Obj*)self;
}

// ============================================================
// Block::exepA (vtable signature: Obj* self)
// ============================================================
Obj* exepA_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Block* blk = (Block*)self;
	if (blk->objtype == LetterType::OT_Block) {
		int m = 0;
		if (blk->children->size != 0) {
			Obj* child_m = *(Obj**)get_list(blk->children, m);
			if (child_m->objtype == LetterType::OT_Word) {
				if (child_m->letter != NULL && child_m->letter->text != NULL &&
					child_m->letter->text->size == 1 && child_m->letter->text->data[0] == 'T') {
					m++;
					Obj* val2 = *(Obj**)get_list(blk->children, m);
					VariClass* type = (VariClass*)GC_alloc(thgc, CType::_VariClass);
					type->objtype = LetterType::OT_VariClass;
					type->vt = NULL;
					type->n = 0;
				head:
					if (val2->objtype == LetterType::OT_Block) {
						Block* blk2 = (Block*)val2;
						if (blk2->children->size == 0) {
							ArrType* arrtype = (ArrType*)GC_alloc(thgc, CType::_ArrType);
							arrtype->objtype = LetterType::OT_ArrType;
							arrtype->vt = NULL;
							arrtype->base = (Obj*)type;
							type = (VariClass*)arrtype;
							m++;
							val2 = *(Obj**)get_list(blk->children, m);
							goto head;
						} else {
							return ErrorA(thgc, LetterType::OT_Error, val2->letter);
						}
					}
					*n = 1;
					val2 = *(Obj**)get_list(primary->children, *n);
					if (val2->objtype == LetterType::OT_Word) {
						Obj* word = val2;
						(*n)++;
						val2 = *(Obj**)get_list(primary->children, *n);
						if (val2->objtype == LetterType::OT_Block) {
							GenericFunction* func = (GenericFunction*)GC_alloc(thgc, CType::_GenericFunction);
							func->objtype = LetterType::OT_GenericFunction;
							func->vt = NULL;
							func->rettype = (Obj*)type;
							func->vmap = create_mapy(thgc, CType::_List);
							func->blocks = create_list(thgc, sizeof(Block*), CType::_List);
							Obj* val = vt_exeA(thgc, val2, local);
							if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
							Block* blk3 = (Block*)val;
							for (int i = 0; i < blk3->rets->size; i++) {
								Obj* ret_i = *(Obj**)get_list(blk3->rets, i);
								if (ret_i->objtype == LetterType::OT_Word) {
									VariClass* vc = (VariClass*)GC_alloc(thgc, CType::_VariClass);
									vc->objtype = LetterType::OT_VariClass;
									vc->vt = NULL;
									vc->n = i;
									add_mapy(thgc, func->vmap, ret_i->letter->text, (char*)vc);
								} else {
									return ErrorA(thgc, LetterType::OT_Error, val2->letter);
								}
							}
							(*n)++;
							val2 = *(Obj**)get_list(primary->children, *n);
							if (val2->objtype == LetterType::OT_CallBlock) {
								func->draw = (Block*)val2;
								for (int bi = 0; bi < local->blocks->size; bi++) {
									add_list(thgc, func->blocks, *get_list(local->blocks, bi));
								}
								declareA(thgc, local, word->letter->text, (Obj*)func);
								return (Obj*)func;
							}
							return ErrorA(thgc, LetterType::OT_Error, val2->letter);
						} else if (val2->objtype == LetterType::OT_CallBlock) {
							Function* func = (Function*)GC_alloc(thgc, CType::_Function);
							func->objtype = LetterType::OT_Function;
							func->vt = NULL;
							func->rettype = (Obj*)type;
							func->blocks = create_list(thgc, sizeof(Block*), CType::_List);
							func->draw = (Block*)val2;
							for (int bi = 0; bi < local->blocks->size; bi++) {
								add_list(thgc, func->blocks, *get_list(local->blocks, bi));
							}
							declareA(thgc, local, word->letter->text, (Obj*)func);
							return (Obj*)func;
						}
					}
				}
			}
		}
	}
	return exeRangeA_Block(thgc, blk, 0, blk->children->size, local, true);
}

// ============================================================
// Block::PrimaryA (vtable signature: Obj* self)
// ============================================================
Obj* PrimaryA_Block(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Block* blk = (Block*)self;
	if (blk->objtype == LetterType::OT_Bracket) {
		if (val2->objtype == LetterType::OT_Block) {
			if (blk->rets->size == 1) {
				Block* blk_v2 = (Block*)val2;
				if (blk_v2->children->size == 0) {
					Obj* cls = *(Obj**)get_list(blk->rets, 0);
					if (cls != NULL) {
						ArrType* typearray = (ArrType*)GC_alloc(thgc, CType::_ArrType);
						typearray->objtype = LetterType::OT_ArrType;
						typearray->vt = NULL;
						typearray->base = cls;
						return (Obj*)typearray;
					} else {
						return ErrorA(thgc, LetterType::OT_Error, val2->letter);
					}
				} else {
					return ErrorA(thgc, LetterType::OT_Error, val2->letter);
				}
			} else {
				return ErrorA(thgc, LetterType::OT_Error, blk->letter);
			}
		} else if (val2->objtype == LetterType::OT_Bracket) {
			Obj* val = vt_exeA(thgc, val2, local);
			if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
			Block* block = (Block*)val;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			for (int i = 0; i < block->rets->size; i++) {
				Obj* rv = *(Obj**)get_list(block->rets, i);
				if (rv->objtype == LetterType::OT_Word) {
					Obj* cls = *(Obj**)get_list(blk->rets, i % blk->rets->size);
					if (cls != NULL) {
						Variable* variable = (Variable*)GC_alloc(thgc, CType::_Variable);
						variable->objtype = LetterType::OT_Variable;
						variable->vt = NULL;
						variable->vartype = cls;
						declareA(thgc, local, rv->letter->text, (Obj*)variable);
						char** slot = get_list(block->rets, i);
						*slot = (char*)variable;
					} else {
						return ErrorA(thgc, LetterType::OT_Error, blk->letter);
					}
				} else {
					return ErrorA(thgc, LetterType::OT_Error, rv->letter);
				}
			}
			return (Obj*)block;
		} else if (val2->objtype == LetterType::OT_Word) {
			Obj* word = val2;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			Variable* variable = (Variable*)GC_alloc(thgc, CType::_Variable);
			variable->objtype = LetterType::OT_Variable;
			variable->vt = NULL;
			variable->vartype = *(Obj**)get_list(blk->rets, 0);
			declareA(thgc, local, word->letter->text, (Obj*)variable);
			return (Obj*)variable;
		} else if (val2->objtype == LetterType::OT_Left) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (blk->rets->size == 1) {
				Obj* ret0 = *(Obj**)get_list(blk->rets, 0);
				return vt_PrimaryA(thgc, ret0, n, local, primary, val2);
			} else {
				return ErrorA(thgc, LetterType::OT_Error, blk->letter);
			}
		} else if (val2->objtype == LetterType::OT_Dot) {
			if (blk->rets->size == 1) {
				Obj* ret0 = *(Obj**)get_list(blk->rets, 0);
				return vt_PrimaryA(thgc, ret0, n, local, primary, val2);
			} else {
				return ErrorA(thgc, LetterType::OT_Error, blk->letter);
			}
		}
	} else if (blk->objtype == LetterType::OT_Block || blk->objtype == LetterType::OT_Array) {
		if (val2->objtype == LetterType::OT_Block) {
			Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
			if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
			Block* block = (Block*)val;
			Obj* me = (Obj*)blk;
			for (int i = 0; i < block->rets->size; i++) {
				if (me->objtype == LetterType::OT_Block || me->objtype == LetterType::OT_Array) {
					Block* meblk = (Block*)me;
					Obj* ret_i = *(Obj**)get_list(block->rets, i);
					if (ret_i->objtype == LetterType::OT_Number) {
						// Number indexing: me = meblk->rets[num.value]
						// TODO: get number value
					} else if (ret_i->objtype == LetterType::OT_StrObj) {
						// String indexing via labelmap
						// TODO: labelmap lookup
					} else {
						return ErrorA(thgc, LetterType::OT_Error, ret_i->letter);
					}
				} else {
					return ErrorA(thgc, LetterType::OT_Error, val2->letter);
				}
			}
			(*n)++;
			return me;
		} else if (val2->objtype == LetterType::OT_Dot) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				Obj* word = val2;
				(*n)++;
				if (str_matchA(word->letter->text, "length", 6)) {
					Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
					return num;
				} else if (str_matchA(word->letter->text, "Sort", 4)) {
					Obj* v2 = *(Obj**)get_list(primary->children, *n);
					if (v2->objtype == LetterType::OT_Bracket) {
						Obj* val = vt_GetterA(thgc, vt_exeA(thgc, v2, local), local);
						if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
						(*n)++;
						return (Obj*)blk;
					} else {
						return ErrorA(thgc, LetterType::OT_Error, v2->letter);
					}
				} else if (str_matchA(word->letter->text, "Range", 5)) {
					Obj* v2 = *(Obj**)get_list(primary->children, *n);
					if (v2->objtype == LetterType::OT_Bracket) {
						Obj* val = vt_GetterA(thgc, vt_exeA(thgc, v2, local), local);
						if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
						(*n)++;
						return (Obj*)blk;
					} else {
						return ErrorA(thgc, LetterType::OT_Error, v2->letter);
					}
				} else {
					return ErrorA(thgc, LetterType::OT_NG, val2->letter);
				}
			} else {
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			}
		} else {
			return ErrorA(thgc, LetterType::OT_NG, val2->letter);
		}
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Block::SelfA (vtable signature: Obj* self)
// ============================================================
Obj* SelfA_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	for (int i = 0; i < blk->rets->size; i++) {
		Obj* rv = *(Obj**)get_list(blk->rets, i);
		rv = vt_SelfA(thgc, rv, local);
		char** slot = get_list(blk->rets, i);
		*slot = (char*)rv;
	}
	return (Obj*)blk;
}

// ============================================================
// Block::GetterA (vtable signature: Obj* self)
// ============================================================
Obj* GetterA_Block(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	for (int i = 0; i < blk->rets->size; i++) {
		Obj* rv = *(Obj**)get_list(blk->rets, i);
		rv = vt_GetterA(thgc, rv, local);
		char** slot = get_list(blk->rets, i);
		*slot = (char*)rv;
	}
	return (Obj*)blk;
}

// ============================================================
// CallBlock::exepA (vtable signature: Obj* self)
// ============================================================
Obj* exepA_CallBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return (Obj*)self;
}

// ============================================================
// CallBlock::exeA (vtable signature: Obj* self)
// ============================================================
Obj* exeA_CallBlock(ThreadGC* thgc, Obj* self, Master* local) {
	Block* blk = (Block*)self;
	Block* block2 = (Block*)*(Obj**)get_list(blk->children, 1);
	local->blocks->size = 0;
	add_list(thgc, local->blocks, (char*)block2);
	exeA_Block(thgc, (Obj*)block2, local);
	if (local->blocks->size > 0) local->blocks->size--;
	if (local->blockslist->size > 0) local->blockslist->size--;
	return (Obj*)blk;
}

// ============================================================
// initVTables - VTable初期化
// ============================================================
void initVTables() {
	// デフォルトで全ルックアップを初期化
	for (int i = 0; i < VT_LOOKUP_SIZE; i++) {
		vt_lookup[i] = &vt_Obj_default;
	}

	// --- Operator ---
	vt_Operator = vt_Obj_default;
	vt_Operator.exeA = exeA_Operator;
	vt_lookup[(int)LetterType::_Equal] = &vt_Operator;
	vt_lookup[(int)LetterType::_Colon] = &vt_Operator;
	vt_lookup[(int)LetterType::_LessThan] = &vt_Operator;
	vt_lookup[(int)LetterType::_LessEqual] = &vt_Operator;
	vt_lookup[(int)LetterType::_MoreThan] = &vt_Operator;
	vt_lookup[(int)LetterType::_MoreEqual] = &vt_Operator;
	vt_lookup[(int)LetterType::_EqualEqual] = &vt_Operator;
	vt_lookup[(int)LetterType::_NotEqual] = &vt_Operator;
	vt_lookup[(int)LetterType::_Plus] = &vt_Operator;
	vt_lookup[(int)LetterType::_Minus] = &vt_Operator;
	vt_lookup[(int)LetterType::_Mul] = &vt_Operator;
	vt_lookup[(int)LetterType::_Div] = &vt_Operator;
	vt_lookup[(int)LetterType::_Mod] = &vt_Operator;
	vt_lookup[(int)LetterType::_And] = &vt_Operator;
	vt_lookup[(int)LetterType::_Percent] = &vt_Operator;

	// --- Primary ---
	vt_Primary = vt_Obj_default;
	vt_Primary.exeA = exeA_Primary;
	// Primary uses OT_None but vt is set explicitly in ParsePrimary

	// --- Block (OT_Block, OT_Bracket, OT_Call1, OT_Call2) ---
	vt_Block = vt_Obj_default;
	vt_Block.exeA = exeA_Block;
	vt_Block.exepA = exepA_Block;
	vt_Block.GetterA = GetterA_Block;
	vt_Block.SelfA = SelfA_Block;
	vt_Block.PrimaryA = PrimaryA_Block;
	vt_lookup[(int)LetterType::OT_Block] = &vt_Block;
	vt_lookup[(int)LetterType::OT_Bracket] = &vt_Block;
	vt_lookup[(int)LetterType::OT_Call1] = &vt_Block;
	vt_lookup[(int)LetterType::OT_Call2] = &vt_Block;

	// --- Block_Array (OT_Array) ---
	vt_Block_Array = vt_Obj_default;
	vt_Block_Array.GetterA = GetterA_Block;
	vt_Block_Array.SelfA = SelfA_Block;
	vt_Block_Array.PrimaryA = PrimaryA_Block;
	vt_lookup[(int)LetterType::OT_Array] = &vt_Block_Array;

	// --- CallBlock ---
	vt_CallBlock = vt_Obj_default;
	vt_CallBlock.exeA = exeA_CallBlock;
	vt_CallBlock.exepA = exepA_CallBlock;
	vt_lookup[(int)LetterType::OT_CallBlock] = &vt_CallBlock;

	// --- Word ---
	vt_Word = vt_Obj_default;
	vt_Word.GetterA = GetterA_Word;
	vt_Word.SelfA = SelfA_Word;
	vt_lookup[(int)LetterType::OT_Word] = &vt_Word;

	// --- Variable ---
	vt_Variable = vt_Obj_default;
	vt_Variable.GetterA = GetterA_Variable;
	vt_Variable.PrimaryA = PrimaryA_Variable;
	vt_lookup[(int)LetterType::OT_Variable] = &vt_Variable;

	// --- Function ---
	vt_Function = vt_Obj_default;
	vt_Function.PrimaryA = PrimaryA_Function;
	vt_lookup[(int)LetterType::OT_Function] = &vt_Function;

	// --- Number ---
	vt_Number = vt_Obj_default;
	vt_Number.PrimaryA = PrimaryA_Number;
	vt_lookup[(int)LetterType::OT_Number] = &vt_Number;

	// obj2a2.h の型を初期化
	initVTables_A2();

	// obj2a3.h の型を初期化
	initVTables_A3();

	// obj2b.h, obj2b2.h, obj2b3.h の型を初期化 (B-pass)
	initVTables_B();
	initVTables_B2();
	initVTables_B3();
}
