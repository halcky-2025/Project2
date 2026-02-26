// ============================================================
// obj2c.h - C隗｣譫舌ヱ繧ｹ (LLVM IR code generation) - Obj2C.cs converted
// ============================================================

// ============================================================
// 蜑肴婿螳｣險
// ============================================================
Obj* SelfC_Word(ThreadGC* thgc, Obj* self, Master* local);
Obj* GetterC_Word(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_Word(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Word(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* OpeC_Value(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* ope_Value(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2);
Obj* PrimaryC_Value(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
void Dec_ClassObj(ThreadGC* thgc, Obj* self, Master* local);
Obj* PrimaryC_Type(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_Var(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Generic(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* GetterC_Number(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_Number(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Number2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PlusC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* MinusC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* MulC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* DivC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* MoreThanC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* LessThanC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* MoreEqualC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* LessEqualC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* NotEqualC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* EqualEqualC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* exeC_FloatVal(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PlusC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* MinusC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* MulC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* DivC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* MoreThanC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* LessThanC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* MoreEqualC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* LessEqualC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* NotEqualC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* EqualEqualC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2);
Obj* GetterC_StrObj(ThreadGC* thgc, Obj* self, Master* local);
Obj* exeC_StrObj(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeC_BoolVal2(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_BoolVal2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_BoolVal2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exepC_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* exeC_Null(ThreadGC* thgc, Obj* self, Master* local);
Obj* exepC_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary);
Obj* PrimaryC_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryC_ArrayConstructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);
Obj* PrimaryC_Constructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2);

// ============================================================
// Word::SelfC
// ============================================================
Obj* SelfC_Word(ThreadGC* thgc, Obj* self, Master* local) {
	Obj* obj = vt_SelfC(thgc, getC(thgc, local, self->letter->text), local);
	obj->letter = self->letter;
	return obj;
}

// ============================================================
// Word::GetterC
// ============================================================
Obj* GetterC_Word(ThreadGC* thgc, Obj* self, Master* local) {
	Obj* obj = vt_GetterC(thgc, vt_SelfC(thgc, getC(thgc, local, self->letter->text), local), local);
	obj->letter = self->letter;
	return obj;
}

// ============================================================
// Word::exepC
// ============================================================
Obj* exepC_Word(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Word::PrimaryC
// ============================================================
Obj* PrimaryC_Word(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	(*n)++;
	return self;
}

// ============================================================
// Value::OpeC
// ============================================================
Obj* OpeC_Value(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Obj* cls = self->vartype;
	LVari* selfvari = self->vari_c;

	if (val2->objtype == LetterType::OT_Value) {
		Obj* value = val2;
		Obj* vcls = value->vartype;
		if (vcls->objtype == LetterType::OT_Var) vcls = ((VariClass*)vcls)->base;
		value->vartype = vcls;

		if (cls == local->Int || cls == local->Short) {
			const char* sizetype = "i32";
			if (cls == local->Short) sizetype = "i16";

			if (vcls == local->StrT) {
				if (str_matchA(op, "+", 1)) {
					if (strcmp(sizetype, "i32") != 0) {
						LVari* v = make_vn(thgc, "i32");
						add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, selfvari));
						selfvari = v;
					}
					LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
					LVari* nsvari = make_vn(thgc, "%NumberStringType");
					add_list(thgc, func->comps, (char*)make_lload(thgc, nsvari, make_lvari_c(thgc, "%NumberStringType*", "@NumberString"), false));
					LVari* v0 = make_vn(thgc, "%StringType*");
					LCall* call0 = make_lcall(thgc, v0, nsvari);
					lcall_add(thgc, call0, thgc4);
					lcall_add(thgc, call0, selfvari);
					add_list(thgc, func->comps, (char*)call0);
					LVari* as2vari = make_vn(thgc, "%AddStringType2*");
					add_list(thgc, func->comps, (char*)make_lload(thgc, as2vari, make_lvari_c(thgc, "%AddStringType2*", "@AddString2"), false));
					LVari* v1 = make_vn(thgc, "%StringType*");
					LCall* call1 = make_lcall(thgc, v1, as2vari);
					lcall_add(thgc, call1, thgc4);
					lcall_add(thgc, call1, v0);
					lcall_add(thgc, call1, value->vari_c);
					add_list(thgc, func->comps, (char*)call1);
					Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					ret->vartype = local->StrT;
					ret->vari_c = v1;
					return ret;
				}
				else return ErrorC(thgc, LetterType::OT_Error, self->letter, "値の演算は+がサポートされていません。");
			}
			else if (vcls == local->Int || vcls == local->Short) {
				Obj* ret_cls = cls;
				if (cls != vcls) {
					sizetype = "i32";
					if (vcls == local->Short) {
						LVari* v = make_vn(thgc, "i32");
						add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, value->vari_c));
						value->vari_c = v;
						ret_cls = local->Int;
					}
					else if (cls == local->Short) {
						LVari* v = make_vn(thgc, "i32");
						add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, selfvari));
						selfvari = v;
						ret_cls = local->Int;
					}
				}
				LVari* vari = make_lvari_c(thgc, sizetype, (char*)fmt_str(thgc, "%%v%d", llvm_n++)->data);
				if (str_matchA(op, "+", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Add, vari, selfvari, val2->vari_c));
				else if (str_matchA(op, "-", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, vari, selfvari, val2->vari_c));
				else if (str_matchA(op, "*", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Mul, vari, selfvari, val2->vari_c));
				else if (str_matchA(op, "/", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_SDiv, vari, selfvari, val2->vari_c));
				else if (str_matchA(op, "%", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_SRem, vari, selfvari, val2->vari_c));
				else if (str_matchA(op, "==", 2)) {
					vari->type = createString(thgc, (char*)"i1", 2, 1);
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_Eq, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, "!=", 2)) {
					vari->type = createString(thgc, (char*)"i1", 2, 1);
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_Ne, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, "<", 1)) {
					vari->type = createString(thgc, (char*)"i1", 2, 1);
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SLT, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, ">", 1)) {
					vari->type = createString(thgc, (char*)"i1", 2, 1);
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SGT, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, "<=", 2)) {
					vari->type = createString(thgc, (char*)"i1", 2, 1);
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SLE, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, ">=", 2)) {
					vari->type = createString(thgc, (char*)"i1", 2, 1);
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SGE, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else return ErrorC(thgc, LetterType::OT_Error, self->letter, "値の演算はサポートされていません。");
				Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
				ret->vartype = ret_cls; ret->vari_c = vari;
				return ret;
			}
		}
		else if (cls == local->Bool) {
			// empty - same as C#
		}
		else if (cls == local->StrT) {
			if (vcls == local->Int) {
				if (vcls == local->Short) {
					LVari* v = make_vn(thgc, "i32");
					add_list(thgc, func->comps, (char*)make_lcast(thgc, LCAST_Bitcast, v, value->vari_c));
					value->vari_c = v;
				}
				if (str_matchA(op, "+", 1)) {
					LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
					LVari* nsvari = make_vn(thgc, "%NumberStringType");
					add_list(thgc, func->comps, (char*)make_lload(thgc, nsvari, make_lvari_c(thgc, "%NumberStringType*", "@NumberString"), false));
					LVari* v0 = make_vn(thgc, "%StringType*");
					LCall* call0 = make_lcall(thgc, v0, nsvari);
					lcall_add(thgc, call0, thgc4);
					lcall_add(thgc, call0, value->vari_c);
					add_list(thgc, func->comps, (char*)call0);
					LVari* as2vari = make_vn(thgc, "%AddStringType2*");
					add_list(thgc, func->comps, (char*)make_lload(thgc, as2vari, make_lvari_c(thgc, "%AddStringType2*", "@AddString2"), false));
					LVari* v1 = make_vn(thgc, "%StringType*");
					LCall* call1 = make_lcall(thgc, v1, as2vari);
					lcall_add(thgc, call1, thgc4);
					lcall_add(thgc, call1, selfvari);
					lcall_add(thgc, call1, v0);
					add_list(thgc, func->comps, (char*)call1);
					Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					ret->vartype = local->StrT; ret->vari_c = v1;
					return ret;
				}
				else return ErrorC(thgc, LetterType::OT_Error, self->letter, "値の演算はサポートされていません。");
			}
			else if (vcls == local->StrT) {
				if (str_matchA(op, "+", 1)) {
					LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
					LVari* as2vari = make_vn(thgc, "%AddStringType2*");
					add_list(thgc, func->comps, (char*)make_lload(thgc, as2vari, make_lvari_c(thgc, "%AddStringType2*", "@AddString2"), false));
					LVari* v1 = make_vn(thgc, "%StringType*");
					LCall* call1 = make_lcall(thgc, v1, as2vari);
					lcall_add(thgc, call1, thgc4);
					lcall_add(thgc, call1, selfvari);
					lcall_add(thgc, call1, value->vari_c);
					add_list(thgc, func->comps, (char*)call1);
					Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					ret->vartype = local->StrT; ret->vari_c = v1;
					return ret;
				}
				else if (str_matchA(op, "==", 2)) {
					LVari* vari = make_vn(thgc, "i1");
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_Eq, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, "!=", 2)) {
					LVari* vari = make_vn(thgc, "i1");
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_Ne, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, "<", 1)) {
					LVari* vari = make_vn(thgc, "i1");
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SLT, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, ">", 1)) {
					LVari* vari = make_vn(thgc, "i1");
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SGT, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, "<=", 2)) {
					LVari* vari = make_vn(thgc, "i1");
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SLE, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else if (str_matchA(op, ">=", 2)) {
					LVari* vari = make_vn(thgc, "i1");
					add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SGE, vari, selfvari, val2->vari_c));
					Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
					r->vartype = local->Bool; r->vari_c = vari;
					return r;
				}
				else return ErrorC(thgc, LetterType::OT_Error, self->letter, "値の演算はサポートされていません。");
			}
			else return ErrorC(thgc, LetterType::OT_Error, self->letter, "値の演算はサポートされていません。");
		}
	}
	return ErrorC(thgc, LetterType::OT_Error, self->letter, "値の演算は値同士でなければなりません。");
}

