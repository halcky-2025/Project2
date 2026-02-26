// ============================================================
// obj2c_part2.h - C conversion of Obj2C.cs lines 2000-5000
// Converted from C# partial classes to C free functions
// ============================================================

// Forward declarations for this file
Obj* PrimaryC_ClassObj_tail(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryC_Base(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* GetterC_Variable(ThreadGC* thgc, Obj* self, Master* local);
Obj* SelfC_Variable(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryC_Variable(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* EqualC_Variable(ThreadGC* thgc, Variable* self, String* op, Master* local, Obj* val2);
Obj* InC_Variable(ThreadGC* thgc, Variable* self, String* op, Master* local, Obj* val2);
Obj* GetterC_Function(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryC_Function(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
void Dec_Function(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryC_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);

// ============================================================
// Helper: Bitcast.Cast equivalent
// ============================================================
static inline LVari* BitcastCast(ThreadGC* thgc, Master* local, Obj* targetCls, Obj* srcCls, LVari* srcVari) {
	if (targetCls == srcCls) return srcVari;
	LFunc* func = llvm_get_func(local->llvm);
	LVari* v = make_vn(thgc, "i8*");
	add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, srcVari));
	return v;
}

// ============================================================
// Helper: make Value object
// ============================================================
static inline Obj* make_value(ThreadGC* thgc, Obj* cls, LVari* vari) {
	Obj* val = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, NULL);
	val->cls = cls;
	val->vari_c = vari;
	return val;
}

// ============================================================
// Helper: make VoiVal object
// ============================================================
static inline Obj* make_voival(ThreadGC* thgc) {
	return make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
}

// ============================================================
// Helper: make Iterator object
// ============================================================
static inline Obj* make_iterator(ThreadGC* thgc, int idx, Block* blk) {
	Obj* it = make_cobj(thgc, CType::_CObj, LetterType::OT_Iterator, NULL);
	it->value = (Obj*)blk;
	return it;
}

// ============================================================
// ClassObj::PrimaryC tail (lines 2000-2063)
// This handles the continuation after constructor argument matching
// ============================================================
Obj* PrimaryC_ClassObj_tail(ThreadGC* thgc, ClassObj* cls, int* n, Master* local, Primary* primary, Obj* val2, LVari* vari) {
	LFunc* func = llvm_get_func(local->llvm);

	// add local.comps to func.comps
	for (int i = 0; i < local->compsC->size; i++) {
		LComp* c = *(LComp**)get_list(local->compsC, i);
		add_list(thgc, func->comps, (char*)c);
	}
	local->compsC = create_list(thgc, sizeof(LComp*), CType::_List);

	LVari* go_v;
	LVari* rn4;
	LCall* go_call;
	LCall* init_call;

	if (cls->cls_c == local->Int) vari = make_vn(thgc, "i32");
	else if (cls->cls_c == local->Bool) vari = make_vn(thgc, "i2");
	else {
		char buf[128]; snprintf(buf, sizeof(buf), "%s*", cls->model ? cls->model->data : "i8");
		vari = make_vn(thgc, buf);
	}
	func->vari->type = createString(thgc, (char*)"%FuncType**", 11, 1);
	LVari* fv = make_vn(thgc, "%FuncType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, fv, func->vari, false));

	{
		char buf[128]; snprintf(buf, sizeof(buf), "%s*", cls->model ? cls->model->data : "i8");
		go_v = make_vn(thgc, buf);
	}
	rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	{
		char buf[128]; snprintf(buf, sizeof(buf), "%s*", cls->model ? cls->model->data : "i8");
		go_call = make_lcall(thgc, go_v, make_lvari_c(thgc, buf, cls->drawcall ? cls->drawcall->data : ""));
		lcall_add(thgc, go_call, rn4);
		lcall_add(thgc, go_call, fv);
	}

	// Get first variable from draw block
	Block* drawblk = (Block*)*(Obj**)get_list(((Obj*)cls)->draw->children, 0);
	if (drawblk->vmapA && drawblk->vmapA->kvs->size > 0) {
		KV* kv0 = *(KV**)get_list(drawblk->vmapA->kvs, 0);
		Variable* vv = (Variable*)kv0->value;
		if (val2 && val2->objtype == LetterType::OT_Value) {
			Obj* val2v = val2;
			if (vv->cls == val2v->cls) {
				lcall_add(thgc, go_call, val2->vari_c);
			} else {
				if (vv->cls == local->Int) {
					LVari* v = make_vn(thgc, "i32");
					add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
					lcall_add(thgc, go_call, v);
				} else if (vv->cls == local->Short) {
					LVari* v = make_vn(thgc, "i16");
					add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
					lcall_add(thgc, go_call, v);
				} else if (vv->cls == local->Bool) {
					LVari* v = make_vn(thgc, "i1");
					add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
					lcall_add(thgc, go_call, v);
				} else {
					lcall_add(thgc, go_call, val2->vari_c);
				}
			}
		}
	}
	lcall_add(thgc, go_call, val2->vari_c);
	add_list(thgc, func->comps, (char*)go_call);
	init_call = make_lcall(thgc, NULL, make_lvari_c(thgc, vari->type ? (char*)vari->type->data : "void", cls->call_c ? cls->call_c->data : ""));
	lcall_add(thgc, init_call, rn4);
	lcall_add(thgc, init_call, go_v);
	add_list(thgc, func->comps, (char*)init_call);
	return make_value(thgc, (Obj*)cls, vari);
}

// ============================================================
// Base::PrimaryC (lines 2064-2116)
// ============================================================
Obj* PrimaryC_Base(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	int m = 0;
	if (val2->objtype == LetterType::OT_Block) {
		Block* blk = (Block*)val2;
		if (blk->children->size == 1) {
			Obj* ch0 = *(Obj**)get_list(blk->children, 0);
			if (ch0->objtype == LetterType::OT_Number) {
				m = ch0->number_val;
			}
		}
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
	}
	ClassObj* cls = (ClassObj*)*(Obj**)get_list(self->extends_list, m);

	if (val2->objtype == LetterType::OT_Bracket) {
		add_list(thgc, local->calls, (char*)local->KouhoSet2);
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		local->calls->size--;
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		Block* blk = (Block*)val;
		Block* block1 = (Block*)*(Obj**)get_list(((Obj*)cls)->draw->children, 0);
		// Get varr from block1.vmapA.Values
		int varrCount = block1->vmapA->kvs->size;
		if (varrCount != blk->rets->size) return ErrorC(thgc, LetterType::OT_Error, val2->letter, "baseの引数があっていません");
		for (int i = 0; i < varrCount; i++) {
			KV* kv = *(KV**)get_list(block1->vmapA->kvs, i);
			Obj* varr_i = (Obj*)kv->value;
			Obj* ret_i = *(Obj**)get_list(blk->rets, i);
			// varr[i].opesC["="](...)
			vt_opeC(thgc, varr_i, createString(thgc, (char*)"=", 1, 1), local, ret_i);
		}
		return make_voival(thgc);
	}

	if (val2->letter == local->letter && local->kouhos == NULL) {
		local->KouhoSet2();
	}
	val2 = vt_GetterC(thgc, val2, local);
	if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;

	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj || val2->objtype == LetterType::OT_BoolVal || val2->objtype == LetterType::OT_Value) {
		Block* block1 = (Block*)*(Obj**)get_list(((Obj*)cls)->draw->children, 0);
		int varrCount = block1->vmapA->kvs->size;
		if (varrCount != 1) return ErrorC(thgc, LetterType::OT_Error, val2->letter, "base元のクラスの引数は1個ではありません");
		for (int i = 0; i < varrCount; i++) {
			KV* kv = *(KV**)get_list(block1->vmapA->kvs, i);
			Obj* varr_i = (Obj*)kv->value;
			vt_opeC(thgc, varr_i, createString(thgc, (char*)"=", 1, 1), local, val2);
		}
		(*n)++;
		return make_voival(thgc);
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "baseの引数が適切に設定されていません");
}

// ============================================================
// Variable::GetterC (lines 2127-2231)
// ============================================================
Obj* GetterC_Variable(ThreadGC* thgc, Obj* self, Master* local) {
	Variable* vself = (Variable*)self;
	LFunc* func = llvm_get_func(local->llvm);
	Obj* cls = vself->cls;

	// Check vari type "[n]"
	if (self->vari_c && self->vari_c->type && str_matchA(self->vari_c->type, "[n]", 3)) {
		self->vari_c->type = createString(thgc, (char*)"i8**", 4, 1);
		LVari* v = make_vn(thgc, "i8*");
		add_list(thgc, func->comps, (char*)make_lload(thgc, v, self->vari_c, false));
		if (cls->objtype == LetterType::OT_Var) cls = ((Var*)cls)->cls;
		if (cls == local->Int) {
			LVari* v2 = make_vn(thgc, "i32");
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_PtrToInt, v2, v));
			return make_value(thgc, cls, v2);
		} else if (cls == local->Bool) {
			LVari* v2 = make_vn(thgc, "i1");
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_PtrToInt, v2, v));
			return make_value(thgc, cls, v2);
		} else if (cls->objtype == LetterType::OT_FuncType) {
			FuncType* functype = (FuncType*)cls;
			Obj* fnobj = make_cobj(thgc, CType::_CObj, LetterType::OT_Function, NULL);
			fnobj->ret = functype->cls;
			fnobj->draws = (List*)functype->draws;
			fnobj->vari_c = v;
			return fnobj;
		} else if (cls->objtype == LetterType::OT_ArrType) {
			return make_value(thgc, cls, v);
		} else {
			char buf[128]; snprintf(buf, sizeof(buf), "%s*", cls->model ? cls->model->data : "i8");
			self->vari_c = make_vn(thgc, buf);
			return make_value(thgc, cls, v);
		}
	}
	// Check vari type "[s]"
	else if (self->vari_c && self->vari_c->type && str_matchA(self->vari_c->type, "[s]", 3)) {
		self->vari_c->type = createString(thgc, (char*)"%HashType*", 10, 1);
		LVari* varig = make_vn(thgc, "%GetHashType");
		add_list(thgc, func->comps, (char*)make_lload(thgc, varig, make_lvari_c(thgc, "%GetHashType*", "@GetHash"), false));
		char buf[128]; snprintf(buf, sizeof(buf), "%s*", cls->model ? cls->model->data : "i8");
		LVari* v = make_vn(thgc, buf);
		LCall* call = make_lcall(thgc, v, varig);
		lcall_add(thgc, call, self->vari_c);
		lcall_add(thgc, call, vself->strvari);
		add_list(thgc, func->comps, (char*)call);
		if (cls == local->Int) {
			LVari* v2 = make_vn(thgc, "i32");
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_PtrToInt, v2, v));
			return make_value(thgc, cls, v2);
		} else if (cls == local->Bool) {
			LVari* v2 = make_vn(thgc, "i1");
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_PtrToInt, v2, v));
			return make_value(thgc, cls, v2);
		} else if (cls->objtype == LetterType::OT_FuncType) {
			FuncType* functype = (FuncType*)cls;
			Obj* fnobj = make_cobj(thgc, CType::_CObj, LetterType::OT_Function, NULL);
			fnobj->ret = functype->cls;
			fnobj->draws = (List*)functype->draws;
			fnobj->vari_c = v;
			return fnobj;
		} else if (cls->objtype == LetterType::OT_ArrType) {
			return make_value(thgc, cls, v);
		} else {
			return make_value(thgc, cls, v);
		}
	}
	// Check vari type "s[n]"
	else if (self->vari_c && self->vari_c->type && str_matchA(self->vari_c->type, "s[n]", 4)) {
		self->vari_c->type = createString(thgc, (char*)"%StringType*", 12, 1);
		LVari* varig = make_vn(thgc, "%GetCharType");
		add_list(thgc, func->comps, (char*)make_lload(thgc, varig, make_lvari_c(thgc, "%GetCharType*", "@GetChar"), false));
		LVari* v = make_vn(thgc, "i16");
		LCall* call = make_lcall(thgc, v, varig);
		lcall_add(thgc, call, self->vari_c);
		lcall_add(thgc, call, vself->strvari);
		add_list(thgc, func->comps, (char*)call);
		return make_value(thgc, cls, v);
	}

	// Normal path: flush local.comps
	for (int i = 0; i < local->compsC->size; i++) {
		LComp* c = *(LComp**)get_list(local->compsC, i);
		add_list(thgc, func->comps, (char*)c);
	}
	local->compsC = create_list(thgc, sizeof(LComp*), CType::_List);

	if (cls->objtype == LetterType::OT_Var) cls = ((Var*)cls)->cls;
	LVari* vari = NULL;
	if (cls == local->Int) vari = make_vn(thgc, "i32");
	else if (cls == local->Short) vari = make_vn(thgc, "i16");
	else if (cls == local->Bool) vari = make_vn(thgc, "i1");
	else if (cls->objtype == LetterType::OT_FuncType) {
		FuncType* functype = (FuncType*)cls;
		vari = make_vn(thgc, "%FuncType*");
		add_list(thgc, func->comps, (char*)make_lload(thgc, vari, self->vari_c, false));
		Obj* fnobj = make_cobj(thgc, CType::_CObj, LetterType::OT_Function, NULL);
		fnobj->ret = functype->cls;
		fnobj->draws = (List*)functype->draws;
		fnobj->vari_c = vari;
		return fnobj;
	}
	else if (cls->objtype == LetterType::OT_ArrType) {
		vari = make_vn(thgc, "%HashType*");
		add_list(thgc, func->comps, (char*)make_lload(thgc, vari, self->vari_c, false));
		return make_value(thgc, cls, vari);
	}
	else {
		char buf[128]; snprintf(buf, sizeof(buf), "%s*", cls->model ? cls->model->data : "i8");
		vari = make_vn(thgc, buf);
	}
	add_list(thgc, func->comps, (char*)make_lload(thgc, vari, self->vari_c, false));
	return make_value(thgc, cls, vari);
}

