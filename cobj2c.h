// ============================================================
// cobj2c.h - C解析パス (LLVM IR code generation) - CObj2C.cs converted
// ============================================================

// ヘルパーマクロ
#define IS_WEG_C(obj) ((obj)->objtype == LetterType::OT_Wait || (obj)->objtype == LetterType::OT_Error || (obj)->objtype == LetterType::OT_NG)

// 前方宣言
Obj* exeC_Comment(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_Comment(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_LinearFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeC_Comment2(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_Comment2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepC_Dolor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepC_HtmObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exepC_TagBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* exeC_TagBlock(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* opeC_ElemObj(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2);
Obj* exepC_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeC_SignalFunction(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryC_SignalFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* GetterC_SignalFunction(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeC_ServerFunction(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryC_ServerFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* GetterC_ServerFunction(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryC_Connect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryC_ConnectStock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryC_Address(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Mountain(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Sum(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Question(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);

// ============================================================
// Comment::exeC
// ============================================================
Obj* exeC_Comment(ThreadGC* thgc, Obj* self, Master* local) {
	Block* block = *(Block**)get_list(self->children, 0);
	add_list(thgc, local->blocks, (char*)block);
	vt_exeC(thgc, (Obj*)block, local);
	// comelet.comment = this; comelet.Renew(); comelet.nums.Add(0);
	// (CommentLet handling is C#-side metadata, simplified here)
	add_list(thgc, local->comments, (char*)self->letter);
	local->blocks->size--;
	local->comments->size--;
	return self;
}

// ============================================================
// Comment::exepC
// ============================================================
Obj* exepC_Comment(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Block* blk = *(Block**)get_list(self->children, 0);
	add_list(thgc, local->blocks, (char*)blk);

	LVari* obj = make_lvari_c(thgc, "%GCObjectPtr", "%obj");
	LVari* rn = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	LVari* parent = make_lvari_c(thgc, "%ElementType*", "%parent");
	LVari* parentte = make_lvari_c(thgc, "%TreeElementType*", "%parentte");
	LVari* deletes = make_lvari_c(thgc, "%ListType*", "%deletes");

	int n2 = llvm_n++;
	char modelbuf[64];
	snprintf(modelbuf, sizeof(modelbuf), "%%com%d", n2);
	self->model = createString(thgc, modelbuf, (int)strlen(modelbuf), 1);

	char elbuf[64];
	snprintf(elbuf, sizeof(elbuf), "@el%d", n2);
	LVari* func_y = make_lvari(thgc, createString(thgc, (char*)"void", 4, 1), createString(thgc, elbuf, (int)strlen(elbuf), 1));
	LFunc* func = make_lfunc(thgc, local->llvm, func_y);
	add_list(thgc, func->draws, (char*)rn);
	add_list(thgc, func->draws, (char*)obj);
	add_list(thgc, func->draws, (char*)parent);
	add_list(thgc, func->draws, (char*)parentte);
	add_list(thgc, func->draws, (char*)deletes);
	add_list(thgc, local->llvm->comps, (char*)func);
	add_list(thgc, local->llvm->funcs, (char*)func);

	LVari* l_thgc = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, l_thgc, make_lvari_c(thgc, "%ThreadGCType**", "@thgcp"), false));
	add_list(thgc, local->llvm->tagparents, (char*)parent);
	add_list(thgc, local->llvm->tagparenttes, (char*)parentte);

	// comelet handling (simplified)
	add_list(thgc, local->comments, (char*)self->letter);

	LTypeDec* typedec = make_ltypedec(thgc, self->model, NULL, false);
	add_list(thgc, local->llvm->types, (char*)typedec);
	add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i8*", 3, 1), createString(thgc, (char*)"blk", 3, 1)));

	char checkbuf[64];
	snprintf(checkbuf, sizeof(checkbuf), "@elCheck%d", n2);
	LVari* thgc3 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* ac_val = make_lvari_c(thgc, "%GC_AddClassType", "%addclass");
	LVari* countv = make_lvari_c(thgc, "i32", "0");

	char namebuf[64];
	snprintf(namebuf, sizeof(namebuf), "comment%d", n2);
	int namelen = (int)strlen(namebuf);
	char atnamebuf[66];
	snprintf(atnamebuf, sizeof(atnamebuf), "@%s", namebuf);
	LStrV* strv = make_lstrv(thgc, createString(thgc, atnamebuf, (int)strlen(atnamebuf), 1), createString(thgc, namebuf, namelen, 1), namelen * 1);
	add_list(thgc, local->llvm->strs, (char*)strv);

	self->n_c = local->llvm->cn++;
	char typbuf[16];
	snprintf(typbuf, sizeof(typbuf), "%d", self->n_c);
	LVari* typ = make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, typbuf, (int)strlen(typbuf), 1));

	char tnpbuf[32];
	snprintf(tnpbuf, sizeof(tnpbuf), "%%tnp%d", llvm_n++);
	LVari* tnp = make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, tnpbuf, (int)strlen(tnpbuf), 1));
	add_list(thgc, local->llvm->main_func->comps, (char*)make_lload(thgc, tnp, make_lvari_c(thgc, "i32*", "@cnp"), false));

	char tvbuf[32];
	snprintf(tvbuf, sizeof(tvbuf), "%%cv%d", llvm_n++);
	LVari* tv = make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, tvbuf, (int)strlen(tvbuf), 1));
	add_list(thgc, local->llvm->main_func->comps, (char*)make_lbinop(thgc, LBOP_Add, tv, tnp, typ));

	LCall* ac_call = make_lcall(thgc, NULL, ac_val);
	add_list(thgc, ac_call->comps, (char*)thgc3);
	add_list(thgc, ac_call->comps, (char*)tv);
	add_list(thgc, ac_call->comps, (char*)countv);
	add_list(thgc, ac_call->comps, (char*)strv);
	add_list(thgc, ac_call->comps, (char*)make_lvari_c(thgc, "%GCCheckFuncType", checkbuf));
	add_list(thgc, ac_call->comps, (char*)make_lvari_c(thgc, "%GCFinalizeFuncType", "null"));
	add_list(thgc, local->llvm->main_func->comps, (char*)ac_call);

	LVari* thgc2 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* i8p = make_lvari_c(thgc, "i8*", "%self");
	LVari* funccheck_y = make_lvari(thgc, createString(thgc, (char*)"void", 4, 1), createString(thgc, checkbuf, (int)strlen(checkbuf), 1));
	LFunc* funccheck = make_lfunc(thgc, local->llvm, funccheck_y);
	add_list(thgc, funccheck->draws, (char*)thgc2);
	add_list(thgc, funccheck->draws, (char*)i8p);
	add_list(thgc, local->llvm->comps, (char*)funccheck);

	// funccheck: gete, load CopyObject, load, call, store for index 0
	{
		char vbuf[32];
		snprintf(vbuf, sizeof(vbuf), "%%v%d", llvm_n++);
		LVari* vari = make_lvari_c(thgc, "i8*", vbuf);
		LGete* gete = make_lgete(thgc, self->model, vari, i8p);
		add_list(thgc, gete->comps, (char*)make_lvari_c(thgc, "i32", "0"));
		add_list(thgc, gete->comps, (char*)make_lvari_c(thgc, "i32", "0"));
		add_list(thgc, funccheck->comps, (char*)gete);

		LVari* co_val2 = make_lvari_c(thgc, "%CopyObjectType", "%co");
		add_list(thgc, funccheck->comps, (char*)make_lload(thgc, co_val2, make_lvari_c(thgc, "%CopyObjectType*", "@CopyObject"), false));

		snprintf(vbuf, sizeof(vbuf), "%%v%d", llvm_n++);
		LVari* vari2 = make_lvari_c(thgc, "i8*", vbuf);
		add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));

		snprintf(vbuf, sizeof(vbuf), "%%v%d", llvm_n++);
		LVari* co_cval = make_lvari_c(thgc, "i8*", vbuf);
		LCall* co_call = make_lcall(thgc, co_cval, co_val2);
		add_list(thgc, co_call->comps, (char*)thgc2);
		add_list(thgc, co_call->comps, (char*)vari);
		add_list(thgc, funccheck->comps, (char*)co_call);
		add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
	}

	// funcdec
	char decbuf[64];
	snprintf(decbuf, sizeof(decbuf), "@eldec%d", n2);
	LVari* rn6 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	LVari* block_v = make_lvari_c(thgc, "i8*", "%block");
	char modstar[72];
	snprintf(modstar, sizeof(modstar), "%s*", modelbuf);
	LVari* funcdec_y = make_lvari(thgc, createString(thgc, modstar, (int)strlen(modstar), 1), createString(thgc, decbuf, (int)strlen(decbuf), 1));
	LFunc* funcdec = make_lfunc(thgc, local->llvm, funcdec_y);
	add_list(thgc, funcdec->draws, (char*)rn6);
	add_list(thgc, funcdec->draws, (char*)block_v);
	add_list(thgc, local->llvm->comps, (char*)funcdec);

	LVari* thgc6 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	add_list(thgc, funcdec->comps, (char*)make_lload(thgc, thgc6, make_lvari_c(thgc, "%ThreadGCType**", "@thgcp"), false));

	LVari* gmvari = make_lvari_c(thgc, "%GC_mallocType", "%gm");
	add_list(thgc, funcdec->comps, (char*)make_lload(thgc, gmvari, make_lvari_c(thgc, "%GC_mallocType*", "@GC_malloc"), false));

	{
		char vbuf[32];
		snprintf(vbuf, sizeof(vbuf), "%%obj%d", llvm_n++);
		LVari* go_v = make_lvari_c(thgc, "%GCObjectPtr", vbuf);

		char tmpbuf[16];
		snprintf(tmpbuf, sizeof(tmpbuf), "%d", self->n_c);
		LVari* tmp = make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, tmpbuf, (int)strlen(tmpbuf), 1));

		char tmibuf[32];
		snprintf(tmibuf, sizeof(tmibuf), "%%tmi%d", llvm_n++);
		LVari* tmi = make_lvari_c(thgc, "i32", tmibuf);
		add_list(thgc, funcdec->comps, (char*)make_lload(thgc, tmi, make_lvari_c(thgc, "i32*", "@cnp"), false));

		char tmvbuf[32];
		snprintf(tmvbuf, sizeof(tmvbuf), "%%tmv%d", llvm_n++);
		LVari* tmv = make_lvari_c(thgc, "i32", tmvbuf);
		add_list(thgc, funcdec->comps, (char*)make_lbinop(thgc, LBOP_Add, tmv, tmi, tmp));

		LCall* go_call = make_lcall(thgc, go_v, gmvari);
		add_list(thgc, go_call->comps, (char*)thgc6);
		add_list(thgc, go_call->comps, (char*)tmv);
		add_list(thgc, funcdec->comps, (char*)go_call);
		add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, make_lvari_c(thgc, "ptr", "@client"), go_v));

		snprintf(vbuf, sizeof(vbuf), "%%v%d", llvm_n++);
		LVari* go_c2 = make_lvari(thgc, createString(thgc, modstar, (int)strlen(modstar), 1), createString(thgc, vbuf, (int)strlen(vbuf), 1));
		add_list(thgc, funcdec->comps, (char*)make_lcast(thgc, LCAST_Bitcast, go_c2, go_v));

		snprintf(vbuf, sizeof(vbuf), "%%v%d", llvm_n++);
		LVari* vc = make_lvari_c(thgc, "i8*", vbuf);
		LGete* gete = make_lgete(thgc, self->model, vc, go_c2);
		add_list(thgc, gete->comps, (char*)make_lvari_c(thgc, "i32", "0"));
		add_list(thgc, gete->comps, (char*)make_lvari_c(thgc, "i32", "0"));
		add_list(thgc, funcdec->comps, (char*)gete);
		add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, vc, block_v));

		char opbuf[32];
		snprintf(opbuf, sizeof(opbuf), "%%v%d", llvm_n++);
		char opsbuf[72];
		snprintf(opsbuf, sizeof(opsbuf), "%s**", modelbuf);
		LVari* objptr3 = make_lvari(thgc, createString(thgc, opsbuf, (int)strlen(opsbuf), 1), createString(thgc, opbuf, (int)strlen(opbuf), 1));
		add_list(thgc, funcdec->comps, (char*)make_lalloca(thgc, objptr3));
		add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, objptr3, go_c2));

		int count = 8;
		int order = 1;

		List* decs = create_list(thgc, sizeof(Obj*), CType::_List);

		// iterate blk.vmapA
		for (int _i = 0; _i < blk->vmapA->kvs->size; _i++) {
			KV* kv = *(KV**)get_list(blk->vmapA->kvs, _i);
			Obj* v = (Obj*)kv->value;
			if (v->objtype == LetterType::OT_Variable) {
				v->order = order++;
				// variable.cls handling simplified - set typedec based on type
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i8*", 3, 1), kv->key));
				count += 8;
				// funccheck GC copy for non-primitive types
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2_ = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2_, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval_ = make_lvari_c(thgc, "i8*", vb);
					LVari* co_val3 = make_lvari_c(thgc, "%CopyObjectType", "%co");
					LCall* co_call_ = make_lcall(thgc, co_cval_, co_val3);
					add_list(thgc, co_call_->comps, (char*)thgc2);
					add_list(thgc, co_call_->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call_);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval_));
				}
			} else if (v->objtype == LetterType::OT_Function) {
				if (v->identity == 0) v->identity = llvm_n++;
				v->order = order++;
				add_list(thgc, decs, (char*)v);
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));

				// funccheck copy
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);
					// duplicate gete for Function (matches C# code)
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2_ = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2_, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval_ = make_lvari_c(thgc, "i8*", vb);
					LVari* co_val3 = make_lvari_c(thgc, "%CopyObjectType", "%co");
					LCall* co_call_ = make_lcall(thgc, co_cval_, co_val3);
					add_list(thgc, co_call_->comps, (char*)thgc2);
					add_list(thgc, co_call_->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call_);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval_));
				}
				count += 8;

				// funcdec: call dec, load, gete, store
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va0 = make_lvari_c(thgc, "%FuncType*", vb);
					// build call name "@{name}dec{identity}"
					// (simplified - uses letter name + dec + identity)
					char fcbuf[128];
					// TODO: proper letter name extraction
					LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", fcbuf));
					add_list(thgc, fu_call->comps, (char*)block_v);
					add_list(thgc, funcdec->comps, (char*)fu_call);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va2 = make_lvari(thgc, createString(thgc, modstar, (int)strlen(modstar), 1), createString(thgc, vb, (int)strlen(vb), 1));
					add_list(thgc, funcdec->comps, (char*)make_lload(thgc, va2, objptr3, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va = make_lvari_c(thgc, "%FuncType*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, va, va2);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funcdec->comps, (char*)ge);
					add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, va, va0));
				}
			} else if (v->objtype == LetterType::OT_ClassObj) {
				if (v->identity == 0) v->identity = llvm_n++;
				v->order = order++;
				add_list(thgc, decs, (char*)v);
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));

				// funccheck copy (same pattern as Function)
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2_ = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2_, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval_ = make_lvari_c(thgc, "i8*", vb);
					LVari* co_val3 = make_lvari_c(thgc, "%CopyObjectType", "%co");
					LCall* co_call_ = make_lcall(thgc, co_cval_, co_val3);
					add_list(thgc, co_call_->comps, (char*)thgc2);
					add_list(thgc, co_call_->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call_);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval_));
				}
				count += 8;

				// funcdec: same pattern
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va0 = make_lvari_c(thgc, "%FuncType*", vb);
					LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", "@classdec"));
					add_list(thgc, fu_call->comps, (char*)block_v);
					add_list(thgc, funcdec->comps, (char*)fu_call);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va2 = make_lvari(thgc, createString(thgc, modstar, (int)strlen(modstar), 1), createString(thgc, vb, (int)strlen(vb), 1));
					add_list(thgc, funcdec->comps, (char*)make_lload(thgc, va2, objptr3, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va = make_lvari_c(thgc, "%FuncType*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, va, va2);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funcdec->comps, (char*)ge);
					add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, va, va0));
				}
			}
		}

		// Store into local
		local->blok = block_v;
		local->typedecC = typedec;
		local->funccheck = funccheck;
		local->funcdec = funcdec;
		local->i8p = i8p;
		local->objptr3 = objptr3;
		local->co_val = make_lvari_c(thgc, "%CopyObjectType", "%co");
		local->countv = countv;
		local->odr = order;
		local->countC = count;

		// decs[i].Dec(local)
		for (int _d = 0; _d < decs->size; _d++) {
			Obj* dec = *(Obj**)get_list(decs, _d);
			// Dec is a virtual dispatch - would need vt_Dec if available
			// For now, this matches the C# pattern
		}

		add_list(thgc, funcdec->comps, (char*)make_lret(thgc, go_c2));
		add_list(thgc, funccheck->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));

		// func body
		char vvbuf[32];
		snprintf(vvbuf, sizeof(vvbuf), "%%vv%d", llvm_n++);
		LVari* vv = make_lvari(thgc, createString(thgc, modstar, (int)strlen(modstar), 1), createString(thgc, vvbuf, (int)strlen(vvbuf), 1));
		LCall* vv_call = make_lcall(thgc, vv, funcdec_y);
		add_list(thgc, vv_call->comps, (char*)rn);
		add_list(thgc, vv_call->comps, (char*)obj);
		add_list(thgc, func->comps, (char*)vv_call);

		char opbuf2[32];
		snprintf(opbuf2, sizeof(opbuf2), "%%objptr%d", llvm_n++);
		LVari* objptr = make_lvari(thgc, createString(thgc, opsbuf, (int)strlen(opsbuf), 1), createString(thgc, opbuf2, (int)strlen(opbuf2), 1));
		add_list(thgc, func->comps, (char*)make_lalloca(thgc, objptr));
		add_list(thgc, func->comps, (char*)make_lstore(thgc, objptr, vv));

		((ObjBlock*)((Block*)blk)->obj)->obj->bas = objptr;
		vt_exeC(thgc, (Obj*)blk, local);

		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", "")));
		local->llvm->funcs->size--;
		local->llvm->tagparents->size--;
		local->llvm->tagparenttes->size--;
		local->blocks->size--;
		local->comments->size--;
		self->vari_c = func_y;
	}
	return self;
}