// ============================================================
// Value::ope
// ============================================================
Obj* ope_Value(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Obj* cls = self->vartype;
	LVari* vari = NULL;
	LVari* vari2 = NULL;
	if (cls == local->Int) {
		vari = make_vn(thgc, "i32*");
	}
	else if (cls == local->Bool) {
		vari = make_vn(thgc, "i2*");
	}
	add_list(thgc, func->comps, (char*)make_lload(thgc, vari, self->vari_c, false));
	Obj* val = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
	val->vartype = cls; val->vari_c = vari2;
	if (str_matchA(key, "+", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Add, vari2, vari, val2->vari_c));
	else if (str_matchA(key, "-", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Sub, vari2, vari, val2->vari_c));
	else if (str_matchA(key, "*", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_Mul, vari2, vari, val2->vari_c));
	else if (str_matchA(key, "/", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_SDiv, vari2, vari, val2->vari_c));
	else if (str_matchA(key, "%", 1)) add_list(thgc, func->comps, (char*)make_lbinop(thgc, LBOP_SRem, vari2, vari, val2->vari_c));
	else if (str_matchA(key, "==", 2)) {
		vari2->type = createString(thgc, (char*)"i1", 2, 1);
		add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_Eq, vari2, vari, val2->vari_c));
	}
	else if (str_matchA(key, "!=", 2)) {
		vari2->type = createString(thgc, (char*)"i1", 2, 1);
		add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_Ne, vari2, vari, val2->vari_c));
	}
	else if (str_matchA(key, ">", 1)) {
		vari2->type = createString(thgc, (char*)"i1", 2, 1);
		add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SGT, vari2, vari, val2->vari_c));
	}
	else if (str_matchA(key, "<", 1)) {
		vari2->type = createString(thgc, (char*)"i1", 2, 1);
		add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SLT, vari2, vari, val2->vari_c));
	}
	else if (str_matchA(key, ">=", 2)) {
		vari2->type = createString(thgc, (char*)"i1", 2, 1);
		add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SGE, vari2, vari, val2->vari_c));
	}
	else if (str_matchA(key, "<=", 2)) {
		vari2->type = createString(thgc, (char*)"i1", 2, 1);
		add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_SLE, vari2, vari, val2->vari_c));
	}
	return val;
}

