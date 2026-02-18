
// ============================================================
// objz.h - Z解析パス (静的解析 / 型チェック)
// ============================================================

// 前方宣言
Obj* exeZ(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepZ(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* GetterZ(ThreadGC* thgc, Obj* self, Master* local);
Obj* SelfZ(ThreadGC* thgc, Obj* self, Master* local);
Obj* opeZ(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2);
Obj* PrimaryZ(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeZ_Block(ThreadGC* thgc, Block* self, Master* local);
Obj* exepZ_Block(ThreadGC* thgc, Block* self, int* n, Master* local, Primary* primary);
Obj* exeRangeZ(ThreadGC* thgc, Block* self, int start, int end, Master* local, bool id);
Obj* PrimaryZ_Block(ThreadGC* thgc, Block* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* SelfZ_Block(ThreadGC* thgc, Block* self, Master* local);
Obj* GetterZ_Block(ThreadGC* thgc, Block* self, Master* local);

// ============================================================
// declareZ
// ============================================================
void declareZ(ThreadGC* thgc, Master* local, String* name, Obj* obj) {
	Block* blk = *(Block**)get_list(local->blocks, local->blocks->size - 1);
	add_mapy(thgc, blk->vmapA, name, (char*)obj);
}

// ============================================================
// GetterZ (デフォルト: selfを返す)
// ============================================================
Obj* GetterZ(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// SelfZ (デフォルト: selfを返す)
// ============================================================
Obj* SelfZ(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// PrimaryZ (デフォルト: n++してselfを返す)
// ============================================================
Obj* PrimaryZ(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	(*n)++;
	return self;
}

// ============================================================
// exeZ (デフォルト: 例外)
// ============================================================
Obj* exeZ(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// Operator::exeZ
// ============================================================
Obj* exeZ_Operator(ThreadGC* thgc, Obj* self, Master* local) {
	ATSSpan* letter = self->letter;
	if (letter != NULL && letter->text != NULL) {
		int len = letter->text->size;
		char* data = letter->text->data;
		bool is_assign = (len == 1 && data[0] == '=');
		bool is_colon = (len == 1 && data[0] == ':');
		bool is_in = (len == 2 && data[0] == 'i' && data[1] == 'n');
		if (is_assign || is_colon || is_in) {
			Obj* val1 = SelfZ(thgc, exeZ(thgc, *(Obj**)get_list(self->children, 0), local), local);
			if (val1->objtype == LetterType::OT_Wait || val1->objtype == LetterType::OT_Error || val1->objtype == LetterType::OT_NG) return val1;
			Obj* val2 = GetterZ(thgc, exeZ(thgc, *(Obj**)get_list(self->children, 1), local), local);
			if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
			return self;
		}
	}
	{
		Obj* val1 = GetterZ(thgc, exeZ(thgc, *(Obj**)get_list(self->children, 0), local), local);
		if (val1->objtype == LetterType::OT_Wait || val1->objtype == LetterType::OT_Error || val1->objtype == LetterType::OT_NG) return val1;
		Obj* val2 = GetterZ(thgc, exeZ(thgc, *(Obj**)get_list(self->children, 1), local), local);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
		return self;
	}
}

// ============================================================
// Primary::exeZ
// ============================================================
Obj* exeZ_Primary(ThreadGC* thgc, Primary* self, Master* local) {
	if (local->comments != NULL && local->comments->size > 0) {
	}
	Obj* val1 = NULL;
	for (int i = 0; i < self->children->size - 1;) {
		Obj* child = *(Obj**)get_list(self->children, i);
		switch (child->objtype) {
			case LetterType::OT_Comment2:
				i++;
				continue;
			case LetterType::OT_Htm:
			case LetterType::OT_Word:
			case LetterType::OT_Number:
			case LetterType::OT_FloatVal:
			case LetterType::OT_BoolVal:
			case LetterType::OT_StrObj:
			case LetterType::OT_Print:
			case LetterType::OT_Return:
			case LetterType::OT_Goto:
			case LetterType::OT_Continue:
			case LetterType::OT_Var:
			case LetterType::OT_If:
			case LetterType::OT_Elif:
			case LetterType::OT_Else:
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
				i++;
				break;
			case LetterType::OT_Bracket:
			case LetterType::OT_Block:
				val1 = exepZ_Block(thgc, (Block*)child, &i, local, self);
				i++;
				break;
			case LetterType::OT_Model:
			case LetterType::OT_Gene:
			case LetterType::OT_Class:
				val1 = exepZ(thgc, child, &i, local, self);
				break;
			case LetterType::OT_Dot:
			case LetterType::OT_Left:
			case LetterType::OT_Right:
				i++;
				break;
			case LetterType::OT_TagBlock:
			case LetterType::OT_CallBlock:
				i++;
				break;
			default:
				break;
		}
		if (val1 != NULL && (val1->objtype == LetterType::OT_Wait || val1->objtype == LetterType::OT_Error || val1->objtype == LetterType::OT_NG)) return val1;
	}
	if (val1 == NULL) val1 = *(Obj**)get_list(self->children, self->children->size - 1);
	return val1;
}

// ============================================================
// Obj::exepZ (TODO: 移植中)
// ============================================================
Obj* exepZ(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return (Obj*)self;
}

// ============================================================
// Block::exepZ
// ============================================================
Obj* exepZ_Block(ThreadGC* thgc, Block* self, int* n, Master* local, Primary* primary) {
	if (self->objtype == LetterType::OT_Block) {
		int m = 0;
		if (self->children->size != 0) {
			Obj* child_m = *(Obj**)get_list(self->children, m);
			if (child_m->objtype == LetterType::OT_Word) {
				if (child_m->letter != NULL && child_m->letter->text != NULL &&
					child_m->letter->text->size == 1 && child_m->letter->text->data[0] == 'T') {
					m++;
					Obj* val2 = *(Obj**)get_list(self->children, m);
					VariClass* type = (VariClass*)GC_alloc(thgc, CType::_VariClass);
					type->objtype = LetterType::OT_VariClass;
					type->n = 0;
				head:
					if (val2->objtype == LetterType::OT_Block) {
						Block* blk = (Block*)val2;
						if (blk->children->size == 0) {
							ArrType* arrtype = (ArrType*)GC_alloc(thgc, CType::_ArrType);
							arrtype->objtype = LetterType::OT_ArrType;
							arrtype->base = (Obj*)type;
							type = (VariClass*)arrtype;
							m++;
							val2 = *(Obj**)get_list(self->children, m);
							goto head;
						}
					}
					*n = 1;
					val2 = *(Obj**)get_list(primary->children, *n);
					if (val2->objtype == LetterType::OT_Word) {
						Obj* word = val2;
						(*n)++;
						val2 = *(Obj**)get_list(primary->children, *n);
						bool gene = false;
						if (val2->objtype == LetterType::OT_Block) {
							GenericFunction* func = (GenericFunction*)GC_alloc(thgc, CType::_GenericFunction);
							func->objtype = LetterType::OT_GenericFunction;
							func->rettype = (Obj*)type;
							func->vmap = create_mapy(thgc, CType::_List);
							func->blocks = create_list(thgc, sizeof(Block*), CType::_List);
							Block* blk = (Block*)exeZ_Block(thgc, (Block*)val2, local);
							for (int i = 0; i < blk->rets->size; i++) {
								Obj* ret_i = *(Obj**)get_list(blk->rets, i);
								if (ret_i->objtype == LetterType::OT_Word) {
									VariClass* vc = (VariClass*)GC_alloc(thgc, CType::_VariClass);
									vc->objtype = LetterType::OT_VariClass;
									vc->n = i;
									add_mapy(thgc, func->vmap, ret_i->letter->text, (char*)vc);
								}
							}
							(*n)++;
							val2 = *(Obj**)get_list(primary->children, *n);
							if (val2->objtype == LetterType::OT_CallBlock) {
								func->draw = (Block*)val2;
								for (int bi = 0; bi < local->blocks->size; bi++) {
									add_list(thgc, func->blocks, *get_list(local->blocks, bi));
								}
								declareZ(thgc, local, word->letter->text, (Obj*)func);
								return (Obj*)func;
							}
						} else if (val2->objtype == LetterType::OT_CallBlock) {
							Function* func = (Function*)GC_alloc(thgc, CType::_Function);
							func->objtype = LetterType::OT_Function;
							func->rettype = (Obj*)type;
							func->blocks = create_list(thgc, sizeof(Block*), CType::_List);
							func->draw = (Block*)val2;
							for (int bi = 0; bi < local->blocks->size; bi++) {
								add_list(thgc, func->blocks, *get_list(local->blocks, bi));
							}
							declareZ(thgc, local, word->letter->text, (Obj*)func);
							return (Obj*)func;
						}
					}
				}
			}
		}
	}
	return exeRangeZ(thgc, self, 0, self->children->size, local, true);
}

// ============================================================
// Block::exeZ
// ============================================================
Obj* exeZ_Block(ThreadGC* thgc, Block* self, Master* local) {
	if (self->objtype == LetterType::OT_Array) return (Obj*)self;
	return exeRangeZ(thgc, self, 0, self->children->size, local, true);
}

// ============================================================
// Block::exeRangeZ
// ============================================================
Obj* exeRangeZ(ThreadGC* thgc, Block* self, int start, int end, Master* local, bool id) {
	for (int i = 0; i < self->children->size; i++) {
		Obj* child = *(Obj**)get_list(self->children, i);
		Obj* val = exeZ(thgc, child, local);
		if (self->objtype != LetterType::OT_Call1 && self->objtype != LetterType::OT_Call2) {
			if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		}
		add_list(thgc, self->rets, (char*)val);
	}
	return (Obj*)self;
}

// ============================================================
// Block::PrimaryZ
// ============================================================
Obj* PrimaryZ_Block(ThreadGC* thgc, Block* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (self->objtype == LetterType::OT_Bracket) {
		if (val2->objtype == LetterType::OT_Block) {
			Block* blk_v2 = (Block*)val2;
			if (self->rets->size == 1 || blk_v2->children->size == 0) {
				Obj* cls = *(Obj**)get_list(self->rets, 0);
				if (cls->objtype != LetterType::OT_None) {
					ArrType* typearray = (ArrType*)GC_alloc(thgc, CType::_ArrType);
					typearray->objtype = LetterType::OT_ArrType;
					typearray->base = cls;
					return (Obj*)typearray;
				}
			}
		} else if (val2->objtype == LetterType::OT_Bracket) {
			Block* block = (Block*)exeZ_Block(thgc, (Block*)val2, local);
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			for (int i = 0; i < block->rets->size; i++) {
				val2 = *(Obj**)get_list(block->rets, i);
				Obj* cls = *(Obj**)get_list(self->rets, i % self->rets->size);
				if (val2->objtype == LetterType::OT_Word && cls != NULL) {
					Variable* variable = (Variable*)GC_alloc(thgc, CType::_Variable);
					variable->objtype = LetterType::OT_Variable;
					variable->vartype = cls;
					declareZ(thgc, local, val2->letter->text, (Obj*)variable);
					// block->rets[i] = variable
					char** slot = get_list(block->rets, i);
					*slot = (char*)variable;
				}
			}
			return (Obj*)block;
		} else if (val2->objtype == LetterType::OT_Word) {
			Obj* word = val2;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			Variable* variable = (Variable*)GC_alloc(thgc, CType::_Variable);
			variable->objtype = LetterType::OT_Variable;
			variable->vartype = *(Obj**)get_list(self->rets, 0);
			declareZ(thgc, local, word->letter->text, (Obj*)variable);
			return (Obj*)variable;
		} else if (val2->objtype == LetterType::OT_Left) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (self->rets->size == 1 || ((Block*)val2)->children->size == 0) {
				Obj* type = *(Obj**)get_list(self->rets, 0);
				if (type != NULL) {
					FuncType2* functype = (FuncType2*)GC_alloc(thgc, CType::_FuncType2);
					functype->objtype = LetterType::OT_FuncType;
					functype->rettype = type;
					functype->draws = create_list(thgc, sizeof(Obj*), CType::_List);
					(*n)++;
					val2 = *(Obj**)get_list(primary->children, *n);
					GetterZ(thgc, exeZ(thgc, val2, local), local);
					if (val2->objtype == LetterType::OT_ClassObj) {
						add_list(thgc, functype->draws, (char*)val2);
						(*n)++;
						return (Obj*)functype;
					} else if (val2->objtype == LetterType::OT_Generic) {
						Obj* generic = val2;
						(*n)++;
						val2 = *(Obj**)get_list(primary->children, *n);
						if (val2->objtype == LetterType::OT_Block) {
							GetterZ(thgc, exeZ_Block(thgc, (Block*)val2, local), local);
							Block* blk = (Block*)val2;
							GenericObj* genericobj = (GenericObj*)GC_alloc(thgc, CType::_GenericObj);
							genericobj->objtype = LetterType::OT_GenericObj;
							genericobj->generic = generic;
							genericobj->draws = create_list(thgc, sizeof(Obj*), CType::_List);
							for (int i = 0; i < blk->rets->size; i++) {
								Obj* rv = *(Obj**)get_list(blk->rets, i);
								if (rv->objtype == LetterType::OT_ClassObj || rv->objtype == LetterType::OT_GenericObj) {
									add_list(thgc, genericobj->draws, (char*)rv);
								}
							}
							Generic* gen = (Generic*)generic;
							if (gen->vmap->kvs->size != genericobj->draws->size) {
								// error
							}
							(*n)++;
							add_list(thgc, functype->draws, (char*)genericobj);
							return (Obj*)functype;
						}
					} else if (val2->objtype == LetterType::OT_Bracket) {
						Block* blk = (Block*)GetterZ(thgc, exeZ_Block(thgc, (Block*)val2, local), local);
						for (int i = 0; i < blk->rets->size; i++) {
							Obj* rv = *(Obj**)get_list(blk->rets, i);
							if (rv->objtype == LetterType::OT_ClassObj || rv->objtype == LetterType::OT_GenericObj) {
								add_list(thgc, functype->draws, (char*)rv);
							}
						}
						(*n)++;
						return (Obj*)functype;
					}
				}
			}
		} else if (val2->objtype == LetterType::OT_Dot) {
			// throw
		}
	} else if (self->objtype == LetterType::OT_Block || self->objtype == LetterType::OT_Array) {
		if (val2->objtype == LetterType::OT_Block) {
			Block* block = (Block*)val2;
			GetterZ_Block(thgc, block, local);
			Obj* me = (Obj*)self;
			for (int i = 0; i < block->rets->size; i++) {
				if (me->objtype == LetterType::OT_Block || me->objtype == LetterType::OT_Array) {
					Block* blk = (Block*)me;
					Obj* ret_i = *(Obj**)get_list(block->rets, i);
					if (ret_i->objtype == LetterType::OT_Number) {
						// Number indexing
						// me = blk->rets[num.value]
					} else if (ret_i->objtype == LetterType::OT_StrObj) {
						// String indexing via labelmap
					}
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
				if (word->letter != NULL && word->letter->text != NULL) {
					int tlen = word->letter->text->size;
					char* tdata = word->letter->text->data;
					if (tlen == 6 && memcmp(tdata, "length", 6) == 0) {
						Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
						return num;
					}
				}
			}
		}
	}
	return (Obj*)self;
}

// ============================================================
// Block::SelfZ
// ============================================================
Obj* SelfZ_Block(ThreadGC* thgc, Block* self, Master* local) {
	for (int i = 0; i < self->rets->size; i++) {
		Obj* rv = *(Obj**)get_list(self->rets, i);
		rv = SelfZ(thgc, rv, local);
		char** slot = get_list(self->rets, i);
		*slot = (char*)rv;
	}
	return (Obj*)self;
}

// ============================================================
// Block::GetterZ
// ============================================================
Obj* GetterZ_Block(ThreadGC* thgc, Block* self, Master* local) {
	for (int i = 0; i < self->rets->size; i++) {
		Obj* rv = *(Obj**)get_list(self->rets, i);
		rv = GetterZ(thgc, rv, local);
		char** slot = get_list(self->rets, i);
		*slot = (char*)rv;
	}
	return (Obj*)self;
}

// ============================================================
// CallBlock::exepZ
// ============================================================
Obj* exepZ_CallBlock(ThreadGC* thgc, Block* self, int* n, Master* local, Primary* primary) {
	return (Obj*)self;
}

// ============================================================
// CallBlock::exeZ
// ============================================================
Obj* exeZ_CallBlock(ThreadGC* thgc, Block* self, Master* local) {
	Block* block2 = (Block*)*(Obj**)get_list(self->children, 1);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"object", 6, 1), (char*)local->Object);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"void", 4, 1), (char*)local->Void);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"int", 3, 1), (char*)local->Int);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"short", 5, 1), (char*)local->Short);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"Str", 3, 1), (char*)local->StrT);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"bool", 4, 1), (char*)local->Bool);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"float", 5, 1), (char*)local->FloatT);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"MouseDown", 9, 1), (char*)local->MouseEventT);
	add_mapy(thgc, block2->vmapA, createString(thgc, (char*)"KeyDown", 7, 1), (char*)local->KeyEventT);
	for (int i = 0; i < local->analblocks->size; i++) {
		Block* block = *(Block**)get_list(local->analblocks, i);
		// local->blocks = { block }
		local->blocks->size = 0;
		add_list(thgc, local->blocks, (char*)block);
		exeZ_Block(thgc, block, local);
		// local->blockslist.RemoveAt(last)
		if (local->blockslist->size > 0) {
			local->blockslist->size--;
		}
	}
	return (Obj*)self;
}