// ============================================================
// Comment2::exeC
// ============================================================
Obj* exeC_Comment2(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// Comment2::exepC
// ============================================================
Obj* exepC_Comment2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Clones::RightRightC (not a vtable method, helper)
// ============================================================
Obj* RightRightC_Clones(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	return self;
}

// ============================================================
// Dolor::exepC
// ============================================================
Obj* exepC_Dolor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		String* wordname = val2->letter->text;
		if (str_matchA(wordname, "type", 4)) {
			Obj* clones = make_cobj(thgc, CType::_CObj, LetterType::OT_Clones, self->letter);
			return clones;
		} else if (str_matchA(wordname, "func", 4)) {
			// break (no-op)
		}
	} else if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		local->compsC->size--;
		if (IS_WEG_C(val)) return val;
		Block* blk = (Block*)val;
		if (blk->rets->size != 1) {
			return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
		}
		Obj* ret0 = *(Obj**)get_list(blk->rets, 0);
		if (ret0->objtype == LetterType::OT_Number || ret0->objtype == LetterType::OT_StrObj) {
			if (local->comments->size > 0) {
				LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
				// tagvars.Last() length check
				String* lasttagvar = *(String**)get_list(local->llvm->tagvars, local->llvm->tagvars->size - 1);
				if (lasttagvar != NULL && lasttagvar->size > 0) {
					char strbuf[32];
					snprintf(strbuf, sizeof(strbuf), "%%str%d", llvm_n++);
					LVari* str = make_lvari_c(thgc, "%StringType*", strbuf);
					int n2val = 1;
					char vbuf[32];
					snprintf(vbuf, sizeof(vbuf), "%%v%d", llvm_n++);
					LVari* v2 = make_lvari_c(thgc, "i8*", vbuf);
					char svbuf[32];
					snprintf(svbuf, sizeof(svbuf), "@s%d", llvm_n++);
					LStrV* sv = make_lstrv(thgc, createString(thgc, svbuf, (int)strlen(svbuf), 1), lasttagvar, lasttagvar->size * n2val);
					add_list(thgc, local->llvm->strs, (char*)sv);
					char getebuf[64];
					snprintf(getebuf, sizeof(getebuf), "[%d x i8]", lasttagvar->size * n2val + 1);
					LGete* gete = make_lgete(thgc, createString(thgc, getebuf, (int)strlen(getebuf), 1), v2, (LVari*)sv);
					add_list(thgc, gete->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, gete->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)gete);

					char fvbuf[32];
					snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
					LVari* fv = make_lvari_c(thgc, "%CreateStringType*", fvbuf);
					add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv, make_lvari_c(thgc, "%CreateStringType*", "@CreateString"), false));
					char lenbuf[16];
					snprintf(lenbuf, sizeof(lenbuf), "%d", lasttagvar->size);
					char n2buf[16];
					snprintf(n2buf, sizeof(n2buf), "%d", n2val);
					LCall* cscall = make_lcall(thgc, str, fv);
					add_list(thgc, cscall->comps, (char*)thgc4);
					add_list(thgc, cscall->comps, (char*)v2);
					add_list(thgc, cscall->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, lenbuf, (int)strlen(lenbuf), 1)));
					add_list(thgc, cscall->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, n2buf, (int)strlen(n2buf), 1)));
					add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)cscall);

					LVari* lasttagvarvar = *(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1);
					if (lasttagvarvar == NULL) {
						// tagvarvars[last] = str
						*(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1) = str;
						return NULL;
					} else {
						char fv2buf[32];
						snprintf(fv2buf, sizeof(fv2buf), "%%fv%d", llvm_n++);
						LVari* fv2 = make_lvari_c(thgc, "%AddStringType2", fv2buf);
						add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv2, make_lvari_c(thgc, "%AddStringType2*", "@AddString2"), false));
						char str2buf[32];
						snprintf(str2buf, sizeof(str2buf), "%%str%d", llvm_n++);
						LVari* str2 = make_lvari_c(thgc, "%StringType*", str2buf);
						LCall* as_call = make_lcall(thgc, str2, fv2);
						add_list(thgc, as_call->comps, (char*)thgc4);
						add_list(thgc, as_call->comps, (char*)lasttagvarvar);
						add_list(thgc, as_call->comps, (char*)str);
						add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)as_call);
						*(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1) = str2;
					}
					// tagvars[last] = ""
					*(String**)get_list(local->llvm->tagvars, local->llvm->tagvars->size - 1) = createString(thgc, (char*)"", 0, 1);
				}
				{
					char fvbuf[32];
					snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
					LVari* fv = make_lvari_c(thgc, "%CreateLetterType", fvbuf);
					add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv, make_lvari_c(thgc, "%CreateLetterType*", "@CreateLetterType"), false));
					char strbuf[32];
					snprintf(strbuf, sizeof(strbuf), "%%str%d", llvm_n++);
					LVari* str = make_lvari_c(thgc, "%StringType*", strbuf);
					if (ret0->objtype == LetterType::OT_Number) {
						char fv2buf[32];
						snprintf(fv2buf, sizeof(fv2buf), "%%fv%d", llvm_n++);
						LVari* fv2 = make_lvari_c(thgc, "%NumberStringType", fv2buf);
						add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv2, make_lvari_c(thgc, "%NumberStringType*", "@NumberString"), false));
						LCall* ns_call = make_lcall(thgc, str, fv2);
						add_list(thgc, ns_call->comps, (char*)thgc4);
						add_list(thgc, ns_call->comps, (char*)ret0->vari_c);
						add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ns_call);
					} else {
						str = ret0->vari_c;
					}
					LVari* lasttagvarvar = *(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1);
					if (lasttagvarvar == NULL) {
						*(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1) = str;
						return NULL;
					} else {
						char fv2buf[32];
						snprintf(fv2buf, sizeof(fv2buf), "%%fv%d", llvm_n++);
						LVari* fv2 = make_lvari_c(thgc, "%AddStringType2", fv2buf);
						add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv2, make_lvari_c(thgc, "%AddStringType2*", "@AddString2"), false));
						char str2buf[32];
						snprintf(str2buf, sizeof(str2buf), "%%str%d", llvm_n++);
						LVari* str2 = make_lvari_c(thgc, "%StringType*", str2buf);
						LCall* as_call = make_lcall(thgc, str2, fv2);
						add_list(thgc, as_call->comps, (char*)thgc4);
						add_list(thgc, as_call->comps, (char*)lasttagvarvar);
						add_list(thgc, as_call->comps, (char*)str);
						add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)as_call);
						*(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1) = str2;
					}
				}
			} else {
				return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
			}
		} else {
			return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
		}
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
}