// ============================================================
// Value::PrimaryC
// ============================================================
Obj* PrimaryC_Value(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Obj* cls = self->vartype;

	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			String* name = val2->letter->text;
			Block* blk0 = (Block*)*(Obj**)get_list(((ClassObj*)cls)->draw->children, 0);
			Block* blk1 = (Block*)*(Obj**)get_list(((ClassObj*)cls)->draw->children, 1);
			Map* vmap0 = blk0->vmapA;
			Map* vmap1 = blk1->vmapA;
			char* found0 = get_mapy(vmap0, name);
			if (found0 != NULL) {
				Obj* v = (Obj*)found0;
				if (v->objtype == LetterType::OT_Variable) {
					Variable* variable = (Variable*)v;
					LVari* vari;
					Obj* vcls = variable->vartype;
					if (vcls->objtype == LetterType::OT_Var) vcls = ((VariClass*)vcls)->base;
					variable->vartype = vcls;
					if (vcls == local->Int) vari = make_vn(thgc, "i32*");
					else if (vcls == local->Bool) vari = make_vn(thgc, "i2*");
					else vari = make_vn(thgc, (char*)vcls->model->data);
					char orderbuf[16]; snprintf(orderbuf, sizeof(orderbuf), "%d", variable->order);
					LGete* gete = make_lgete_idx2(thgc, (char*)cls->model->data, vari, self->vari_c,
						make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf));
					add_list(thgc, func->comps, (char*)gete);
					Variable* newvar = (Variable*)make_cobj(thgc, CType::_CVariable, LetterType::OT_Variable, variable->letter);
					newvar->vartype = vcls;
					newvar->vari_c = vari;
					return (Obj*)newvar;
				}
				else {
					char* found1 = get_mapy(vmap1, name);
					if (found1 != NULL && ((Obj*)found1)->objtype == LetterType::OT_Function) {
						Function* f = (Function*)found1;
						LVari* vari;
						Obj* fret = f->rettype;
						if (fret->objtype == LetterType::OT_Var) fret = ((VariClass*)fret)->base;
						f->rettype = fret;
						if (fret == local->Int) vari = make_vn(thgc, "i32*");
						else if (fret == local->Bool) vari = make_vn(thgc, "i2*");
						else vari = make_vn(thgc, (char*)fret->model->data);
						char orderbuf[16]; snprintf(orderbuf, sizeof(orderbuf), "%d", f->order);
						LGete* gete = make_lgete_idx2(thgc, (char*)cls->model->data, vari, self->vari_c,
							make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf));
						add_list(thgc, func->comps, (char*)gete);
						func->vari = vari;
						return (Obj*)f;
					}
				}
			}
			else {
				char* found1 = get_mapy(vmap1, name);
				if (found1 != NULL) {
					Obj* v = (Obj*)found1;
					if (v->objtype == LetterType::OT_Variable) {
						Variable* variable = (Variable*)v;
						LVari* vari;
						Obj* vcls = variable->vartype;
						if (vcls->objtype == LetterType::OT_Var) vcls = ((VariClass*)vcls)->base;
						variable->vartype = vcls;
						if (vcls == local->Int) vari = make_vn(thgc, "i32*");
						else if (vcls == local->Bool) vari = make_vn(thgc, "i2*");
						else vari = make_vn(thgc, (char*)vcls->model->data);
						char orderbuf[16]; snprintf(orderbuf, sizeof(orderbuf), "%d", variable->order);
						LGete* gete = make_lgete_idx2(thgc, (char*)cls->model->data, vari, self->vari_c,
							make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf));
						add_list(thgc, func->comps, (char*)gete);
						Variable* newvar = (Variable*)make_cobj(thgc, CType::_CVariable, LetterType::OT_Variable, variable->letter);
						newvar->vartype = vcls;
						newvar->vari_c = vari;
						return (Obj*)newvar;
					}
					else if (v->objtype == LetterType::OT_Function) {
						Function* f = (Function*)v;
						LVari* vari;
						Obj* fret = f->rettype;
						if (fret->objtype == LetterType::OT_Var) fret = ((VariClass*)fret)->base;
						f->rettype = fret;
						if (fret == local->Int) vari = make_vn(thgc, "i32*");
						else if (fret == local->Bool) vari = make_vn(thgc, "i2*");
						else vari = make_vn(thgc, (char*)fret->model->data);
						char orderbuf[16]; snprintf(orderbuf, sizeof(orderbuf), "%d", f->order);
						LGete* gete = make_lgete_idx2(thgc, (char*)cls->model->data, vari, self->vari_c,
							make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", orderbuf));
						add_list(thgc, func->comps, (char*)gete);
						func->vari = vari;
						return (Obj*)f;
					}
				}
			}
		}
	}
	else if (val2->objtype == LetterType::OT_Block) {
		(*n)++;
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		Block* blk = (Block*)val;
		if (cls->objtype == LetterType::OT_Var) cls = ((VariClass*)cls)->base;
		self->vartype = cls;
		if (cls == local->StrT) {
			self->vari_c->type = createString(thgc, (char*)"s[n]", 4, 1);
			Obj* bval = *(Obj**)get_list(blk->rets, 0);
			if (bval->objtype == LetterType::OT_Value && bval->vartype == local->Int) {
				Variable* variable = (Variable*)make_cobj(thgc, CType::_CVariable, LetterType::OT_Variable, self->letter);
				variable->vartype = local->Short;
				variable->vari_c = self->vari_c;
				variable->forvari = bval->vari_c;
				return (Obj*)variable;
			}
			else return ErrorC(thgc, LetterType::OT_Error, self->letter, "変数のブロックは整数型をとるしかありません。");
		}
		if (blk->rets->size > 0) {
			Obj* bval = *(Obj**)get_list(blk->rets, 0);
			if (bval->objtype == LetterType::OT_Value) {
				if (bval->vartype == local->Int) {
					LVari* v0 = make_vn(thgc, "%ListType**");
					add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%HashType", v0, self->vari_c,
						make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
					LVari* v05 = make_vn(thgc, "%ListType*");
					add_list(thgc, func->comps, (char*)make_lload(thgc, v05, v0, false));
					LVari* varigl = make_vn(thgc, "%GetListType");
					add_list(thgc, func->comps, (char*)make_lload(thgc, varigl, make_lvari_c(thgc, "%GetListType*", "@GetList"), false));
					LVari* v1 = make_vn(thgc, "%KeyValueType**");
					LCall* call1 = make_lcall(thgc, v1, varigl);
					lcall_add(thgc, call1, v05);
					lcall_add(thgc, call1, bval->vari_c);
					add_list(thgc, func->comps, (char*)call1);
					LVari* v15 = make_vn(thgc, "%KeyValueType*");
					add_list(thgc, func->comps, (char*)make_lload(thgc, v15, v1, false));
					LVari* v2 = make_vn(thgc, "[n]");
					add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%KeyValueType", v2, v15,
						make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
					self->vari_c = v2;
					if (cls->objtype == LetterType::OT_ArrType) {
						ArrType* arrtype = (ArrType*)cls;
						Variable* variable = (Variable*)make_cobj(thgc, CType::_CVariable, LetterType::OT_Variable, self->letter);
						variable->vartype = arrtype->base;
						variable->vari_c = self->vari_c;
						return (Obj*)variable;
					}
					else return ErrorC(thgc, LetterType::OT_Error, self->letter, "ブロックは配列型をとるしかありません。");
				}
				else if (bval->vartype == local->StrT) {
					self->vari_c->type = createString(thgc, (char*)"[s]", 3, 1);
					if (cls->objtype == LetterType::OT_Var) cls = ((VariClass*)cls)->base;
					self->vartype = cls;
					if (cls->objtype == LetterType::OT_ArrType) {
						ArrType* arrtype = (ArrType*)cls;
						Variable* variable = (Variable*)make_cobj(thgc, CType::_CVariable, LetterType::OT_Variable, self->letter);
						variable->vartype = arrtype->base;
						variable->vari_c = self->vari_c;
						variable->forvari = bval->vari_c;
						return (Obj*)variable;
					}
					else return ErrorC(thgc, LetterType::OT_Error, self->letter, "ブロックは配列型をとるしかありません。");
				}
				else return ErrorC(thgc, LetterType::OT_Error, blk->letter, "配列の引数は整数か文字列だけです。");
			}
		}
		return ErrorC(thgc, LetterType::OT_Error, blk->letter, "配列の引数は整数か文字列だけです。");
	}
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "値の後続が適切ではないです。");
}

