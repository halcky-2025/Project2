// ============================================================
// obj2a3.h - A解析パス (vtable dispatch版) 第3ブロック
// Value, Class, Type, If, While, For, Gene, Model 等
// ============================================================

// ============================================================
// 前方宣言
// ============================================================
Obj* PrimaryA_Value(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_Type(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Var(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Generic(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_BoolVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* opeA_SqlString(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2);
Obj* PrimaryA_SqlString(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_ArrayConstructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_Constructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_Base(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepA_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_GenericFunction2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_ModelVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_GeneChild(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_GeneMutate(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_GeneNew(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_GeneCross(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_GeneStore(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_GeneSelect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryA_GeneSort(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepA_GeneLabel(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryA_GeneLabelValue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);

// ============================================================
// ヘルパー: CallBlockの2ブロックを実行する共通パターン
// ============================================================
static void _exeCallBlockPairA(ThreadGC* thgc, Obj* callblk, Obj* owner, Master* local) {
	Block* blk1 = (Block*)*(Obj**)get_list(callblk->children, 0);
	ObjBlock* objblk1 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
	objblk1->objtype = LetterType::OT_ObjBlock;
	objblk1->vt = NULL;
	objblk1->obj = owner;
	objblk1->n = 0;
	blk1->obj = (Obj*)objblk1;
	add_list(thgc, local->blocks, (char*)blk1);
	vt_exeA(thgc, (Obj*)blk1, local);

	Block* blk2 = (Block*)*(Obj**)get_list(callblk->children, 1);
	ObjBlock* objblk2 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
	objblk2->objtype = LetterType::OT_ObjBlock;
	objblk2->vt = NULL;
	objblk2->obj = owner;
	objblk2->n = 1;
	blk2->obj = (Obj*)objblk2;
	add_list(thgc, local->blocks, (char*)blk2);
	vt_exeA(thgc, (Obj*)blk2, local);

	if (local->blocks->size > 0) local->blocks->size--;
	if (local->blocks->size > 0) local->blocks->size--;
}

// ============================================================
// ヘルパー: Geneの追加CallBlock (left/right) を実行
// ============================================================
static void _exeGeneExtBlockA(ThreadGC* thgc, Obj* callblk, Obj* owner, Master* local) {
	Block* blk3 = (Block*)*(Obj**)get_list(callblk->children, 0);
	ObjBlock* objblk3 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
	objblk3->objtype = LetterType::OT_ObjBlock;
	objblk3->vt = NULL;
	objblk3->obj = owner;
	objblk3->n = 2;
	blk3->obj = (Obj*)objblk3;
	add_list(thgc, local->blocks, (char*)blk3);
	vt_exeA(thgc, (Obj*)blk3, local);

	Block* blk4 = (Block*)*(Obj**)get_list(callblk->children, 1);
	ObjBlock* objblk4 = (ObjBlock*)GC_alloc(thgc, CType::_ObjBlock);
	objblk4->objtype = LetterType::OT_ObjBlock;
	objblk4->vt = NULL;
	objblk4->obj = owner;
	objblk4->n = 3;
	blk4->obj = (Obj*)objblk4;
	add_list(thgc, local->blocks, (char*)blk4);
	vt_exeA(thgc, (Obj*)blk4, local);

	if (local->blocks->size > 0) local->blocks->size--;
	if (local->blocks->size > 0) local->blocks->size--;
}

// ============================================================
// Value::PrimaryA - 値型の後置演算
// ============================================================
Obj* PrimaryA_Value(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	} else if (val2->objtype == LetterType::OT_Bracket) {
		vt_exeA(thgc, val2, local);
		(*n)++;
		return self;
	} else if (val2->objtype == LetterType::OT_Block) {
		vt_exeA(thgc, val2, local);
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Class::exepA
// ============================================================
Obj* exepA_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Class::PrimaryA - クラス宣言
// ============================================================
Obj* PrimaryA_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Obj* obj = (Obj*)GC_alloc(thgc, CType::_ClassObj);
	obj->objtype = LetterType::OT_ClassObj;
	obj->vt = NULL;
	obj->rename = NULL;
	obj->version = NULL;

	if (val2->objtype == LetterType::OT_Word) {
		Obj* word = val2;
		obj = getA(thgc, local, word->letter->text);
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (local->comments->size > 0) {
			return ErrorA(thgc, LetterType::OT_Error, self->letter);
		}
		// Generic block handling
		if (val2->objtype == LetterType::OT_Block) {
			Generic* generic = (Generic*)obj;
			Obj* val = vt_exeA(thgc, val2, local);
			if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
			Block* block = (Block*)val;
			add_list(thgc, local->blocks, (char*)block);
			for (int i = 0; i < block->rets->size; i++) {
				Obj* ret_i = *(Obj**)get_list(block->rets, i);
				if (ret_i->objtype == LetterType::OT_Word) {
					VariClass* vc = (VariClass*)GC_alloc(thgc, CType::_VariClass);
					vc->objtype = LetterType::OT_VariClass;
					vc->vt = NULL;
					vc->n = i;
					add_mapy(thgc, generic->vmap, ret_i->letter->text, (char*)vc);
				} else {
					return ErrorA(thgc, LetterType::OT_Error, ret_i->letter);
				}
			}
			if (local->blocks->size > 0) local->blocks->size--;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			obj = (Obj*)generic;
		}
		obj->letter = word->letter;
		// Right arrow → rename
		if (val2->objtype == LetterType::OT_Right) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				(*n)++;
				obj->rename = val2->letter->text;
				val2 = *(Obj**)get_list(primary->children, *n);
			} else {
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			}
		} else {
			obj->rename = word->letter->text;
		}
		declareRA(thgc, local, obj->rename, obj);
	}
	// Left arrow → extends
	if (val2->objtype == LetterType::OT_Left) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		// extends handling (simplified)
		if (val2->objtype == LetterType::OT_Bracket) {
			val2 = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
			if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
		}
		if (val2->objtype == LetterType::OT_ClassObj) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
		} else if (val2->objtype == LetterType::OT_Generic) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Block) {
				vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
			}
		}
	}
	// CallBlock → class body
	if (val2->objtype == LetterType::OT_CallBlock) {
		_exeCallBlockPairA(thgc, val2, obj, local);
		(*n)++;
		return obj;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Type::PrimaryA - 型の後置演算 (変数・関数宣言)
// ============================================================
Obj* PrimaryA_Type(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	// Array type: T[]
	if (val2->objtype == LetterType::OT_Block) {
		Block* blk = (Block*)val2;
		if (blk->children->size == 0) {
			ArrType* arrtype = (ArrType*)GC_alloc(thgc, CType::_ArrType);
			arrtype->objtype = LetterType::OT_ArrType;
			arrtype->vt = NULL;
			arrtype->base = self;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			return PrimaryA_Type(thgc, (Obj*)arrtype, n, local, primary, val2);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	// Function type: T(args)
	if (val2->objtype == LetterType::OT_Left) {
		FuncType2* functype = (FuncType2*)GC_alloc(thgc, CType::_FuncType2);
		functype->objtype = LetterType::OT_FuncType;
		functype->vt = NULL;
		functype->rettype = self;
		functype->draws = create_list(thgc, sizeof(Obj*), CType::_List);
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Bracket) {
			val2 = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
			if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
			Block* blk = (Block*)val2;
			for (int i = 0; i < blk->rets->size; i++) {
				Obj* rv = *(Obj**)get_list(blk->rets, i);
				if (rv->objtype == LetterType::OT_ClassObj || rv->objtype == LetterType::OT_GenericObj) {
					add_list(thgc, functype->draws, (char*)rv);
				}
			}
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
		} else {
			Obj* gval = vt_GetterA(thgc, val2, local);
			if (gval->objtype == LetterType::OT_ClassObj || gval->objtype == LetterType::OT_Variable) {
				add_list(thgc, functype->draws, (char*)gval);
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				return PrimaryA_Type(thgc, (Obj*)functype, n, local, primary, val2);
			}
		}
		self = (Obj*)functype;
	}
	// Dot → .new / .method
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			// .new → Constructor
			if (self->objtype == LetterType::OT_ClassObj || self->objtype == LetterType::OT_ArrType) {
				Obj* ctor = make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
				ctor->vt = &vt_Constructor;
				ctor->children = create_list(thgc, sizeof(Obj*), CType::_List);
				add_list(thgc, ctor->children, (char*)self);
				return ctor;
			} else if (self->objtype == LetterType::OT_Model || self->objtype == LetterType::OT_Gene) {
				// model/gene member access → SqlString
				Obj* sqlstr = make_cobj(thgc, CType::_CObj, LetterType::_None, val2->letter);
				sqlstr->vt = &vt_SqlString;
				return sqlstr;
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	// Dollar → comment variable
	if (val2->objtype == LetterType::OT_Dolor) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	// Word → variable/function declaration
	Obj* word = NULL;
	Obj* wordre = NULL;
	if (val2->objtype == LetterType::OT_Word) {
		word = val2;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		// Right arrow → rename
		if (val2->objtype == LetterType::OT_Right) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				wordre = val2;
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
			} else {
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			}
		}
	}
	// Bracket → multiple variable declaration
	if (val2->objtype == LetterType::OT_Bracket && word == NULL) {
		Block* blk = (Block*)vt_exeA(thgc, val2, local);
		(*n)++;
		for (int i = 0; i < blk->rets->size; i++) {
			Obj* rv = *(Obj**)get_list(blk->rets, i);
			if (rv->objtype == LetterType::OT_Word) {
				Variable* variable = (Variable*)GC_alloc(thgc, CType::_Variable);
				variable->objtype = LetterType::OT_Variable;
				variable->vt = NULL;
				variable->vartype = self;
				declareA(thgc, local, rv->letter->text, (Obj*)variable);
				declareRA(thgc, local, rv->letter->text, (Obj*)variable);
			}
		}
		return (Obj*)blk;
	}
	// GenericFunction: T [params] {|body}
	if (val2->objtype == LetterType::OT_Block) {
		GenericFunction* func = (GenericFunction*)GC_alloc(thgc, CType::_GenericFunction);
		func->objtype = LetterType::OT_GenericFunction;
		func->vt = NULL;
		func->rettype = self;
		func->vmap = create_mapy(thgc, CType::_List);
		func->blocks = create_list(thgc, sizeof(Block*), CType::_List);
		Obj* val = vt_exeA(thgc, val2, local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		Block* blk = (Block*)val;
		for (int i = 0; i < blk->rets->size; i++) {
			Obj* ret_i = *(Obj**)get_list(blk->rets, i);
			if (ret_i->objtype == LetterType::OT_Word) {
				VariClass* vc = (VariClass*)GC_alloc(thgc, CType::_VariClass);
				vc->objtype = LetterType::OT_VariClass;
				vc->vt = NULL;
				vc->n = i;
				add_mapy(thgc, func->vmap, ret_i->letter->text, (char*)vc);
			}
		}
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_CallBlock) {
			func->draw = (Block*)val2;
			_exeCallBlockPairA(thgc, val2, (Obj*)func, local);
			for (int bi = 0; bi < local->blocks->size; bi++) {
				add_list(thgc, func->blocks, *get_list(local->blocks, bi));
			}
			(*n)++;
			if (word != NULL) {
				String* fname = word->letter->text;
				declareA(thgc, local, fname, (Obj*)func);
				declareRA(thgc, local, wordre ? wordre->letter->text : fname, (Obj*)func);
			}
			return (Obj*)func;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	// Function: T word {|body}
	if (val2->objtype == LetterType::OT_CallBlock) {
		Function* func = (Function*)GC_alloc(thgc, CType::_Function);
		func->objtype = LetterType::OT_Function;
		func->vt = NULL;
		func->rettype = self;
		func->draw = (Block*)val2;
		func->blocks = create_list(thgc, sizeof(Block*), CType::_List);
		_exeCallBlockPairA(thgc, val2, (Obj*)func, local);
		for (int bi = 0; bi < local->blocks->size; bi++) {
			add_list(thgc, func->blocks, *get_list(local->blocks, bi));
		}
		(*n)++;
		if (word != NULL) {
			String* fname = word->letter->text;
			declareA(thgc, local, fname, (Obj*)func);
			declareRA(thgc, local, wordre ? wordre->letter->text : fname, (Obj*)func);
			((Obj*)func)->letter = word->letter;
		} else {
			((Obj*)func)->letter = val2->letter;
		}
		return (Obj*)func;
	}
	// Variable declaration (no CallBlock)
	if (word != NULL) {
		Variable* variable = (Variable*)GC_alloc(thgc, CType::_Variable);
		variable->objtype = LetterType::OT_Variable;
		variable->vt = NULL;
		variable->vartype = self;
		declareA(thgc, local, word->letter->text, (Obj*)variable);
		((Obj*)variable)->letter = word->letter;
		declareRA(thgc, local, wordre ? wordre->letter->text : word->letter->text, (Obj*)variable);
		return (Obj*)variable;
	}
	return self;
}

// ============================================================
// Var::exepA
// ============================================================
Obj* exepA_Var(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Generic::PrimaryA
// ============================================================
Obj* PrimaryA_Generic(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Generic* gen = (Generic*)self;
	if (val2->objtype == LetterType::OT_Block) {
		Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		Block* block = (Block*)val;
		GenericObj* geneobj = (GenericObj*)GC_alloc(thgc, CType::_GenericObj);
		geneobj->objtype = LetterType::OT_GenericObj;
		geneobj->vt = NULL;
		geneobj->generic = self;
		geneobj->draws = create_list(thgc, sizeof(Obj*), CType::_List);
		for (int i = 0; i < block->rets->size; i++) {
			Obj* rv = *(Obj**)get_list(block->rets, i);
			add_list(thgc, geneobj->draws, (char*)rv);
		}
		return (Obj*)geneobj;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// FloatVal::exepA / PrimaryA
// ============================================================
Obj* exepA_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryA_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// StrObj::PrimaryA
// ============================================================
Obj* PrimaryA_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// BoolVal::PrimaryA
// ============================================================
Obj* PrimaryA_BoolVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// VoiVal::exepA / PrimaryA
// ============================================================
Obj* exepA_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return ErrorA(thgc, LetterType::OT_NG, self->letter);
}
Obj* PrimaryA_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Null::PrimaryA
// ============================================================
Obj* PrimaryA_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// SqlString::opeA
// ============================================================
Obj* opeA_SqlString(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) {
	if (key != NULL && key->size >= 1) {
		char c = key->data[0];
		if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=') return self;
		if (key->size == 2) {
			if (memcmp(key->data, "==", 2) == 0 || memcmp(key->data, "!=", 2) == 0 ||
				memcmp(key->data, ">=", 2) == 0 || memcmp(key->data, "<=", 2) == 0) return self;
			if (memcmp(key->data, "or", 2) == 0) return self;
		}
		if (key->size == 3 && memcmp(key->data, "and", 3) == 0) return self;
		if (c == '>' || c == '<') return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}

// ============================================================
// SqlString::PrimaryA
// ============================================================
Obj* PrimaryA_SqlString(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ArrayConstructor::PrimaryA
// ============================================================
Obj* PrimaryA_ArrayConstructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		(*n)++;
		Block* arr = make_cblock(thgc, CType::_Block, LetterType::OT_Array);
		return (Obj*)arr;
	}
	Obj* val3 = vt_GetterA(thgc, val2, local);
	if (val3->objtype == LetterType::OT_Wait || val3->objtype == LetterType::OT_Error || val3->objtype == LetterType::OT_NG) return val3;
	if (val3->objtype == LetterType::OT_Number) {
		(*n)++;
		Block* arr = make_cblock(thgc, CType::_Block, LetterType::OT_Array);
		return (Obj*)arr;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Constructor::PrimaryA
// ============================================================
Obj* PrimaryA_Constructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		val2 = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
	}
	Obj* val3 = vt_GetterA(thgc, val2, local);
	if (val3->objtype == LetterType::OT_Wait || val3->objtype == LetterType::OT_Error || val3->objtype == LetterType::OT_NG) return val3;
	if (val3->objtype == LetterType::OT_Number || val3->objtype == LetterType::OT_StrObj ||
		val3->objtype == LetterType::OT_BoolVal) {
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Base::PrimaryA
// ============================================================
Obj* PrimaryA_Base(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Block) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
	}
	if (val2->objtype == LetterType::OT_Bracket) {
		vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
	}
	Obj* val3 = vt_GetterA(thgc, val2, local);
	if (val3->objtype == LetterType::OT_Wait || val3->objtype == LetterType::OT_Error || val3->objtype == LetterType::OT_NG) return val3;
	if (val3->objtype == LetterType::OT_Number || val3->objtype == LetterType::OT_StrObj ||
		val3->objtype == LetterType::OT_BoolVal) {
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// If::exepA
// ============================================================
Obj* exepA_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// If::PrimaryA - if/elif/else分岐
// ============================================================
Obj* PrimaryA_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		_exeCallBlockPairA(thgc, val2, self, local);
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
	head:
		if (val2->objtype == LetterType::OT_Elif) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_CallBlock) {
				_exeCallBlockPairA(thgc, val2, self, local);
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				goto head;
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		} else if (val2->objtype == LetterType::OT_Else) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_CallBlock) {
				_exeCallBlockPairA(thgc, val2, self, local);
				(*n)++;
				return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// While::exepA / PrimaryA
// ============================================================
Obj* exepA_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryA_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		_exeCallBlockPairA(thgc, val2, self, local);
		Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
		Obj* retobj = *(Obj**)get_list(blk2->rets, blk2->rets->size - 1);
		(*n)++;
		return retobj;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// For::exepA / PrimaryA
// ============================================================
Obj* exepA_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryA_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		_exeCallBlockPairA(thgc, val2, self, local);
		Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
		Obj* retobj = *(Obj**)get_list(blk2->rets, blk2->rets->size - 1);
		(*n)++;
		return retobj;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Switch::exepA / PrimaryA
// ============================================================
Obj* exepA_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryA_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		_exeCallBlockPairA(thgc, val2, self, local);
		Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
		Obj* retobj = *(Obj**)get_list(blk2->rets, blk2->rets->size - 1);
		(*n)++;
		return retobj;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Break::exepA / PrimaryA - 残りのPrimary項目を実行
// ============================================================
Obj* exepA_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextA_Primary(thgc, (Primary*)primary, local, n, &val2);
		if (val2 != NULL && (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterA(thgc, val2, local);
	(*n)--;
	return val2 ? val2 : self;
}
Obj* PrimaryA_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextA_Primary(thgc, (Primary*)primary, local, n, &val2);
		if (val2 != NULL && (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterA(thgc, val2, local);
	return val2 ? val2 : self;
}

// ============================================================
// Continue::exepA
// ============================================================
Obj* exepA_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Return::exepA / PrimaryA
// ============================================================
Obj* exepA_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextA_Primary(thgc, (Primary*)primary, local, n, &val2);
		if (val2 != NULL && (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterA(thgc, val2, local);
	(*n)--;
	return val2 ? val2 : self;
}
Obj* PrimaryA_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextA_Primary(thgc, (Primary*)primary, local, n, &val2);
		if (val2 != NULL && (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterA(thgc, val2, local);
	return val2 ? val2 : self;
}

// ============================================================
// Goto::exepA / PrimaryA
// ============================================================
Obj* exepA_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryA_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj ||
		val2->objtype == LetterType::OT_Word) {
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Print::exepA / PrimaryA
// ============================================================
Obj* exepA_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryA_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket || val2->objtype == LetterType::OT_Block) {
		Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
	}
	val2 = vt_GetterA(thgc, val2, local);
	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj) {
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
	}
	// consume remaining
	Obj* v = NULL;
	for (; *n < primary->children->size - 1;) {
		NextA_Primary(thgc, (Primary*)primary, local, n, &v);
		if (v != NULL && (v->objtype == LetterType::OT_Wait || v->objtype == LetterType::OT_Error || v->objtype == LetterType::OT_NG)) return v;
	}
	if (v != NULL) {
		v = vt_GetterA(thgc, v, local);
		if (v->objtype == LetterType::OT_Wait || v->objtype == LetterType::OT_Error || v->objtype == LetterType::OT_NG) return v;
		if (v->objtype == LetterType::OT_Number || v->objtype == LetterType::OT_StrObj ||
			v->objtype == LetterType::OT_BoolVal) {
			(*n)++;
			return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
		}
	}
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}

// ============================================================
// GenericFunction::PrimaryA
// ============================================================
Obj* PrimaryA_GenericFunction2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	GenericFunction* gf = (GenericFunction*)self;
	if (val2->objtype == LetterType::OT_Block) {
		Obj* val = vt_GetterA(thgc, val2, local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		// Create Function from GenericFunction with type arguments
		Function* func = (Function*)GC_alloc(thgc, CType::_Function);
		func->objtype = LetterType::OT_Function;
		func->vt = NULL;
		func->rettype = gf->rettype;
		func->blocks = gf->blocks;
		func->draw = gf->draw;
		return vt_PrimaryA(thgc, (Obj*)func, n, local, primary, val2);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Model::exepA / PrimaryA
// ============================================================
Obj* exepA_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryA_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	// model [block].method()
	if (val2->objtype == LetterType::OT_Block) {
		val2 = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
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
					return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
				}
				return ErrorA(thgc, LetterType::OT_NG, val2->letter);
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
	}
	// model.Store / model.await
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Bracket) {
				Obj* val = vt_exeA(thgc, val2, local);
				if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
				(*n)++;
				return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
			}
			if (val2->objtype == LetterType::OT_CallBlock) {
				_exeCallBlockPairA(thgc, val2, self, local);
				(*n)++;
				return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
			}
			if (val2->objtype == LetterType::OT_Dot) {
				// .Store.await pattern
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				if (val2->objtype == LetterType::OT_Word) {
					(*n)++;
					val2 = *(Obj**)get_list(primary->children, *n);
				}
				if (val2->objtype == LetterType::OT_Bracket) {
					vt_exeA(thgc, val2, local);
					(*n)++;
					return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
				}
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	// model word {|body} → ModelObj instance
	if (val2->objtype == LetterType::OT_Word) {
		Obj* word = val2;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		Obj* gj = getA(thgc, local, word->letter->text);
		ModelObj* mobj = (ModelObj*)gj;
		mobj->letter = word->letter;
		// Right arrow → rename
		if (val2->objtype == LetterType::OT_Right) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				gj->rename = val2->letter->text;
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
			} else {
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			}
		} else {
			gj->rename = word->letter->text;
		}
		declareRA(thgc, local, gj->rename, gj);
		if (val2->objtype == LetterType::OT_CallBlock) {
			mobj->draw = (Block*)val2;
			_exeCallBlockPairA(thgc, val2, gj, local);
			(*n)++;
			return gj;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	// model {|body} (anonymous)
	if (val2->objtype == LetterType::OT_CallBlock) {
		_exeCallBlockPairA(thgc, val2, self, local);
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, NULL);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ModelVal::PrimaryA
// ============================================================
Obj* PrimaryA_ModelVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
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
				return self;
			}
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Gene::exepA / PrimaryA - 遺伝子型宣言
// ============================================================
Obj* exepA_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryA_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		Obj* word = val2;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		Obj* gj = getA(thgc, local, word->letter->text);
		GeneObj* gobj = (GeneObj*)gj;
		gobj->letter = word->letter;
		// Right arrow → rename
		if (val2->objtype == LetterType::OT_Right) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				gj->rename = val2->letter->text;
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
			} else {
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			}
		} else {
			gj->rename = word->letter->text;
		}
		declareRA(thgc, local, gj->rename, gj);
		if (val2->objtype == LetterType::OT_CallBlock) {
			gobj->call = (Block*)val2;
			_exeCallBlockPairA(thgc, val2, gj, local);
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
		gene_head:
			if (val2->objtype == LetterType::OT_Left) {
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				if (val2->objtype == LetterType::OT_CallBlock) {
					_exeGeneExtBlockA(thgc, val2, gj, local);
					(*n)++;
					val2 = *(Obj**)get_list(primary->children, *n);
					goto gene_head;
				}
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			} else if (val2->objtype == LetterType::OT_Right) {
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				if (val2->objtype == LetterType::OT_CallBlock) {
					_exeGeneExtBlockA(thgc, val2, gj, local);
					(*n)++;
					val2 = *(Obj**)get_list(primary->children, *n);
					goto gene_head;
				}
				return ErrorA(thgc, LetterType::OT_Error, val2->letter);
			}
			return gj;
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	// Anonymous gene {|body}
	if (val2->objtype == LetterType::OT_CallBlock) {
		GeneObj* gj = (GeneObj*)GC_alloc(thgc, CType::_GeneObj);
		gj->objtype = LetterType::OT_Gene;
		gj->vt = NULL;
		gj->name = NULL;
		gj->call = (Block*)val2;
		gj->blocks = create_list(thgc, sizeof(Block*), CType::_List);
		gj->vmap = create_mapy(thgc, CType::_List);
		_exeCallBlockPairA(thgc, val2, (Obj*)gj, local);
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
	gene_head2:
		if (val2->objtype == LetterType::OT_Left) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_CallBlock) {
				_exeGeneExtBlockA(thgc, val2, (Obj*)gj, local);
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				goto gene_head2;
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		} else if (val2->objtype == LetterType::OT_Right) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_CallBlock) {
				_exeGeneExtBlockA(thgc, val2, (Obj*)gj, local);
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				goto gene_head2;
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return (Obj*)gj;
	}
	// gene.name → member access
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			if (local->gene != NULL) {
				Generic* gen = (Generic*)local->gene;
				char* found = get_mapy(gen->vmap, val2->letter->text);
				if (found != NULL) return (Obj*)found;
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneChild::PrimaryA
// ============================================================
Obj* PrimaryA_GeneChild(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		if (val2->children->size == 0) {
			(*n)++;
			return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneMutate::PrimaryA
// ============================================================
Obj* PrimaryA_GeneMutate(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		if (val2->children->size == 0) {
			(*n)++;
			return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneNew::PrimaryA
// ============================================================
Obj* PrimaryA_GeneNew(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		if (val2->children->size == 0) {
			(*n)++;
			return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneCross::PrimaryA
// ============================================================
Obj* PrimaryA_GeneCross(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		if (val2->children->size == 2) {
			Obj* val = vt_GetterA(thgc, vt_exeA(thgc, val2, local), local);
			if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
			(*n)++;
			return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneStore::PrimaryA
// ============================================================
Obj* PrimaryA_GeneStore(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		if (val2->children->size == 0) {
			return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneSelect::PrimaryA
// ============================================================
Obj* PrimaryA_GeneSelect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_exeA(thgc, val2, local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		if (val2->children->size <= 1) {
			(*n)++;
			return (Obj*)make_cblock(thgc, CType::_Block, LetterType::OT_Block);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneSort::PrimaryA
// ============================================================
Obj* PrimaryA_GeneSort(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_exeA(thgc, val2, local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		if (val2->children->size <= 1) {
			(*n)++;
			return (Obj*)make_cblock(thgc, CType::_Block, LetterType::OT_Block);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneLabel::exepA
// ============================================================
Obj* exepA_GeneLabel(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// GeneLabelValue::PrimaryA
// ============================================================
Obj* PrimaryA_GeneLabelValue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			if (str_matchA(val2->letter->text, "length", 6)) {
				return make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
			}
			return ErrorA(thgc, LetterType::OT_Error, val2->letter);
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// initVTables_A3 - obj2a3.h用VTable初期化
// ============================================================
void initVTables_A3() {
	// --- FloatVal ---
	vt_FloatVal = vt_Obj_default;
	vt_FloatVal.exepA = exepA_FloatVal;
	vt_FloatVal.PrimaryA = PrimaryA_FloatVal;
	vt_lookup[(int)LetterType::OT_FloatVal] = &vt_FloatVal;

	// --- StrObj ---
	vt_StrObj = vt_Obj_default;
	vt_StrObj.PrimaryA = PrimaryA_StrObj;
	vt_lookup[(int)LetterType::OT_StrObj] = &vt_StrObj;

	// --- BoolVal ---
	vt_BoolVal = vt_Obj_default;
	vt_BoolVal.PrimaryA = PrimaryA_BoolVal;
	vt_lookup[(int)LetterType::OT_BoolVal] = &vt_BoolVal;

	// --- Var ---
	vt_Var = vt_Obj_default;
	vt_Var.exepA = exepA_Var;
	vt_lookup[(int)LetterType::OT_Var] = &vt_Var;

	// --- Class ---
	vt_Class = vt_Obj_default;
	vt_Class.exepA = exepA_Class;
	vt_Class.PrimaryA = PrimaryA_Class;
	vt_lookup[(int)LetterType::OT_Class] = &vt_Class;

	// --- Generic ---
	vt_Generic = vt_Obj_default;
	vt_Generic.PrimaryA = PrimaryA_Generic;
	vt_lookup[(int)LetterType::OT_Generic] = &vt_Generic;

	// --- If ---
	vt_If = vt_Obj_default;
	vt_If.exepA = exepA_If;
	vt_If.PrimaryA = PrimaryA_If;
	vt_lookup[(int)LetterType::OT_If] = &vt_If;

	// --- While ---
	vt_While = vt_Obj_default;
	vt_While.exepA = exepA_While;
	vt_While.PrimaryA = PrimaryA_While;
	vt_lookup[(int)LetterType::OT_While] = &vt_While;

	// --- For ---
	vt_For = vt_Obj_default;
	vt_For.exepA = exepA_For;
	vt_For.PrimaryA = PrimaryA_For;
	vt_lookup[(int)LetterType::OT_For] = &vt_For;

	// --- Switch ---
	vt_Switch = vt_Obj_default;
	vt_Switch.exepA = exepA_Switch;
	vt_Switch.PrimaryA = PrimaryA_Switch;
	vt_lookup[(int)LetterType::OT_Switch] = &vt_Switch;

	// --- Continue ---
	vt_Continue = vt_Obj_default;
	vt_Continue.exepA = exepA_Continue;
	vt_lookup[(int)LetterType::OT_Continue] = &vt_Continue;

	// --- Return ---
	vt_Return = vt_Obj_default;
	vt_Return.exepA = exepA_Return;
	vt_Return.PrimaryA = PrimaryA_Return;
	vt_lookup[(int)LetterType::OT_Return] = &vt_Return;

	// --- Goto ---
	vt_Goto = vt_Obj_default;
	vt_Goto.exepA = exepA_Goto;
	vt_Goto.PrimaryA = PrimaryA_Goto;
	vt_lookup[(int)LetterType::OT_Goto] = &vt_Goto;

	// --- Print ---
	vt_Print = vt_Obj_default;
	vt_Print.exepA = exepA_Print;
	vt_Print.PrimaryA = PrimaryA_Print;
	vt_lookup[(int)LetterType::OT_Print] = &vt_Print;

	// --- GeneLabel ---
	vt_GeneLabel = vt_Obj_default;
	vt_GeneLabel.exepA = exepA_GeneLabel;
	vt_lookup[(int)LetterType::OT_GeneLabel] = &vt_GeneLabel;

	// --- Model ---
	vt_Model = vt_Obj_default;
	vt_Model.exepA = exepA_Model;
	vt_Model.PrimaryA = PrimaryA_Model;
	vt_lookup[(int)LetterType::OT_Model] = &vt_Model;

	// --- Gene ---
	vt_Gene = vt_Obj_default;
	vt_Gene.exepA = exepA_Gene;
	vt_Gene.PrimaryA = PrimaryA_Gene;
	vt_lookup[(int)LetterType::OT_Gene] = &vt_Gene;

	// --- GenericFunction ---
	vt_GenericFunction = vt_Obj_default;
	vt_GenericFunction.PrimaryA = PrimaryA_GenericFunction2;
	vt_lookup[(int)LetterType::OT_GenericFunction] = &vt_GenericFunction;

	// --- SqlString (no OT_, set explicitly at creation) ---
	vt_SqlString = vt_Obj_default;
	vt_SqlString.opeA = opeA_SqlString;
	vt_SqlString.PrimaryA = PrimaryA_SqlString;

	// --- Constructor (no OT_, set explicitly at creation) ---
	vt_Constructor = vt_Obj_default;
	vt_Constructor.PrimaryA = PrimaryA_Constructor;

	// --- ArrayConstructor (no OT_, set explicitly at creation) ---
	vt_ArrayConstructor = vt_Obj_default;
	vt_ArrayConstructor.PrimaryA = PrimaryA_ArrayConstructor;

	// --- Base (no OT_, set explicitly at creation) ---
	vt_Base = vt_Obj_default;
	vt_Base.PrimaryA = PrimaryA_Base;

	// --- GeneChild (no OT_, set explicitly at creation) ---
	vt_GeneChild = vt_Obj_default;
	vt_GeneChild.PrimaryA = PrimaryA_GeneChild;

	// --- GeneMutate (no OT_, set explicitly at creation) ---
	vt_GeneMutate = vt_Obj_default;
	vt_GeneMutate.PrimaryA = PrimaryA_GeneMutate;

	// --- GeneNew (no OT_, set explicitly at creation) ---
	vt_GeneNew = vt_Obj_default;
	vt_GeneNew.PrimaryA = PrimaryA_GeneNew;

	// --- GeneCross (no OT_, set explicitly at creation) ---
	vt_GeneCross = vt_Obj_default;
	vt_GeneCross.PrimaryA = PrimaryA_GeneCross;

	// --- GeneStore (no OT_, set explicitly at creation) ---
	vt_GeneStore = vt_Obj_default;
	vt_GeneStore.PrimaryA = PrimaryA_GeneStore;

	// --- GeneSelect (no OT_, set explicitly at creation) ---
	vt_GeneSelect = vt_Obj_default;
	vt_GeneSelect.PrimaryA = PrimaryA_GeneSelect;

	// --- GeneSort (no OT_, set explicitly at creation) ---
	vt_GeneSort = vt_Obj_default;
	vt_GeneSort.PrimaryA = PrimaryA_GeneSort;

	// --- GeneLabelValue (no OT_, set explicitly at creation) ---
	vt_GeneLabelValue = vt_Obj_default;
	vt_GeneLabelValue.PrimaryA = PrimaryA_GeneLabelValue;

	// --- ModelVal (no OT_, set explicitly at creation) ---
	vt_ModelVal = vt_Obj_default;
	vt_ModelVal.PrimaryA = PrimaryA_ModelVal;

	// --- VoiVal (no OT_, set explicitly at creation) ---
	vt_VoiVal = vt_Obj_default;
	vt_VoiVal.exepA = exepA_VoiVal;
	vt_VoiVal.PrimaryA = PrimaryA_VoiVal;

	// --- Null (no OT_, set explicitly at creation) ---
	vt_Null = vt_Obj_default;
	vt_Null.PrimaryA = PrimaryA_Null;
}