// ============================================================
// HtmObj::exepC
// ============================================================
Obj* exepC_HtmObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (local->comments->size > 0) {
		*(String**)get_list(local->llvm->tagvars, local->llvm->tagvars->size - 1) = self->letter->text;
		return NULL;
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, self->letter);
}

// ============================================================
// TagBlock::exeC
// ============================================================
Obj* exeC_TagBlock(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// TagBlock::exepC
// ============================================================
Obj* exepC_TagBlock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (local->comments->size > 0) {
		Block* blk1 = *(Block**)get_list(self->children, 0);
		int order = local->odr;
		add_list(thgc, local->blocks, (char*)blk1);
		Block* blk2 = *(Block**)get_list(self->children, 1);
		List* decs = create_list(thgc, sizeof(Obj*), CType::_List);
		LVari* thgc2 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
		LVari* block = local->blok;
		LTypeDec* typedec = local->typedecC;
		int count = local->countC;
		LFunc* funccheck = local->funccheck;
		LFunc* funcdec = local->funcdec;
		LVari* i8p = local->i8p;
		LVari* objptr3 = local->objptr3;
		LVari* co_val = local->co_val;

		// iterate blk1.vmapA
		for (int _i = 0; _i < blk1->vmapA->kvs->size; _i++) {
			KV* kv = *(KV**)get_list(blk1->vmapA->kvs, _i);
			Obj* v = (Obj*)kv->value;
			if (v->objtype == LetterType::OT_Variable) {
				v->order = order++;
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i8*", 3, 1), kv->key));
				count += 8;
				// funccheck copy
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2 = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval = make_lvari_c(thgc, "i8*", vb);
					LCall* co_call = make_lcall(thgc, co_cval, co_val);
					add_list(thgc, co_call->comps, (char*)thgc2);
					add_list(thgc, co_call->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
				}
			} else if (v->objtype == LetterType::OT_Function) {
				if (v->identity == 0) v->identity = llvm_n++;
				v->order = order++;
				add_list(thgc, decs, (char*)v);
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));
				// funccheck
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2 = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval = make_lvari_c(thgc, "i8*", vb);
					LCall* co_call = make_lcall(thgc, co_cval, co_val);
					add_list(thgc, co_call->comps, (char*)thgc2);
					add_list(thgc, co_call->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
				}
				count += 8;
				// funcdec pattern
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va0 = make_lvari_c(thgc, "%FuncType*", vb);
					LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", "@funcdec"));
					add_list(thgc, fu_call->comps, (char*)block);
					add_list(thgc, funcdec->comps, (char*)fu_call);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					String* modstar = self->model;
					char msbuf[72];
					snprintf(msbuf, sizeof(msbuf), "%.*s*", modstar->size, modstar->data);
					LVari* va2 = make_lvari(thgc, createString(thgc, msbuf, (int)strlen(msbuf), 1), createString(thgc, vb, (int)strlen(vb), 1));
					add_list(thgc, funcdec->comps, (char*)make_lload(thgc, va2, objptr3, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va = make_lvari_c(thgc, "%FuncType*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, va, va2);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funcdec->comps, (char*)ge);
					add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, va, va0));
				}
			} else if (v->objtype == LetterType::OT_ClassObj) {
				if (v->identity == 0) v->identity = llvm_n++;
				v->order = order++;
				add_list(thgc, decs, (char*)v);
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));
				// funccheck + funcdec (same pattern)
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2 = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval = make_lvari_c(thgc, "i8*", vb);
					LCall* co_call = make_lcall(thgc, co_cval, co_val);
					add_list(thgc, co_call->comps, (char*)thgc2);
					add_list(thgc, co_call->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
				}
				count += 8;
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va0 = make_lvari_c(thgc, "%FuncType*", vb);
					LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", "@classdec"));
					add_list(thgc, fu_call->comps, (char*)block);
					add_list(thgc, funcdec->comps, (char*)fu_call);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					String* modstar = self->model;
					char msbuf[72];
					snprintf(msbuf, sizeof(msbuf), "%.*s*", modstar->size, modstar->data);
					LVari* va2 = make_lvari(thgc, createString(thgc, msbuf, (int)strlen(msbuf), 1), createString(thgc, vb, (int)strlen(vb), 1));
					add_list(thgc, funcdec->comps, (char*)make_lload(thgc, va2, objptr3, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va = make_lvari_c(thgc, "%FuncType*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, va, va2);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funcdec->comps, (char*)ge);
					add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, va, va0));
				}
			}
		}

		// iterate blk2.vmapA (same patterns for second block)
		for (int _i = 0; _i < blk2->vmapA->kvs->size; _i++) {
			KV* kv = *(KV**)get_list(blk2->vmapA->kvs, _i);
			Obj* v = (Obj*)kv->value;
			if (v->objtype == LetterType::OT_Variable) {
				v->order = order++;
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"i8*", 3, 1), kv->key));
				count += 8;
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2 = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval = make_lvari_c(thgc, "i8*", vb);
					LCall* co_call = make_lcall(thgc, co_cval, co_val);
					add_list(thgc, co_call->comps, (char*)thgc2);
					add_list(thgc, co_call->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
				}
			} else if (v->objtype == LetterType::OT_Function) {
				if (v->identity == 0) v->identity = llvm_n++;
				v->order = order++;
				add_list(thgc, decs, (char*)v);
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2 = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval = make_lvari_c(thgc, "i8*", vb);
					LCall* co_call = make_lcall(thgc, co_cval, co_val);
					add_list(thgc, co_call->comps, (char*)thgc2);
					add_list(thgc, co_call->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
				}
				count += 8;
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va0 = make_lvari_c(thgc, "%FuncType*", vb);
					LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", "@funcdec"));
					add_list(thgc, fu_call->comps, (char*)block);
					add_list(thgc, funcdec->comps, (char*)fu_call);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					String* modstr = self->model;
					char msbuf[72];
					snprintf(msbuf, sizeof(msbuf), "%.*s*", modstr->size, modstr->data);
					LVari* va2 = make_lvari(thgc, createString(thgc, msbuf, (int)strlen(msbuf), 1), createString(thgc, vb, (int)strlen(vb), 1));
					add_list(thgc, funcdec->comps, (char*)make_lload(thgc, va2, objptr3, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va = make_lvari_c(thgc, "%FuncType*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, va, va2);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funcdec->comps, (char*)ge);
					add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, va, va0));
				}
			} else if (v->objtype == LetterType::OT_ClassObj) {
				if (v->identity == 0) v->identity = llvm_n++;
				v->order = order++;
				add_list(thgc, decs, (char*)v);
				add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, createString(thgc, (char*)"%FuncType*", 10, 1), kv->key));
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari = make_lvari_c(thgc, "i8*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, vari, i8p);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funccheck->comps, (char*)ge);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* vari2 = make_lvari_c(thgc, "i8*", vb);
					add_list(thgc, funccheck->comps, (char*)make_lload(thgc, vari2, vari, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* co_cval = make_lvari_c(thgc, "i8*", vb);
					LCall* co_call = make_lcall(thgc, co_cval, co_val);
					add_list(thgc, co_call->comps, (char*)thgc2);
					add_list(thgc, co_call->comps, (char*)vari);
					add_list(thgc, funccheck->comps, (char*)co_call);
					add_list(thgc, funccheck->comps, (char*)make_lstore(thgc, vari, co_cval));
				}
				count += 8;
				{
					char vb[32];
					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va0 = make_lvari_c(thgc, "%FuncType*", vb);
					LCall* fu_call = make_lcall(thgc, va0, make_lvari_c(thgc, "%FuncType*", "@classdec"));
					add_list(thgc, fu_call->comps, (char*)block);
					add_list(thgc, funcdec->comps, (char*)fu_call);

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					String* modstr = self->model;
					char msbuf[72];
					snprintf(msbuf, sizeof(msbuf), "%.*s*", modstr->size, modstr->data);
					LVari* va2 = make_lvari(thgc, createString(thgc, msbuf, (int)strlen(msbuf), 1), createString(thgc, vb, (int)strlen(vb), 1));
					add_list(thgc, funcdec->comps, (char*)make_lload(thgc, va2, objptr3, false));

					snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
					LVari* va = make_lvari_c(thgc, "%FuncType*", vb);
					char ordbuf[16];
					snprintf(ordbuf, sizeof(ordbuf), "%d", v->order);
					LGete* ge = make_lgete(thgc, self->model, va, va2);
					add_list(thgc, ge->comps, (char*)make_lvari_c(thgc, "i32", "0"));
					add_list(thgc, ge->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, ordbuf, (int)strlen(ordbuf), 1)));
					add_list(thgc, funcdec->comps, (char*)ge);
					add_list(thgc, funcdec->comps, (char*)make_lstore(thgc, va, va0));
				}
			}
		}

		local->odr = order;
		local->countC = count;
		// countv.name = count (update the LVari name)
		{
			char cbuf[16];
			snprintf(cbuf, sizeof(cbuf), "%d", count);
			local->countv->name = createString(thgc, cbuf, (int)strlen(cbuf), 1);
		}

		// decs[i].Dec(local) - virtual dispatch
		for (int _d = 0; _d < decs->size; _d++) {
			// Dec dispatch placeholder
		}

		Obj* obj_result = vt_GetterC(thgc, vt_exeC(thgc, (Obj*)blk1, local), local);
		Block* obj_blk = (Block*)obj_result;

		LVari* l_thgc = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
		if (local->llvm->tagvars->size > 0) {
			char fvbuf[32];
			snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
			LVari* fv = make_lvari_c(thgc, "%CreateLetterTpe*", fvbuf);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv, make_lvari_c(thgc, "%CreateLetterTpe*", "@CreateLetter"), false));
			LVari* lastparent = *(LVari**)get_list(local->llvm->tagparents, local->llvm->tagparents->size - 1);
			LVari* lasttagvarvar = *(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1);
			LCall* cl_call = make_lcall(thgc, NULL, fv);
			add_list(thgc, cl_call->comps, (char*)lastparent);
			add_list(thgc, cl_call->comps, (char*)lasttagvarvar);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)cl_call);
		}

		bool id = false;
		LVari* ev = NULL;
		Obj* ret0 = *(Obj**)get_list(obj_blk->rets, 0);

		// Element creation based on type
		switch (ret0->objtype) {
		case LetterType::OT_Div: {
			char fvbuf[32];
			snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
			LVari* fv = make_lvari_c(thgc, "%CreateDivType*", fvbuf);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv, make_lvari_c(thgc, "%CreateDivType*", "@CreateDiv"), false));
			char evbuf[32];
			snprintf(evbuf, sizeof(evbuf), "%%ev%d", llvm_n++);
			ev = make_lvari_c(thgc, "%ElementType*", evbuf);
			LCall* ev_call = make_lcall(thgc, ev, fv);
			add_list(thgc, ev_call->comps, (char*)l_thgc);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ev_call);
			snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
			LVari* fv4 = make_lvari_c(thgc, "%ElementAddType*", fvbuf);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv4, make_lvari_c(thgc, "%ElementAddType*", "@ElementAdd"), false));
			LCall* ea_call = make_lcall(thgc, NULL, fv4);
			add_list(thgc, ea_call->comps, (char*)l_thgc);
			add_list(thgc, ea_call->comps, (char*)(local->llvm->tagparents->size == 0 ? NULL : *(LVari**)get_list(local->llvm->tagparents, local->llvm->tagparents->size - 1)));
			add_list(thgc, ea_call->comps, (char*)ev);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ea_call);
			add_list(thgc, local->llvm->tagparents, (char*)ev);
		} break;
		case LetterType::OT_Sheet: {
			char fvbuf[32];
			snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
			LVari* fv = make_lvari_c(thgc, "%CreateDivType*", fvbuf);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv, make_lvari_c(thgc, "%CreateDivType*", "@CreateSheet"), false));
			char evbuf[32];
			snprintf(evbuf, sizeof(evbuf), "%%ev%d", llvm_n++);
			ev = make_lvari_c(thgc, "%ElementType*", evbuf);
			LCall* ev_call = make_lcall(thgc, ev, fv);
			add_list(thgc, ev_call->comps, (char*)l_thgc);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ev_call);
			snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
			LVari* fv4 = make_lvari_c(thgc, "%ElementAddType*", fvbuf);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv4, make_lvari_c(thgc, "%ElementAddType*", "@ElementAdd"), false));
			LCall* ea_call = make_lcall(thgc, NULL, fv4);
			add_list(thgc, ea_call->comps, (char*)l_thgc);
			add_list(thgc, ea_call->comps, (char*)(local->llvm->tagparents->size == 0 ? NULL : *(LVari**)get_list(local->llvm->tagparents, local->llvm->tagparents->size - 1)));
			add_list(thgc, ea_call->comps, (char*)ev);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ea_call);
			add_list(thgc, local->llvm->tagparents, (char*)ev);
		} break;
		case LetterType::OT_Br: {
			char fvbuf[32];
			snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
			LVari* fv2 = make_lvari_c(thgc, "%CreateKaigyouType*", fvbuf);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv2, make_lvari_c(thgc, "%CreateKaigyouType*", "@CreateKaigyou"), false));
			char evbuf[32];
			snprintf(evbuf, sizeof(evbuf), "%%ev%d", llvm_n++);
			LVari* ev2 = make_lvari_c(thgc, "%ElementType*", evbuf);
			LCall* br_call = make_lcall(thgc, ev2, fv2);
			add_list(thgc, br_call->comps, (char*)(local->llvm->tagparents->size == 0 ? make_lvari_c(thgc, "%ElementType*", "null") : *(LVari**)get_list(local->llvm->tagparents, local->llvm->tagparents->size - 1)));
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)br_call);
			return self;
		}
		default:
			break;
		}

		// Element properties (left, top, gomousedown, etc.)
		for (int i = 1; i < (int)blk1->rets->size; i++) {
			Obj* val = *(Obj**)get_list(blk1->rets, i);
			// labelmapn check
			char ibuf[16];
			snprintf(ibuf, sizeof(ibuf), "%d", i);
			// Property setting via label names - simplified
			// (Full label handling would check labelmapn for each property)
		}

		// tagvars push
		add_list(thgc, local->llvm->tagvars, (char*)createString(thgc, (char*)"", 0, 1));
		add_list(thgc, local->llvm->tagvarvars, (char*)NULL);
		add_list(thgc, local->blocks, (char*)blk2);
		vt_exeC(thgc, *(Obj**)get_list(self->children, 1), local);

		LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
		if (ev != NULL) {
			// Check if tagvars.Last() has content
			String* lasttagvar = *(String**)get_list(local->llvm->tagvars, local->llvm->tagvars->size - 1);
			if (lasttagvar != NULL && lasttagvar->size > 0) {
				char strbuf[32];
				snprintf(strbuf, sizeof(strbuf), "%%str%d", llvm_n++);
				LVari* str = make_lvari_c(thgc, "%StringType*", strbuf);
				int n2val = 1;
				char vb[32];
				snprintf(vb, sizeof(vb), "%%v%d", llvm_n++);
				LVari* v2 = make_lvari_c(thgc, "i8*", vb);
				char svbuf[32];
				snprintf(svbuf, sizeof(svbuf), "@s%d", llvm_n++);
				LStrV* sv = make_lstrv(thgc, createString(thgc, svbuf, (int)strlen(svbuf), 1), lasttagvar, lasttagvar->size * n2val);
				add_list(thgc, local->llvm->strs, (char*)sv);
				char getebuf[64];
				snprintf(getebuf, sizeof(getebuf), "[%d x i8]", lasttagvar->size * n2val + 1);
				LGete* gete = make_lgete(thgc, createString(thgc, getebuf, (int)strlen(getebuf), 1), v2, (LVari*)sv);
				add_list(thgc, gete->comps, (char*)make_lvari_c(thgc, "i32", "0"));
				add_list(thgc, gete->comps, (char*)make_lvari_c(thgc, "i32", "0"));
				add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)gete);

				char fvbuf[32];
				snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
				LVari* fv = make_lvari_c(thgc, "%CreateStringType*", fvbuf);
				add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv, make_lvari_c(thgc, "%CreateStringType*", "@CreateString"), false));
				char lenbuf[16];
				snprintf(lenbuf, sizeof(lenbuf), "%d", lasttagvar->size);
				char n2buf[16];
				snprintf(n2buf, sizeof(n2buf), "%d", n2val);
				LCall* cscall = make_lcall(thgc, str, fv);
				add_list(thgc, cscall->comps, (char*)thgc4);
				add_list(thgc, cscall->comps, (char*)v2);
				add_list(thgc, cscall->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, lenbuf, (int)strlen(lenbuf), 1)));
				add_list(thgc, cscall->comps, (char*)make_lvari(thgc, createString(thgc, (char*)"i32", 3, 1), createString(thgc, n2buf, (int)strlen(n2buf), 1)));
				add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)cscall);

				LVari* lasttagvarvar = *(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1);
				if (lasttagvarvar == NULL) {
					*(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1) = str;
				} else {
					char fv2buf[32];
					snprintf(fv2buf, sizeof(fv2buf), "%%fv%d", llvm_n++);
					LVari* fv2 = make_lvari_c(thgc, "%AddStringType2", fv2buf);
					add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv2, make_lvari_c(thgc, "%AddStringType2*", "@AddString2"), false));
					char str2buf[32];
					snprintf(str2buf, sizeof(str2buf), "%%str%d", llvm_n++);
					LVari* str2 = make_lvari_c(thgc, "%StringType*", str2buf);
					LCall* as_call = make_lcall(thgc, str2, fv2);
					add_list(thgc, as_call->comps, (char*)thgc4);
					add_list(thgc, as_call->comps, (char*)lasttagvarvar);
					add_list(thgc, as_call->comps, (char*)str);
					add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)as_call);
					*(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1) = str2;
				}
				*(String**)get_list(local->llvm->tagvars, local->llvm->tagvars->size - 1) = createString(thgc, (char*)"", 0, 1);
			}
			LVari* lasttagvarvar2 = *(LVari**)get_list(local->llvm->tagvarvars, local->llvm->tagvarvars->size - 1);
			if (lasttagvarvar2 != NULL) {
				char fvbuf[32];
				snprintf(fvbuf, sizeof(fvbuf), "%%fv%d", llvm_n++);
				LVari* fv4 = make_lvari_c(thgc, "%CreateLetterType", fvbuf);
				add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv4, make_lvari_c(thgc, "%CreateLetterType*", "@CreateLetter"), false));
				LCall* cl_call = make_lcall(thgc, NULL, fv4);
				add_list(thgc, cl_call->comps, (char*)thgc4);
				add_list(thgc, cl_call->comps, (char*)*(LVari**)get_list(local->llvm->tagparents, local->llvm->tagparents->size - 1));
				add_list(thgc, cl_call->comps, (char*)lasttagvarvar2);
				add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)cl_call);
			}
			// CreateEnd
			char fv3buf[32];
			snprintf(fv3buf, sizeof(fv3buf), "%%fv%d", llvm_n++);
			LVari* fv3 = make_lvari_c(thgc, "%CreateEndType", fv3buf);
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv3, make_lvari_c(thgc, "%CreateEndType*", "@CreateEnd"), false));
			char ev3buf[32];
			snprintf(ev3buf, sizeof(ev3buf), "%%ev%d", llvm_n++);
			LVari* ev3 = make_lvari_c(thgc, "%ElementType*", ev3buf);
			LCall* end_call = make_lcall(thgc, ev3, fv3);
			add_list(thgc, end_call->comps, (char*)thgc4);
			add_list(thgc, end_call->comps, (char*)*(LVari**)get_list(local->llvm->tagparents, local->llvm->tagparents->size - 1));
			add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)end_call);
		}

		local->blocks->size--;
		local->blocks->size--;
		local->llvm->tagvars->size--;
		local->llvm->tagvarvars->size--;
		local->llvm->tagparents->size--;
		if (id) local->llvm->tagparenttes->size--;
	}
	return self;
}