// ============================================================
// Variable::SelfC (lines 2232-2247)
// ============================================================
Obj* SelfC_Variable(ThreadGC* thgc, Obj* self, Master* local) {
	Variable* vself = (Variable*)self;
	if (vself->clone_flag) return self;
	LFunc* func = llvm_get_func(local->llvm);
	for (int i = 0; i < local->compsC->size; i++) {
		LComp* c = *(LComp**)get_list(local->compsC, i);
		add_list(thgc, func->comps, (char*)c);
	}
	local->compsC = create_list(thgc, sizeof(LComp*), CType::_List);
	Obj* cls = vself->cls;
	if (cls->objtype == LetterType::OT_Var) cls = ((Var*)cls)->cls;
	if (cls == local->Int) func->vari->type = createString(thgc, (char*)"i32*", 4, 1);
	else if (cls == local->Short) func->vari->type = createString(thgc, (char*)"i16*", 4, 1);
	else if (cls == local->Bool) func->vari->type = createString(thgc, (char*)"i1*", 3, 1);
	else if (cls->objtype == LetterType::OT_FuncType) func->vari->type = createString(thgc, (char*)"%FuncType**", 11, 1);
	else if (cls->objtype == LetterType::OT_ArrType) func->vari->type = createString(thgc, (char*)"%HashType**", 11, 1);
	else {
		char buf[128]; snprintf(buf, sizeof(buf), "%s**", cls->model ? cls->model->data : "i8");
		func->vari->type = createString(thgc, buf, (int)strlen(buf), 1);
	}
	// Clone variable
	Obj* variable = make_cobj(thgc, CType::_CObj, LetterType::OT_Variable, self->letter);
	((Variable*)variable)->cls = vself->cls;
	((Variable*)variable)->clone_flag = vself->clone_flag;
	variable->vari_c = func->vari;
	return variable;
}

// ============================================================
// Variable::PrimaryC (lines 2248-2251)
// ============================================================
Obj* PrimaryC_Variable(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Variable* vself = (Variable*)self;
	// Value.New(cls, local, letter).PrimaryC(...)
	Obj* newval = make_value(thgc, vself->cls, NULL);
	newval->letter = self->letter;
	return vt_PrimaryC(thgc, newval, n, local, primary, val2);
}

// ============================================================
// Variable::EqualC (lines 2255-2346)
// ============================================================
Obj* EqualC_Variable(ThreadGC* thgc, Variable* self, String* op, Master* local, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Obj* cls = self->cls;

	// vari.type == "[n]"
	if (((Obj*)self)->vari_c && ((Obj*)self)->vari_c->type && str_matchA(((Obj*)self)->vari_c->type, "[n]", 3)) {
		((Obj*)self)->vari_c->type = createString(thgc, (char*)"i8**", 4, 1);
		if (cls == local->Int || cls == local->Short) {
			LVari* v2 = make_vn(thgc, "i8*");
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_IntToPtr, v2, val2->vari_c));
			add_list(thgc, func->comps, (char*)make_lstore(thgc, ((Obj*)self)->vari_c, v2));
			return make_value(thgc, cls, v2);
		} else if (cls == local->Bool) {
			LVari* v2 = make_vn(thgc, "i8*");
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_IntToPtr, v2, val2->vari_c));
			add_list(thgc, func->comps, (char*)make_lstore(thgc, ((Obj*)self)->vari_c, v2));
			return make_value(thgc, cls, v2);
		} else if (cls->objtype == LetterType::OT_FuncType) {
			add_list(thgc, func->comps, (char*)make_lstore(thgc, ((Obj*)self)->vari_c, val2->vari_c));
			FuncType* functype = (FuncType*)cls;
			Obj* fnobj = make_cobj(thgc, CType::_CObj, LetterType::OT_Function, NULL);
			fnobj->ret = functype->cls;
			fnobj->draws = (List*)functype->draws;
			fnobj->vari_c = ((Obj*)self)->vari_c;
			return fnobj;
		} else if (cls->objtype == LetterType::OT_ArrType) {
			add_list(thgc, func->comps, (char*)make_lstore(thgc, ((Obj*)self)->vari_c, val2->vari_c));
			return make_value(thgc, cls, ((Obj*)self)->vari_c);
		} else {
			add_list(thgc, func->comps, (char*)make_lstore(thgc, ((Obj*)self)->vari_c, val2->vari_c));
			return make_value(thgc, cls, ((Obj*)self)->vari_c);
		}
	}
	// vari.type == "[s]"
	else if (((Obj*)self)->vari_c && ((Obj*)self)->vari_c->type && str_matchA(((Obj*)self)->vari_c->type, "[s]", 3)) {
		((Obj*)self)->vari_c->type = createString(thgc, (char*)"%HashType*", 10, 1);
		LVari* varise = make_vn(thgc, "%SetHashType");
		add_list(thgc, func->comps, (char*)make_lload(thgc, varise, make_lvari_c(thgc, "%SetHashType*", "@SetHash"), false));
		if (cls == local->Int || cls == local->Short) {
			LVari* v2 = make_vn(thgc, "i8*");
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_IntToPtr, v2, val2->vari_c));
			LCall* sc = make_lcall(thgc, NULL, varise);
			lcall_add(thgc, sc, ((Obj*)self)->vari_c);
			lcall_add(thgc, sc, self->strvari);
			lcall_add(thgc, sc, v2);
			add_list(thgc, func->comps, (char*)sc);
			return make_value(thgc, cls, v2);
		} else if (cls == local->Bool) {
			LVari* v2 = make_vn(thgc, "i8*");
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_IntToPtr, v2, val2->vari_c));
			LCall* sc = make_lcall(thgc, NULL, varise);
			lcall_add(thgc, sc, ((Obj*)self)->vari_c);
			lcall_add(thgc, sc, self->strvari);
			lcall_add(thgc, sc, v2);
			add_list(thgc, func->comps, (char*)sc);
			return make_value(thgc, cls, v2);
		} else if (cls->objtype == LetterType::OT_FuncType) {
			add_list(thgc, func->comps, (char*)make_lstore(thgc, ((Obj*)self)->vari_c, val2->vari_c));
			FuncType* functype = (FuncType*)cls;
			Obj* fnobj = make_cobj(thgc, CType::_CObj, LetterType::OT_Function, NULL);
			fnobj->ret = functype->cls;
			fnobj->draws = (List*)functype->draws;
			fnobj->vari_c = ((Obj*)self)->vari_c;
			LCall* sc = make_lcall(thgc, NULL, varise);
			lcall_add(thgc, sc, ((Obj*)self)->vari_c);
			lcall_add(thgc, sc, self->strvari);
			lcall_add(thgc, sc, val2->vari_c);
			add_list(thgc, func->comps, (char*)sc);
			return fnobj;
		} else if (cls->objtype == LetterType::OT_ArrType) {
			LCall* sc = make_lcall(thgc, NULL, varise);
			lcall_add(thgc, sc, ((Obj*)self)->vari_c);
			lcall_add(thgc, sc, self->strvari);
			lcall_add(thgc, sc, val2->vari_c);
			add_list(thgc, func->comps, (char*)sc);
			return make_value(thgc, cls, ((Obj*)self)->vari_c);
		} else {
			LCall* sc = make_lcall(thgc, NULL, varise);
			lcall_add(thgc, sc, ((Obj*)self)->vari_c);
			lcall_add(thgc, sc, self->strvari);
			lcall_add(thgc, sc, val2->vari_c);
			add_list(thgc, func->comps, (char*)sc);
			return make_value(thgc, cls, ((Obj*)self)->vari_c);
		}
	}
	// vari.type == "s[n]"
	else if (((Obj*)self)->vari_c && ((Obj*)self)->vari_c->type && str_matchA(((Obj*)self)->vari_c->type, "s[n]", 4)) {
		return ErrorC(thgc, LetterType::OT_Error, ((Obj*)self)->letter, "文字列に代入はできません");
	}

	// Normal store path
	if (val2->objtype == LetterType::OT_Value) {
		LVari* va = BitcastCast(thgc, local, cls, val2->cls, val2->vari_c);
		add_list(thgc, func->comps, (char*)make_lstore(thgc, ((Obj*)self)->vari_c, va));
	} else {
		return ErrorC(thgc, LetterType::OT_Error, val2->letter, "変数への代入が不正です");
	}
	// Clone
	Obj* variable = make_cobj(thgc, CType::_CObj, LetterType::OT_Variable, ((Obj*)self)->letter);
	((Variable*)variable)->cls = self->cls;
	((Variable*)variable)->clone_flag = self->clone_flag;
	variable->vari_c = ((Obj*)self)->vari_c;
	return variable;
}

// ============================================================
// Variable::InC (lines 2347-2381)
// ============================================================
Obj* InC_Variable(ThreadGC* thgc, Variable* self, String* op, Master* local, Obj* val2) {
	Obj* cls = self->cls;
	if (cls->objtype == LetterType::OT_Var) {
		Var* varobj = (Var*)cls;
		if (varobj->cls == NULL) return ErrorC(thgc, LetterType::OT_Wait, ((Obj*)self)->letter, "varの型が判明しません");
	}
head:
	if (val2->objtype == LetterType::OT_Bracket) {
		add_list(thgc, local->calls, (char*)local->KouhoSet2);
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		local->calls->size--;
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		Block* block = (Block*)val;
		if (block->rets->size == 1) {
			val2 = *(Obj**)get_list(block->rets, 0);
			goto head;
		} else {
			return ErrorC(thgc, LetterType::OT_Error, val2->letter, "()の中の数が間違っています。");
		}
	}
	val2 = vt_GetterC(thgc, val2, local);
	if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
	if (val2->objtype == LetterType::OT_Block) {
		Obj* it = make_iterator(thgc, -1, (Block*)val2);
		((Obj*)self)->value = val2;
		return it;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "Iteratorの代入が不正です");
}

// ============================================================
// Function::GetterC (lines 2393-2411)
// ============================================================
Obj* GetterC_Function(ThreadGC* thgc, Obj* self, Master* local) {
	LFunc* func = llvm_get_func(local->llvm);
	for (int i = 0; i < local->compsC->size; i++) {
		LComp* c = *(LComp**)get_list(local->compsC, i);
		add_list(thgc, func->comps, (char*)c);
	}
	local->compsC = create_list(thgc, sizeof(LComp*), CType::_List);
	LVari* fv = make_vn(thgc, "%FuncType*");
	if (self->draws == NULL) {
		func->vari->type = createString(thgc, (char*)"%FuncType**", 11, 1);
		add_list(thgc, func->comps, (char*)make_lload(thgc, fv, func->vari, false));
	} else {
		add_list(thgc, func->comps, (char*)make_lload(thgc, fv, self->vari_c, false));
	}
	self->vari_c = fv;
	return self;
}

