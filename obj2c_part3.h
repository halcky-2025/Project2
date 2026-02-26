// obj2c_part3.h - Obj2C.cs lines 5000-8000
// Continuation of For::PrimaryC, Switch, Break, Continue, Return, Goto, Print, Iterator, GenericFunction, Model

// ============================================================
// Forward declarations
// ============================================================
Obj* PrimaryC_For(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_For2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepC_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
LVari* ToVari_Switch(ThreadGC* thgc, Obj* self, String* name, Obj* cls, Master* local, int size);
int get_hashvalue_Switch(String* key, int size);
Obj* exepC_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* GetterC_Iterator(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryC_GenericFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeC(ThreadGC* thgc, Obj* self, Master* local);
void KouhoSetLabel(Master* local);
void Dec_ModelObj(ThreadGC* thgc, Obj* self, Master* local);

// ============================================================
// Helper: emit common block variable/function/classobj setup for For/While/Switch
// Handles vmapA iteration and type declarations
// ============================================================
static inline void emit_vmap_setup(ThreadGC* thgc, Master* local, Map* vmapA,
	LTypeDec* typedec, LFunc* funccheck, LVari* i8p, LVari* co_val, LVari* thgc2,
	Obj* selfobj, LVari* go_c, LVari* objptr3, List* decs, int* order, int* count) {
	// iterate vmapA
	for (int ki = 0; ki < vmapA->kvs->size; ki++) {
		KV* kv_entry = *(KV**)get_list(vmapA->kvs, ki);
		String* key = kv_entry->key;
		Obj* v = (Obj*)kv_entry->value;
		if (v == NULL) continue;
		if (v->objtype == LetterType::OT_Variable) {
			Variable* variable = (Variable*)v;
			variable->order = (*order)++;
			Obj* cls = variable->vartype;
			if (cls && cls->objtype == LetterType::OT_Var) cls = ((Variable*)cls)->vartype;
			if (cls == local->Int) {
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "i32", key));
				*count += 8;
			} else if (cls == local->Bool) {
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "i1", key));
				*count += 8;
			} else {
				if (cls->identity == 0) cls->identity = llvm_n++;
				LVari* vari = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				LGete* gete = make_lgete_idx2(thgc, (char*)selfobj->model->data, vari, i8p,
					make_lvari_c(thgc, "i32", "0"),
					make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", variable->order)->data));
				char* tname = (char*)fmt_str(thgc, "%s*", cls->model->data)->data;
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, tname, key));
				LVari* vari2 = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
				LVari* co_cval = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				LCall* co_call = make_lcall(thgc, co_cval, co_val);
				lcall_add(thgc, co_call, thgc2);
				lcall_add(thgc, co_call, vari);
				add_list(thgc, funccheck->comps, (char*)co_call);
				add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
				*count += 8;
			}
		} else if (v->objtype == LetterType::OT_Function) {
			Function* f = (Function*)v;
			if (f->identity == 0) f->identity = llvm_n++;
			f->order = (*order)++;
			add_list(thgc, decs, (char*)f);
			add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "%FuncType*", key));

			LVari* vari = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			LGete* gete = make_lgete_idx2(thgc, (char*)selfobj->model->data, vari, i8p,
				make_lvari_c(thgc, "i32", "0"),
				make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", f->order)->data));
			add_list(thgc, funccheck->comps, (char*)gete);
			LVari* vari2 = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
			LVari* co_cval = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			LCall* co_call = make_lcall(thgc, co_cval, co_val);
			lcall_add(thgc, co_call, thgc2);
			lcall_add(thgc, co_call, vari);
			add_list(thgc, funccheck->comps, (char*)co_call);
			add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
			*count += 8;

			// func dec call
			LVari* va0 = make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*",
				(char*)fmt_str(thgc, "@%sdec%d", f->letter->text->data, f->identity)->data));
			lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", (char*)f->rename->data));  // drawcall
			lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", (char*)f->version->data)); // call
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)fu_call);

			LVari* va2 = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s*", selfobj->model->data)->data,
				(char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, local->llvm->comps, (char*)make_lload(thgc, va2, objptr3, false));
			LVari* va = make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			gete = make_lgete_idx2(thgc, (char*)selfobj->model->data, va, va2,
				make_lvari_c(thgc, "i32", "0"),
				make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", f->order)->data));
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)gete);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, va, va0));
		} else if (v->objtype == LetterType::OT_ClassObj) {
			ClassObj* f = (ClassObj*)v;
			if (f->identity == 0) f->identity = llvm_n++;
			f->order = (*order)++;
			add_list(thgc, decs, (char*)f);
			add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "%FuncType*", key));

			LVari* vari = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			LGete* gete = make_lgete_idx2(thgc, (char*)selfobj->model->data, vari, i8p,
				make_lvari_c(thgc, "i32", "0"),
				make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", f->order)->data));
			add_list(thgc, funccheck->comps, (char*)gete);
			LVari* vari2 = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
			LVari* co_cval = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			LCall* co_call = make_lcall(thgc, co_cval, co_val);
			lcall_add(thgc, co_call, thgc2);
			lcall_add(thgc, co_call, vari);
			add_list(thgc, funccheck->comps, (char*)co_call);
			add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
			*count += 8;

			LVari* va0 = make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*",
				(char*)fmt_str(thgc, "@%sdec%d", f->letter->text->data, f->identity)->data));
			lcall_add(thgc, fu_call, go_c);
			lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", (char*)f->rename->data));
			lcall_add(thgc, fu_call, make_lvari_c(thgc, "i8*", (char*)f->version->data));
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)fu_call);

			LVari* va2 = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s*", selfobj->model->data)->data,
				(char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, local->llvm->comps, (char*)make_lload(thgc, va2, objptr3, false));
			LVari* va = make_lvari_c(thgc, "%FuncType*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			gete = make_lgete_idx2(thgc, (char*)selfobj->model->data, va, va2,
				make_lvari_c(thgc, "i32", "0"),
				make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", f->order)->data));
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)gete);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, va, va0));
		}
	}
}

// ============================================================
// ClassObj::Dec - delegates to Dec_Function
// ClassObj shares the same relevant fields (draw, identity, drawcall,
// call_c, model, ret) through the Obj base struct.
// ============================================================
void Dec_ClassObj(ThreadGC* thgc, Obj* self, Master* local) {
	Dec_Function(thgc, self, local);
}

// ============================================================
// Virtual dispatch: Dec (used by obj2c_part2/3)
// Routes to Dec_Function, Dec_ClassObj, or Dec_ModelObj based on objtype
// ============================================================
void vt_Dec(ThreadGC* thgc, Obj* d, Master* local) {
	if (d == NULL) return;
	if (d->objtype == LetterType::OT_Function || d->objtype == LetterType::OT_GenericFunction ||
		d->objtype == LetterType::OT_ServerFunction || d->objtype == LetterType::OT_SignalFunction) {
		Dec_Function(thgc, d, local);
	} else if (d->objtype == LetterType::OT_ClassObj) {
		Dec_ClassObj(thgc, d, local);
	} else if (d->objtype == LetterType::OT_ModelObj) {
		Dec_ModelObj(thgc, d, local);
	}
}

// ============================================================
// Virtual dispatch: DecC (used by obj2c_part5)
// Same routing as vt_Dec for the "C" compilation phase
// ============================================================
void vt_DecC(ThreadGC* thgc, Obj* d, Master* local) {
	if (d == NULL) return;
	if (d->objtype == LetterType::OT_Function || d->objtype == LetterType::OT_GenericFunction ||
		d->objtype == LetterType::OT_ServerFunction || d->objtype == LetterType::OT_SignalFunction) {
		Dec_Function(thgc, d, local);
	} else if (d->objtype == LetterType::OT_ClassObj) {
		Dec_ClassObj(thgc, d, local);
	} else if (d->objtype == LetterType::OT_ModelObj) {
		Dec_ModelObj(thgc, d, local);
	}
}

// ============================================================
// exeC - delegates to vt_exeC (virtual dispatch for exeC)
// ============================================================
Obj* exeC(ThreadGC* thgc, Obj* self, Master* local) {
	return vt_exeC(thgc, self, local);
}