// ============================================================
// ElemType::exepC
// ============================================================
Obj* exepC_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Obj* val2 = *(Obj**)get_list(primary->children, *n + 1);
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* elemobj = make_cobj(thgc, CType::_CObj, LetterType::OT_ElemObj, val2->letter);
		return elemobj;
	}
	return self;
}

// ============================================================
// ElemType::PrimaryC
// ============================================================
Obj* PrimaryC_ElemType(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		Obj* elemobj = make_cobj(thgc, CType::_CObj, LetterType::OT_ElemObj, val2->letter);
		// declareB
		declareB(thgc, local, val2->letter->text, elemobj);
		return elemobj;
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
}

// ============================================================
// ElemObj::opeC
// ============================================================
Obj* opeC_ElemObj(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) {
	bool is_plus = str_matchA(key, "+", 1);
	bool is_minus = str_matchA(key, "-", 1);
	bool is_mul = str_matchA(key, "*", 1);
	bool is_div = str_matchA(key, "/", 1);
	if ((is_plus || is_minus || is_mul || is_div) && val2 == NULL) {
		// op flags and key setting (simplified)
		return self;
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, self->letter);
}

// ============================================================
// Signal::exepC
// ============================================================
Obj* exepC_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Signal::PrimaryC
// ============================================================
Obj* PrimaryC_Signal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_CallBlock) {
			(*n)++;
			// Check if inside ServerFunction block
			for (int i = local->blocks->size - 1; i > 0; i--) {
				Block* b = *(Block**)get_list(local->blocks, i);
				if (((ObjBlock*)b->obj)->obj->objtype == LetterType::OT_ServerFunction) {
					return NULL; // func
				}
			}
			return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, self->letter);
		}
		return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
}

