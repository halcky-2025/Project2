// ============================================================
// obj2b2.h - B解析パス (vtable dispatch版) - Block 2
// Word, Value, Class, Type, Var, Generic, Number, FloatVal,
// SqlString, StrObj, BoolVal, VoiVal, Null, ArrayConstructor,
// Constructor, Base, Variable, Function, If, While, For, Switch,
// Break, Continue, Return, Goto, Print, Iterator,
// GenericFunction, Model, ModelObj, ModelVal, Gene, GeneObj,
// GeneChild, GeneMutate, GeneNew, GeneCross, GeneVal,
// GeneStore, GeneSelect, GeneSort, GeneLabel, GeneLabelValue,
// NewBlock, CrossBlock, MigrateBlock
// ============================================================

// 前方宣言
Obj* SelfB_Word(ThreadGC* thgc, Obj* self, Master* local);
Obj* GetterB_Word(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_Word(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Word(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_Value(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_Type(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Var(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Generic(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_Number(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_Number(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Number(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_FloatVal(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_SqlString(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_SqlString(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_SqlString(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* opeB_SqlString(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2);
Obj* exeB_StrObj(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_BoolVal(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_BoolVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_BoolVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_Null(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepB_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_ArrayConstructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_Constructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_Base(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* GetterB_Variable(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryB_Variable(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_Function(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* GetterB_Iterator(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryB_GenericFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_ModelObj(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryB_ModelVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_GeneChild(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_GeneMutate(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_GeneNew(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_GeneCross(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_GeneVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_GeneStore(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_GeneSelect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryB_GeneSort(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepB_GeneLabel(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryB_GeneLabelValue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeB_NewBlock(ThreadGC* thgc, Obj* self, Master* local);
Obj* exeB_CrossBlock(ThreadGC* thgc, Obj* self, Master* local);
Obj* exeB_MigrateBlock(ThreadGC* thgc, Obj* self, Master* local);
Obj* exeB_GeneObj(ThreadGC* thgc, Obj* self, Master* local);

// ============================================================
// Word::SelfB
// ============================================================
Obj* SelfB_Word(ThreadGC* thgc, Obj* self, Master* local) {
	Obj* obj = getB(thgc, local, self->letter->text, self->letter);
	// clone + set letter
	return obj;
}

// ============================================================
// Word::GetterB
// ============================================================
Obj* GetterB_Word(ThreadGC* thgc, Obj* self, Master* local) {
	Obj* obj = getB(thgc, local, self->letter->text, self->letter);
	obj = vt_GetterB(thgc, obj, local);
	// clone + set letter
	return obj;
}

// ============================================================
// Word::exepB
// ============================================================
Obj* exepB_Word(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	// kouho check: if (letter == local.letter && local.kouhos == null) local.calls.Last()()
	return self;
}

// ============================================================
// Word::PrimaryB
// ============================================================
Obj* PrimaryB_Word(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	(*n)++;
	return self;
}

// ============================================================
// Value::PrimaryB
// ============================================================
Obj* PrimaryB_Value(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			// vmap lookup
			return self; // simplified
		}
	} else if (val2->objtype == LetterType::OT_Bracket) {
		// FuncType check and call
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		(*n)++;
		return self; // simplified: return Value.New(functype.cls, local, letter)
	} else if (val2->objtype == LetterType::OT_Block) {
		(*n)++;
		// array access
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		return self; // simplified
	}
	// fallback
	(*n)++;
	return self;
}

// ============================================================
// Class::exepB
// ============================================================
Obj* exepB_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	// kouho check
	return self;
}

// ============================================================
// Class::PrimaryB
// ============================================================
Obj* PrimaryB_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// Generic/block handling, extends chain, CallBlock lookup
		if (ch->objtype == LetterType::OT_CallBlock) {
			(*n)++;
			// return local.getB2(val2 as CallBlock)
			return self;
		}
	}
	// Left extends chain
	if (val2->objtype == LetterType::OT_Left) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// extends handling
		(*n)++;
		return self;
	}
	if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Type::PrimaryB
// ============================================================
Obj* PrimaryB_Type(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	// type = this
	// head: array type [], func type (), etc.
	if (val2->objtype == LetterType::OT_Block) {
		Block* blk = (Block*)val2;
		if (blk->children->size == 0) {
			// ArrType creation
			(*n)++;
			Obj* ch = *(Obj**)get_list(primary->children, *n);
			// recursive goto head
		}
		return ErrorA(thgc, LetterType::OT_Error, val2->letter);
	}
	// Dot -> .new, .field
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			// new / SqlString / etc.
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	// Word -> variable declaration
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// Variable creation + declareB
		if (ch->objtype == LetterType::OT_CallBlock) {
			// function declaration
			(*n)++;
			return self;
		}
		// variable declaration
		return self;
	}
	// Bracket -> multiple variable declaration
	if (val2->objtype == LetterType::OT_Bracket) {
		Block* blk = (Block*)vt_exeB(thgc, val2, local);
		(*n)++;
		return (Obj*)blk;
	}
	// CallBlock -> function
	if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		return self;
	}
	return self;
}

// ============================================================
// Var::exepB
// ============================================================
Obj* exepB_Var(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	// kouho check
	return self;
}

// ============================================================
// Generic::PrimaryB
// ============================================================
Obj* PrimaryB_Generic(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Block) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		// GenericObj creation
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Number::exeB, exepB, PrimaryB
// ============================================================
Obj* exeB_Number(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* exepB_Number(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_Number(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			// random etc.
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// FloatVal::exeB, exepB, PrimaryB
// ============================================================
Obj* exeB_FloatVal(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* exepB_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// SqlString::exeB, exepB, PrimaryB, opeB
// ============================================================
Obj* exeB_SqlString(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* exepB_SqlString(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_SqlString(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}
Obj* opeB_SqlString(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) {
	// arithmetic/comparison/logical ops handling
	if (str_matchA(key, "+", 1) || str_matchA(key, "-", 1) || str_matchA(key, "*", 1) ||
		str_matchA(key, "/", 1) || str_matchA(key, "=", 1)) {
		// TypeCheck: cls must be Int/Float/Short
		return self;
	}
	if (str_matchA(key, "==", 2) || str_matchA(key, "!=", 2) || str_matchA(key, ">", 1) ||
		str_matchA(key, "<", 1) || str_matchA(key, ">=", 2) || str_matchA(key, "<=", 2)) {
		// return new SqlString { cls = local.Bool }
		return self;
	}
	if (str_matchA(key, "and", 3) || str_matchA(key, "or", 2)) {
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}

// ============================================================
// StrObj::exeB, exepB, PrimaryB
// ============================================================
Obj* exeB_StrObj(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* exepB_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	// return Value.New(local.Str, local, letter)
	return self;
}
Obj* PrimaryB_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// BoolVal::exeB, exepB, PrimaryB
// ============================================================
Obj* exeB_BoolVal(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* exepB_BoolVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_BoolVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// VoiVal::exepB, PrimaryB
// ============================================================
Obj* exepB_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return ErrorA(thgc, LetterType::OT_Error, self->letter);
}
Obj* PrimaryB_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Null::exeB, exepB, PrimaryB
// ============================================================
Obj* exeB_Null(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* exepB_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ArrayConstructor::PrimaryB
// ============================================================
Obj* PrimaryB_ArrayConstructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		(*n)++;
		// return Block(Array) { rets = { Value.New(cls) } }
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Constructor::PrimaryB
// ============================================================
Obj* PrimaryB_Constructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		(*n)++;
		// Value.New(cls, local, letter)
		return self;
	}
	// fallback: consume remaining Primary items
	Obj* val0 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val0);
		if (val0 != NULL && IS_WEG(val0)) return val0;
	}
	(*n)++;
	return self;
}

// ============================================================
// Base::PrimaryB
// ============================================================
Obj* PrimaryB_Base(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	int m = 0;
	if (val2->objtype == LetterType::OT_Block) {
		// m = block[0] as Number
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
	}
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		// check constructor args
		return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
	}
	// fallback
	val2 = vt_GetterB(thgc, val2, local);
	if (IS_WEG(val2)) return val2;
	(*n)++;
	return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
}

// ============================================================
// Variable::GetterB, PrimaryB
// ============================================================
Obj* GetterB_Variable(ThreadGC* thgc, Obj* self, Master* local) {
	// return Value.New(cls, local, letter).GetterB(local)
	return self; // simplified
}
Obj* PrimaryB_Variable(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	// return Value.New(cls, local, letter).PrimaryB(...)
	return PrimaryB_Value(thgc, self, n, local, primary, val2);
}

// ============================================================
// Function::PrimaryB
// ============================================================
Obj* PrimaryB_Function(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		// TypeCheck args, return Value.New(ret, local, letter)
		(*n)++;
		return self;
	}
	// fallback: consume remaining Primary
	Obj* val0 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val0);
		if (val0 != NULL && IS_WEG(val0)) return val0;
	}
	if (val0 != NULL) {
		val0 = vt_GetterB(thgc, val0, local);
		if (IS_WEG(val0)) return val0;
	}
	(*n)++;
	return self;
}

// ============================================================
// If::exepB, PrimaryB
// ============================================================
Obj* exepB_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}
Obj* PrimaryB_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// elif/else chain
		while (ch->objtype == LetterType::OT_Elif) {
			(*n)++;
			ch = *(Obj**)get_list(primary->children, *n);
			if (ch->objtype == LetterType::OT_CallBlock) {
				(*n)++;
				ch = *(Obj**)get_list(primary->children, *n);
			} else return ErrorA(thgc, LetterType::OT_Error, ch->letter);
		}
		if (ch->objtype == LetterType::OT_Else) {
			(*n)++;
			ch = *(Obj**)get_list(primary->children, *n);
			if (ch->objtype == LetterType::OT_CallBlock) {
				(*n)++;
				// return Value.New(ret, local, val.letter)
				return self;
			}
			return ErrorA(thgc, LetterType::OT_Error, ch->letter);
		}
		// no else: return Value.New(ret, local, letter)
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// While::exepB, PrimaryB
// ============================================================
Obj* exepB_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// For::exepB, PrimaryB
// ============================================================
Obj* exepB_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Switch::exepB, PrimaryB
// ============================================================
Obj* exepB_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Break::exepB, PrimaryB
// ============================================================
Obj* exepB_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val2);
		if (val2 != NULL && IS_WEG(val2)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterB(thgc, val2, local);
	// search blocks for While/For/Switch and add to returns
	(*n)--;
	return (val2 != NULL) ? val2 : self;
}
Obj* PrimaryB_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	val2 = NULL;
	(*n)++;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val2);
		if (val2 != NULL && IS_WEG(val2)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterB(thgc, val2, local);
	return (val2 != NULL) ? val2 : self;
}

// ============================================================
// Continue::exepB, PrimaryB
// ============================================================
Obj* exepB_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val2);
		if (val2 != NULL && IS_WEG(val2)) return val2;
	}
	(*n)--;
	return self;
}
Obj* PrimaryB_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	val2 = NULL;
	(*n)++;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val2);
		if (val2 != NULL && IS_WEG(val2)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterB(thgc, val2, local);
	return (val2 != NULL) ? val2 : self;
}

// ============================================================
// Return::exepB, PrimaryB
// ============================================================
Obj* exepB_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val2);
		if (val2 != NULL && IS_WEG(val2)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterB(thgc, val2, local);
	// search blocks for Function and add to returns
	(*n)--;
	return (val2 != NULL) ? val2 : self;
}
Obj* PrimaryB_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	val2 = NULL;
	(*n)++;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val2);
		if (val2 != NULL && IS_WEG(val2)) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterB(thgc, val2, local);
	return (val2 != NULL) ? val2 : self;
}

// ============================================================
// Goto::exepB, PrimaryB
// ============================================================
Obj* exepB_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	// label name extraction
	(*n)++;
	return self;
}

// ============================================================
// Print::exepB, PrimaryB
// ============================================================
Obj* exepB_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket || val2->objtype == LetterType::OT_Block) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
	}
	// fallback: consume remaining
	Obj* val0 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextB_Primary(thgc, primary, local, n, &val0);
		if (val0 != NULL && IS_WEG(val0)) return val0;
	}
	if (val0 != NULL) {
		val0 = vt_GetterB(thgc, val0, local);
		if (IS_WEG(val0)) return val0;
	}
	(*n)++;
	return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
}

// ============================================================
// Iterator::GetterB
// ============================================================
Obj* GetterB_Iterator(ThreadGC* thgc, Obj* self, Master* local) {
	// return value.rets[n] or value.rets[n].rets[m]
	return self; // simplified
}

// ============================================================
// GenericFunction::PrimaryB
// ============================================================
Obj* PrimaryB_GenericFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Block) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// return new Function(ret) { blocks, draw, bracket, block = blk }
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Model::exepB, PrimaryB
// ============================================================
Obj* exepB_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Block) {
		// model [ModelObj] or model.Where/Select/Sort
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// Dot chain: Where, FirstAwait, Select, Sort
		if (ch->objtype == LetterType::OT_Dot) {
			(*n)++;
			Obj* ch2 = *(Obj**)get_list(primary->children, *n);
			(*n)++;
			return self;
		}
		return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
	} else if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			Obj* ch2 = *(Obj**)get_list(primary->children, *n);
			// Store, await handling
			return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	} else if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// model name { ... }
		if (ch->objtype == LetterType::OT_CallBlock) {
			(*n)++;
			return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	} else if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::_None, self->letter); // VoiVal
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ModelObj::exeB
// ============================================================
Obj* exeB_ModelObj(ThreadGC* thgc, Obj* self, Master* local) { return self; }

// ============================================================
// ModelVal::PrimaryB
// ============================================================
Obj* PrimaryB_ModelVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			// update / vmap lookup
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Gene::exepB, PrimaryB
// ============================================================
Obj* exepB_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_Gene(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// gene name declaration
		if (ch->objtype == LetterType::OT_Right) {
			(*n)++;
			Obj* ch2 = *(Obj**)get_list(primary->children, *n);
			// table, version, CallBlock handling
			(*n)++;
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	} else if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		// anonymous gene with Left/Right extensions
		return self;
	} else if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			return self;
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneChild, GeneMutate, GeneNew, GeneCross, GeneVal,
// GeneStore, GeneSelect, GeneSort PrimaryB
// ============================================================
Obj* PrimaryB_GeneChild(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket && val2->children->size == 0) { (*n)++; return self; }
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}
Obj* PrimaryB_GeneMutate(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket && val2->children->size == 0) { (*n)++; return self; }
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}
Obj* PrimaryB_GeneNew(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket && val2->children->size == 0) { (*n)++; return self; }
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}
Obj* PrimaryB_GeneCross(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket && val2->children->size == 2) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}
Obj* PrimaryB_GeneVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) { (*n)++; return self; }
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}
Obj* PrimaryB_GeneStore(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket && val2->children->size == 0) { (*n)++; return self; }
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}
Obj* PrimaryB_GeneSelect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}
Obj* PrimaryB_GeneSort(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		if (IS_WEG(val)) return val;
		(*n)++;
		return self;
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// GeneLabel::exepB, GeneLabelValue::PrimaryB
// ============================================================
Obj* exepB_GeneLabel(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* PrimaryB_GeneLabelValue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		Obj* ch = *(Obj**)get_list(primary->children, *n);
		if (ch->objtype == LetterType::OT_Word) {
			(*n)++;
			return self; // length etc.
		}
		return ErrorA(thgc, LetterType::OT_Error, ch->letter);
	}
	return ErrorA(thgc, LetterType::OT_NG, val2->letter);
}