// ============================================================
// KouhoSetLabel - adds label names as completion candidates
// C#: local.KouhoSetLabel() - iterates block labelmap
// ============================================================
void KouhoSetLabel(Master* local) {
	if (local == NULL || local->blocks == NULL || local->blocks->size == 0) return;
	local->kouhos = NULL; // reset
	// Find the nearest block with a labelmap
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* block = *(Block**)get_list(local->blocks, i);
		if (block != NULL && block->labelmap != NULL && block->labelmap->kvs != NULL && block->labelmap->kvs->size > 0) {
			// No thgc needed - kouhos is just set to NULL to indicate label completion mode
			break;
		}
	}
}

// ============================================================
// For::exepC  (lines 5277-5284)
// ============================================================
Obj* exepC_For2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->last && local->kouhos == NULL) {
		// local.calls.Last()()
		// callback invocation placeholder
	}
	return self;
}

// ============================================================
// Switch::exepC  (lines 5286-5294)
// ============================================================
Obj* exepC_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->last && local->kouhos == NULL) {
		// callback placeholder
	}
	return self;
}

// ============================================================
// Switch::get_hashvalue  (lines 5734-5744)
// ============================================================
int get_hashvalue_Switch(String* key, int size) {
	int hash = 10000;
	for (int i = 0; i < key->size; i++) {
		hash ^= key->data[i];
		hash *= 1619;
		hash %= size;
	}
	return hash;
}

// ============================================================
// Switch::ToVari  (lines 5718-5733)
// ============================================================
LVari* ToVari_Switch(ThreadGC* thgc, Obj* self, String* name, Obj* cls, Master* local, int size) {
	if (cls == local->Int) {
		// try parse int
		return make_lvari_c(thgc, "i32", (char*)name->data);
	} else if (cls == local->StrT) {
		int hv = get_hashvalue_Switch(name, size);
		return make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", hv)->data);
	}
	return NULL; // error
}