// ============================================================
// Class::exepC
// ============================================================
Obj* exepC_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Class::PrimaryC
// ============================================================
Obj* PrimaryC_Class(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Word) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Block) {
			Obj* block = vt_exeC(thgc, val2, local);
			Block* blk = (Block*)block;
			add_list(thgc, local->blocks, (char*)blk);
			for (int i = 0; i < blk->rets->size; i++) {
				val2 = *(Obj**)get_list(blk->rets, i);
			}
			remove_list(thgc, local->blocks, local->blocks->size - 1);
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
		}
	}
head:
	if (val2->objtype == LetterType::OT_Left) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Bracket) {
			Obj* val = vt_GetterC(thgc, vt_exeC(thgc, *(Obj**)get_list(primary->children, *n), local), local);
			if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
			Block* blk = (Block*)val2;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			for (int i = 0; i < blk->rets->size; i++) {
				Obj* ri = *(Obj**)get_list(blk->rets, i);
				if (ri->objtype == LetterType::OT_ClassObj || ri->objtype == LetterType::OT_GeneObj) {
					goto head;
				}
			else return ErrorC(thgc, LetterType::OT_Error, blk->letter, "クラスの継承先はクラスでなければなりません");
			}
		}
		else {
			val2 = vt_GetterC(thgc, val2, local);
			if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
			if (val2->objtype == LetterType::OT_ClassObj) {
				(*n)++;
				val2 = *(Obj**)get_list(primary->children, *n);
				goto head;
			}
			else if (val2->objtype == LetterType::OT_Generic) {
				(*n)++;
				if (val2->objtype == LetterType::OT_Block) {
					val2 = vt_GetterC(thgc, vt_exeC(thgc, *(Obj**)get_list(primary->children, *n), local), local);
					if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
					Block* block = (Block*)val2;
					(*n)++;
					val2 = *(Obj**)get_list(primary->children, *n);
					goto head;
				}
			else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "ジェネリッククラスは[]で引数をもたなければならない");
			}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "クラスの継承先はクラスでなければなりません");
		}
	}
	if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		return self;
	}
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "クラスの宣言の{|}が抜けています");
}