// ============================================================
// SignalFunction::exeC
// ============================================================
Obj* exeC_SignalFunction(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// SignalFunction::PrimaryC
// ============================================================
Obj* PrimaryC_SignalFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
}

// ============================================================
// SignalFunction::GetterC
// ============================================================
Obj* GetterC_SignalFunction(ThreadGC* thgc, Obj* self, Master* local) {
	// add comps to func.comps
	for (int i = 0; i < local->compsC->size; i++) {
		add_list(thgc, llvm_get_func(local->llvm)->comps, *(char**)get_list(local->compsC, i));
	}
	local->compsC = create_list(thgc, sizeof(LComp*), CType::_List);

	char vbuf[32];
	snprintf(vbuf, sizeof(vbuf), "%%v%d", llvm_n++);
	LVari* fv = make_lvari_c(thgc, "%MemFuncType*", vbuf);
	// func.vari.type = "%MemFuncType**"
	llvm_get_func(local->llvm)->vari->type = createString(thgc, (char*)"%MemFuncType**", 14, 1);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, fv, llvm_get_func(local->llvm)->vari, false));
	// Return clone-like result
	Obj* sf = make_cobj(thgc, CType::_CObj, self->objtype, self->letter);
	sf->vari_c = fv;
	return sf;
}

// ============================================================
// ServerClient::exepC
// ============================================================
Obj* exepC_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// ServerClient::PrimaryC
// ============================================================
Obj* PrimaryC_ServerClient(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			// lookup sigmapA[name + ":" + word.name]
			// simplified - would need string concatenation
			return NULL;
		}
		return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
	} else if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_CallBlock) {
			(*n)++;
			// lookup sigmapA and return ServerFunction
			return NULL;
		}
		return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
	} else if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		// draw = val2 as CallBlock, Dec(local)
		return NULL;
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
}