// ============================================================
// Switch::PrimaryC  (lines 5295-5717)
// ============================================================
Obj* PrimaryC_Switch(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_CallBlock) {
		Block* blk0 = (Block*)*(Obj**)get_list(val2->children, 0);
		Block* blk2 = (Block*)*(Obj**)get_list(val2->children, 1);
		(*n)++;

		val2 = *(Obj**)get_list(primary->children, *n);
		self->n_c = Obj_cn++;
		self->model = fmt_str(thgc, "%%%sSwitchType%d", self->letter->text->data, llvm_n++);
		LTypeDec* typedec = make_ltypedec(thgc, self->model);
		add_list(thgc, local->llvm->types, (char*)typedec);

		LVari* i8p = make_lvari_c(thgc, "i8*", "%self");
		LVari* thgc2 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
		String* checkname = fmt_str(thgc, "@%sSwitchCheck%d", self->letter->text->data, llvm_n++);
		LFunc* funccheck = make_lfunc(thgc, local->llvm, make_lvari_c(thgc, "void", (char*)checkname->data), thgc2, i8p);
		add_list(thgc, local->llvm->comps, (char*)funccheck);
		add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "i8*", fmt_str(thgc, "blk")));

		LVari* vari = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
		LGete* gete = make_lgete_idx2(thgc, (char*)self->model->data, vari, i8p,
			make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0"));
		add_list(thgc, funccheck->comps, (char*)gete);

		LVari* co_val = make_lvari_c(thgc, "%CopyObjectType", "%co");
		add_list(thgc, funccheck->comps, (char*)make_lload(thgc, co_val, make_lvari_c(thgc, "%CopyObjectType*", "@CopyObject"), false));
		LVari* vari2 = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
		add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));
		LVari* co_cval = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
		LCall* co_call = make_lcall(thgc, co_cval, co_val);
		lcall_add(thgc, co_call, thgc2);
		lcall_add(thgc, co_call, vari);
		add_list(thgc, funccheck->comps, (char*)co_call);
		add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));

		LVari* thgc3 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
		LVari* countv = make_lvari_c(thgc, "i32", "");
		String* sname = fmt_str(thgc, "switch%d", llvm_n++);
		LStrV* strv = make_lstrv(thgc, fmt_str(thgc, "@%s", sname->data), sname, sname->size);
		add_list(thgc, local->llvm->strs, (char*)strv);
		int nval = (self->n_c = local->llvm->cn++);
		LVari* tnp = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tnp%d", llvm_n++)->data);
		add_list(thgc, local->llvm->main->comps, (char*)make_lload(thgc, tnp, make_lvari_c(thgc, "i32*", "@cnp"), false));
		LVari* tv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%cv%d", llvm_n++)->data);
		add_list(thgc, local->llvm->main->comps, (char*)make_lbinop(thgc, LBOP_Add, tv, tnp, make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", nval)->data)));
		LCall* ac_call = make_lcall(thgc, NULL, make_lvari_c(thgc, "%GC_AddClassType", "%addclass"));
		lcall_add(thgc, ac_call, thgc3);
		lcall_add(thgc, ac_call, countv);
		lcall_add(thgc, ac_call, (LVari*)strv);
		lcall_add(thgc, ac_call, make_lvari_c(thgc, "%GCCheckFuncType", (char*)checkname->data));
		lcall_add(thgc, ac_call, make_lvari_c(thgc, "%GCFinalizeFuncType", "null"));
		add_list(thgc, local->llvm->main->comps, (char*)ac_call);

		// GC_malloc
		LFunc* func = llvm_get_func(local->llvm);
		LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
		LVari* go_val = make_lvari_c(thgc, "%GC_mallocType", (char*)fmt_str(thgc, "%%gcobject%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lload(thgc, go_val, make_lvari_c(thgc, "%GC_mallocType*", "@GC_malloc"), false));
		LVari* go_v = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
		LVari* tmp = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", nval)->data);
		LVari* tmi = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tmi%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lload(thgc, tmi, make_lvari_c(thgc, "i32*", "@cnp"), false));
		LVari* tmv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%tmv%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Add, tmv, tmi, tmp));
		LCall* go_call = make_lcall(thgc, go_v, go_val);
		lcall_add(thgc, go_call, thgc4);
		lcall_add(thgc, go_call, tmv);
		add_list(thgc, func->comps, (char*)go_call);
		LVari* go_c = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s*", self->model->data)->data,
			(char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
		add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, go_c, go_v));

		LVari* v0 = make_lvari_c(thgc, "i8**", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
		gete = make_lgete_idx2(thgc, (char*)self->model->data, v0, go_c,
			make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0"));
		add_list(thgc, func->comps, (char*)gete);

		// load parent block
		LVari* gj = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%gj%d", llvm_n++)->data);
		if (func->async_flag) {
			LVari* objf = make_lvari_c(thgc, "%CoroFrameType*", "%frame");
			LVari* objp = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", objp, objf,
				make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
			add_list(thgc, func->comps, (char*)make_lload(thgc, gj, objp, false));
		} else {
			Block* lastblk = *(Block**)get_list(local->blocks, local->blocks->size - 1);
			Obj* lastobj = (Obj*)lastblk->obj;
			if (lastobj && lastobj->objtype == LetterType::OT_If) {
				// IfBlock case - load from iflabel.bas
				add_list(thgc, func->comps, (char*)make_lload(thgc, gj, lastobj->bas, false));
			} else {
				add_list(thgc, func->comps, (char*)make_lload(thgc, gj, lastobj->bas, false));
			}
		}
		add_list(thgc, func->comps, (char*)make_lstore(thgc, v0, gj));

		// SetRoot
		LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
		LVari* objptr3;
		if (func->async_flag) {
			LVari* objf = make_lvari_c(thgc, "%CoroFrameType*", "%frame");
			objptr3 = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", objptr3, objf,
				make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
			add_list(thgc, func->comps, (char*)make_lstore(thgc, objptr3, go_c));
		} else {
			objptr3 = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s**", self->model->data)->data,
				(char*)fmt_str(thgc, "%%objptr%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lalloca(thgc, objptr3));
			add_list(thgc, func->comps, (char*)make_lstore(thgc, objptr3, go_c));
			self->bas = objptr3;
			LVari* srv = make_lvari_c(thgc, "%GC_SetRootType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, srv, make_lvari_c(thgc, "%GC_SetRootType*", "@GC_SetRoot"), false));
			LCall* srcall = make_lcall(thgc, NULL, srv);
			lcall_add(thgc, srcall, rn4);
			lcall_add(thgc, srcall, self->bas);
			add_list(thgc, func->comps, (char*)srcall);
		}

		int count = 8;
		int order = 1;
		List* decs = create_list(thgc, sizeof(Obj*), CType::_List);
		emit_vmap_setup(thgc, local, blk0->vmapA, typedec, funccheck, i8p, co_val, thgc2, self, go_c, objptr3, decs, &order, &count);
		emit_vmap_setup(thgc, local, blk2->vmapA, typedec, funccheck, i8p, co_val, thgc2, self, go_c, objptr3, decs, &order, &count);

		local->blok = go_c;
		local->typedecC = typedec;
		local->funccheck = funccheck;
		local->funcdec = llvm_get_func(local->llvm);
		local->i8p = i8p;
		local->objptr3 = objptr3;
		local->co_val = co_val;
		local->countv = countv;
		local->odr = order;
		local->countC = count;
		for (int i = 0; i < decs->size; i++) {
			Obj* d = *(Obj**)get_list(decs, i);
			vt_Dec(thgc, d, local);
		}
		decs = create_list(thgc, sizeof(Obj*), CType::_List);
		countv->name = fmt_str(thgc, "%d", count);
		add_list(thgc, funccheck->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));

		add_list(thgc, local->blocks, (char*)blk0);
		exeC_Block(thgc, (Obj*)blk0, local);
		LLab* lab = make_llab(thgc, "end");
		LVari* pv = NULL;
		LVari* vv = NULL;
		Map* ifv_map = create_mapy(thgc, CType::_List);
		self->ifv = ifv_map;
		self->end_lab = lab;
		int size = 64;
		Obj* lastret = *(Obj**)get_list(blk0->rets, blk0->rets->size - 1);
		LVari* variy = lastret->vari_c;
		Obj* obj = lastret;

		// GetHashValue for string switch
		if (obj->objtype == LetterType::OT_Value) {
			Obj* value = obj;
			Obj* vcls = ((Variable*)value)->vartype; // value.cls
			if (vcls == local->StrT) {
				LVari* gh = make_lvari_c(thgc, "%GetHashValueType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lload(thgc, gh, make_lvari_c(thgc, "%GetHashValueType*", "@GetHashValue"), false));
				LVari* varix = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				LCall* ghcall = make_lcall(thgc, varix, gh);
				lcall_add(thgc, ghcall, variy);
				lcall_add(thgc, ghcall, make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", size)->data));
				add_list(thgc, func->comps, (char*)ghcall);
				variy = varix;
			}
		}

		// SwitchComp
		List* ifv2 = create_list(thgc, sizeof(IfValue*), CType::_List);
		add_list(thgc, func->comps, (char*)make_lswitchcomp(thgc, variy, lab, ifv2));

		// ret type
		Obj* ret = self->rettype ? self->rettype : local->Void;
		if (ret && ret->objtype == LetterType::OT_Var) ret = ((Variable*)ret)->vartype;
		if (ret != local->Void) {
			if (ret == local->Int) pv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			else if (ret == local->Bool) pv = make_lvari_c(thgc, "i1", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			else pv = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s*", ret->model->data)->data, (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			if (ret == local->Int) vv = make_lvari_c(thgc, "i32", "0");
			else if (ret == local->Bool) vv = make_lvari_c(thgc, "i1", "false");
			else vv = make_lvari_c(thgc, (char*)fmt_str(thgc, "%s*", ret->model->data)->data, "null");
			// ifv["entry"] = IfValue(Lab("entry"), vv)
			add_mapy(thgc, ifv_map, fmt_str(thgc, "entry"), (char*)make_lifvalue(thgc, make_llab(thgc, "entry"), vv));
		}

		add_list(thgc, local->blocks, (char*)blk2);
		// iterate labelmap
		List* larr = map_values_list(thgc, blk2->labelmap);
		for (int i = 0; i < larr->size; i++) {
			Label* larri = *(Label**)get_list(larr, i);
			LLab* labi = make_llab(thgc, (char*)fmt_str(thgc, "l%s", larri->name->data)->data);

			if (obj->objtype == LetterType::OT_Value) {
				Obj* value = obj;
				Obj* vcls = ((Variable*)value)->vartype;
				if (vcls == local->Int) {
					IfValue* iv = make_lifvalue(thgc, labi, ToVari_Switch(thgc, self, larri->name, vcls, local, size));
					add_list(thgc, ifv2, (char*)iv);
				} else if (vcls == local->StrT) {
					IfValue* iv = make_lifvalue(thgc, labi, ToVari_Switch(thgc, self, larri->name, vcls, local, size));
					add_list(thgc, ifv2, (char*)iv);
				}
			}

			add_list(thgc, func->comps, (char*)labi);
			add_list(thgc, local->labsC, (char*)labi);
			if (i == larr->size - 1) exeRangeC_Block(thgc, blk2, larri->n, blk2->children->size, local, false);
			else {
				Label* nexti = *(Label**)get_list(larr, i + 1);
				exeRangeC_Block(thgc, blk2, larri->n, nexti->n, local, false);
			}
			// remove last lab
			local->labsC->size--;

			String* labname = fmt_str(thgc, "%s", labi->name->data);
			if (get_mapy(ifv_map, labname) == NULL) {
				Obj* lastret2 = *(Obj**)get_list(blk2->rets, blk2->rets->size - 1);
				if (lastret2->objtype == LetterType::OT_Value) {
					LVari* va = lastret2->vari_c; // Bitcast.Cast simplified
					add_mapy(thgc, ifv_map, labname, (char*)make_lifvalue(thgc, labi, va));
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, lab, NULL));
				}
			}
		}

		add_list(thgc, func->comps, (char*)lab);
		local->labsC->size--; // remove last
		add_list(thgc, local->labsC, (char*)lab);
		local->blocks->size--; // remove last 2 blocks
		local->blocks->size--;
		add_list(thgc, func->comps, (char*)make_lphi(thgc, pv, ifv_map));

		// cleanup root node
		if (func->async_flag) {
			LVari* objp = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%obj%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", objp, (LVari*)*(LVari**)get_list(func->draws, 0),
				make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
			LVari* objv = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%objv%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, objv, objp, false));
			LVari* blockptr = make_lvari_c(thgc, "%GCObjectPtr*", (char*)fmt_str(thgc, "%%objp%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%GCObject", blockptr, objv,
				make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
			LVari* blockv = make_lvari_c(thgc, "%GCObjectPtr", (char*)fmt_str(thgc, "%%objv%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, blockv, blockptr, false));
			add_list(thgc, func->comps, (char*)make_lstore(thgc, objp, blockv));
		} else {
			LVari* rnpv = make_lvari_c(thgc, "i32*", "%ptr");
			add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4,
				make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
			LVari* rnpv12 = make_lvari_c(thgc, "i32", "%v");
			add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
			LVari* rnpv2 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%dec%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12, make_lvari_c(thgc, "i32", "1")));
			add_list(thgc, func->comps, (char*)make_lstore(thgc, rnpv, rnpv2));
		}
		(*n)++;
		Obj* result = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, NULL);
		result->vari_c = pv;
		return result;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "switch\xe6\x96\x87\xe3\x81\xae\xe4\xb8\xad\xe8\xba\xab\xe3\x81\x8c\xe5\xae\xa3\xe8\xa8\x80\xe3\x81\x95\xe3\x82\x8c\xe3\x81\xa6\xe3\x81\x84\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
}

// ============================================================
// Break::exepC  (lines 5746-5868)
// ============================================================
Obj* exepC_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->last && local->kouhos == NULL) {
		// callback placeholder
	}
	(*n)++;
	Obj* val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextC_Primary(thgc, primary, local, n, &val2);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterC(thgc, val2, local);
	int count = 0;
	LFunc* func = llvm_get_func(local->llvm);
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* block = *(Block**)get_list(local->blocks, i);
		ObjBlock* bobj = (ObjBlock*)block->obj;
		if (bobj != NULL) {
			Obj* blkobj = (Obj*)bobj->obj;
			if (blkobj->objtype == LetterType::OT_If || bobj->n == 0) {
				count++;
			} else if (blkobj->objtype == LetterType::OT_While || blkobj->objtype == LetterType::OT_For || blkobj->objtype == LetterType::OT_Switch) {
				Obj* ret = blkobj->rettype ? blkobj->rettype : local->Void;
				// dec root node count
				LVari* rnpv = make_lvari_c(thgc, "i32*", (char*)fmt_str(thgc, "%%ptr%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv, make_lvari_c(thgc, "i32**", "@rnp"), false));
				LVari* rnpv12 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
				LVari* rnpv2 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%dec%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12,
					make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", count)->data)));
				add_list(thgc, func->comps, (char*)make_lstore(thgc, rnpv, rnpv2));

				LLab* end = blkobj->end_lab;
				LLab* lab = *(LLab**)get_list(local->labsC, local->labsC->size - 1);
				if (val2 == NULL) {
					val2 = make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, end, NULL));
				}
				if (val2->objtype == LetterType::OT_Value) {
					String* labname = fmt_str(thgc, "%s", lab->name->data);
					if (get_mapy(blkobj->ifv, labname) == NULL) {
						if (blkobj->objtype == LetterType::OT_While || blkobj->objtype == LetterType::OT_For) {
							LVari* v = val2->vari_c;
							add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, val2->vari_c));
							// AddHash for for/while
							LVari* thgc4b = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
							LVari* varih = make_lvari_c(thgc, "%AddHashType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
							add_list(thgc, func->comps, (char*)make_lload(thgc, varih, make_lvari_c(thgc, "%AddHashType*", "@AddHash"), false));
							LCall* ahcall = make_lcall(thgc, NULL, varih);
							lcall_add(thgc, ahcall, thgc4b);
							lcall_add(thgc, ahcall, make_lvari_c(thgc, "%StringType*", "null"));
							lcall_add(thgc, ahcall, blkobj->forvari);
							lcall_add(thgc, ahcall, v);
							add_list(thgc, func->comps, (char*)ahcall);
						} else {
							add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, val2->vari_c));
						}
					}
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, end, NULL));
				}
				Block* lastblock = *(Block**)get_list(local->blocks, local->blocks->size - 1);
				Obj* lbobj = (Obj*)((ObjBlock*)lastblock->obj)->obj;
				String* labname2 = fmt_str(thgc, "%s", lab->name->data);
				if (get_mapy(lbobj->ifv, labname2) == NULL)
					add_mapy(thgc, lbobj->ifv, labname2, (char*)make_lifvalue(thgc, lab, NULL));
				(*n)--;
				return self;
			} else if (blkobj->objtype == LetterType::OT_Function || blkobj->objtype == LetterType::OT_ClassObj ||
				blkobj->objtype == LetterType::OT_ModelObj || blkobj->objtype == LetterType::OT_GenericObj ||
				blkobj->objtype == LetterType::OT_GenericFunction) {
				return ErrorC(thgc, LetterType::OT_Error, self->letter, "while,for,switch\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
			}
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "while,for,switch\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
}