// ============================================================
// Type::PrimaryC
// ============================================================
Obj* PrimaryC_Type(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Obj* type = self;
head:
	if (val2->objtype == LetterType::OT_Block) {
		Block* blk = (Block*)val2;
		if (blk->children->size == 0) {
			ArrType* arrtype = (ArrType*)GC_alloc(thgc, CType::_ArrType);
			arrtype->objtype = LetterType::OT_ArrType;
			arrtype->base = type;
			arrtype->letter = val2->letter;
			type = (Obj*)arrtype;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			goto head;
		}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "配列型は引数の数が0です");
	}
	else if (val2->objtype == LetterType::OT_Left) {
		FuncType2* functype = (FuncType2*)GC_alloc(thgc, CType::_FuncType2);
		functype->objtype = LetterType::OT_FuncType;
		functype->rettype = type;
		functype->draws = create_list(thgc, sizeof(Obj*), CType::_List);
		type = (Obj*)functype;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Bracket) {
			val2 = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
			if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
			Block* blk = (Block*)val2;
			for (int i = 0; i < blk->rets->size; i++) {
				val2 = *(Obj**)get_list(blk->rets, i);
				add_list(thgc, functype->draws, (char*)val2);
			}
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
		}
		else {
			val2 = vt_GetterC(thgc, val2, local);
			if (val2->objtype == LetterType::OT_Wait || val2->objtype == LetterType::OT_Error || val2->objtype == LetterType::OT_NG) return val2;
			add_list(thgc, functype->draws, (char*)val2);
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			goto head;
		}
	}

	Obj* word = NULL;
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			if (type->objtype == LetterType::OT_ArrType) {
				if (str_matchA(val2->letter->text, "new", 3)) {
					Obj* ac = make_cobj(thgc, CType::_CObj, LetterType::OT_ArrayConstructor, val2->letter);
					ac->vartype = type;
					return ac;
				}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "配列型の後続にこれません");
			}
			else if (type->objtype == LetterType::OT_ClassObj || type->objtype == LetterType::OT_ArrType) {
				if (str_matchA(val2->letter->text, "new", 3)) {
					Obj* ctor = make_cobj(thgc, CType::_CObj, LetterType::OT_Constructor, self->letter);
					ctor->vartype = type;
					return ctor;
				}
				else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "繧ｯ繝ｩ繧ｹ縺ｮ蠕檎ｶ壹↓縺薙ｌ縺ｾ縺帙ｓ");
			}
			else if (type->objtype == LetterType::OT_ModelObj || type->objtype == LetterType::OT_GeneObj) {
				if (str_matchA(val2->letter->text, "new", 3)) {
					Obj* ctor = make_cobj(thgc, CType::_CObj, LetterType::OT_Constructor, val2->letter);
					ctor->vartype = type;
					return ctor;
				}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "型の後続にこれません");
			}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "関数型は.が来れません");
		}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, ".の後には単語が必要です");
	}
	else if (val2->objtype == LetterType::OT_Dolor) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			(*n)++;
			return type;
		}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "コメント変数の名前を指定してください。");
	}
	else if (val2->objtype == LetterType::OT_Word) {
		word = val2;
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
	}
	else if (val2->objtype == LetterType::OT_Bracket) {
		Obj* blk = vt_exeC(thgc, val2, local);
		Block* blk2 = (Block*)blk;
		(*n)++;
		for (int i = 0; i < blk2->rets->size; i++) {
			Obj* ri = *(Obj**)get_list(blk2->rets, i);
			if (ri->objtype == LetterType::OT_Word) {
				Obj* variable = declareC(thgc, local, ri->letter->text);
				variable->letter = ri->letter;
			}
			else return ErrorC(thgc, LetterType::OT_Error, blk2->letter, "変数宣言は名前だけです");
		}
		return blk;
	}

	if (val2->objtype == LetterType::OT_Block) {
		Obj* blk = vt_exeC(thgc, val2, local);
		Block* blk2 = (Block*)blk;
		for (int i = 0; i < blk2->rets->size; i++) {
			Obj* ri = *(Obj**)get_list(blk2->rets, i);
			if (ri->objtype != LetterType::OT_Word) {
		return ErrorC(thgc, LetterType::OT_Error, blk2->letter, "ジェネリック関数の宣言は名前しか引数をとれません");
			}
		}
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_CallBlock) {
			(*n)++;
			if (word != NULL) declareC(thgc, local, word->letter->text);
			return self;
		}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "ジェネリック関数の{|}が指定されていません");
	}
	else if (val2->objtype == LetterType::OT_CallBlock) {
		(*n)++;
		if (word != NULL) {
			Obj* variable = declareC(thgc, local, word->letter->text);
			variable->letter = word->letter;
		}
		return self;
	}
	else {
		if (word == NULL) return type;
		else {
			Obj* variable = declareC(thgc, local, word->letter->text);
			variable->letter = word->letter;
			return variable;
		}
	}
}