// ============================================================
// Function::PrimaryC (lines 2412-2632)
// ============================================================
Obj* PrimaryC_Function(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Function* fself = (Function*)self;

	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		LVari* vari;
		Obj* ret = self->ret;
		if (ret->objtype == LetterType::OT_Var) ret = ((Var*)ret)->cls;
		if (ret == local->Int) vari = make_vn(thgc, "i32");
		else if (ret == local->Bool) vari = make_vn(thgc, "i2");
		else if (ret->objtype == LetterType::OT_FuncType) {
			vari = make_vn(thgc, "%FuncType*");
		}
		else {
			char buf[128]; snprintf(buf, sizeof(buf), "%s*", ret->model ? ret->model->data : "i8");
			vari = make_vn(thgc, buf);
		}
		Block* blk = (Block*)val;
		(*n)++;

		if (self->draws == NULL) {
			// No draws - use model/drawcall
			char buf[128]; snprintf(buf, sizeof(buf), "%s*", self->model ? self->model->data : "i8");
			LVari* go_v = make_vn(thgc, buf);
			LCall* go_call = make_lcall(thgc, go_v, make_lvari_c(thgc, buf, self->drawcall ? self->drawcall->data : ""));
			lcall_add(thgc, go_call, self->vari_c);
			Block* drawblk = (Block*)*(Obj**)get_list(self->draw->children, 0);
			for (int i = 0; i < drawblk->vmapA->kvs->size; i++) {
				KV* kv = *(KV**)get_list(drawblk->vmapA->kvs, i);
				Obj* varray_i = (Obj*)kv->value;
				if (varray_i->objtype == LetterType::OT_Variable && i < blk->rets->size) {
					Obj* ret_i = *(Obj**)get_list(blk->rets, i);
					if (ret_i->objtype == LetterType::OT_Value) {
						Variable* variable = (Variable*)varray_i;
						if (ret_i->cls == variable->cls) {
							lcall_add(thgc, go_call, ret_i->vari_c);
						} else {
							if (variable->cls == local->Int) {
								LVari* v = make_vn(thgc, "i32");
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, ret_i->vari_c));
								lcall_add(thgc, go_call, v);
							} else if (variable->cls == local->Short) {
								LVari* v = make_vn(thgc, "i16");
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, ret_i->vari_c));
								lcall_add(thgc, go_call, v);
							} else if (variable->cls == local->Bool) {
								LVari* v = make_vn(thgc, "i1");
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, ret_i->vari_c));
								lcall_add(thgc, go_call, v);
							} else {
								lcall_add(thgc, go_call, ret_i->vari_c);
							}
						}
					}
				}
			}
			LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
			add_list(thgc, func->comps, (char*)go_call);
			LCall* init_call = make_lcall(thgc, vari, make_lvari_c(thgc, vari->type ? (char*)vari->type->data : "void", self->call_c ? self->call_c->data : ""));
			lcall_add(thgc, init_call, rn4);
			lcall_add(thgc, init_call, go_v);
			add_list(thgc, func->comps, (char*)init_call);
		} else {
			// Has draws - use FuncType gete
			LVari* dv = make_vn(thgc, "i8**");
			LGete* gete = make_lgete_idx2(thgc, "%FuncType", dv, self->vari_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1"));
			add_list(thgc, func->comps, (char*)gete);
			LVari* dv2 = make_vn(thgc, "i8*");
			add_list(thgc, func->comps, (char*)make_lload(thgc, dv2, dv, false));
			LVari* go_v = make_vn(thgc, "i8*");
			LCall* go_call = make_lcall(thgc, go_v, dv2);
			lcall_add(thgc, go_call, self->vari_c);
			for (int i = 0; i < blk->rets->size; i++) {
				Obj* ret_i = *(Obj**)get_list(blk->rets, i);
				if (ret_i->objtype == LetterType::OT_Value) {
					Obj* draws_i = *(Obj**)get_list(self->draws, i);
					if (ret_i->cls == draws_i) {
						lcall_add(thgc, go_call, ret_i->vari_c);
					} else {
						if (draws_i == local->Int) {
							LVari* v = make_vn(thgc, "i32");
							add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, ret_i->vari_c));
							lcall_add(thgc, go_call, v);
						} else if (draws_i == local->Short) {
							LVari* v = make_vn(thgc, "i16");
							add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, ret_i->vari_c));
							lcall_add(thgc, go_call, v);
						} else if (draws_i == local->Bool) {
							LVari* v = make_vn(thgc, "i1");
							add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, ret_i->vari_c));
							lcall_add(thgc, go_call, v);
						} else {
							lcall_add(thgc, go_call, ret_i->vari_c);
						}
					}
				}
			}
			add_list(thgc, func->comps, (char*)go_call);
			LVari* cv = make_vn(thgc, "i8**");
			LGete* cgete = make_lgete_idx2(thgc, "%FuncType", cv, self->vari_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2"));
			add_list(thgc, func->comps, (char*)cgete);
			LVari* cv2 = make_vn(thgc, "i8*");
			add_list(thgc, func->comps, (char*)make_lload(thgc, cv2, cv, false));
			LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
			LCall* init_call = make_lcall(thgc, vari, cv2);
			lcall_add(thgc, init_call, rn4);
			lcall_add(thgc, init_call, go_v);
			add_list(thgc, func->comps, (char*)init_call);
		}
		return make_value(thgc, ret, vari);
	}

	// No bracket - walk primary children
	val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextC_Primary(thgc, primary, local, n, &val2);
	}
	if (val2 != NULL) {
		val2 = vt_GetterC(thgc, val2, local);
		if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj || val2->objtype == LetterType::OT_BoolVal || val2->objtype == LetterType::OT_Value) {
			(*n)++;
			LVari* vari;
			Obj* ret = self->ret;
			if (ret->objtype == LetterType::OT_Var) ret = ((Var*)ret)->cls;
			if (ret == local->Int) vari = make_vn(thgc, "i32");
			else if (ret == local->Bool) vari = make_vn(thgc, "i2");
			else if (ret->objtype == LetterType::OT_FuncType) {
				vari = make_vn(thgc, "%FuncType*");
			}
			else {
				char buf[128]; snprintf(buf, sizeof(buf), "%s*", ret->model ? ret->model->data : "i8");
				vari = make_vn(thgc, buf);
			}

			if (self->draws == NULL) {
				char buf[128]; snprintf(buf, sizeof(buf), "%s*", self->model ? self->model->data : "i8");
				LVari* go_v = make_vn(thgc, buf);
				LCall* go_call = make_lcall(thgc, go_v, make_lvari_c(thgc, buf, self->drawcall ? self->drawcall->data : ""));
				lcall_add(thgc, go_call, self->vari_c);
				Block* drawblk = (Block*)*(Obj**)get_list(self->draw->children, 0);
				KV* kv0 = *(KV**)get_list(drawblk->vmapA->kvs, 0);
				Variable* vv = (Variable*)kv0->value;
				if (val2->objtype == LetterType::OT_Value && vv->cls == val2->cls) {
					lcall_add(thgc, go_call, val2->vari_c);
				} else {
					if (vv->cls == local->Int) {
						LVari* v = make_vn(thgc, "i32");
						add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
						lcall_add(thgc, go_call, v);
					} else if (vv->cls == local->Short) {
						LVari* v = make_vn(thgc, "i16");
						add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
						lcall_add(thgc, go_call, v);
					} else if (vv->cls == local->Bool) {
						LVari* v = make_vn(thgc, "i1");
						add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
						lcall_add(thgc, go_call, v);
					} else {
						lcall_add(thgc, go_call, val2->vari_c);
					}
				}
				add_list(thgc, func->comps, (char*)go_call);
				LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
				LCall* init_call = make_lcall(thgc, vari, make_lvari_c(thgc, vari->type ? (char*)vari->type->data : "void", self->call_c ? self->call_c->data : ""));
				lcall_add(thgc, init_call, rn4);
				lcall_add(thgc, init_call, go_v);
				add_list(thgc, func->comps, (char*)init_call);
			} else {
				LVari* dv = make_vn(thgc, "i8**");
				LGete* gete = make_lgete_idx2(thgc, "%FuncType", dv, self->vari_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1"));
				add_list(thgc, func->comps, (char*)gete);
				LVari* dv2 = make_vn(thgc, "i8*");
				add_list(thgc, func->comps, (char*)make_lload(thgc, dv2, dv, false));
				LVari* go_v = make_vn(thgc, "i8*");
				LCall* go_call = make_lcall(thgc, go_v, dv2);
				lcall_add(thgc, go_call, self->vari_c);
				if (val2->objtype == LetterType::OT_Value) {
					Obj* draws0 = *(Obj**)get_list(self->draws, 0);
					if (val2->cls == draws0) {
						lcall_add(thgc, go_call, val2->vari_c);
					} else {
						if (draws0 == local->Int) {
							LVari* v = make_vn(thgc, "i32");
							add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
							lcall_add(thgc, go_call, v);
						} else if (draws0 == local->Short) {
							LVari* v = make_vn(thgc, "i16");
							add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
							lcall_add(thgc, go_call, v);
						} else if (draws0 == local->Bool) {
							LVari* v = make_vn(thgc, "i1");
							add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
							lcall_add(thgc, go_call, v);
						} else {
							lcall_add(thgc, go_call, val2->vari_c);
						}
					}
				} else {
					return ErrorC(thgc, LetterType::OT_NG, val2->letter, "関数の引数が適切ではありません");
				}
				add_list(thgc, func->comps, (char*)go_call);
				LVari* cv = make_vn(thgc, "i8**");
				LGete* cgete = make_lgete_idx2(thgc, "%FuncType", cv, self->vari_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2"));
				add_list(thgc, func->comps, (char*)cgete);
				LVari* cv2 = make_vn(thgc, "i8*");
				add_list(thgc, func->comps, (char*)make_lload(thgc, cv2, cv, false));
				LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
				LCall* init_call = make_lcall(thgc, vari, cv2);
				lcall_add(thgc, init_call, rn4);
				lcall_add(thgc, init_call, go_v);
				add_list(thgc, func->comps, (char*)init_call);
			}
			if (ret->objtype == LetterType::OT_FuncType) {
				FuncType* functype = (FuncType*)ret;
				Obj* fnobj = make_cobj(thgc, CType::_CObj, LetterType::OT_Function, NULL);
				fnobj->ret = functype->cls;
				fnobj->draws = (List*)functype->draws;
				fnobj->vari_c = vari;
				return fnobj;
			}
			return make_value(thgc, ret, vari);
		}
	}
	return ErrorC(thgc, LetterType::OT_NG, val2 ? val2->letter : NULL, "()のような関数に適切な引数が来ていません");
}