// ============================================================
// NewBlock, CrossBlock, MigrateBlock, GeneObj exeB
// ============================================================
Obj* exeB_NewBlock(ThreadGC* thgc, Obj* self, Master* local) {
	// rets.Add(new Number(0))
	return self;
}
Obj* exeB_CrossBlock(ThreadGC* thgc, Obj* self, Master* local) {
	// return Value.New(gj, local, letter)
	return self;
}
Obj* exeB_MigrateBlock(ThreadGC* thgc, Obj* self, Master* local) {
	// rets.Add(new FloatVal(0.1f) { cls = local.Float })
	return self;
}
Obj* exeB_GeneObj(ThreadGC* thgc, Obj* self, Master* local) { return self; }

// ============================================================
// initVTables_B2 - B-pass vtable初期化 (Block 2)
// ============================================================
void initVTables_B2() {
	// --- Word ---
	vt_Word.SelfB = SelfB_Word;
	vt_Word.GetterB = GetterB_Word;
	vt_Word.exepB = exepB_Word;
	vt_Word.PrimaryB = PrimaryB_Word;

	// --- Number ---
	vt_Number.exeB = exeB_Number;
	vt_Number.exepB = exepB_Number;
	vt_Number.PrimaryB = PrimaryB_Number;

	// --- FloatVal ---
	vt_FloatVal.exeB = exeB_FloatVal;
	vt_FloatVal.exepB = exepB_FloatVal;
	vt_FloatVal.PrimaryB = PrimaryB_FloatVal;

	// --- SqlString ---
	vt_SqlString.exeB = exeB_SqlString;
	vt_SqlString.exepB = exepB_SqlString;
	vt_SqlString.PrimaryB = PrimaryB_SqlString;
	vt_SqlString.opeB = opeB_SqlString;

	// --- StrObj ---
	vt_StrObj.exeB = exeB_StrObj;
	vt_StrObj.exepB = exepB_StrObj;
	vt_StrObj.PrimaryB = PrimaryB_StrObj;

	// --- BoolVal ---
	vt_BoolVal.exeB = exeB_BoolVal;
	vt_BoolVal.exepB = exepB_BoolVal;
	vt_BoolVal.PrimaryB = PrimaryB_BoolVal;

	// --- VoiVal ---
	vt_VoiVal.exepB = exepB_VoiVal;
	vt_VoiVal.PrimaryB = PrimaryB_VoiVal;

	// --- Null ---
	vt_Null.exeB = exeB_Null;
	vt_Null.exepB = exepB_Null;
	vt_Null.PrimaryB = PrimaryB_Null;

	// --- Class ---
	vt_Class.exepB = exepB_Class;
	vt_Class.PrimaryB = PrimaryB_Class;

	// --- Type (ClassObj acts as Type) ---
	vt_ClassObj.PrimaryB = PrimaryB_Type;

	// --- Var ---
	vt_Var.exepB = exepB_Var;

	// --- Generic ---
	vt_Generic.PrimaryB = PrimaryB_Generic;

	// --- Variable ---
	vt_Variable.GetterB = GetterB_Variable;
	vt_Variable.PrimaryB = PrimaryB_Variable;

	// --- Function ---
	vt_Function.PrimaryB = PrimaryB_Function;

	// --- GenericFunction ---
	vt_GenericFunction.PrimaryB = PrimaryB_GenericFunction;

	// --- ArrayConstructor ---
	vt_ArrayConstructor.PrimaryB = PrimaryB_ArrayConstructor;

	// --- Constructor ---
	vt_Constructor.PrimaryB = PrimaryB_Constructor;

	// --- Base ---
	vt_Base.PrimaryB = PrimaryB_Base;

	// --- If ---
	vt_If.exepB = exepB_If;
	vt_If.PrimaryB = PrimaryB_If;

	// --- While ---
	vt_While.exepB = exepB_While;
	vt_While.PrimaryB = PrimaryB_While;

	// --- For ---
	vt_For.exepB = exepB_For;
	vt_For.PrimaryB = PrimaryB_For;

	// --- Switch ---
	vt_Switch.exepB = exepB_Switch;
	vt_Switch.PrimaryB = PrimaryB_Switch;

	// --- Break ---
	vt_Break.exepB = exepB_Break;
	vt_Break.PrimaryB = PrimaryB_Break;

	// --- Continue ---
	vt_Continue.exepB = exepB_Continue;
	vt_Continue.PrimaryB = PrimaryB_Continue;

	// --- Return ---
	vt_Return.exepB = exepB_Return;
	vt_Return.PrimaryB = PrimaryB_Return;

	// --- Goto ---
	vt_Goto.exepB = exepB_Goto;
	vt_Goto.PrimaryB = PrimaryB_Goto;

	// --- Print ---
	vt_Print.exepB = exepB_Print;
	vt_Print.PrimaryB = PrimaryB_Print;

	// --- Model ---
	vt_Model.exepB = exepB_Model;
	vt_Model.PrimaryB = PrimaryB_Model;

	// --- ModelObj ---
	vt_ModelObj.exeB = exeB_ModelObj;

	// --- ModelVal ---
	vt_ModelVal.PrimaryB = PrimaryB_ModelVal;

	// --- Gene ---
	vt_Gene.exepB = exepB_Gene;
	vt_Gene.PrimaryB = PrimaryB_Gene;

	// --- GeneObj ---
	vt_GeneObj.exeB = exeB_GeneObj;

	// --- GeneChild ---
	vt_GeneChild.PrimaryB = PrimaryB_GeneChild;

	// --- GeneMutate ---
	vt_GeneMutate.PrimaryB = PrimaryB_GeneMutate;

	// --- GeneNew ---
	vt_GeneNew.PrimaryB = PrimaryB_GeneNew;

	// --- GeneCross ---
	vt_GeneCross.PrimaryB = PrimaryB_GeneCross;

	// --- GeneVal ---
	vt_GeneVal.PrimaryB = PrimaryB_GeneVal;

	// --- GeneStore ---
	vt_GeneStore.PrimaryB = PrimaryB_GeneStore;

	// --- GeneSelect ---
	vt_GeneSelect.PrimaryB = PrimaryB_GeneSelect;

	// --- GeneSort ---
	vt_GeneSort.PrimaryB = PrimaryB_GeneSort;

	// --- GeneLabel ---
	vt_GeneLabel.exepB = exepB_GeneLabel;

	// --- GeneLabelValue ---
	vt_GeneLabelValue.PrimaryB = PrimaryB_GeneLabelValue;

	// --- Iterator ---
	vt_Iterator.GetterB = GetterB_Iterator;

	// --- NewBlock ---
	vt_NewBlock.exeB = exeB_NewBlock;

	// --- CrossBlock ---
	vt_CrossBlock.exeB = exeB_CrossBlock;

	// --- MigrateBlock ---
	vt_MigrateBlock.exeB = exeB_MigrateBlock;
}