// ============================================================
// ServerFunction::exeC
// ============================================================
Obj* exeC_ServerFunction(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// ServerFunction::PrimaryC
// ============================================================
Obj* PrimaryC_ServerFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, self->letter);
}

// ============================================================
// ServerFunction::GetterC
// ============================================================
Obj* GetterC_ServerFunction(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// Connect::PrimaryC
// ============================================================
Obj* PrimaryC_Connect(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			// word.name == "new"
			if (val2->objtype == LetterType::OT_Bracket) {
				Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
				if (IS_WEG_C(val)) return val;
				Block* blk1 = (Block*)val;
				if (blk1->rets->size == 1) {
					// create MemTable
					char mtbuf[32];
					snprintf(mtbuf, sizeof(mtbuf), "%%mt%d", llvm_n++);
					LVari* mt = make_lvari_c(thgc, "%CreateMemTableType", mtbuf);
					add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, mt, make_lvari_c(thgc, "%CreateMemTableType*", "@CreateMemTable"), false));
					char mvbuf[32];
					snprintf(mvbuf, sizeof(mvbuf), "%%mv%d", llvm_n++);
					LVari* mv = make_lvari_c(thgc, "%MemTableType*", mvbuf);
					LCall* mv_call = make_lcall(thgc, mv, mt);
					add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)mv_call);
					self->vari_c = mv;
					// Return ConnectStock-like object
					Obj* cs = make_cobj(thgc, CType::_CObj, LetterType::OT_ConnectStock, self->letter);
					return cs;
				}
				return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
			}
			return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
		}
		return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
}