// ============================================================
// Helper: process vmapA for typedec/funccheck/funcdraw/funcdec
// Used by Function::Dec, If::PrimaryC, While::PrimaryC, For::PrimaryC
// ============================================================
static void process_vmapA_block(ThreadGC* thgc, Master* local, Map* vmapA,
	LTypeDec* typedec, LFunc* funccheck, LVari* co_val, LVari* thgc2, LVari* i8p,
	const char* model_str, int* order, int* count, List* decs,
	LFunc* funcdraw, LVari* go_c, LFunc* funcdec, LVari* objptr3, LVari* go_v_dec,
	bool is_draw_block)
{
	LFunc* func = llvm_get_func(local->llvm);
	for (int ki = 0; ki < vmapA->kvs->size; ki++) {
		KV* kv = *(KV**)get_list(vmapA->kvs, ki);
		Obj* v = (Obj*)kv->value;
		if (v->objtype == LetterType::OT_Variable) {
			Variable* variable = (Variable*)v;
			v->order = (*order)++;
			Obj* cls = variable->cls;
			if (cls->objtype == LetterType::OT_Var) cls = ((Var*)cls)->cls;
			variable->cls = cls;
			if (cls == local->Int) {
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i32", 3, 1), kv->key));
				*count += 8;
				if (is_draw_block && funcdraw) {
					LVari* vdraw = make_vn(thgc, "i32");
					add_list(thgc, funcdraw->draws, (char*)vdraw);
					char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", v->order);
					LVari* varii = make_vn(thgc, "i32*");
					add_list(thgc, funcdraw->comps, (char*)make_lgete_idx2(thgc, model_str, varii, go_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
					add_list(thgc, funcdraw->comps, (char*)make_lstore(thgc, varii, vdraw));
				}
			} else if (cls == local->Bool) {
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i1", 2, 1), kv->key));
				*count += 8;
				if (is_draw_block && funcdraw) {
					LVari* vdraw = make_vn(thgc, "i1");
					add_list(thgc, funcdraw->draws, (char*)make_vn(thgc, "i32"));
					char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", v->order);
					LVari* varii = make_vn(thgc, "i1*");
					add_list(thgc, funcdraw->comps, (char*)make_lgete_idx2(thgc, model_str, varii, go_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
					add_list(thgc, funcdraw->comps, (char*)make_lstore(thgc, varii, vdraw));
				}
			} else {
				if (cls->identity == 0) cls->identity = llvm_n++;
				char buf[128]; snprintf(buf, sizeof(buf), "%s*", cls->model ? cls->model->data : "i8");
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, buf, (int)strlen(buf), 1), kv->key));
				*count += 8;
				if (is_draw_block && funcdraw) {
					LVari* vdraw = make_vn(thgc, buf);
					add_list(thgc, funcdraw->draws, (char*)vdraw);
					char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", v->order);
					LVari* varii = make_vn(thgc, buf);
					add_list(thgc, funcdraw->comps, (char*)make_lgete_idx2(thgc, model_str, varii, go_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
					add_list(thgc, funcdraw->comps, (char*)make_lstore(thgc, varii, vdraw));
				}
				// GC check
				LVari* vari = make_vn(thgc, "i8*");
				char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", v->order);
				add_list(thgc, funccheck->comps, (char*)make_lgete_idx2(thgc, model_str, vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
				LVari* vari2 = make_vn(thgc, "i8*");
				add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
				LVari* co_cval = make_vn(thgc, "i8*");
				LCall* co_call = make_lcall(thgc, co_cval, co_val);
				lcall_add(thgc, co_call, thgc2);
				lcall_add(thgc, co_call, vari);
				add_list(thgc, funccheck->comps, (char*)co_call);
				add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
			}
		}
		else if (v->objtype == LetterType::OT_Function) {
			Function* f = (Function*)v;
			if (f->identity == 0) f->identity = llvm_n++;
			v->order = (*order)++;
			add_list(thgc, decs, (char*)f);
			add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));
			if (is_draw_block && funcdraw) {
				add_list(thgc, funcdraw->draws, (char*)make_vn(thgc, "%FuncType*"));
			}
			// GC check
			LVari* vari = make_vn(thgc, "i8*");
			char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", v->order);
			add_list(thgc, funccheck->comps, (char*)make_lgete_idx2(thgc, model_str, vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
			LVari* vari2 = make_vn(thgc, "i8*");
			add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
			LVari* co_cval = make_vn(thgc, "i8*");
			LCall* co_call = make_lcall(thgc, co_cval, co_val);
			lcall_add(thgc, co_call, thgc2);
			lcall_add(thgc, co_call, vari);
			add_list(thgc, funccheck->comps, (char*)co_call);
			add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
			*count += 8;

			// Function dec + init calls
			if (funcdec) {
				const char* fname = f->letter ? f->letter->text->data : "f";
				LVari* va0 = make_vn(thgc, "%FuncType*");
				LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "@%sdec%d", fname, f->identity)->data));
				lcall_add(thgc, fu_call, go_v_dec ? go_v_dec : go_c);
				lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", f->drawcall ? f->drawcall->data : ""));
				lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", f->call_c ? f->call_c->data : ""));
				add_list(thgc, funcdec->comps, (char*)fu_call);
				LVari* va2 = make_vn(thgc, (char*)fmt_str(thgc, "%s*", model_str)->data);
				add_list(thgc, funcdec->comps, (char*)make_lload(thgc, va2, objptr3, false));
				LVari* va = make_vn(thgc, "%FuncType*");
				add_list(thgc, funcdec->comps, (char*)make_lgete_idx2(thgc, model_str, va, va2, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
				add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, va, va0));
			}
			if (is_draw_block && funcdraw) {
				const char* fname = f->letter ? f->letter->text->data : "f";
				LVari* va0 = make_vn(thgc, "%FuncType*");
				LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "@%sdec%d", fname, f->identity)->data));
				lcall_add(thgc, fu_call, go_c);
				lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", f->drawcall ? f->drawcall->data : ""));
				lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", f->call_c ? f->call_c->data : ""));
				add_list(thgc, funcdraw->comps, (char*)fu_call);
				LVari* va = make_vn(thgc, "%FuncType*");
				add_list(thgc, funcdraw->comps, (char*)make_lgete_idx2(thgc, model_str, va, go_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
				add_list(thgc, funcdraw->comps, (char*)make_lstore(thgc, va, va0));
			}
		}
		else if (v->objtype == LetterType::OT_ClassObj) {
			ClassObj* f = (ClassObj*)v;
			if (f->identity == 0) f->identity = llvm_n++;
			v->order = (*order)++;
			add_list(thgc, decs, (char*)f);
			add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));
			if (is_draw_block && funcdraw) {
				add_list(thgc, funcdraw->draws, (char*)make_vn(thgc, "%FuncType*"));
			}
			// GC check
			LVari* vari = make_vn(thgc, "i8*");
			char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", v->order);
			add_list(thgc, funccheck->comps, (char*)make_lgete_idx2(thgc, model_str, vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
			LVari* vari2 = make_vn(thgc, "i8*");
			add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
			LVari* co_cval = make_vn(thgc, "i8*");
			LCall* co_call = make_lcall(thgc, co_cval, co_val);
			lcall_add(thgc, co_call, thgc2);
			lcall_add(thgc, co_call, vari);
			add_list(thgc, funccheck->comps, (char*)co_call);
			add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
			*count += 8;

			if (funcdec) {
				const char* fname = f->letter ? f->letter->text->data : "f";
				LVari* va0 = make_vn(thgc, "%FuncType*");
				LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "@%sdec%d", fname, f->identity)->data));
				lcall_add(thgc, fu_call, go_v_dec ? go_v_dec : go_c);
				lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", f->drawcall ? f->drawcall->data : ""));
				lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", f->call_c ? f->call_c->data : ""));
				add_list(thgc, funcdec->comps, (char*)fu_call);
				LVari* va2 = make_vn(thgc, (char*)fmt_str(thgc, "%s*", model_str)->data);
				add_list(thgc, funcdec->comps, (char*)make_lload(thgc, va2, objptr3, false));
				LVari* va = make_vn(thgc, "%FuncType*");
				add_list(thgc, funcdec->comps, (char*)make_lgete_idx2(thgc, model_str, va, va2, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
				add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, va, va0));
			}
			if (is_draw_block && funcdraw) {
				const char* fname = f->letter ? f->letter->text->data : "f";
				LVari* va0 = make_vn(thgc, "%FuncType*");
				LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "@%sdec%d", fname, f->identity)->data));
				lcall_add(thgc, fu_call, go_c);
				lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", f->drawcall ? f->drawcall->data : ""));
				lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", f->call_c ? f->call_c->data : ""));
				add_list(thgc, funcdraw->comps, (char*)fu_call);
				LVari* va = make_vn(thgc, "%FuncType*");
				add_list(thgc, funcdraw->comps, (char*)make_lgete_idx2(thgc, model_str, va, go_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
				add_list(thgc, funcdraw->comps, (char*)make_lstore(thgc, va, va0));
			}
		}
	}
}