// ============================================================
// Var::exepC
// ============================================================
Obj* exepC_Var(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Generic::PrimaryC
// ============================================================
Obj* PrimaryC_Generic(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Block) {
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		Block* block = (Block*)val;
		return (Obj*)block;
	}
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "ジェネリッククラスは[]を引数にとります");
}

// ============================================================
// Number::GetterC
// ============================================================
Obj* GetterC_Number(ThreadGC* thgc, Obj* self, Master* local) {
	char buf[32];
	snprintf(buf, sizeof(buf), "%d", self->n_c);
	Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
	ret->vartype = local->Int;
	ret->vari_c = make_lvari_c(thgc, "i32", buf);
	return ret;
}

// ============================================================
// Number::exepC
// ============================================================
Obj* exepC_Number(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Number::PrimaryC
// ============================================================
Obj* PrimaryC_Number2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (val2->objtype == LetterType::OT_Dot) {
		(*n)++;
		val2 = *(Obj**)get_list(primary->children, *n);
		if (val2->objtype == LetterType::OT_Word) {
			String* wname = val2->letter->text;
			(*n)++;
			val2 = *(Obj**)get_list(primary->children, *n);
			if (str_matchA(wname, "random", 6)) {
				if (val2->objtype == LetterType::OT_Bracket) {
					(*n)++;
				if (((Block*)val2)->children->size != 0) return ErrorC(thgc, LetterType::OT_Error, val2->letter, "randomの引数は0個です");
					Obj* num = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, NULL);
					num->vartype = local->Int;
					return num;
				}
			else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "randomの()が指定されていません");
			}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, "適切な後続の単語ではありません");
		}
		else return ErrorC(thgc, LetterType::OT_Error, val2->letter, ".の後に単語が来ていません");
	}
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, ".が来ていません");
}

// ============================================================
// Number::PlusC
// ============================================================
Obj* PlusC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2 == NULL) return self;
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, self->letter);
		r->n_c = self->n_c + val2->n_c;
		r->vartype = local->Int;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	else if (val2->objtype == LetterType::OT_StrObj) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_StrObj, self->letter);
		r->vartype = local->StrT;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の+は整数か小数か文字列です");
}

// ============================================================
// Number::MinusC
// ============================================================
Obj* MinusC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2 == NULL) {
		Obj* r = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, self->letter);
		r->n_c = -(self->n_c);
		r->vartype = local->Int;
		return r;
	}
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, self->letter);
		r->n_c = self->n_c - val2->n_c;
		r->vartype = local->Int;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の-は整数か小数です");
}

// ============================================================
// Number::MulC
// ============================================================
Obj* MulC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, self->letter);
		r->n_c = self->n_c * val2->n_c;
		r->vartype = local->Int;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の*は整数か小数です");
}

// ============================================================
// Number::DivC
// ============================================================
Obj* DivC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CNumber, LetterType::OT_Number, self->letter);
		r->n_c = self->n_c / val2->n_c;
		r->vartype = local->Int;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の/は整数か小数です");
}

// ============================================================
// Number::MoreThanC
// ============================================================
Obj* MoreThanC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->n_c = (self->n_c > val2->n_c) ? 1 : 0;
		r->vartype = local->Bool;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の>は整数か小数です");
}

// ============================================================
// Number::LessThanC
// ============================================================
Obj* LessThanC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->n_c = (self->n_c < val2->n_c) ? 1 : 0;
		r->vartype = local->Bool;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の<は整数か小数です");
}

// ============================================================
// Number::MoreEqualC
// ============================================================
Obj* MoreEqualC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->n_c = (self->n_c >= val2->n_c) ? 1 : 0;
		r->vartype = local->Bool;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の>=は整数か小数です");
}

// ============================================================
// Number::LessEqualC
// ============================================================
Obj* LessEqualC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->n_c = (self->n_c <= val2->n_c) ? 1 : 0;
		r->vartype = local->Bool;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の<=は整数か小数です");
}