// ============================================================
// Class::exepZ
// ============================================================
Obj* exepZ_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);
	Obj* obj = (Obj*)GC_alloc(thgc, CType::_ClassObj);
	obj->objtype = LetterType::OT_ClassObj;
	if (val2->objtype == LetterType::OT_Word) {
		Obj* word = val2;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Block) {
			Generic* generic = (Generic*)GC_alloc(thgc, CType::_Generic);
			generic->objtype = LetterType::OT_Generic;
			generic->vmap = create_mapy(thgc, CType::_List);
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			obj = (Obj*)generic;
		}
		declareZ(thgc, local, word->letter->text, obj);
		((ClassObj*)obj)->letter = word->letter;
		if (val2->objtype == LetterType::OT_Right) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				(*n)++;
				obj->rename = val2->letter->text;
			}
		}
	}
	return self;
}

// ============================================================
// Model::exepZ
// ============================================================
Obj* exepZ_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);
	if (val2->objtype == LetterType::OT_Word) {
		Obj* word = val2;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		ModelObj* gj = (ModelObj*)GC_alloc(thgc, CType::_ModelObj);
		gj->objtype = LetterType::OT_Model;
		gj->letter = word->letter;
		gj->draw = NULL;
		gj->rename = NULL;
		gj->letter2 = NULL;
		declareZ(thgc, local, word->letter->text, (Obj*)gj);
		if (val2->objtype == LetterType::OT_Right) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				gj->rename = word->letter->text;
				if (val2->objtype == LetterType::OT_Block) {
					(*n)++;
					val2 = *(Obj**)get_list(primary->children, *n);
				}
			}
		}
		if (val2->objtype == LetterType::OT_CallBlock) {
			Block* callblk = (Block*)val2;
			Block* inner = (Block*)*(Obj**)get_list(callblk->children, 1);
			gj->letter2 = inner->letter2;
			gj->draw = callblk;
			ObjBlock* objblk = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
			objblk->objtype = LetterType::OT_ObjBlock;
			objblk->obj = (Obj*)gj;
			objblk->n = 1;
			inner->obj = (Obj*)objblk;
			(*n)++;
			return (Obj*)gj;
		}
	}
	return self;
}