// ============================================================
// Function::Dec (lines 2646-3149) - large method
// ============================================================
void Dec_Function(ThreadGC* thgc, Obj* self, Master* local) {
	Function* fself = (Function*)self;
	if (fself->identity == 0) fself->identity = llvm_n++;
	self->ifv = create_mapy(thgc, CType::_KV);

	Obj* ret = self->ret;
	if (ret->objtype == LetterType::OT_Var) ret = ((Var*)ret)->cls;
	const char* type_str;
	if (ret == local->Int) type_str = "i32";
	else if (ret == local->Bool) type_str = "i1";
	else {
		char buf[128]; snprintf(buf, sizeof(buf), "%s*", ret->model ? ret->model->data : "i8");
		type_str = (char*)createString(thgc, buf, (int)strlen(buf), 1)->data;
	}

	self->n_c = obj_cn++;
	const char* fname = self->letter ? self->letter->text->data : "f";
	char callname[256]; snprintf(callname, sizeof(callname), "@%s%d", fname, fself->identity);
	self->call_c = createString(thgc, callname, (int)strlen(callname), 1);
	self->vari_c = make_lvari_c(thgc, "void", callname);

	char drawcallname[256]; snprintf(drawcallname, sizeof(drawcallname), "@%sDraw%d", fname, fself->identity);
	self->drawcall = createString(thgc, drawcallname, (int)strlen(drawcallname), 1);

	char modelname[256]; snprintf(modelname, sizeof(modelname), "%%%s%dFuncType", fname, fself->identity);
	self->model = createString(thgc, modelname, (int)strlen(modelname), 1);

	// Create main function
	LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	LVari* obj_v = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s*", modelname)->data, "%obj");
	LFunc* lfunc = make_lfunc(thgc, local->llvm, self->vari_c);
	add_list(thgc, lfunc->draws, (char*)rn4);
	add_list(thgc, lfunc->draws, (char*)obj_v);
	add_list(thgc, local->llvm->comps, (char*)lfunc);
	add_list(thgc, local->llvm->funcs, (char*)lfunc);

	// thgc setup
	LVari* thgcptr4 = make_lvari_c(thgc, "%ThreadGCType**", "%thgcptr");
	add_list(thgc, lfunc->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", thgcptr4, rn4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	add_list(thgc, lfunc->comps, (char*)make_lload(thgc, thgc4, thgcptr4, false));

	// Alloca + Store + SetRoot
	LVari* objptr = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s**", modelname)->data, "%objptr");
	add_list(thgc, lfunc->comps, (char*)make_lalloca(thgc, objptr));
	add_list(thgc, lfunc->comps, (char*)make_lstore(thgc, objptr, obj_v));
	LVari* srv = make_vn(thgc, "%GC_SetRootType");
	add_list(thgc, lfunc->comps, (char*)make_lload(thgc, srv, make_lvari_c(thgc, "%GC_SetRootType*", "@GC_SetRoot"), false));
	LCall* srcall = make_lcall(thgc, NULL, srv);
	lcall_add(thgc, srcall, rn4);
	lcall_add(thgc, srcall, objptr);
	add_list(thgc, lfunc->comps, (char*)srcall);

	// Create draw function
	LVari* rn5 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	LVari* blk_v = make_lvari_c(thgc, "i8*", "%fptr1");
	LFunc* funcdraw = make_lfunc(thgc, local->llvm, make_lvari_c(thgc, (char*)fmt_str(thgc, "%s*", modelname)->data, drawcallname));
	add_list(thgc, funcdraw->draws, (char*)rn5);
	add_list(thgc, funcdraw->draws, (char*)blk_v);
	add_list(thgc, local->llvm->comps, (char*)funcdraw);

	// TypeDec
	LTypeDec* typedec = make_ltypedec(thgc, createString(thgc, modelname, (int)strlen(modelname), 1), NULL, false);
	add_list(thgc, local->llvm->types, (char*)typedec);
	add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i8*", 3, 1), createString(thgc, (char*)"blk", 3, 1)));

	// Check function
	LVari* thgc2 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* i8p = make_lvari_c(thgc, "i8*", "%self");
	char checkname[256]; snprintf(checkname, sizeof(checkname), "@%sCheck%d", fname, fself->identity);
	LFunc* funccheck = make_lfunc(thgc, local->llvm, make_lvari_c(thgc, "void", checkname));
	add_list(thgc, funccheck->draws, (char*)thgc2);
	add_list(thgc, funccheck->draws, (char*)i8p);
	add_list(thgc, local->llvm->comps, (char*)funccheck);

	LVari* vari = make_vn(thgc, "i8*");
	add_list(thgc, funccheck->comps, (char*)make_lgete_idx2(thgc, modelname, vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
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

	// AddClass in main
	LVari* thgc3 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* ac_val = make_lvari_c(thgc, "%GC_AddClassType", "%addclass");
	LVari* countv = make_lvari_c(thgc, "i32", "0");
	int namelen = (int)strlen(fname);
	LStrV* strv = make_lstrv(thgc,
		fmt_str(thgc, "@%s%d", fname, fself->identity),
		createString(thgc, (char*)fname, namelen, 1), namelen);
	add_list(thgc, local->llvm->strs, (char*)strv);
	int cn_val = local->llvm->cn++;
	self->n_c = cn_val;
	LVari* typ = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", cn_val)->data);
	LVari* tnp = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tnp%d", llvm_n++)->data);
	add_list(thgc, local->llvm->main_func->comps, (char*)make_lload(thgc, tnp, make_lvari_c(thgc, "i32*", "@cnp"), false));
	LVari* tv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%cv%d", llvm_n++)->data);
	add_list(thgc, local->llvm->main_func->comps, (char*)make_lbinop(thgc, LBOP_Add, tv, tnp, typ));
	LCall* ac_call = make_lcall(thgc, NULL, ac_val);
	lcall_add(thgc, ac_call, thgc3);
	lcall_add(thgc, ac_call, (LVari*)strv);
	lcall_add(thgc, ac_call, countv);
	lcall_add(thgc, ac_call, make_lvari_c(thgc, "%GCCheckFuncType", checkname));
	lcall_add(thgc, ac_call, make_lvari_c(thgc, "%GCFinalizeFuncType", "null"));
	add_list(thgc, local->llvm->main_func->comps, (char*)ac_call);

	// funcdraw body: thgc setup + CloneObject
	LVari* thgcptr5 = make_lvari_c(thgc, "%ThreadGCType**", "%thgcptr");
	add_list(thgc, funcdraw->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", thgcptr5, rn5, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	LVari* thgc5 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	add_list(thgc, funcdraw->comps, (char*)make_lload(thgc, thgc5, thgcptr5, false));
	LVari* go_val = make_lvari_c(thgc, "%CopyObjectType", "%gcobject");
	add_list(thgc, funcdraw->comps, (char*)make_lload(thgc, go_val, make_lvari_c(thgc, "%CopyObjectType*", "@CloneObject"), false));
	LVari* v3 = make_vn(thgc, "i8**");
	add_list(thgc, funcdraw->comps, (char*)make_lgete_idx2(thgc, "%FuncType", v3, blk_v, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "3")));
	LVari* v4 = make_vn(thgc, "i8*");
	add_list(thgc, funcdraw->comps, (char*)make_lload(thgc, v4, v3, false));
	LVari* go_v = make_lvari_c(thgc, "i8*", "%obj");
	LCall* go_call_draw = make_lcall(thgc, go_v, go_val);
	lcall_add(thgc, go_call_draw, thgc5);
	lcall_add(thgc, go_call_draw, v4);
	add_list(thgc, funcdraw->comps, (char*)go_call_draw);
	LVari* go_c = make_vn(thgc, (char*)fmt_str(thgc, "%s*", modelname)->data);
	add_list(thgc, funcdraw->comps, (char*)make_lcast(thgc, LCAST_Bitcast, go_c, go_v));

	// funcdec function
	LVari* rn6 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	LVari* block_v = make_lvari_c(thgc, "i8*", "%block");
	LVari* funcptr = make_lvari_c(thgc, "i8*", "%fptr");
	LVari* funcptr2 = make_lvari_c(thgc, "i8*", "%fptr2");
	char decname[256]; snprintf(decname, sizeof(decname), "@%sdec%d", fname, fself->identity);
	LFunc* funcdec = make_lfunc(thgc, local->llvm, make_lvari_c(thgc, "%FuncType*", decname));
	add_list(thgc, funcdec->draws, (char*)rn6);
	add_list(thgc, funcdec->draws, (char*)block_v);
	add_list(thgc, funcdec->draws, (char*)funcptr);
	add_list(thgc, funcdec->draws, (char*)funcptr2);
	add_list(thgc, local->llvm->comps, (char*)funcdec);
	LVari* thgc6 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	add_list(thgc, funcdec->comps, (char*)make_lload(thgc, thgc6, make_lvari_c(thgc, "%ThreadGCType**", "@thgcp"), false));

	// GC_malloc for FuncType
	LVari* gmvari = make_lvari_c(thgc, "%GC_mallocType", "%gm");
	add_list(thgc, funcdec->comps, (char*)make_lload(thgc, gmvari, make_lvari_c(thgc, "%GC_mallocType*", "@GC_malloc"), false));
	LVari* tmp28 = make_lvari_c(thgc, "i32", "28");
	LVari* go_v_dec = make_lvari_c(thgc, "i8*", "%obj");
	LCall* gc_call = make_lcall(thgc, go_v_dec, gmvari);
	lcall_add(thgc, gc_call, thgc6);
	lcall_add(thgc, gc_call, tmp28);
	add_list(thgc, funcdec->comps, (char*)gc_call);
	LVari* go_c3 = make_vn(thgc, "%FuncType*");
	add_list(thgc, funcdec->comps, (char*)make_lcast(thgc, LCAST_Bitcast, go_c3, go_v_dec));

	// Set FuncType fields
	LVari* vc0 = make_vn(thgc, "i8*");
	add_list(thgc, funcdec->comps, (char*)make_lgete_idx2(thgc, "%FuncType", vc0, go_c3, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, vc0, block_v));
	LVari* vc1 = make_vn(thgc, "i8*");
	add_list(thgc, funcdec->comps, (char*)make_lgete_idx2(thgc, "%FuncType", vc1, go_c3, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, vc1, funcptr));
	LVari* vc2 = make_vn(thgc, "i8*");
	add_list(thgc, funcdec->comps, (char*)make_lgete_idx2(thgc, "%FuncType", vc2, go_c3, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, vc2, funcptr2));

	// Alloca + SetRoot for FuncType
	LVari* objptr2 = make_lvari_c(thgc, "%FuncType**", "%objptr");
	add_list(thgc, funcdec->comps, (char*)make_lalloca(thgc, objptr2));
	add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, objptr2, go_c3));
	LVari* srv2 = make_vn(thgc, "%GC_SetRootType");
	add_list(thgc, funcdec->comps, (char*)make_lload(thgc, srv2, make_lvari_c(thgc, "%GC_SetRootType*", "@GC_SetRoot"), false));
	LCall* srcall2 = make_lcall(thgc, NULL, srv2);
	lcall_add(thgc, srcall2, rn6);
	lcall_add(thgc, srcall2, objptr2);
	add_list(thgc, funcdec->comps, (char*)srcall2);

	// GC_malloc for this type
	LVari* go_v2 = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
	LVari* tmp_n = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", self->n_c)->data);
	LVari* tmi = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tmi%d", llvm_n++)->data);
	add_list(thgc, funcdec->comps, (char*)make_lload(thgc, tmi, make_lvari_c(thgc, "i32*", "@cnp"), false));
	LVari* tmv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tmv%d", llvm_n++)->data);
	add_list(thgc, funcdec->comps, (char*)make_lbinop(thgc, LBOP_Add, tmv, tmi, tmp_n));
	LCall* gc_call2 = make_lcall(thgc, go_v2, gmvari);
	lcall_add(thgc, gc_call2, thgc6);
	lcall_add(thgc, gc_call2, tmv);
	add_list(thgc, funcdec->comps, (char*)gc_call2);
	LVari* go_c2 = make_vn(thgc, (char*)fmt_str(thgc, "%s*", modelname)->data);
	add_list(thgc, funcdec->comps, (char*)make_lcast(thgc, LCAST_Bitcast, go_c2, go_v2));

	// Set blk field on typed object
	LVari* vc3 = make_vn(thgc, "i8*");
	add_list(thgc, funcdec->comps, (char*)make_lgete_idx2(thgc, modelname, vc3, go_c2, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, vc3, block_v));

	// Alloca + SetRoot for typed object
	LVari* objptr3 = make_vn(thgc, (char*)fmt_str(thgc, "%s**", modelname)->data);
	add_list(thgc, funcdec->comps, (char*)make_lalloca(thgc, objptr3));
	add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, objptr3, go_c2));
	LCall* srcall3 = make_lcall(thgc, NULL, srv2);
	lcall_add(thgc, srcall3, thgc6);
	lcall_add(thgc, srcall3, objptr3);
	add_list(thgc, funcdec->comps, (char*)srcall3);

	// Set parent pointer
	LVari* vc4 = make_vn(thgc, (char*)fmt_str(thgc, "%s**", modelname)->data);
	add_list(thgc, funcdec->comps, (char*)make_lgete_idx2(thgc, "%FuncType", vc4, go_c3, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "3")));
	add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, vc4, go_c2));

	// Process vmapA for draw block (blk0) and body block (blk2)
	int count = 8;
	Block* blk0 = (Block*)*(Obj**)get_list(self->draw->children, 0);
	int order = 1;
	Block* blk2 = (Block*)*(Obj**)get_list(self->draw->children, 1);
	List* decs = create_list(thgc, sizeof(Obj*), CType::_List);

	process_vmapA_block(thgc, local, blk0->vmapA, typedec, funccheck, co_val, thgc2, i8p,
		modelname, &order, &count, decs, funcdraw, go_c, funcdec, objptr3, block_v, true);
	process_vmapA_block(thgc, local, blk2->vmapA, typedec, funccheck, co_val, thgc2, i8p,
		modelname, &order, &count, decs, NULL, NULL, funcdec, objptr3, go_v_dec, false);

	// Dec all collected decs
	for (int i = 0; i < decs->size; i++) {
		Obj* d = *(Obj**)get_list(decs, i);
		vt_Dec(thgc, d, local);
	}
	add_list(thgc, local->llvm->funcs, (char*)funcdec);
	self->bas = objptr2;
	add_list(thgc, local->blocks, (char*)blk0);
	exeC_Block(thgc, (Obj*)blk0, local);
	local->llvm->funcs->size--;
	add_list(thgc, local->blocks, (char*)blk2);

	// funcdraw ret
	add_list(thgc, funcdraw->comps, (char*)make_lret(thgc, go_c));
	char countbuf[32]; snprintf(countbuf, sizeof(countbuf), "%d", count);
	countv->name = createString(thgc, countbuf, (int)strlen(countbuf), 1);
	add_list(thgc, funccheck->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));

	// funcdec ret
	LVari* vv = make_vn(thgc, "%FuncType*");
	add_list(thgc, funcdec->comps, (char*)make_lload(thgc, vv, objptr2, false));

	LVari* rnpv = make_lvari_c(thgc, "i32*", "%ptr");
	add_list(thgc, funcdec->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn6, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	LVari* rnpv12 = make_lvari_c(thgc, "i32", "%v");
	add_list(thgc, funcdec->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
	LVari* rnpv2 = make_lvari_c(thgc, "i32", "%dec");
	add_list(thgc, funcdec->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12, make_lvari_c(thgc, "i32", "1")));
	add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, rnpv, rnpv2));
	add_list(thgc, funcdec->comps, (char*)make_lret(thgc, go_c3));

	// Execute body block
	self->bas = objptr;
	LLab* lab_entry = make_llab(thgc, createString(thgc, (char*)"entry", 5, 1));
	add_list(thgc, local->labs, (char*)lab_entry);
	exeC_Block(thgc, (Obj*)blk2, local);
	local->labs->size--;
	local->blocks->size--;
	local->blocks->size--;

	// Epilogue
	LFunc* curFunc = llvm_get_func(local->llvm);
	rnpv = make_lvari_c(thgc, "i32*", "%ptr");
	add_list(thgc, curFunc->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	rnpv12 = make_lvari_c(thgc, "i32", "%v");
	add_list(thgc, curFunc->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
	rnpv2 = make_lvari_c(thgc, "i32", "%dec");
	add_list(thgc, curFunc->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12, make_lvari_c(thgc, "i32", "1")));
	add_list(thgc, curFunc->comps, (char*)make_lstore(thgc, rnpv, rnpv2));

	if (blk2->rets && blk2->rets->size > 0) {
		Obj* valobj = *(Obj**)get_list(blk2->rets, blk2->rets->size - 1);
		if (valobj->objtype == LetterType::OT_Value) {
			LVari* va = BitcastCast(thgc, local, ret, valobj->cls, valobj->vari_c);
			add_list(thgc, curFunc->comps, (char*)make_lret(thgc, va));
		}
	}
	add_list(thgc, curFunc->comps, (char*)make_lret(thgc, blk2->rets && blk2->rets->size > 0 ? (*(Obj**)get_list(blk2->rets, blk2->rets->size - 1))->vari_c : make_lvari_c(thgc, "void", "")));
	local->llvm->funcs->size--;
}