// ============================================================
// Number::NotEqualC
// ============================================================
Obj* NotEqualC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->n_c = (self->n_c != val2->n_c) ? 1 : 0;
		r->vartype = local->Bool;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の!=は整数か小数です");
}

// ============================================================
// Number::EqualEqualC
// ============================================================
Obj* EqualEqualC_Number(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->n_c = (self->n_c == val2->n_c) ? 1 : 0;
		r->vartype = local->Bool;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "整数の==は整数か小数です");
}

// ============================================================
// FloatVal::exeC
// ============================================================
Obj* exeC_FloatVal(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// FloatVal::exepC
// ============================================================
Obj* exepC_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// FloatVal::PrimaryC
// ============================================================
Obj* PrimaryC_FloatVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "蟆乗焚縺ｫ縺ｯ蠕檎ｶ壹′譚･繧後∪縺帙ｓ");
}

// ============================================================
// FloatVal::PlusC
// ============================================================
Obj* PlusC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2 == NULL) return self;
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	else if (val2->objtype == LetterType::OT_StrObj) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_StrObj, self->letter);
		r->vartype = local->StrT;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の+は整数か小数です");
}

// ============================================================
// FloatVal::MinusC
// ============================================================
Obj* MinusC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2 == NULL) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の-は整数か小数です");
}

// ============================================================
// FloatVal::MulC
// ============================================================
Obj* MulC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の*は整数か小数です");
}

// ============================================================
// FloatVal::DivC
// ============================================================
Obj* DivC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	else if (val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_FloatVal, self->letter);
		r->vartype = local->FloatT;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の/は整数か小数です");
}

// ============================================================
// FloatVal::MoreThanC
// ============================================================
Obj* MoreThanC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の>は整数か小数です");
}

// ============================================================
// FloatVal::LessThanC
// ============================================================
Obj* LessThanC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の<は整数か小数です");
}

// ============================================================
// FloatVal::MoreEqualC
// ============================================================
Obj* MoreEqualC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の>=は整数か小数です");
}

// ============================================================
// FloatVal::LessEqualC
// ============================================================
Obj* LessEqualC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の<=は整数か小数です");
}

// ============================================================
// FloatVal::NotEqualC
// ============================================================
Obj* NotEqualC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の!=は整数か小数です");
}

// ============================================================
// FloatVal::EqualEqualC
// ============================================================
Obj* EqualEqualC_FloatVal(ThreadGC* thgc, Obj* self, String* op, Master* local, Obj* val2) {
	if (val2->objtype == LetterType::OT_Number || val2->objtype == LetterType::OT_FloatVal) {
		Obj* r = make_cobj(thgc, CType::_CObj, LetterType::OT_BoolVal, self->letter);
		r->vartype = local->Bool;
		return r;
	}
	return ErrorC(thgc, LetterType::OT_Error, val2->letter, "小数の==は整数か小数です");
}

// ============================================================
// StrObj::GetterC
// ============================================================
Obj* GetterC_StrObj(ThreadGC* thgc, Obj* self, Master* local) {
	LFunc* func = llvm_get_func(local->llvm);
	LVari* csvari = make_vn(thgc, "%CreateStringType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, csvari, make_lvari_c(thgc, "%CreateStringType*", "@CreateString"), false));
	LVari* v2 = make_vn(thgc, "i8*");

	// Get string value from self - use letter->text as the source
	String* value = self->letter->text;
	int n = 1;
	if (value != NULL) {
		for (int i = 0; i < value->size; i++) {
			if ((unsigned char)value->data[i] >= 128) {
				n = 2;
				break;
			}
		}
	}
	int slen = (value != NULL) ? value->size : 0;
	String* svname = fmt_str(thgc, "@s%d", llvm_n++);
	LStrV* sv = make_lstrv(thgc, svname, value, slen * n);
	add_list(thgc, local->llvm->strs, (char*)sv);

	String* gete_type = fmt_str(thgc, "[%d x i8]", slen * n + 1);
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, (char*)gete_type->data, v2, (LVari*)sv,
		make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	LVari* v = make_vn(thgc, "%StringType*");
	LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	char lenbuf[16]; snprintf(lenbuf, sizeof(lenbuf), "%d", slen * n);
	char nbuf[16]; snprintf(nbuf, sizeof(nbuf), "%d", n);
	LCall* call = make_lcall(thgc, v, csvari);
	lcall_add(thgc, call, thgc4);
	lcall_add(thgc, call, v2);
	lcall_add(thgc, call, make_lvari_c(thgc, "i32", lenbuf));
	lcall_add(thgc, call, make_lvari_c(thgc, "i32", nbuf));
	add_list(thgc, func->comps, (char*)call);
	Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
	ret->vartype = local->StrT;
	ret->vari_c = v;
	return ret;
}

// ============================================================
// StrObj::exeC
// ============================================================
Obj* exeC_StrObj(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// StrObj::exepC
// ============================================================
Obj* exepC_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// StrObj::PrimaryC
// ============================================================
Obj* PrimaryC_StrObj(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "文字列には後続が来れません");
}

// ============================================================
// BoolVal::exeC
// ============================================================
Obj* exeC_BoolVal2(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// BoolVal::exepC
// ============================================================
Obj* exepC_BoolVal2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// BoolVal::PrimaryC
// ============================================================
Obj* PrimaryC_BoolVal2(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "bool蛟､縺ｫ縺ｯ蠕檎ｶ壹′縺薙ｌ縺ｾ縺帙ｓ");
}

// ============================================================
// VoiVal::exepC
// ============================================================
Obj* exepC_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return make_cobj(thgc, CType::_CObj, LetterType::OT_Error, NULL);
}