// ============================================================
// ConnectStock::PrimaryC
// ============================================================
Obj* PrimaryC_ConnectStock(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			String* wordname = (*(Obj**)get_list(primary->children, *n - 1))->letter->text;
			if (str_matchA(wordname, "back", 4)) {
				// memtable.back = coroutine
				return make_cobj(thgc, CType::_CObj, LetterType::OT_Variable, self->letter);
			} else if (str_matchA(wordname, "Store", 5)) {
				if (val2->objtype == LetterType::OT_Bracket) {
					Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
					if (IS_WEG_C(val)) return val;
					(*n)++;
					return make_cobj(thgc, CType::_CObj, LetterType::OT_VoiVal, self->letter);
				}
				return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
			} else {
				// lookup in value's draw blocks
				return make_cobj(thgc, CType::_CObj, LetterType::OT_Variable, self->letter);
			}
		}
		return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
	} else if (val2->objtype == LetterType::OT_Block) {
		Obj* blk01 = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		Block* blk_result = (Block*)blk01;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Dot) {
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (val2->objtype == LetterType::OT_Word) {
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				String* word2name = (*(Obj**)get_list(primary->children, *n - 1))->letter->text;
				if (str_matchA(word2name, "Output", 6)) {
					if (val2->objtype == LetterType::OT_Bracket) {
						return make_cobj(thgc, CType::_CObj, LetterType::OT_ElemObj, self->letter);
					}
					return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
				} else if (str_matchA(word2name, "Select", 6)) {
					if (val2->objtype == LetterType::OT_Bracket) {
						Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
						if (IS_WEG_C(val)) return val;
						return val;
					}
					return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
				} else if (str_matchA(word2name, "First", 5)) {
					if (val2->objtype == LetterType::OT_Bracket) {
						return make_cobj(thgc, CType::_CObj, LetterType::OT_ModelVal, self->letter);
					}
					return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
				}
				return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
			}
			return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
		}
		return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Address::PrimaryC