// ============================================================
// If::exepC (lines 3159-3166)
// ============================================================
Obj* exepC_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->letter && local->kouhos == NULL) {
		// call last callback
		if (local->calls->size > 0) {
			typedef void (*CallbackFn)();
			CallbackFn cb = *(CallbackFn*)get_list(local->calls, local->calls->size - 1);
			if (cb) cb();
		}
	}
	return self;
}

// ============================================================
// Helper: setup scope block (GC alloc + type + check for if/elif/else/while/for)
// ============================================================
static void setup_scope_gc(ThreadGC* thgc, Master* local, Obj* self,
	const char* typename_str, const char* checkname,
	LTypeDec** out_typedec, LFunc** out_funccheck, LVari** out_co_val,
	LVari** out_thgc2, LVari** out_i8p, LVari** out_go_c, LVari** out_objptr3,
	int* out_count, int* out_order)
{
	LFunc* func = llvm_get_func(local->llvm);

	// TypeDec
	*out_typedec = make_ltypedec(thgc, createString(thgc, (char*)typename_str, (int)strlen(typename_str), 1), NULL, false);
	add_list(thgc, local->llvm->types, (char*)*out_typedec);

	// Funccheck
	*out_thgc2 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	*out_i8p = make_lvari_c(thgc, "i8*", "%self");
	*out_funccheck = make_lfunc(thgc, local->llvm, make_lvari_c(thgc, "void", (char*)checkname));
	add_list(thgc, (*out_funccheck)->draws, (char*)*out_thgc2);
	add_list(thgc, (*out_funccheck)->draws, (char*)*out_i8p);
	add_list(thgc, local->llvm->comps, (char*)*out_funccheck);

	add_list(thgc, ((LComp*)*out_typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i8*", 3, 1), createString(thgc, (char*)"blk", 3, 1)));

	// Check function body: gete + CopyObject
	LVari* vari = make_vn(thgc, "i8*");
	add_list(thgc, (*out_funccheck)->comps, (char*)make_lgete_idx2(thgc, typename_str, vari, *out_i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	*out_co_val = make_lvari_c(thgc, "%CopyObjectType", "%co");
	add_list(thgc, (*out_funccheck)->comps, (char*)make_lload(thgc, *out_co_val, make_lvari_c(thgc, "%CopyObjectType*", "@CopyObject"), false));
	LVari* vari2 = make_vn(thgc, "i8*");
	add_list(thgc, (*out_funccheck)->comps, (char*)make_lload(thgc, vari2, vari, false));
	LVari* co_cval = make_vn(thgc, "i8*");
	LCall* co_call = make_lcall(thgc, co_cval, *out_co_val);
	lcall_add(thgc, co_call, *out_thgc2);
	lcall_add(thgc, co_call, vari);
	add_list(thgc, (*out_funccheck)->comps, (char*)co_call);
	add_list(thgc, (*out_funccheck)->comps, (char*)make_lstore(thgc, vari, co_cval));

	// AddClass in main
	LVari* thgc3 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* ac_val = make_lvari_c(thgc, "%GC_AddClassType", "%addclass");
	LVari* countv = make_lvari_c(thgc, "i32", "");
	char namebuf[128]; snprintf(namebuf, sizeof(namebuf), "scope%d", llvm_n++);
	LStrV* strv = make_lstrv(thgc,
		fmt_str(thgc, "@%s", namebuf),
		createString(thgc, namebuf, (int)strlen(namebuf), 1), (int)strlen(namebuf));
	add_list(thgc, local->llvm->strs, (char*)strv);
	int cn = local->llvm->cn++;
	self->n_c = cn;
	LVari* typ = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", cn)->data);
	LVari* tnp = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tnp%d", llvm_n++)->data);
	add_list(thgc, local->llvm->main_func->comps, (char*)make_lload(thgc, tnp, make_lvari_c(thgc, "i32*", "@cnp"), false));
	LVari* tv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%cv%d", llvm_n++)->data);
	add_list(thgc, local->llvm->main_func->comps, (char*)make_lbinop(thgc, LBOP_Add, tv, tnp, typ));
	LCall* ac_call = make_lcall(thgc, NULL, ac_val);
	lcall_add(thgc, ac_call, thgc3);
	lcall_add(thgc, ac_call, (LVari*)strv);
	lcall_add(thgc, ac_call, countv);
	lcall_add(thgc, ac_call, make_lvari_c(thgc, "%GCCheckFuncType", (char*)checkname));
	lcall_add(thgc, ac_call, make_lvari_c(thgc, "%GCFinalizeFuncType", "null"));
	add_list(thgc, local->llvm->main_func->comps, (char*)ac_call);

	// GC_malloc in current func
	LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* go_val = make_vn(thgc, "%GC_mallocType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, go_val, make_lvari_c(thgc, "%GC_mallocType*", "@GC_malloc"), false));
	LVari* go_v = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
	LVari* tmp = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", cn)->data);
	LVari* tmi = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tmi%d", llvm_n++)->data);
	add_list(thgc, func->comps, (char*)make_lload(thgc, tmi, make_lvari_c(thgc, "i32*", "@cnp"), false));
	LVari* tmv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tmv%d", llvm_n++)->data);
	add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Add, tmv, tmi, tmp));
	LCall* go_call = make_lcall(thgc, go_v, go_val);
	lcall_add(thgc, go_call, thgc4);
	lcall_add(thgc, go_call, tmv);
	add_list(thgc, func->comps, (char*)go_call);
	*out_go_c = make_vn(thgc, (char*)fmt_str(thgc, "%s*", typename_str)->data);
	add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, *out_go_c, go_v));

	// Set parent pointer
	LVari* v0 = make_vn(thgc, "i8**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, typename_str, v0, *out_go_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	LVari* gj = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%gj%d", llvm_n++)->data);
	if (func->async_flag) {
		LVari* objf = make_lvari_c(thgc, "%CoroFrameType*", "%frame");
		LVari* objp = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", objp, objf, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
		add_list(thgc, func->comps, (char*)make_lload(thgc, gj, objp, false));
	} else if (local->blocks->size > 0) {
		Block* lastblk = *(Block**)get_list(local->blocks, local->blocks->size - 1);
		Obj* lastobj = lastblk->obj ? (Obj*)lastblk->obj : NULL;
		if (lastobj && lastobj->objtype == LetterType::OT_IfBlock) {
			add_list(thgc, func->comps, (char*)make_lload(thgc, gj, lastobj->bas, false));
		} else if (lastobj) {
			add_list(thgc, func->comps, (char*)make_lload(thgc, gj, lastobj->bas, false));
		}
	}
	add_list(thgc, func->comps, (char*)make_lstore(thgc, v0, gj));

	// Alloca + SetRoot
	if (!func->async_flag) {
		*out_objptr3 = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s**", typename_str)->data, (char*)fmt_str(thgc, "%%objptr%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lalloca(thgc, *out_objptr3));
		add_list(thgc, func->comps, (char*)make_lstore(thgc, *out_objptr3, *out_go_c));
		self->bas = *out_objptr3;
		LVari* srv = make_vn(thgc, "%GC_SetRootType");
		add_list(thgc, func->comps, (char*)make_lload(thgc, srv, make_lvari_c(thgc, "%GC_SetRootType*", "@GC_SetRoot"), false));
		LCall* srcall = make_lcall(thgc, NULL, srv);
		lcall_add(thgc, srcall, thgc4);
		lcall_add(thgc, srcall, self->bas);
		add_list(thgc, func->comps, (char*)srcall);
	} else {
		LVari* objf = make_lvari_c(thgc, "%CoroFrameType*", "%frame");
		*out_objptr3 = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", *out_objptr3, objf, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
		add_list(thgc, func->comps, (char*)make_lstore(thgc, *out_objptr3, *out_go_c));
	}
	*out_count = 8;
	*out_order = 1;
}

// ============================================================
// Helper: decrement root node pointer (rnp--)
// ============================================================
static void emit_rnp_dec(ThreadGC* thgc, Master* local) {
	LFunc* func = llvm_get_func(local->llvm);
	if (func->async_flag) {
		LVari* objp = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
		LVari* objf = make_lvari_c(thgc, "%CoroFrameType*", "%frame");
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", objp, objf, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
		LVari* objv = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%objv%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lload(thgc, objv, objp, false));
		LVari* blockptr = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%objp%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%GCObject", blockptr, objv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
		LVari* blockv = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%objv%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lload(thgc, blockv, blockptr, false));
		add_list(thgc, func->comps, (char*)make_lstore(thgc, objp, blockv));
	} else {
		LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
		LVari* rnpv = make_lvari_c(thgc, "i32*", "%ptr");
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
		LVari* rnpv12 = make_lvari_c(thgc, "i32", "%v");
		add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
		LVari* rnpv2 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%dec%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12, make_lvari_c(thgc, "i32", "1")));
		add_list(thgc, func->comps, (char*)make_lstore(thgc, rnpv, rnpv2));
	}
}