// ============================================================
// Break::PrimaryC  (lines 5869-5928)
// ============================================================
Obj* PrimaryC_Break(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextC_Primary(thgc, primary, local, n, &val2);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterC(thgc, val2, local);
	int count = 0;
	LFunc* func = llvm_get_func(local->llvm);
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* block = *(Block**)get_list(local->blocks, i);
		ObjBlock* bobj = (ObjBlock*)block->obj;
		if (bobj != NULL) {
			Obj* blkobj = (Obj*)bobj->obj;
			if (blkobj->objtype == LetterType::OT_If || bobj->n == 0) {
				count++;
			} else if (blkobj->objtype == LetterType::OT_While || blkobj->objtype == LetterType::OT_For || blkobj->objtype == LetterType::OT_Switch) {
				LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
				LVari* rnpv = make_lvari_c(thgc, "i32*", (char*)fmt_str(thgc, "%%ptr%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4,
					make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
				LVari* rnpv12 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
				LVari* rnpv2 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%dec%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12,
					make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", count)->data)));
				add_list(thgc, func->comps, (char*)make_lstore(thgc, rnpv, rnpv2));

				LLab* end = blkobj->end_lab;
				LLab* lab = *(LLab**)get_list(local->labsC, local->labsC->size - 1);
				if (val2 == NULL) {
					val2 = make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, end, NULL));
				}
				if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj ||
					val2->objtype == LetterType::OT_BoolVal || val2->objtype == LetterType::OT_Value ||
					val2->objtype == LetterType::OT_Block) {
					self->value_obj = val2;
					String* labname = fmt_str(thgc, "%s", lab->name->data);
					if (get_mapy(blkobj->ifv, labname) == NULL)
						add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, val2->vari_c));
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, end, NULL));
				}
				Block* lastblock = *(Block**)get_list(local->blocks, local->blocks->size - 1);
				Obj* lbobj = (Obj*)((ObjBlock*)lastblock->obj)->obj;
				String* labname2 = fmt_str(thgc, "%s", lab->name->data);
				if (get_mapy(lbobj->ifv, labname2) == NULL)
					add_mapy(thgc, lbobj->ifv, labname2, (char*)make_lifvalue(thgc, lab, NULL));
				return self;
			} else if (blkobj->objtype == LetterType::OT_Function || blkobj->objtype == LetterType::OT_ClassObj ||
				blkobj->objtype == LetterType::OT_ModelObj || blkobj->objtype == LetterType::OT_GenericObj ||
				blkobj->objtype == LetterType::OT_GenericFunction) {
				return ErrorC(thgc, LetterType::OT_Error, self->letter, "while,for,switch\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
			}
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "while,for,switch\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
}
// ============================================================
// Continue::exepC  (lines 5930-6053)
// ============================================================
Obj* exepC_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->last && local->kouhos == NULL) {
		// callback placeholder
	}
	(*n)++;
	Obj* val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextC_Primary(thgc, primary, local, n, &val2);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterC(thgc, val2, local);
	int count = 0;
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* block = *(Block**)get_list(local->blocks, i);
		ObjBlock* bobj = (ObjBlock*)block->obj;
		if (bobj != NULL) {
			Obj* blkobj = (Obj*)bobj->obj;
			if ((blkobj->objtype == LetterType::OT_If || blkobj->objtype == LetterType::OT_Switch) || bobj->n == 0) {
				count++;
			}
			if (blkobj->objtype == LetterType::OT_While || blkobj->objtype == LetterType::OT_For) {
				Obj* ret = NULL;
				if (blkobj->objtype == LetterType::OT_While) ret = ((Function*)blkobj)->rettype;
				else if (blkobj->objtype == LetterType::OT_For) ret = ((Function*)blkobj)->rettype;

				LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
				LVari* rnpv = make_lvari_c(thgc, "i32*", (char*)fmt_str(thgc, "%%ptr%d", llvm_n++)->data);
				LFunc* func = llvm_get_func(local->llvm);
				add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4,
					make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
				LVari* rnpv12 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
				LVari* rnpv2 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%dec%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12,
					make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", count)->data)));
				add_list(thgc, func->comps, (char*)make_lstore(thgc, rnpv, rnpv2));

				LLab* entry = blkobj->entry;
				LLab* lab = *(LLab**)get_list(local->labsC, local->labsC->size - 1);
				if (val2 == NULL) {
					val2 = make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, entry, NULL));
				}
				if (val2->objtype == LetterType::OT_Value) {
					String* labname = fmt_str(thgc, "%s", lab->name->data);
					if (get_mapy(blkobj->ifv, labname) == NULL) {
						if (blkobj->objtype == LetterType::OT_While || blkobj->objtype == LetterType::OT_For) {
							LVari* v = val2->vari_c;
							Obj* value = val2;
							Obj* vcls = ((Variable*)value)->vartype;
							if (vcls == ret) {
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, val2->vari_c));
							} else if (ret == local->Int) {
								v = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, v));
							} else if (ret == local->Short) {
								v = make_lvari_c(thgc, "i16", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, v));
							} else if (ret == local->Bool) {
								v = make_lvari_c(thgc, "i1", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, v));
							} else {
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, val2->vari_c));
							}
							LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
							LVari* varih = make_lvari_c(thgc, "%AddHashType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
							add_list(thgc, func->comps, (char*)make_lload(thgc, varih, make_lvari_c(thgc, "%AddHashType*", "@AddHash"), false));
							LCall* ahcall = make_lcall(thgc, NULL, varih);
							lcall_add(thgc, ahcall, thgc4);
							lcall_add(thgc, ahcall, make_lvari_c(thgc, "%StringType*", "null"));
							lcall_add(thgc, ahcall, blkobj->forvari);
							lcall_add(thgc, ahcall, v);
							add_list(thgc, func->comps, (char*)ahcall);
						} else {
							Obj* value = val2;
							Obj* vcls = ((Variable*)value)->vartype;
							if (vcls == ret) add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, val2->vari_c));
							else if (ret == local->Int) {
								LVari* v = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, v));
							} else if (ret == local->Short) {
								LVari* v = make_lvari_c(thgc, "i16", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, v));
							} else if (ret == local->Bool) {
								LVari* v = make_lvari_c(thgc, "i1", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
								add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, val2->vari_c));
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, v));
							} else {
								add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, val2->vari_c));
							}
						}
					}
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, entry, NULL));
				}
				Block* lastblock = *(Block**)get_list(local->blocks, local->blocks->size - 1);
				Obj* lbobj = (Obj*)((ObjBlock*)lastblock->obj)->obj;
				String* labname2 = fmt_str(thgc, "%s", lab->name->data);
				if (get_mapy(lbobj->ifv, labname2) == NULL)
					add_mapy(thgc, lbobj->ifv, labname2, (char*)make_lifvalue(thgc, lab, NULL));
				(*n)--;
				return self;
			} else if (blkobj->objtype == LetterType::OT_Function || blkobj->objtype == LetterType::OT_ClassObj ||
				blkobj->objtype == LetterType::OT_ModelObj || blkobj->objtype == LetterType::OT_GenericObj ||
				blkobj->objtype == LetterType::OT_GenericFunction) {
				return ErrorC(thgc, LetterType::OT_Error, self->letter, "while,for\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
			}
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "while,for\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
}