// ============================================================
Obj* PrimaryC_Address(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			if (val2->objtype == LetterType::OT_Dot) {
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				if (val2->objtype == LetterType::OT_ServerClient) {
					(*n)++;
					// LoadProjectC and return vmapA lookup
					return NULL;
				}
				return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
			}
		}
		return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Mountain::exepC
// ============================================================
Obj* exepC_Mountain(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	(*n)++;
	Obj* val2 = *(Obj**)get_list(primary->children, *n);

	// First range word
	if (val2->objtype == LetterType::OT_Word) {
		// Parse range notation (e.g., "A1", "B2")
		// Build array blocks for range
		List* rets_list = create_list(thgc, sizeof(List*), CType::_List);

		// Parse first word into indices
		List* ret = create_list(thgc, sizeof(int), CType::_List);
		// (simplified range parsing)
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		add_list(thgc, rets_list, (char*)ret);

		if (rets_list->size == 1) {
			// Check for Mountain separator
			if (val2->objtype == LetterType::OT_Mountain) {
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				// Parse second range word
				List* ret2 = create_list(thgc, sizeof(int), CType::_List);
				add_list(thgc, rets_list, (char*)ret2);
				(*n)--;
			}
			// Build result block
			Block* blk = make_cblock(thgc, CType::_CBlock, LetterType::OT_Array);
			Block* blk1 = make_cblock(thgc, CType::_CBlock, LetterType::OT_Array);
			blk->rets = create_list(thgc, sizeof(Obj*), CType::_List);
			blk1->rets = create_list(thgc, sizeof(Obj*), CType::_List);
			add_list(thgc, blk->rets, (char*)blk1);
			add_list(thgc, blk->rets, (char*)blk1);
			return (Obj*)blk;
		}
		// Two ranges
		Block* blk = make_cblock(thgc, CType::_CBlock, LetterType::OT_Array);
		Block* blk1 = make_cblock(thgc, CType::_CBlock, LetterType::OT_Array);
		Block* blk2 = make_cblock(thgc, CType::_CBlock, LetterType::OT_Array);
		blk->rets = create_list(thgc, sizeof(Obj*), CType::_List);
		blk1->rets = create_list(thgc, sizeof(Obj*), CType::_List);
		blk2->rets = create_list(thgc, sizeof(Obj*), CType::_List);
		add_list(thgc, blk->rets, (char*)blk1);
		add_list(thgc, blk->rets, (char*)blk2);
		return (Obj*)blk;
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, val2->letter);
}

// ============================================================
// Sum::PrimaryC
// ============================================================
Obj* PrimaryC_Sum(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		(*n)++;
		// Sum computation - simplified for C pass
		Obj* val = vt_GetterB(thgc, vt_exeB(thgc, val2, local), local);
		local->compsC->size--;
		if (IS_WEG_C(val)) return val;
		// Return Number result
		Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, self->letter);
		return num;
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
}

// ============================================================
// Question::exepC
// ============================================================
Obj* exepC_Question(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	Block* blk = make_cblock(thgc, CType::_CBlock, LetterType::OT_Array);
	Block* blk1 = make_cblock(thgc, CType::_CBlock, LetterType::OT_Array);
	blk->rets = create_list(thgc, sizeof(Obj*), CType::_List);
	blk1->rets = create_list(thgc, sizeof(Obj*), CType::_List);
	add_list(thgc, blk->rets, (char*)blk1);
	add_list(thgc, blk->rets, (char*)blk1);
	for (int i = 0; i < 2; i++) {
		Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
		add_list(thgc, blk1->rets, (char*)num);
	}
	return (Obj*)blk1;
}

// ============================================================
// LinearFunction::PrimaryC
// ============================================================
Obj* PrimaryC_LinearFunction(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Bracket) {
		(*n)++;
		// Animation function creation (simplified for C pass)
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		local->compsC->size--;
		if (IS_WEG_C(val)) return val;
		// Return AnimationFunction-like result
		Obj* anif = make_cobj(thgc, CType::_CObj, LetterType::OT_AnimationFunction, self->letter);
		return anif;
	}
	return make_cobj(thgc, CType::_CObj, LetterType::OT_NG, val2->letter);
}