// ============================================================
// VoiVal::PrimaryC
// ============================================================
Obj* PrimaryC_VoiVal(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "void蛟､縺ｫ縺ｯ蠕檎ｶ壹′縺薙ｌ縺ｾ縺帙ｓ");
}

// ============================================================
// Null::exeC
// ============================================================
Obj* exeC_Null(ThreadGC* thgc, Obj* self, Master* local) {
	return self;
}

// ============================================================
// Null::exepC
// ============================================================
Obj* exepC_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	return self;
}

// ============================================================
// Null::PrimaryC
// ============================================================
Obj* PrimaryC_Null(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "null縺ｫ縺ｯ蠕檎ｶ壹′譚･縺ｾ縺帙ｓ");
}

// ============================================================
// ArrayConstructor::PrimaryC
// ============================================================
Obj* PrimaryC_ArrayConstructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	Obj* cls = self->vartype;
	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		if (val->objtype == LetterType::OT_Wait || val->objtype == LetterType::OT_Error || val->objtype == LetterType::OT_NG) return val;
		Block* blk = (Block*)val;
	if (blk->rets->size != 1) return ErrorC(thgc, LetterType::OT_Error, val2->letter, "配列型のコンストラクタの引数は1です");
		(*n)++;
		Obj* r0 = *(Obj**)get_list(blk->rets, 0);
		if (r0->objtype == LetterType::OT_Number) {
			Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
			ret->vartype = cls;
			return ret;
		}
		else return ErrorC(thgc, LetterType::OT_Error, blk->letter, "配列型のコンストラクタの引数は1です");
	}
	Obj* val3 = vt_GetterA(thgc, val2, local);
	if (val3->objtype == LetterType::OT_Wait || val3->objtype == LetterType::OT_Error || val3->objtype == LetterType::OT_NG) return val3;
	if (val3->objtype == LetterType::OT_Number) {
		Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
		ret->vartype = cls;
		return ret;
	}
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "コンストラクタの引数が適切ではありません");
}

// ============================================================
// Constructor::PrimaryC
// ============================================================
Obj* PrimaryC_Constructor(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	LFunc* func = llvm_get_func(local->llvm);
	Obj* cls = self->vartype;

	if (val2->objtype == LetterType::OT_Bracket) {
		Obj* val = vt_GetterC(thgc, vt_exeC(thgc, val2, local), local);
		Block* blk = (Block*)val;
		(*n)++;
		// add local comps to func
		for (int i = 0; i < local->compsC->size; i++) {
			LComp* comp = *(LComp**)get_list(local->compsC, i);
			add_list(thgc, func->comps, (char*)comp);
		}
		local->compsC = create_list(thgc, sizeof(LComp*), CType::_List);

		LVari* vari;
		if (cls == local->Int) vari = make_vn(thgc, "i32");
		else if (cls == local->Bool) vari = make_vn(thgc, "i2");
		else vari = make_vn(thgc, (char*)cls->model->data);

		func->vari->type = createString(thgc, (char*)"%FuncType**", 11, 1);
		LVari* fv = make_vn(thgc, "%FuncType*");
		add_list(thgc, func->comps, (char*)make_lload(thgc, fv, func->vari, false));

		LVari* go_v = make_vn(thgc, (char*)fmt_str(thgc, "%s*", cls->model->data)->data);
		LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");

		String* drawcall_str = fmt_str(thgc, "@%sDraw%d", cls->letter->text->data, cls->identity);
		LCall* go_call = make_lcall(thgc, go_v, make_lvari_c(thgc, (char*)fmt_str(thgc, "%s*", cls->model->data)->data, (char*)drawcall_str->data));
		lcall_add(thgc, go_call, rn4);
		lcall_add(thgc, go_call, fv);

		// Add constructor arguments from blk->rets
		Block* draw0 = (Block*)*(Obj**)get_list(cls->children, 0);
		// iterate through draw0->vmapA values matching with blk->rets
		for (int i = 0; i < blk->rets->size; i++) {
			Obj* ret_i = *(Obj**)get_list(blk->rets, i);
			if (ret_i->vari_c != NULL) {
				lcall_add(thgc, go_call, ret_i->vari_c);
			}
		}
		add_list(thgc, func->comps, (char*)go_call);

		String* call_str = fmt_str(thgc, "@%s%d", cls->letter->text->data, cls->identity);
		LCall* init_call = make_lcall(thgc, NULL, make_lvari_c(thgc, (char*)vari->type->data, (char*)call_str->data));
		lcall_add(thgc, init_call, rn4);
		lcall_add(thgc, init_call, go_v);
		add_list(thgc, func->comps, (char*)init_call);

		Obj* ret = make_cobj(thgc, CType::_CObj, LetterType::OT_Value, self->letter);
		ret->vartype = cls;
		ret->vari_c = go_v;
		return ret;
	}
	return ErrorC(thgc, LetterType::OT_NG, val2->letter, "コンストラクタの引数が適切ではありません");
}