// ============================================================
// Gene::exepZ
// ============================================================
Obj* exepZ_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);
	if (val2->objtype == LetterType::OT_Word) {
		Obj* word = val2;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		GeneObj* gj = (GeneObj*)GC_alloc(thgc, CType::_GeneObj);
		gj->objtype = LetterType::OT_Gene;
		gj->name = word->letter->text;
		gj->letter = self->letter;
		gj->call = NULL;
		gj->blocks = create_list(thgc, sizeof(Block*), CType::_List);
		gj->vmap = create_mapy(thgc, CType::_List);
		// local->gene->vmap[word.name] = gj
		if (local->gene != NULL) {
			Generic* gen = (Generic*)local->gene;
			add_mapy(thgc, gen->vmap, word->letter->text, (char*)gj);
		}
		declareZ(thgc, local, word->letter->text, (Obj*)gj);
		if (val2->objtype == LetterType::OT_Right) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				(*n)++;
				gj->letter2 = NULL;
				// gj->rename = word->letter->text; (C#: (val2 as Word).name)
				if (val2->objtype == LetterType::OT_Block) {
					(*n)++;
					val2 = *(Obj**)get_list(primary->children, *n);
				}
			}
		}
		if (val2->objtype == LetterType::OT_CallBlock) {
			Block* callblk = (Block*)val2;
			Block* inner = (Block*)*(Obj**)get_list(callblk->children, 1);
			gj->letter2 = inner->letter2;
			gj->call = callblk;
			ObjBlock* objblk = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
			objblk->objtype = LetterType::OT_ObjBlock;
			objblk->obj = (Obj*)gj;
			objblk->n = 1;
			inner->obj = (Obj*)objblk;
			for (int bi = 0; bi < local->blocks->size; bi++) {
				add_list(thgc, gj->blocks, *get_list(local->blocks, bi));
			}
			(*n)++;
			return (Obj*)gj;
		}
	} else if (val2->objtype == LetterType::OT_CallBlock) {
		GeneObj* gj = (GeneObj*)GC_alloc(thgc, CType::_GeneObj);
		gj->objtype = LetterType::OT_Gene;
		gj->name = NULL;
		gj->letter = NULL;
		gj->letter2 = NULL;
		gj->call = (Block*)val2;
		gj->blocks = create_list(thgc, sizeof(Block*), CType::_List);
		gj->vmap = create_mapy(thgc, CType::_List);
		if (local->gene != NULL) {
			Generic* gen = (Generic*)local->gene;
			add_mapy(thgc, gen->vmap, createString(thgc, (char*)"", 0, 1), (char*)gj);
		}
		Block* callblk = (Block*)val2;
		Block* inner = (Block*)*(Obj**)get_list(callblk->children, 1);
		ObjBlock* objblk = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
		objblk->objtype = LetterType::OT_ObjBlock;
		objblk->obj = (Obj*)gj;
		objblk->n = 1;
		inner->obj = (Obj*)objblk;
		for (int bi = 0; bi < local->blocks->size; bi++) {
			add_list(thgc, gj->blocks, *get_list(local->blocks, bi));
		}
		(*n)++;
		return (Obj*)gj;
	}
	return self;
}