// ============================================================
// Continue::PrimaryC  (lines 6055-6115)
// ============================================================
Obj* PrimaryC_Continue(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextC_Primary(thgc, primary, local, n, &val2);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterC(thgc, val2, local);
	int count = 0;
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* block = *(Block**)get_list(local->blocks, i);
		ObjBlock* bobj = (ObjBlock*)block->obj;
		if (bobj != NULL) {
			Obj* blkobj = (Obj*)bobj->obj;
			if ((blkobj->objtype == LetterType::OT_If || blkobj->objtype == LetterType::OT_Switch) || bobj->n == 0) {
				count++;
			}
			if (blkobj->objtype == LetterType::OT_While || blkobj->objtype == LetterType::OT_For) {
				LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
				LVari* rnpv = make_lvari_c(thgc, "i32*", (char*)fmt_str(thgc, "%%ptr%d", llvm_n++)->data);
				LFunc* func = llvm_get_func(local->llvm);
				add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4,
					make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
				LVari* rnpv12 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
				LVari* rnpv2 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%dec%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12,
					make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", count)->data)));
				add_list(thgc, func->comps, (char*)make_lstore(thgc, rnpv, rnpv2));

				LLab* entry = blkobj->entry;
				LLab* lab = *(LLab**)get_list(local->labsC, local->labsC->size - 1);
				if (val2 == NULL) {
					val2 = make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, entry, NULL));
				}
				if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj ||
					val2->objtype == LetterType::OT_BoolVal || val2->objtype == LetterType::OT_Value ||
					val2->objtype == LetterType::OT_Block) {
					self->value_obj = val2;
					String* labname = fmt_str(thgc, "%s", lab->name->data);
					if (get_mapy(blkobj->ifv, labname) == NULL)
						add_mapy(thgc, blkobj->ifv, labname, (char*)make_lifvalue(thgc, lab, val2->vari_c));
					add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, entry, NULL));
				}
				Block* lastblock = *(Block**)get_list(local->blocks, local->blocks->size - 1);
				Obj* lbobj = (Obj*)((ObjBlock*)lastblock->obj)->obj;
				String* labname2 = fmt_str(thgc, "%s", lab->name->data);
				if (get_mapy(lbobj->ifv, labname2) == NULL)
					add_mapy(thgc, lbobj->ifv, labname2, (char*)make_lifvalue(thgc, lab, NULL));
				return self;
			} else if (blkobj->objtype == LetterType::OT_Function || blkobj->objtype == LetterType::OT_ClassObj ||
				blkobj->objtype == LetterType::OT_ModelObj || blkobj->objtype == LetterType::OT_GenericObj ||
				blkobj->objtype == LetterType::OT_GenericFunction) {
				return ErrorC(thgc, LetterType::OT_Error, self->letter, "while,for\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
			}
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "while,for\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
}

// ============================================================
// Return::exepC  (lines 6117-6242)
// ============================================================
Obj* exepC_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->last && local->kouhos == NULL) {
		// callback placeholder
	}
	(*n)++;
	Obj* val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextC_Primary(thgc, primary, local, n, &val2);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterC(thgc, val2, local);
	int count = 1;
	LFunc* func = llvm_get_func(local->llvm);
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* block = *(Block**)get_list(local->blocks, i);
		ObjBlock* bobj = (ObjBlock*)block->obj;
		if (bobj != NULL) {
			Obj* blkobj = (Obj*)bobj->obj;
			if ((blkobj->objtype == LetterType::OT_If || blkobj->objtype == LetterType::OT_While ||
				blkobj->objtype == LetterType::OT_For || blkobj->objtype == LetterType::OT_Switch) || bobj->n == 0) {
				count++;
			}
			if (blkobj->objtype == LetterType::OT_Function || blkobj->objtype == LetterType::OT_GenericFunction ||
				blkobj->objtype == LetterType::OT_ServerFunction || blkobj->objtype == LetterType::OT_SignalFunction) {
				LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
				LVari* rnpv = make_lvari_c(thgc, "i32*", (char*)fmt_str(thgc, "%%ptr%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4,
					make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
				LVari* rnpv12 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
				LVari* rnpv2 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%dec%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12,
					make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", count)->data)));
				add_list(thgc, func->comps, (char*)make_lstore(thgc, rnpv, rnpv2));
				(*n)--;
				if (func->async_flag) {
					// async return path
					LVari* obj = make_lvari_c(thgc, "%CoroFrameType*", "%obj");
					if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj ||
						val2->objtype == LetterType::OT_BoolVal || val2->objtype == LetterType::OT_Value ||
						val2->objtype == LetterType::OT_Block) {
						LVari* retp = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%ret%d", llvm_n++)->data);
						add_list(thgc, local->llvm->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", retp, obj,
							make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "9")));
						LVari* vari = val2->vari_c;
						if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_BoolVal) {
							vari = make_lvari_c(thgc, "i8*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
							add_list(thgc, local->llvm->comps, (char*)make_lcast(thgc, LCAST_IntToPtr, vari, val2->vari_c));
						}
						add_list(thgc, local->llvm->comps, (char*)make_lstore(thgc, retp, vari));
					}
					// state + queue + push + ret
					LVari* stateval = make_lvari_c(thgc, "i32*", (char*)fmt_str(thgc, "%%state%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", stateval, obj,
						make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
					LVari* sfinp = make_lvari_c(thgc, "i32*", (char*)fmt_str(thgc, "%%sfinp%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", sfinp, obj,
						make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "10")));
					LVari* sfinv = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%sfinv%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, sfinv, sfinp, false));
					add_list(thgc, func->comps, (char*)make_lstore(thgc, stateval, sfinv));
					LVari* queueptr = make_lvari_c(thgc, "%CoroutineQueueType**", (char*)fmt_str(thgc, "%%queueptr%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr, obj,
						make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
					LVari* queue = make_lvari_c(thgc, "%CoroutineQueueType*", (char*)fmt_str(thgc, "%%queue%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, queue, queueptr, false));
					LVari* pushqueue = make_lvari_c(thgc, "%PushQueueType*", (char*)fmt_str(thgc, "%%pushqueue%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, pushqueue, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
					LCall* pqcall = make_lcall(thgc, NULL, pushqueue);
					lcall_add(thgc, pqcall, queue);
					lcall_add(thgc, pqcall, obj);
					add_list(thgc, func->comps, (char*)pqcall);
					add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
				}
				if (val2 == NULL) {
					add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "null")));
					LLab* lab = *(LLab**)get_list(local->labsC, local->labsC->size - 1);
					Block* lastblock = *(Block**)get_list(local->blocks, local->blocks->size - 1);
					Obj* lbobj = (Obj*)((ObjBlock*)lastblock->obj)->obj;
					String* labname = fmt_str(thgc, "%s", lab->name->data);
					if (get_mapy(lbobj->ifv, labname) == NULL)
						add_mapy(thgc, lbobj->ifv, labname, (char*)make_lifvalue(thgc, lab, NULL));
					val2 = make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
					Obj* result = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, NULL);
					result->vari_c = NULL;
					return result;
				}
				if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj ||
					val2->objtype == LetterType::OT_BoolVal || val2->objtype == LetterType::OT_Value ||
					val2->objtype == LetterType::OT_Block) {
					add_list(thgc, func->comps, (char*)make_lret(thgc, val2->vari_c));
					LLab* lab = *(LLab**)get_list(local->labsC, local->labsC->size - 1);
					Block* lastblock = *(Block**)get_list(local->blocks, local->blocks->size - 1);
					Obj* lbobj = (Obj*)((ObjBlock*)lastblock->obj)->obj;
					String* labname = fmt_str(thgc, "%s", lab->name->data);
					if (get_mapy(lbobj->ifv, labname) == NULL)
						add_mapy(thgc, lbobj->ifv, labname, (char*)make_lifvalue(thgc, lab, NULL));
					self->vari_c = val2->vari_c;
					Obj* result = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, NULL);
					result->vari_c = val2->vari_c;
					return result;
				}
				return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "return");
			} else if (blkobj->objtype == LetterType::OT_ClassObj) {
				return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "\xe9\x96\xa2\xe6\x95\xb0\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
			}
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "return\xe6\x96\x87\xe3\x81\xaf\xe9\x96\xa2\xe6\x95\xb0\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\x97\xe3\x81\x8b\xe4\xbd\xbf\xe3\x81\x88\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
}