// ============================================================
// If::PrimaryC (lines 3167-4370) - very large method
// ============================================================
Obj* PrimaryC_If(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Obj* ret = self->ret;

	if (val2->objtype == LetterType::OT_CallBlock) {
		Block* blk0 = (Block*)*(Obj**)get_list(val2->children, 0);
		Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);

		if (ret->cls == NULL) return ErrorC(thgc, LetterType::OT_Wait, self->letter, "varの型が判明しません");

		const char* fname = self->letter ? self->letter->text->data : "if";
		char typename_str[256]; snprintf(typename_str, sizeof(typename_str), "%%%sIfType", fname);
		char checkname[256]; snprintf(checkname, sizeof(checkname), "@%sIfCheck%d", fname, llvm_n++);

		self->model = createString(thgc, typename_str, (int)strlen(typename_str), 1);

		LTypeDec* typedec; LFunc* funccheck; LVari* co_val; LVari* thgc2; LVari* i8p;
		LVari* go_c; LVari* objptr3;
		int count, order;
		setup_scope_gc(thgc, local, self, typename_str, checkname,
			&typedec, &funccheck, &co_val, &thgc2, &i8p, &go_c, &objptr3, &count, &order);

		List* decs = create_list(thgc, sizeof(Obj*), CType::_List);

		// Process vmapA for blk0 and blk2
		process_vmapA_block(thgc, local, blk0->vmapA, typedec, funccheck, co_val, thgc2, i8p,
			typename_str, &order, &count, decs, NULL, NULL, func, objptr3, go_c, false);
		process_vmapA_block(thgc, local, blk2->vmapA, typedec, funccheck, co_val, thgc2, i8p,
			typename_str, &order, &count, decs, NULL, NULL, func, objptr3, go_c, false);

		// Store local state
		local->blok = go_c;
		local->typedec_c = typedec;
		local->funccheck_c = funccheck;
		local->funcdec_c = func;
		local->i8p_c = i8p;
		local->objptr3_c = objptr3;
		local->co_val_c = co_val;
		local->odr = order;
		local->count_c = count;

		for (int i = 0; i < decs->size; i++) {
			Obj* d = *(Obj**)get_list(decs, i);
			vt_Dec(thgc, d, local);
		}

		char countbuf[32]; snprintf(countbuf, sizeof(countbuf), "%d", count);
		// countv would need to be set - simplified
		add_list(thgc, funccheck->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));
		add_list(thgc, local->blocks, (char*)blk0);
		exeC_Block(thgc, (Obj*)blk0, local);

		// Branch
		LLab* lab1 = make_llab(thgc, fmt_str(thgc, "then%d", llvm_n++));
		LLab* lab2 = make_llab(thgc, fmt_str(thgc, "else%d", llvm_n++));
		LLab* end = make_llab(thgc, createString(thgc, (char*)"end", 3, 1));

		if (self->calls_list && self->calls_list->size == 1) lab2 = end;
		self->entry_lab = lab1;
		self->end_lab = end;

		// Br on condition
		Obj* last_ret = blk0->rets && blk0->rets->size > 0 ? *(Obj**)get_list(blk0->rets, blk0->rets->size - 1) : NULL;
		LVari* cond = last_ret ? last_ret->vari_c : NULL;
		add_list(thgc, func->comps, (char*)make_lbr(thgc, cond, lab1, lab2));
		add_list(thgc, func->comps, (char*)lab1);

		add_list(thgc, local->blocks, (char*)blk2);
		add_list(thgc, local->labs, (char*)lab1);
		exeC_Block(thgc, (Obj*)blk2, local);
		local->labs->size--;
		local->blocks->size--;
		local->blocks->size--;

		// Handle then branch value
		if (blk2->rets && blk2->rets->size > 0) {
			Obj* lastval = *(Obj**)get_list(blk2->rets, blk2->rets->size - 1);
			if (lastval->objtype == LetterType::OT_Value) {
				LVari* va = BitcastCast(thgc, local, ret, lastval->cls, blk2->vari_c ? blk2->vari_c : lastval->vari_c);
				// Store as ifvalue for phi
				add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, end));
			} else if (lastval->objtype == LetterType::OT_VoiVal) {
				add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, end));
			} else {
				return ErrorC(thgc, LetterType::OT_Error, self->letter, "ifの値がありません");
			}
		}

		// Handle elif/else chain
		if (self->calls_list == NULL || self->calls_list->size != 1) {
			add_list(thgc, func->comps, (char*)lab2);
		}

		// Walk remaining primary children for elif/else
		int m = 1;
	head_if:
		if (self->letter == local->letter && local->kouhos == NULL) {
			// kouho callback
		}
		if (val2->objtype == LetterType::OT_Elif) {
			emit_rnp_dec(thgc, local);
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_CallBlock) {
				Block* blk10 = (Block*)*(Obj**)get_list(val2->children, 0);
				Block* blk12 = (Block*)*(Obj**)get_list(val2->children, 1);

				// Setup elif scope
				char eliftype[256]; snprintf(eliftype, sizeof(eliftype), "%%%sElifType%d", fname, llvm_n++);
				char elifcheck[256]; snprintf(elifcheck, sizeof(elifcheck), "@%sElifCheck%d", fname, llvm_n++);

				LTypeDec* typedec1; LFunc* funccheck2; LVari* co_val1; LVari* thgc21; LVari* i8p1;
				LVari* go_c1; LVari* objptr31;
				int count1, order1;
				Obj* elif_obj = val2; // use val2 as elif placeholder
				elif_obj->model = createString(thgc, eliftype, (int)strlen(eliftype), 1);
				setup_scope_gc(thgc, local, elif_obj, eliftype, elifcheck,
					&typedec1, &funccheck2, &co_val1, &thgc21, &i8p1, &go_c1, &objptr31, &count1, &order1);

				List* decs2 = create_list(thgc, sizeof(Obj*), CType::_List);
				process_vmapA_block(thgc, local, blk10->vmapA, typedec1, funccheck2, co_val1, thgc21, i8p1,
					eliftype, &order1, &count1, decs2, NULL, NULL, func, objptr31, go_c1, false);
				process_vmapA_block(thgc, local, blk12->vmapA, typedec1, funccheck2, co_val1, thgc21, i8p1,
					eliftype, &order1, &count1, decs2, NULL, NULL, func, objptr31, go_c1, false);

				for (int i = 0; i < decs2->size; i++) vt_Dec(thgc, *(Obj**)get_list(decs2, i), local);
				add_list(thgc, funccheck2->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));

				add_list(thgc, local->blocks, (char*)blk0);
				exeC_Block(thgc, (Obj*)blk10, local);

				LLab* lab11 = make_llab(thgc, fmt_str(thgc, "then%d", llvm_n++));
				lab2 = make_llab(thgc, fmt_str(thgc, "else%d", llvm_n++));
				if (self->calls_list && (int)self->calls_list->size == m + 1) lab2 = end;

				Obj* last_ret1 = blk10->rets && blk10->rets->size > 0 ? *(Obj**)get_list(blk10->rets, blk10->rets->size - 1) : NULL;
				add_list(thgc, func->comps, (char*)make_lbr(thgc, last_ret1 ? last_ret1->vari_c : NULL, lab11, lab2));
				add_list(thgc, func->comps, (char*)lab11);
				add_list(thgc, local->blocks, (char*)blk12);
				add_list(thgc, local->labs, (char*)lab11);
				exeC_Block(thgc, (Obj*)blk12, local);
				local->labs->size--;
				local->blocks->size--;
				local->blocks->size--;

				if (blk12->rets && blk12->rets->size > 0) {
					Obj* lastval = *(Obj**)get_list(blk12->rets, blk12->rets->size - 1);
					if (lastval->objtype == LetterType::OT_Value || lastval->objtype == LetterType::OT_VoiVal) {
						add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, end));
					} else {
						return ErrorC(thgc, LetterType::OT_Error, self->letter, "ifの値がありません");
					}
				}
				add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, end));
				m++;
				if (self->calls_list && (int)self->calls_list->size != m) {
					add_list(thgc, func->comps, (char*)lab2);
				}
				if (ret->cls == NULL) return ErrorC(thgc, LetterType::OT_Wait, self->letter, "varの型が判明しません");
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				goto head_if;
			} else {
				return ErrorC(thgc, LetterType::OT_Error, val2->letter, "elif文の中身が宣言されていません。");
			}
		}
		else if (val2->objtype == LetterType::OT_Else) {
			emit_rnp_dec(thgc, local);
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_CallBlock) {
				Block* blk10 = (Block*)*(Obj**)get_list(val2->children, 0);
				Block* blk12 = (Block*)*(Obj**)get_list(val2->children, 1);

				char elsetype[256]; snprintf(elsetype, sizeof(elsetype), "%%%sElseType%d", fname, llvm_n++);
				char elsecheck[256]; snprintf(elsecheck, sizeof(elsecheck), "@%sElseCheck%d", fname, llvm_n++);

				LTypeDec* typedec1; LFunc* funccheck2; LVari* co_val1; LVari* thgc21; LVari* i8p1;
				LVari* go_c1; LVari* objptr31;
				int count1, order1;
				Obj* else_obj = val2;
				else_obj->model = createString(thgc, elsetype, (int)strlen(elsetype), 1);
				setup_scope_gc(thgc, local, else_obj, elsetype, elsecheck,
					&typedec1, &funccheck2, &co_val1, &thgc21, &i8p1, &go_c1, &objptr31, &count1, &order1);

				List* decs2 = create_list(thgc, sizeof(Obj*), CType::_List);
				process_vmapA_block(thgc, local, blk10->vmapA, typedec1, funccheck2, co_val1, thgc21, i8p1,
					elsetype, &order1, &count1, decs2, NULL, NULL, func, objptr31, go_c1, false);
				process_vmapA_block(thgc, local, blk12->vmapA, typedec1, funccheck2, co_val1, thgc21, i8p1,
					elsetype, &order1, &count1, decs2, NULL, NULL, func, objptr31, go_c1, false);

				for (int i = 0; i < decs2->size; i++) vt_Dec(thgc, *(Obj**)get_list(decs2, i), local);
				add_list(thgc, funccheck2->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));

				add_list(thgc, local->labs, (char*)lab2);
				add_list(thgc, local->blocks, (char*)blk0);
				exeC_Block(thgc, (Obj*)blk10, local);
				add_list(thgc, local->blocks, (char*)blk12);
				exeC_Block(thgc, (Obj*)blk12, local);
				local->labs->size--;
				local->blocks->size--;
				local->blocks->size--;

				if (blk12->rets && blk12->rets->size > 0) {
					Obj* lastval = *(Obj**)get_list(blk12->rets, blk12->rets->size - 1);
					if (lastval->objtype == LetterType::OT_Value || lastval->objtype == LetterType::OT_VoiVal) {
						add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, end));
					} else {
						return ErrorC(thgc, LetterType::OT_Error, self->letter, "ifの値がありません");
					}
				}
				emit_rnp_dec(thgc, local);
				add_list(thgc, func->comps, (char*)end);
				(*n)++;

				if (ret->objtype == LetterType::OT_Var) ret = ((Var*)ret)->cls;
				if (ret == local->Void) {
					return make_value(thgc, ret, NULL);
				} else {
					LVari* vari;
					if (ret == local->Int) vari = make_vn(thgc, "i32");
					else if (ret == local->Bool) vari = make_vn(thgc, "i1");
					else {
						char buf[128]; snprintf(buf, sizeof(buf), "%%%sType", ret->cls ? (ret->cls->letter ? ret->cls->letter->text->data : "x") : "x");
						vari = make_vn(thgc, buf);
					}
					// Phi would be created here in full implementation
					return make_value(thgc, ret, vari);
				}
			} else {
				return ErrorC(thgc, LetterType::OT_Error, val2->letter, "else文の中身が宣言されていません。");
			}
		}
		else {
			// No else clause
			emit_rnp_dec(thgc, local);
			add_list(thgc, func->comps, (char*)end);
			local->labs->size--;
			add_list(thgc, local->labs, (char*)end);

			if (ret->objtype == LetterType::OT_Var) ret = ((Var*)ret)->cls;
			if (ret == local->Void) {
				return make_value(thgc, ret, NULL);
			} else {
				LVari* vari;
				if (ret == local->Int) vari = make_vn(thgc, "i32");
				else if (ret == local->Bool) vari = make_vn(thgc, "i1");
				else {
					char buf[128]; snprintf(buf, sizeof(buf), "%%%sType", ret->cls ? (ret->cls->letter ? ret->cls->letter->text->data : "x") : "x");
					vari = make_vn(thgc, buf);
				}
				return make_value(thgc, ret, vari);
			}
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "if文の中身が宣言されていません");
}

// ============================================================
// While::PrimaryC (lines 4374-4818)
// ============================================================
Obj* PrimaryC_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Obj* ret = self->ret;

	if (val2->objtype == LetterType::OT_CallBlock) {
		Block* blk0 = (Block*)*(Obj**)get_list(val2->children, 0);
		Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);

		const char* fname = self->letter ? self->letter->text->data : "while";
		char typename_str[256]; snprintf(typename_str, sizeof(typename_str), "%%%sWhileType%d", fname, llvm_n++);
		char checkname[256]; snprintf(checkname, sizeof(checkname), "@%sWhileCheck%d", fname, llvm_n++);

		self->model = createString(thgc, typename_str, (int)strlen(typename_str), 1);
		self->n_c = obj_cn++;

		LTypeDec* typedec; LFunc* funccheck; LVari* co_val; LVari* thgc2; LVari* i8p;
		LVari* go_c; LVari* objptr3;
		int count, order;
		setup_scope_gc(thgc, local, self, typename_str, checkname,
			&typedec, &funccheck, &co_val, &thgc2, &i8p, &go_c, &objptr3, &count, &order);

		List* decs = create_list(thgc, sizeof(Obj*), CType::_List);

		// Process vmapA for blk0 and blk2
		process_vmapA_block(thgc, local, blk0->vmapA, typedec, funccheck, co_val, thgc2, i8p,
			typename_str, &order, &count, decs, NULL, NULL, func, objptr3, go_c, false);
		process_vmapA_block(thgc, local, blk2->vmapA, typedec, funccheck, co_val, thgc2, i8p,
			typename_str, &order, &count, decs, NULL, NULL, func, objptr3, go_c, false);

		// Store local state
		local->blok = go_c;
		local->typedec_c = typedec;
		local->funccheck_c = funccheck;
		local->funcdec_c = func;
		local->i8p_c = i8p;
		local->objptr3_c = objptr3;
		local->co_val_c = co_val;
		local->odr = order;
		local->count_c = count;

		for (int i = 0; i < decs->size; i++) {
			Obj* d = *(Obj**)get_list(decs, i);
			vt_Dec(thgc, d, local);
		}

		// Return value array setup
		LVari* pv = NULL;
		if (ret->objtype == LetterType::OT_Var) ret = ((Var*)ret)->cls;
		if (ret != local->Void) {
			LVari* vcl = make_vn(thgc, "%CreateHashType");
			add_list(thgc, func->comps, (char*)make_lload(thgc, vcl, make_lvari_c(thgc, "%CreateHashType*", "@CreateHash"), false));
			pv = make_vn(thgc, "%HashType*");
			self->forvari = pv;
			bool objected = true;
			if (ret == local->Int) objected = false;
			else if (ret == local->Bool) objected = false;
			LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
			LCall* hash_call = make_lcall(thgc, pv, vcl);
			lcall_add(thgc, hash_call, thgc4);
			lcall_add(thgc, hash_call, make_lvari_c(thgc, "i1", objected ? "true" : "false"));
			add_list(thgc, func->comps, (char*)hash_call);

			// Add ret variable to typedec
			int retorder = order++;
			if (ret == local->Int) {
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, (char*)"ret", 3, 1)));
				count += 8;
			} else if (ret == local->Bool) {
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i1", 2, 1), createString(thgc, (char*)"ret", 3, 1)));
				count += 8;
			} else {
				if (ret->identity == 0) ret->identity = llvm_n++;
				char buf[128]; snprintf(buf, sizeof(buf), "%s*", ret->model ? ret->model->data : "i8");
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, buf, (int)strlen(buf), 1), createString(thgc, (char*)"ret", 3, 1)));
				count += 8;
				// GC check for ret
				LVari* vari = make_vn(thgc, "i8*");
				char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", retorder);
				add_list(thgc, funccheck->comps, (char*)make_lgete_idx2(thgc, typename_str, vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
				LVari* vari2 = make_vn(thgc, "i8*");
				add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
				LVari* co_cval = make_vn(thgc, "i8*");
				LCall* co_call = make_lcall(thgc, co_cval, co_val);
				lcall_add(thgc, co_call, thgc2);
				lcall_add(thgc, co_call, vari);
				add_list(thgc, funccheck->comps, (char*)co_call);
				add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
			}
			// Store hash to go_c
			LVari* va = make_vn(thgc, "%HashType*");
			char retbuf[32]; snprintf(retbuf, sizeof(retbuf), "%d", retorder);
			add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, typename_str, va, go_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", retbuf)));
			add_list(thgc, func->comps, (char*)make_lstore(thgc, va, pv));
		}

		char countbuf[32]; snprintf(countbuf, sizeof(countbuf), "%d", count);
		add_list(thgc, funccheck->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));

		// Execute condition block (all but last child)
		add_list(thgc, local->blocks, (char*)blk0);
		exeRangeC_Block(thgc, blk0, 0, blk0->children->size - 1, local, false);

		// Loop structure
		LLab* lab_cond = make_llab(thgc, createString(thgc, (char*)"cond", 4, 1));
		self->entry_lab = lab_cond;
		add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, lab_cond));
		add_list(thgc, func->comps, (char*)lab_cond);
		// Phi for pv would go here
		add_list(thgc, local->labs, (char*)lab_cond);

		// Execute last condition child
		Obj* last_child = *(Obj**)get_list(blk0->children, blk0->children->size - 1);
		Obj* obj = vt_exeC(thgc, last_child, local);
		local->labs->size--;

		LLab* lab_loop = make_llab(thgc, createString(thgc, (char*)"loop", 4, 1));
		LLab* lab_end = make_llab(thgc, createString(thgc, (char*)"end", 3, 1));
		self->entry_lab = lab_end;

		add_list(thgc, func->comps, (char*)make_lbr(thgc, obj->vari_c, lab_loop, lab_end));
		add_list(thgc, func->comps, (char*)lab_loop);

		// Execute loop body
		add_list(thgc, local->blocks, (char*)blk2);
		add_list(thgc, local->labs, (char*)lab_loop);
		exeC_Block(thgc, (Obj*)blk2, local);
		local->labs->size--;

		// Handle loop body return value -> AddHash + Br back to cond
		if (ret != local->Void && pv != NULL) {
			if (blk2->rets && blk2->rets->size > 0) {
				Obj* lastval = *(Obj**)get_list(blk2->rets, blk2->rets->size - 1);
				if (lastval->objtype == LetterType::OT_Value) {
					LVari* va = BitcastCast(thgc, local, ret, lastval->cls, lastval->vari_c);
					LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
					LVari* varih = make_vn(thgc, "%AddHashType");
					add_list(thgc, func->comps, (char*)make_lload(thgc, varih, make_lvari_c(thgc, "%AddHashType*", "@AddHash"), false));
					LCall* addcall = make_lcall(thgc, NULL, varih);
					lcall_add(thgc, addcall, thgc4);
					lcall_add(thgc, addcall, make_lvari_c(thgc, "%StringType*", "null"));
					lcall_add(thgc, addcall, pv);
					lcall_add(thgc, addcall, va);
					add_list(thgc, func->comps, (char*)addcall);
					add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, lab_cond));
				}
			}
		}

		local->blocks->size--;
		local->blocks->size--;
		add_list(thgc, func->comps, (char*)lab_end);
		local->labs->size--;
		add_list(thgc, local->labs, (char*)lab_end);

		// Epilogue: rnp--
		emit_rnp_dec(thgc, local);
		(*n)++;
		return make_value(thgc, ret, pv);
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "while文の中身が宣言されていません");
}

// ============================================================
// While::exepC (lines 4820-4827)
// ============================================================
Obj* exepC_While(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->letter && local->kouhos == NULL) {
		if (local->calls->size > 0) {
			typedef void (*CallbackFn)();
			CallbackFn cb = *(CallbackFn*)get_list(local->calls, local->calls->size - 1);
			if (cb) cb();
		}
	}
	return self;
}

// ============================================================
// For::PrimaryC (lines 4831-5020+)
// ============================================================
Obj* PrimaryC_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Obj* ret = self->ret;

	if (val2->objtype == LetterType::OT_CallBlock) {
		Block* blk0 = (Block*)*(Obj**)get_list(val2->children, 0);
		Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);

		const char* fname = self->letter ? self->letter->text->data : "for";
		char typename_str[256]; snprintf(typename_str, sizeof(typename_str), "%%%sForType%d", fname, llvm_n++);
		char checkname[256]; snprintf(checkname, sizeof(checkname), "@%sForCheck%d", fname, llvm_n++);

		self->model = createString(thgc, typename_str, (int)strlen(typename_str), 1);
		self->n_c = obj_cn++;

		LTypeDec* typedec; LFunc* funccheck; LVari* co_val; LVari* thgc2; LVari* i8p;
		LVari* go_c; LVari* objptr3;
		int count, order;
		setup_scope_gc(thgc, local, self, typename_str, checkname,
			&typedec, &funccheck, &co_val, &thgc2, &i8p, &go_c, &objptr3, &count, &order);

		List* decs = create_list(thgc, sizeof(Obj*), CType::_List);

		// Process vmapA for blk0 and blk2
		process_vmapA_block(thgc, local, blk0->vmapA, typedec, funccheck, co_val, thgc2, i8p,
			typename_str, &order, &count, decs, NULL, NULL, func, objptr3, go_c, false);
		process_vmapA_block(thgc, local, blk2->vmapA, typedec, funccheck, co_val, thgc2, i8p,
			typename_str, &order, &count, decs, NULL, NULL, func, objptr3, go_c, false);

		// Store local state
		local->blok = go_c;
		local->typedec_c = typedec;
		local->funccheck_c = funccheck;
		local->funcdec_c = func;
		local->i8p_c = i8p;
		local->objptr3_c = objptr3;
		local->co_val_c = co_val;
		local->odr = order;
		local->count_c = count;

		for (int i = 0; i < decs->size; i++) {
			Obj* d = *(Obj**)get_list(decs, i);
			vt_Dec(thgc, d, local);
		}

		// Return value array setup (same as while)
		LVari* pv = NULL;
		if (ret->objtype == LetterType::OT_Var) ret = ((Var*)ret)->cls;
		if (ret != local->Void) {
			LVari* vcl = make_vn(thgc, "%CreateHashType");
			add_list(thgc, func->comps, (char*)make_lload(thgc, vcl, make_lvari_c(thgc, "%CreateHashType*", "@CreateHash"), false));
			pv = make_vn(thgc, "%HashType*");
			self->forvari = pv;
			bool objected = true;
			if (ret == local->Int) objected = false;
			else if (ret == local->Bool) objected = false;
			LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
			LCall* hash_call = make_lcall(thgc, pv, vcl);
			lcall_add(thgc, hash_call, thgc4);
			lcall_add(thgc, hash_call, make_lvari_c(thgc, "i1", objected ? "true" : "false"));
			add_list(thgc, func->comps, (char*)hash_call);

			// Add ret variable to typedec
			int retorder = order++;
			if (ret == local->Int) {
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, (char*)"ret", 3, 1)));
				count += 8;
			} else if (ret == local->Bool) {
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i1", 2, 1), createString(thgc, (char*)"ret", 3, 1)));
				count += 8;
			} else {
				if (ret->identity == 0) ret->identity = llvm_n++;
				char buf[128]; snprintf(buf, sizeof(buf), "%s*", ret->model ? ret->model->data : "i8");
				add_list(thgc, ((LComp*)typedec)->comps, (char*)make_ltypeval(thgc, createString(thgc, buf, (int)strlen(buf), 1), createString(thgc, (char*)"ret", 3, 1)));
				count += 8;
				// GC check for ret
				LVari* vari = make_vn(thgc, "i8*");
				char orderbuf[32]; snprintf(orderbuf, sizeof(orderbuf), "%d", retorder);
				add_list(thgc, funccheck->comps, (char*)make_lgete_idx2(thgc, typename_str, vari, i8p, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf)));
				LVari* vari2 = make_vn(thgc, "i8*");
				add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
				LVari* co_cval = make_vn(thgc, "i8*");
				LCall* co_call = make_lcall(thgc, co_cval, co_val);
				lcall_add(thgc, co_call, thgc2);
				lcall_add(thgc, co_call, vari);
				add_list(thgc, funccheck->comps, (char*)co_call);
				add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
			}
			// Store hash to go_c
			LVari* va = make_vn(thgc, "%HashType*");
			char retbuf[32]; snprintf(retbuf, sizeof(retbuf), "%d", retorder);
			add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, typename_str, va, go_c, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", retbuf)));
			add_list(thgc, func->comps, (char*)make_lstore(thgc, va, pv));
		}

		char countbuf[32]; snprintf(countbuf, sizeof(countbuf), "%d", count);
		add_list(thgc, funccheck->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));

		// Execute init block (all but last child)
		add_list(thgc, local->blocks, (char*)blk0);
		exeRangeC_Block(thgc, blk0, 0, blk0->children->size - 1, local, false);

		// For loop structure: init -> cond -> loop body -> step -> cond
		LLab* lab_cond = make_llab(thgc, createString(thgc, (char*)"cond", 4, 1));
		self->entry_lab = lab_cond;
		add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, lab_cond));
		add_list(thgc, func->comps, (char*)lab_cond);
		add_list(thgc, local->labs, (char*)lab_cond);

		// Evaluate condition (last child of blk0)
		Obj* last_child = *(Obj**)get_list(blk0->children, blk0->children->size - 1);
		Obj* obj = vt_exeC(thgc, last_child, local);
		local->labs->size--;

		LLab* lab_loop = make_llab(thgc, createString(thgc, (char*)"loop", 4, 1));
		LLab* lab_end = make_llab(thgc, createString(thgc, (char*)"end", 3, 1));
		self->entry_lab = lab_end;

		add_list(thgc, func->comps, (char*)make_lbr(thgc, obj->vari_c, lab_loop, lab_end));
		add_list(thgc, func->comps, (char*)lab_loop);

		// Execute loop body
		add_list(thgc, local->blocks, (char*)blk2);
		add_list(thgc, local->labs, (char*)lab_loop);
		exeC_Block(thgc, (Obj*)blk2, local);
		local->labs->size--;

		// Handle loop body return value -> AddHash + Br back to cond
		if (ret != local->Void && pv != NULL) {
			if (blk2->rets && blk2->rets->size > 0) {
				Obj* lastval = *(Obj**)get_list(blk2->rets, blk2->rets->size - 1);
				if (lastval->objtype == LetterType::OT_Value) {
					LVari* va = BitcastCast(thgc, local, ret, lastval->cls, lastval->vari_c);
					LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
					LVari* varih = make_vn(thgc, "%AddHashType");
					add_list(thgc, func->comps, (char*)make_lload(thgc, varih, make_lvari_c(thgc, "%AddHashType*", "@AddHash"), false));
					LCall* addcall = make_lcall(thgc, NULL, varih);
					lcall_add(thgc, addcall, thgc4);
					lcall_add(thgc, addcall, make_lvari_c(thgc, "%StringType*", "null"));
					lcall_add(thgc, addcall, pv);
					lcall_add(thgc, addcall, va);
					add_list(thgc, func->comps, (char*)addcall);
					add_list(thgc, func->comps, (char*)make_lbr_uncond(thgc, lab_cond));
				}
			}
		}

		local->blocks->size--;
		local->blocks->size--;
		add_list(thgc, func->comps, (char*)lab_end);
		local->labs->size--;
		add_list(thgc, local->labs, (char*)lab_end);

		// Epilogue: rnp--
		emit_rnp_dec(thgc, local);
		(*n)++;
		return make_value(thgc, ret, pv);
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "for文の中身が宣言されていません");
}

// ============================================================
// End of obj2c_part2.h
// ============================================================