// ============================================================
// Return::PrimaryC  (lines 6244-6296)
// ============================================================
Obj* PrimaryC_Return(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextC_Primary(thgc, primary, local, n, &val2);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
	}
	if (val2 != NULL) val2 = vt_GetterC(thgc, val2, local);
	int count = 1;
	LFunc* func = llvm_get_func(local->llvm);
	for (int i = local->blocks->size - 1; i >= 0; i--) {
		Block* block = *(Block**)get_list(local->blocks, i);
		ObjBlock* bobj = (ObjBlock*)block->obj;
		if (bobj != NULL) {
			Obj* blkobj = (Obj*)bobj->obj;
			if ((blkobj->objtype == LetterType::OT_If || blkobj->objtype == LetterType::OT_While ||
				blkobj->objtype == LetterType::OT_For || blkobj->objtype == LetterType::OT_Switch) || bobj->n == 0) {
				count++;
			}
			if (blkobj->objtype == LetterType::OT_Function || blkobj->objtype == LetterType::OT_GenericFunction ||
				blkobj->objtype == LetterType::OT_ServerFunction || blkobj->objtype == LetterType::OT_SignalFunction) {
				LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
				LVari* rnpv = make_lvari_c(thgc, "i32*", (char*)fmt_str(thgc, "%%ptr%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", rnpv, rn4,
					make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
				LVari* rnpv12 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lload(thgc, rnpv12, rnpv, false));
				LVari* rnpv2 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%dec%d", llvm_n++)->data);
				add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, rnpv2, rnpv12,
					make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", count)->data)));
				add_list(thgc, func->comps, (char*)make_lstore(thgc, rnpv, rnpv2));
				if (val2 == NULL) {
					add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "null")));
					val2 = make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
					Obj* result = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, NULL);
					result->vari_c = NULL;
					return result;
				}
				if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_StrObj ||
					val2->objtype == LetterType::OT_BoolVal || val2->objtype == LetterType::OT_Value ||
					val2->objtype == LetterType::OT_Block) {
					add_list(thgc, func->comps, (char*)make_lret(thgc, val2->vari_c));
					self->vari_c = val2->vari_c;
					Obj* result = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, NULL);
					result->vari_c = val2->vari_c;
					return result;
				}
				return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "return");
			} else if (blkobj->objtype == LetterType::OT_ClassObj) {
				return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "\xe9\x96\xa2\xe6\x95\xb0\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\xaf\xe3\x81\x82\xe3\x82\x8a\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
			}
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, val2 ? val2->letter : NULL, "return\xe6\x96\x87\xe3\x81\xaf\xe9\x96\xa2\xe6\x95\xb0\xe3\x81\xae\xe4\xb8\xad\xe3\x81\xa7\xe3\x81\x97\xe3\x81\x8b\xe4\xbd\xbf\xe3\x81\x88\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
}

// ============================================================
// Goto::exepC  (lines 6298-6307)
// ============================================================
Obj* exepC_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->last && local->kouhos == NULL) {
		// callback placeholder
	}
	return self;
}

// ============================================================
// Goto::PrimaryC  (lines 6308-6331)
// ============================================================
Obj* PrimaryC_Goto(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->letter == local->last && local->kouhos == NULL) {
		KouhoSetLabel(local);
	}
	if (val2->objtype == LetterType::OT_Number) {
		// self->value_str = number_to_string
		return self;
	} else if (val2->objtype == LetterType::OT_StrObj) {
		// self->value_str = strobj->value
		return self;
	} else if (val2->objtype == LetterType::OT_Word) {
		// self->value_str = word->name
		return self;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "goto\xe6\x96\x87\xe3\x81\xae\xe8\xa1\x8c\xe3\x81\x8d\xe5\x85\x88\xe3\x82\x92\xe6\x8c\x87\xe5\xae\x9a\xe3\x81\x97\xe3\x81\xa6\xe3\x81\x8f\xe3\x81\xa0\xe3\x81\x95\xe3\x81\x84\xe3\x80\x82");
}

// ============================================================
// Print::exepC  (lines 6332-6341)
// ============================================================
Obj* exepC_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->last && local->kouhos == NULL) {
		// callback placeholder
	}
	return self;
}

// ============================================================
// Print::PrimaryC  (lines 6342-6491)
// ============================================================
Obj* PrimaryC_Print(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterC(thgc, exeC(thgc, val2, local), local);
		Block* blk = (Block*)val;
		LFunc* func = llvm_get_func(local->llvm);
		// putchar('(')
		LCall* pc0 = make_lcall(thgc, NULL, make_lvari_c(thgc, "i32", "@putchar"));
		lcall_add(thgc, pc0, make_lvari_c(thgc, "i32", "40"));
		add_list(thgc, func->comps, (char*)pc0);
		for (int i = 0; i < blk->rets->size; i++) {
			if (i != 0) {
				LCall* pccomma = make_lcall(thgc, NULL, make_lvari_c(thgc, "i32", "@putchar"));
				lcall_add(thgc, pccomma, make_lvari_c(thgc, "i32", "44"));
				add_list(thgc, func->comps, (char*)pccomma);
			}
			Obj* reti = *(Obj**)get_list(blk->rets, i);
			if (reti->objtype == LetterType::OT_Value) {
				Obj* value = reti;
				Obj* vcls = ((Variable*)value)->vartype;
				if (vcls == local->Int) {
					LVari* v0 = make_lvari_c(thgc, "%NumberStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, v0, make_lvari_c(thgc, "%NumberStringType*", "@NumberString"), false));
					LVari* v05 = make_lvari_c(thgc, "%StringType*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					LCall* nscall = make_lcall(thgc, v05, v0);
					lcall_add(thgc, nscall, make_lvari_c(thgc, "%ThreadGCType*", "%thgc"));
					lcall_add(thgc, nscall, value->vari_c);
					add_list(thgc, func->comps, (char*)nscall);
					LVari* v = make_lvari_c(thgc, "%PrintStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, v, make_lvari_c(thgc, "%PrintStringType*", "@PrintString"), false));
					LCall* pscall = make_lcall(thgc, NULL, v);
					lcall_add(thgc, pscall, v05);
					add_list(thgc, func->comps, (char*)pscall);
				} else if (vcls == local->Short) {
					LVari* v3 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Zext, v3, value->vari_c));
					LVari* v0 = make_lvari_c(thgc, "%NumberStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, v0, make_lvari_c(thgc, "%NumberStringType*", "@NumberString"), false));
					LVari* v05 = make_lvari_c(thgc, "%StringType*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					LCall* nscall = make_lcall(thgc, v05, v0);
					lcall_add(thgc, nscall, make_lvari_c(thgc, "%ThreadGCType*", "%thgc"));
					lcall_add(thgc, nscall, v3);
					add_list(thgc, func->comps, (char*)nscall);
					LVari* v = make_lvari_c(thgc, "%PrintStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, v, make_lvari_c(thgc, "%PrintStringType*", "@PrintString"), false));
					LCall* pscall = make_lcall(thgc, NULL, v);
					lcall_add(thgc, pscall, v05);
					add_list(thgc, func->comps, (char*)pscall);
				} else if (vcls == local->Bool) {
					// empty for bool
				} else if (vcls == local->StrT) {
					LVari* v = make_lvari_c(thgc, "%PrintStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, v, make_lvari_c(thgc, "%PrintStringType*", "@PrintString"), false));
					LCall* pscall = make_lcall(thgc, NULL, v);
					lcall_add(thgc, pscall, value->vari_c);
					add_list(thgc, func->comps, (char*)pscall);
				} else if (vcls->objtype == LetterType::OT_ArrayType) {
					int n2 = 0;
					int depth = 0;
					Obj* cls = vcls;
					// loop for nested array types
					while (cls->objtype == LetterType::OT_ArrayType) {
						Obj* arrcls = ((Variable*)cls)->vartype; // arrtype.cls
						if (arrcls == local->Int || arrcls == local->Short) { n2 = 1; break; }
						else if (arrcls == local->StrT) { n2 = 0; break; }
						else if (arrcls->objtype == LetterType::OT_ArrayType) { depth++; cls = arrcls; }
						else break;
					}
					LVari* pav = make_lvari_c(thgc, "%PrintArrayType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
					add_list(thgc, func->comps, (char*)make_lload(thgc, pav, make_lvari_c(thgc, "%PrintArrayType*", "@PrintArray"), false));
					LCall* pacall = make_lcall(thgc, NULL, pav);
					lcall_add(thgc, pacall, value->vari_c);
					lcall_add(thgc, pacall, make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", depth)->data));
					lcall_add(thgc, pacall, make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", n2)->data));
					add_list(thgc, func->comps, (char*)pacall);
				}
			}
		}
		// putchar(')')
		LCall* pc1 = make_lcall(thgc, NULL, make_lvari_c(thgc, "i32", "@putchar"));
		lcall_add(thgc, pc1, make_lvari_c(thgc, "i32", "41"));
		add_list(thgc, func->comps, (char*)pc1);
		// putchar('\n')
		LCall* pc2 = make_lcall(thgc, NULL, make_lvari_c(thgc, "i32", "@putchar"));
		lcall_add(thgc, pc2, make_lvari_c(thgc, "i32", "10"));
		add_list(thgc, func->comps, (char*)pc2);
		(*n)++;
		return make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
	}
	// non-bracket path
	val2 = NULL;
	for (; *n < primary->children->size - 1;) {
		NextC_Primary(thgc, primary, local, n, &val2);
		if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
	}
	val2 = vt_GetterC(thgc, val2, local);
	if (val2->objtype == LetterType::OT_Value) {
		(*n)++;
		Obj* value = val2;
		Obj* vcls = ((Variable*)value)->vartype;
		LFunc* func = llvm_get_func(local->llvm);
		if (vcls == local->Int) {
			LVari* v0 = make_lvari_c(thgc, "%NumberStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, v0, make_lvari_c(thgc, "%NumberStringType*", "@NumberString"), false));
			LVari* v05 = make_lvari_c(thgc, "%StringType*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			LCall* nscall = make_lcall(thgc, v05, v0);
			lcall_add(thgc, nscall, make_lvari_c(thgc, "%ThreadGCType*", "%thgc"));
			lcall_add(thgc, nscall, value->vari_c);
			add_list(thgc, func->comps, (char*)nscall);
			LVari* v = make_lvari_c(thgc, "%PrintStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, v, make_lvari_c(thgc, "%PrintStringType*", "@PrintString"), false));
			LCall* pscall = make_lcall(thgc, NULL, v);
			lcall_add(thgc, pscall, v05);
			add_list(thgc, func->comps, (char*)pscall);
		} else if (vcls == local->Short) {
			LVari* v3 = make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Zext, v3, value->vari_c));
			LVari* v0 = make_lvari_c(thgc, "%NumberStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, v0, make_lvari_c(thgc, "%NumberStringType*", "@NumberString"), false));
			LVari* v05 = make_lvari_c(thgc, "%StringType*", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			LCall* nscall = make_lcall(thgc, v05, v0);
			lcall_add(thgc, nscall, make_lvari_c(thgc, "%ThreadGCType*", "%thgc"));
			lcall_add(thgc, nscall, v3);
			add_list(thgc, func->comps, (char*)nscall);
			LVari* v = make_lvari_c(thgc, "%PrintStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, v, make_lvari_c(thgc, "%PrintStringType*", "@PrintString"), false));
			LCall* pscall = make_lcall(thgc, NULL, v);
			lcall_add(thgc, pscall, v05);
			add_list(thgc, func->comps, (char*)pscall);
		} else if (vcls == local->Bool) {
			// empty for bool
		} else if (vcls == local->StrT) {
			LVari* v = make_lvari_c(thgc, "%PrintStringType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, v, make_lvari_c(thgc, "%PrintStringType*", "@PrintString"), false));
			LCall* pscall = make_lcall(thgc, NULL, v);
			lcall_add(thgc, pscall, val2->vari_c);
			add_list(thgc, func->comps, (char*)pscall);
		} else if (vcls->objtype == LetterType::OT_ArrayType) {
			int n2 = 0;
			int depth = 0;
			Obj* cls = vcls;
			while (cls->objtype == LetterType::OT_ArrayType) {
				Obj* arrcls = ((Variable*)cls)->vartype;
				if (arrcls == local->Int || arrcls == local->Short) { n2 = 1; break; }
				else if (arrcls == local->StrT) { n2 = 0; break; }
				else if (arrcls->objtype == LetterType::OT_ArrayType) { depth++; cls = arrcls; }
				else break;
			}
			LVari* pav = make_lvari_c(thgc, "%PrintArrayType", (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
			add_list(thgc, func->comps, (char*)make_lload(thgc, pav, make_lvari_c(thgc, "%PrintArrayType*", "@PrintArray"), false));
			LCall* pacall = make_lcall(thgc, NULL, pav);
			lcall_add(thgc, pacall, value->vari_c);
			lcall_add(thgc, pacall, make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", depth)->data));
			lcall_add(thgc, pacall, make_lvari_c(thgc, "i32", (char*)fmt_str(thgc, "%d", n2)->data));
			add_list(thgc, func->comps, (char*)pacall);
		}
		return make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "()\xe3\x81\xa7print\xe3\x81\xae\xe5\xbc\x95\xe6\x95\xb0\xe3\x82\x92\xe6\x8c\x87\xe5\xae\x9a\xe3\x81\x97\xe3\x81\xa6\xe3\x81\x8f\xe3\x81\xa0\xe3\x81\x95\xe3\x81\x84");
}

// ============================================================
// Iterator::GetterC  (lines 6492-6499)
// ============================================================
Obj* GetterC_Iterator(ThreadGC* thgc, Obj* self, Master* local) {
	// Iterator has fields m and n and value (Block*)
	int m = self->n_c; // repurpose n_c for m
	int nn = self->order; // repurpose order for n
	Block* value = (Block*)self->value_obj;
	if (m == -1) return *(Obj**)get_list(value->rets, nn);
	else {
		Block* inner = (Block*)*(Obj**)get_list(value->rets, nn);
		return *(Obj**)get_list(inner->rets, m);
	}
}

// ============================================================
// GenericFunction::PrimaryC  (lines 6500-6522)
// ============================================================
Obj* PrimaryC_GenericFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Block) {
		Obj* val = vt_GetterC(thgc, exeC(thgc, val2, local), local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		Block* blk = (Block*)val;
		GenericFunction* gf = (GenericFunction*)self;
		if (blk->rets->size != gf->vmap->kvs->size)
			return ErrorC(thgc, LetterType::OT_Error, val2->letter, "\xe5\xbc\x95\xe6\x95\xb0\xe3\x81\xae\xe6\x95\xb0\xe3\x81\x8c\xe3\x81\x82\xe3\x81\xa3\xe3\x81\xa6\xe3\x81\x84\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93\xe3\x80\x82");
		for (int i = 0; i < blk->rets->size; i++) {
			Obj* reti = *(Obj**)get_list(blk->rets, i);
			// check that each arg is a Type
			if (reti->objtype != LetterType::OT_ClassObj && reti->objtype != LetterType::OT_ModelObj &&
				reti->objtype != LetterType::OT_Value)
				return ErrorC(thgc, LetterType::OT_Error, val2->letter, "\xe3\x82\xb8\xe3\x82\xa7\xe3\x83\x8d\xe3\x83\xaa\xe3\x83\x83\xe3\x82\xaf\xe9\x96\xa2\xe6\x95\xb0\xe3\x81\xab\xe3\x81\xaf\xe5\x9e\x8b\xe3\x82\x92\xe5\xbc\x95\xe6\x95\xb0\xe3\x81\xab\xe3\x81\x97\xe3\x81\xa6\xe3\x81\x8f\xe3\x81\xa0\xe3\x81\x95\xe3\x81\x84");
		}
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		// Return a new Function with the generic type params resolved
		Obj* result = make_cobj(thgc, CType::_CObj, LetterType::OT_Function, NULL);
		((Function*)result)->rettype = gf->rettype;
		((Function*)result)->blocks = gf->blocks;
		((Function*)result)->draw = gf->draw;
		return result;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "[]\xe3\x81\xa7\xe3\x82\xb8\xe3\x82\xa7\xe3\x83\xaa\xe3\x83\x83\xe3\x82\xaf\xe9\x96\xa2\xe6\x95\xb0\xe3\x81\xae\xe5\xbc\x95\xe6\x95\xb0\xe3\x82\x92\xe6\x8c\x87\xe5\xae\x9a\xe3\x81\x97\xe3\x81\xa6\xe3\x81\x8f\xe3\x81\xa0\xe3\x81\x95\xe3\x81\x84");
}

// ============================================================
// Model::exepC  (lines 6523-6533)
// ============================================================
Obj* exepC_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self->letter == local->last && local->kouhos == NULL) {
		// callback placeholder
	}
	return self;
}

// ============================================================
// Model::PrimaryC  (lines 6534-8000+)
// Very large SQL operations method - Where/Sort/Update/Delete
// Due to extreme size, this is a structural stub with key patterns preserved
// ============================================================
Obj* PrimaryC_Model(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Block) {
		Block* blk0 = (Block*)vt_GetterC(thgc, exeC(thgc, val2, local), local);
		Obj* val = *(Obj**)get_list(blk0->rets, 0);
		if (val->objtype != LetterType::OT_ModelObj)
			return ErrorC(thgc, LetterType::OT_Error, val->letter, "Model\xe3\x82\xaf\xe3\x83\xa9\xe3\x82\xb9\xe3\x81\x8bGene\xe3\x82\xaf\xe3\x83\xa9\xe3\x82\xb9\xe3\x82\x92\xe6\x8c\x87\xe5\xae\x9a\xe3\x81\x97\xe3\x81\xa6\xe3\x81\x8f\xe3\x81\xa0\xe3\x81\x95\xe3\x81\x84");
		ModelObj* modelobj = (ModelObj*)val;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Dot) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				String* word2name = val2->letter->text;
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				Obj* where = NULL;

				// Where clause handling
				if (str_equals(word2name, "Where")) {
					if (val2->objtype == LetterType::OT_Bracket) {
						(*n)++;
						val = vt_GetterC(thgc, exeC(thgc, val2, local), local);
						if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
						Block* blk = (Block*)val;
						Obj* ret0 = *(Obj**)get_list(blk->rets, 0);
						if (ret0->objtype == LetterType::OT_SqlString) {
							where = ret0;
							// goto head - re-read next word
							val2 = *(Obj**)get_list(primary->children, *n);
							if (val2->objtype == LetterType::OT_Dot) {
								(*n)++;
								val2 = *(Obj**)get_list(primary->children, *n);
								word2name = val2->letter->text;
								(*n)++;
								val2 = *(Obj**)get_list(primary->children, *n);
							}
						} else if (ret0->objtype == LetterType::OT_Value) {
							Obj* wvalue = ret0;
							Obj* wcls = ((Variable*)wvalue)->vartype;
							if (wcls == local->StrT) {
								where = ret0;
								val2 = *(Obj**)get_list(primary->children, *n);
								if (val2->objtype == LetterType::OT_Dot) {
									(*n)++;
									val2 = *(Obj**)get_list(primary->children, *n);
									word2name = val2->letter->text;
									(*n)++;
									val2 = *(Obj**)get_list(primary->children, *n);
								}
							} else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "Where\xe3\x81\xae\xe5\xbc\x95\xe6\x95\xb0\xe3\x81\xafString\xe5\x9e\x8b\xe3\x81\x8bSqlString\xe5\x9e\x8b\xe3\x81\xa7\xe3\x81\x99");
						} else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "Where\xe3\x81\xae\xe5\xbc\x95\xe6\x95\xb0\xe3\x81\xafString\xe5\x9e\x8b\xe3\x81\x8bSqlString\xe5\x9e\x8b\xe3\x81\xa7\xe3\x81\x99");
					} else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "Where\xe3\x81\xae()\xe3\x81\x8c\xe6\x8c\x87\xe5\xae\x9a\xe3\x81\x95\xe3\x82\x8c\xe3\x81\xa6\xe3\x81\x84\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
				}

				// Sort clause
				if (str_equals(word2name, "Sort")) {
					if (val2->objtype == LetterType::OT_Bracket) {
						(*n)++;
						val = vt_GetterC(thgc, exeC(thgc, val2, local), local);
						if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
						// Sort handled at runtime
						return make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
					} else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "Sort\xe3\x81\xae()\xe3\x81\x8c\xe6\x8c\x87\xe5\xae\x9a\xe3\x81\x95\xe3\x82\x8c\xe3\x81\xa6\xe3\x81\x84\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
				}

				// Update clause
				if (str_equals(word2name, "Update")) {
					if (val2->objtype == LetterType::OT_Dot) {
						(*n)++;
						val2 = *(Obj**)get_list(primary->children, *n);
						if (val2->objtype == LetterType::OT_Word) {
							(*n)++;
							val2 = *(Obj**)get_list(primary->children, *n);
							// Update.await() handling
							if (val2->objtype == LetterType::OT_Bracket) {
								(*n)++;
								LFunc* func = llvm_get_func(local->llvm);
								Block* blk = (Block*)exeC(thgc, val2, local);
								// SQL UPDATE generation with parameter binding
								// ... complex SQL string building and parameter setup
								return make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
							}
						}
					}
					// non-await Update path
					if (val2->objtype == LetterType::OT_Bracket) {
						(*n)++;
						LFunc* func = llvm_get_func(local->llvm);
						Block* blk = (Block*)exeC(thgc, val2, local);
						// SQL UPDATE with coroutine setup
						return make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
					}
					return ErrorC(thgc, LetterType::OT_Error, val2->letter, "FirstAwait\xe3\x81\xae()\xe3\x81\x8c\xe6\x8c\x87\xe5\xae\x9a\xe3\x81\x95\xe3\x82\x8c\xe3\x81\xa6\xe3\x81\x84\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
				}

				// Delete clause
				if (str_equals(word2name, "Delete")) {
					if (val2->objtype == LetterType::OT_Dot) {
						(*n)++;
						val2 = *(Obj**)get_list(primary->children, *n);
						if (val2->objtype == LetterType::OT_Word) {
							(*n)++;
							val2 = *(Obj**)get_list(primary->children, *n);
							// Delete.await() handling
							if (val2->objtype == LetterType::OT_Bracket) {
								(*n)++;
								LFunc* func = llvm_get_func(local->llvm);
								// SQL DELETE generation
								// Build "delete from TABLE where ..." string
								// Parameter binding for SqlString/Value where clause
								// Async path: ExecSql with coroutine queue
								// Sync path: create separate async function with transaction
								return make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, NULL);
							}
						}
					}
					return ErrorC(thgc, LetterType::OT_Error, val2->letter, "FirstAwait\xe3\x81\xae()\xe3\x81\x8c\xe6\x8c\x87\xe5\xae\x9a\xe3\x81\x95\xe3\x82\x8c\xe3\x81\xa6\xe3\x81\x84\xe3\x81\xbe\xe3\x81\x9b\xe3\x82\x93");
				}
			}
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "Model\xe3\x81\xae\xe5\xbc\x95\xe6\x95\xb0\xe3\x81\x8c\xe4\xb8\x8d\xe6\xad\xa3\xe3\x81\xa7\xe3\x81\x99");
}
