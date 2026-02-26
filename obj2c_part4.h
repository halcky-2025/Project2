// obj2c_part4.h - Obj2C.cs lines 8000-11000
// Model::PrimaryC continued (Delete, FirstAwait, SelectAwait, Store, Delete .await/.Bracket paths)
// All code is inside the ongoing PrimaryC_Model function body from obj2c_part3.

// ============================================================
// Helper: emit SQL string creation sequence
// ============================================================
static inline LVari* emit_create_string_sql(ThreadGC* thgc, Master* local, const char* sql, int sql_len) {
	LVari* csvari = make_vn(thgc, "%CreateStringType");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, csvari, make_lvari_c(thgc, "%CreateStringType*", "@CreateString"), false));
	LVari* v2 = make_vn(thgc, "i8*");
	char sname[64]; snprintf(sname, sizeof(sname), "@s%d", llvm_n++);
	LStrV* sv = make_lstrv(thgc, sname, sql, sql_len);
	add_list(thgc, local->llvm->strs, (char*)sv);
	char gtype[64]; snprintf(gtype, sizeof(gtype), "[%d x i8]", sql_len + 1);
	LGete* ge = make_lgete_idx2(thgc, gtype, v2, (LVari*)sv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0"));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ge);
	LVari* v = make_vn(thgc, "%StringType*");
	LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	char slen[32]; snprintf(slen, sizeof(slen), "%d", sql_len);
	LCall* call = make_lcall(thgc, v, csvari);
	lcall_add(thgc, call, thgc4);
	lcall_add(thgc, call, v2);
	lcall_add(thgc, call, make_lvari_c(thgc, "i32", slen));
	lcall_add(thgc, call, make_lvari_c(thgc, "i32", "1"));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)call);
	return v;
}

// ============================================================
// Helper: emit SqlParam for Int type
// ============================================================
static inline void emit_sql_param_int(ThreadGC* thgc, Master* local, LVari* ps, int varis_count, int key, LVari* value_vari) {
	char arrtype[64]; snprintf(arrtype, sizeof(arrtype), "[%d x %%SqlParamType]", varis_count);
	char keystr[16]; snprintf(keystr, sizeof(keystr), "%d", key);
	LVari* pptr = make_vn(thgc, "%SqlParamType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, arrtype, pptr, ps, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", keystr)));
	LVari* kptr = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%SqlParamType", kptr, pptr, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, kptr, make_lvari_c(thgc, "i32", "1")));
	LVari* vptr = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%SqlParamType", vptr, pptr, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, vptr, value_vari));
}

// ============================================================
// Helper: emit SqlParam for Str type (with UTF8 conversion)
// ============================================================
static inline void emit_sql_param_str(ThreadGC* thgc, Master* local, LVari* ps, int varis_count, int key, LVari* value_vari) {
	char arrtype[64]; snprintf(arrtype, sizeof(arrtype), "[%d x %%SqlParamType]", varis_count);
	char keystr[16]; snprintf(keystr, sizeof(keystr), "%d", key);
	LVari* pptr = make_vn(thgc, "%SqlParamType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, arrtype, pptr, ps, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", keystr)));
	LVari* kptr = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%SqlParamType", kptr, pptr, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, kptr, make_lvari_c(thgc, "i32", "4")));
	LVari* vptr = make_vn(thgc, "%TextType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%SqlParamType", vptr, pptr, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	// StringToUTF8
	LVari* utf8f = make_lvari_c(thgc, "%StringUTF8", "%utf8f");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, utf8f, make_lvari_c(thgc, "%StringUTF8*", "@StringUTF8"), false));
	LVari* uc = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lalloca(thgc, uc));
	LVari* utf8v = make_vn(thgc, "i8*");
	LCall* ucall = make_lcall(thgc, utf8v, utf8f);
	lcall_add(thgc, ucall, value_vari);
	lcall_add(thgc, ucall, uc);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ucall);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, vptr, utf8v));
	LVari* lptr = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%TextType", lptr, vptr, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	LVari* uv = make_vn(thgc, "i32");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, uv, uc, false));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, lptr, uv));
}

// ============================================================
// Helper: emit async ExecSql + coroutine push + await return
// ============================================================
static inline void emit_async_execsql_delete(ThreadGC* thgc, Master* local, LVari* v, LVari* ps, int pcount) {
	LVari* tx = make_vn(thgc, "i8*");
	LVari* txp = make_vn(thgc, "i8**");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", txp, make_lvari_c(thgc, "%RootNodeType*", "%rn"), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, tx, txp, false));
	LVari* esql = make_lvari_c(thgc, "%ExecSqlType", "%esql");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, esql, make_lvari_c(thgc, "%ExecSqlType*", "@ExecSql"), false));
	LVari* coroptr = make_lvari_c(thgc, "%CoroFrameType**", "%coroptr");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lalloca(thgc, coroptr));
	char pcstr[16]; snprintf(pcstr, sizeof(pcstr), "%d", pcount);
	LCall* ecall = make_lcall(thgc, NULL, esql);
	lcall_add(thgc, ecall, coroptr);
	lcall_add(thgc, ecall, tx);
	lcall_add(thgc, ecall, v);
	lcall_add(thgc, ecall, ps);
	lcall_add(thgc, ecall, make_lvari_c(thgc, "i32", pcstr));
	lcall_add(thgc, ecall, make_lvari_c(thgc, "ptr", "null"));
	lcall_add(thgc, ecall, make_lvari_c(thgc, "ptr", "null"));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ecall);
	LVari* hv = make_lvari_c(thgc, "%CoroFrameType*", "%corov");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, hv, coroptr, false));

	LVari* pushqueue1 = make_vn(thgc, "%PushQueueType");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, pushqueue1, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LVari* thgcv = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* qv = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qv, (LVari*)get_list_val(llvm_get_func(local->llvm)->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	LVari* qv2 = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, qv2, qv, false));
	LVari* queueptr2 = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, queueptr2, qv2));
	LVari* stateptr = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", stateptr, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, stateptr, make_lvari_c(thgc, "i32", "0")));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue1);
	lcall_add(thgc, pqcall, qv2);
	lcall_add(thgc, pqcall, hv);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)pqcall);
	LVari* statevalptr2 = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", statevalptr2, (LVari*)get_list_val(llvm_get_func(local->llvm)->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	char nstr[16]; snprintf(nstr, sizeof(nstr), "%d", llvm_n++);
	LVari* nvari = make_lvari_c(thgc, "i32", nstr);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, statevalptr2, nvari));

	LVari* parent2 = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", parent2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "6")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, parent2, (LVari*)get_list_val(llvm_get_func(local->llvm)->draws, 0)));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
	char labname[32]; snprintf(labname, sizeof(labname), "aw%d", llvm_n++);
	LLab* lab6 = make_llab(thgc, labname);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)lab6);
}

// ============================================================
// Helper: emit coroutine function setup (BeginTransaction path)
// ============================================================
static inline LFunc* emit_coro_func_setup(ThreadGC* thgc, Master* local, const char* prefix) {
	LVari* obj = make_lvari_c(thgc, "%CoroFrameType*", "%frame");
	char fname[64]; snprintf(fname, sizeof(fname), "@%s%d", prefix, llvm_n++);
	LVari* fy = make_lvari_c(thgc, "void", fname);
	LFunc* func = make_lfunc(thgc, local->llvm, fy, obj);
	add_list(thgc, local->llvm->comps, (char*)func);
	func->async_flag = true;

	LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* rngv = make_lvari_c(thgc, "%RootNodeType**", "%rngv");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", rngv, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "3")));
	LVari* rn4 = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	add_list(thgc, func->comps, (char*)make_lload(thgc, rn4, rngv, false));
	add_list(thgc, func->comps, (char*)make_lload(thgc, thgc4, make_lvari_c(thgc, "%ThreadGCType**", "@thgcp"), false));

	LVari* geterv = make_lvari_c(thgc, "i32*", "%state");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", geterv, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	LVari* statev = make_vn(thgc, "i32");
	add_list(thgc, func->comps, (char*)make_lload(thgc, statev, geterv, false));
	return func;
}

// ============================================================
// Helper: emit BeginTransaction block in coroutine function
// ============================================================
static inline void emit_begin_transaction_block(ThreadGC* thgc, LFunc* func, List* list) {
	LVari* obj = (LVari*)get_list_val(func->draws, 0);
	char labname[32]; snprintf(labname, sizeof(labname), "coro_tx");
	LLab* lab10 = make_llab(thgc, labname);

	add_list(thgc, list, (char*)make_lifvalue(thgc, lab10, make_lvari_c(thgc, "i32", "-2")));
	add_list(thgc, func->comps, (char*)lab10);
	LVari* pushqueue1 = make_vn(thgc, "%PushQueueType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, pushqueue1, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LVari* qvp = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qvp, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	LVari* qv1 = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, qv1, qvp, false));
	LVari* queueptr2 = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr2, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, queueptr2, qv1));
	LVari* hvn = make_vn(thgc, "%CoroFrameType*");
	LVari* bt = make_lvari_c(thgc, "%BeginTransactionType", "%bt");
	LVari* alv = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lalloca(thgc, alv));
	add_list(thgc, func->comps, (char*)make_lload(thgc, bt, make_lvari_c(thgc, "%BeginTransactionType*", "@BeginTransaction"), false));
	LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LCall* btcall = make_lcall(thgc, NULL, bt);
	lcall_add(thgc, btcall, alv);
	lcall_add(thgc, btcall, thgc4);
	lcall_add(thgc, btcall, make_lvari_c(thgc, "ptr", "@sqlp"));
	lcall_add(thgc, btcall, qv1);
	add_list(thgc, func->comps, (char*)btcall);
	add_list(thgc, func->comps, (char*)make_lload(thgc, hvn, alv, false));
	LVari* parent2 = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", parent2, hvn, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "6")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, parent2, (LVari*)get_list_val(func->draws, 0)));
	LVari* qp = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qp, hvn, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, qp, qv1));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue1);
	lcall_add(thgc, pqcall, qv1);
	lcall_add(thgc, pqcall, hvn);
	add_list(thgc, func->comps, (char*)pqcall);
}

// ============================================================
// Helper: emit coro_start label + state store + ret
// ============================================================
static inline LLab* emit_coro_start_block(ThreadGC* thgc, LFunc* func, List* list) {
	char labname[32]; snprintf(labname, sizeof(labname), "coro_start%d", llvm_n++);
	LLab* lab10 = make_llab(thgc, labname);
	add_list(thgc, list, (char*)make_lifvalue(thgc, lab10, make_lvari_c(thgc, "i32", "0")));
	LVari* statevalptr3 = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", statevalptr3, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, statevalptr3, make_lvari_c(thgc, "i32", "0")));
	LVari* sfinp = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", sfinp, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "10")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, sfinp, make_lvari_c(thgc, "i32", "-3")));
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
	add_list(thgc, func->comps, (char*)lab10);
	return lab10;
}

// ============================================================
// Helper: emit ExecSql in coroutine function
// ============================================================
static inline void emit_coro_execsql(ThreadGC* thgc, LFunc* func, LVari* obj, LVari* rn4, LVari* v, LVari* ps, int pcount, const char* dbselect, LVari* sqp) {
	LVari* txp = make_vn(thgc, "i8**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", txp, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "8")));
	LVari* tx = make_vn(thgc, "i8*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, tx, txp, false));
	LVari* txps = make_vn(thgc, "i8**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", txps, rn4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, txps, tx));
	LVari* esql = make_lvari_c(thgc, "%ExecSqlType", "%esql");
	add_list(thgc, func->comps, (char*)make_lload(thgc, esql, make_lvari_c(thgc, "%ExecSqlType*", "@ExecSql"), false));
	LVari* coroptr = make_lvari_c(thgc, "%CoroFrameType**", "%coroptr");
	add_list(thgc, func->comps, (char*)make_lalloca(thgc, coroptr));
	// Load triptr
	LVari* op = make_vn(thgc, "%Triptr**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", op, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "5")));
	LVari* obv = make_vn(thgc, "%Triptr*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, obv, op, false));
	LVari* vp = make_vn(thgc, "%StringType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%Triptr", vp, obv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	LVari* v2 = make_vn(thgc, "%StringType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, v2, vp, false));
	LVari* pps1 = make_vn(thgc, "%SqlParamType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%Triptr", pps1, obv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	LVari* ps2 = make_vn(thgc, "ptr");
	add_list(thgc, func->comps, (char*)make_lload(thgc, ps2, pps1, false));
	char pcstr[16]; snprintf(pcstr, sizeof(pcstr), "%d", pcount);
	if (dbselect != NULL) {
		LVari* sqpp = make_vn(thgc, "ptr");
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%Triptr", sqpp, obv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
		LVari* sqpv = make_vn(thgc, "ptr");
		add_list(thgc, func->comps, (char*)make_lload(thgc, sqpv, sqpp, false));
		LCall* ecall = make_lcall(thgc, NULL, esql);
		lcall_add(thgc, ecall, coroptr); lcall_add(thgc, ecall, tx); lcall_add(thgc, ecall, v2);
		lcall_add(thgc, ecall, ps); lcall_add(thgc, ecall, make_lvari_c(thgc, "i32", pcstr));
		lcall_add(thgc, ecall, make_lvari_c(thgc, "ptr", dbselect));
		lcall_add(thgc, ecall, sqpv);
		add_list(thgc, func->comps, (char*)ecall);
	} else {
		LCall* ecall = make_lcall(thgc, NULL, esql);
		lcall_add(thgc, ecall, coroptr); lcall_add(thgc, ecall, tx); lcall_add(thgc, ecall, v2);
		lcall_add(thgc, ecall, ps); lcall_add(thgc, ecall, make_lvari_c(thgc, "i32", pcstr));
		lcall_add(thgc, ecall, make_lvari_c(thgc, "ptr", "null"));
		lcall_add(thgc, ecall, make_lvari_c(thgc, "ptr", "null"));
		add_list(thgc, func->comps, (char*)ecall);
	}
}

// ============================================================
// Helper: emit coroutine push + state update in coro func
// ============================================================
static inline void emit_coro_push_and_state(ThreadGC* thgc, LFunc* func, LVari* hv, List* list) {
	LVari* pushqueue1 = make_vn(thgc, "%PushQueueType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, pushqueue1, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LVari* qv = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qv, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	LVari* qv2 = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, qv2, qv, false));
	LVari* queueptr2 = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, queueptr2, qv2));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue1);
	lcall_add(thgc, pqcall, qv2);
	lcall_add(thgc, pqcall, hv);
	add_list(thgc, func->comps, (char*)pqcall);
	LVari* statevalptr2 = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", statevalptr2, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	LVari* sfinp2 = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", sfinp2, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "10")));
	LVari* sfinv2 = make_vn(thgc, "i32");
	add_list(thgc, func->comps, (char*)make_lload(thgc, sfinv2, sfinp2, false));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, statevalptr2, sfinv2));

	LVari* parent2 = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", parent2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "6")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, parent2, (LVari*)get_list_val(func->draws, 0)));
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
}

// ============================================================
// Helper: emit TxCommit block in coro func
// ============================================================
static inline void emit_coro_txcommit(ThreadGC* thgc, LFunc* func, LVari* obj, LVari* rn4, List* list) {
	char labname[32]; snprintf(labname, sizeof(labname), "coro_txfin");
	LLab* lab10 = make_llab(thgc, labname);
	add_list(thgc, list, (char*)make_lifvalue(thgc, lab10, make_lvari_c(thgc, "i32", "-3")));
	add_list(thgc, func->comps, (char*)lab10);

	LVari* rp = make_vn(thgc, "i8**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", rp, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "8")));
	LVari* rv = make_vn(thgc, "i8*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, rv, rp, false));
	LVari* rp2 = make_vn(thgc, "i8**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", rp2, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "9")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, rp2, rv));
	LVari* txp = make_vn(thgc, "i8**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", txp, rn4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	LVari* tx = make_vn(thgc, "i8*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, tx, txp, false));
	LVari* txcf = make_vn(thgc, "%TxFinishType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, txcf, make_lvari_c(thgc, "%TxFinishType*", "@TxCommit"), false));
	LVari* hvp = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lalloca(thgc, hvp));
	LCall* txcall = make_lcall(thgc, NULL, txcf);
	lcall_add(thgc, txcall, hvp);
	lcall_add(thgc, txcall, tx);
	add_list(thgc, func->comps, (char*)txcall);
	LVari* hv = make_vn(thgc, "%CoroFrameType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, hv, hvp, false));
	LVari* pushqueue1 = make_vn(thgc, "%PushQueueType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, pushqueue1, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LVari* qv = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qv, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	LVari* qv2 = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, qv2, qv, false));
	LVari* queueptr2 = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, queueptr2, qv2));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue1);
	lcall_add(thgc, pqcall, qv2);
	lcall_add(thgc, pqcall, hv);
	add_list(thgc, func->comps, (char*)pqcall);
	LVari* statevalptr2 = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", statevalptr2, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, statevalptr2, make_lvari_c(thgc, "i32", "-1")));

	LVari* parent2 = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", parent2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "6")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, parent2, (LVari*)get_list_val(func->draws, 0)));
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
}

// ============================================================
// Helper: emit coro_end label + state=-1 + ret
// ============================================================
static inline void emit_coro_end(ThreadGC* thgc, LFunc* func, LVari* obj, List* list) {
	LLab* lab10 = make_llab(thgc, "coro_end");
	add_list(thgc, list, (char*)make_lifvalue(thgc, lab10, make_lvari_c(thgc, "i32", "-1")));
	add_list(thgc, func->comps, (char*)lab10);
	LVari* stateval = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", stateval, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, stateval, make_lvari_c(thgc, "i32", "-1")));
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
}

// ============================================================
// Helper: emit MakeFrame + WaitHandle (sync path)
// ============================================================
static inline LVari* emit_makeframe_waithandle(ThreadGC* thgc, Master* local, LVari* rn4, LFunc* func, LVari* ov) {
	LVari* hv = make_vn(thgc, "%CoroFrameType*");
	LVari* root = make_lvari_c(thgc, "%RootNodeType*", "%rn");
	LVari* mkf = make_vn(thgc, "%MakeFrameType");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, mkf, make_lvari_c(thgc, "%MakeFrameType*", "@MakeFrame"), false));
	LCall* mkcall = make_lcall(thgc, make_lvari_s(thgc, "ptr", hv->name), mkf);
	lcall_add(thgc, mkcall, root);
	lcall_add(thgc, mkcall, make_lvari_s(thgc, "ptr", func->y->name));
	lcall_add(thgc, mkcall, ov);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)mkcall);

	// Check tx == null, set state = -2 if so
	LVari* txp = make_vn(thgc, "ptr");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", txp, rn4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	LVari* tx = make_vn(thgc, "ptr");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, tx, txp, false));
	LVari* eq = make_vn(thgc, "i1");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lcmp(thgc, LCMP_Eq, eq, tx, make_lvari_c(thgc, "ptr", "null")));
	char l1name[32]; snprintf(l1name, sizeof(l1name), "eq%d", llvm_n++);
	char l2name[32]; snprintf(l2name, sizeof(l2name), "th%d", llvm_n++);
	LLab* l1 = make_llab(thgc, l1name);
	LLab* l2 = make_llab(thgc, l2name);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lbr(thgc, eq, l1, l2));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)l1);
	LVari* stateptr = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", stateptr, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, stateptr, make_lvari_c(thgc, "i32", "-2")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lbr(thgc, NULL, NULL, l2));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)l2);

	LVari* wh = make_vn(thgc, "%WaitHandleType");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, wh, make_lvari_c(thgc, "%WaitHandleType*", "@WaitHandle"), false));
	LVari* thgcv = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* ret = make_vn(thgc, "%RowSet*");
	LCall* whcall = make_lcall(thgc, ret, wh);
	lcall_add(thgc, whcall, thgcv);
	lcall_add(thgc, whcall, hv);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)whcall);
	return ret;
}

// ============================================================
// Helper: emit Triptr allocation + store sql string + store ps
// ============================================================
static inline LVari* emit_triptr_alloc(ThreadGC* thgc, Master* local, LVari* v, LVari* ps) {
	LVari* ov = make_vn(thgc, "ptr");
	LCall* mcall = make_lcall(thgc, ov, make_lvari_c(thgc, "ptr", "@malloc"));
	lcall_add(thgc, mcall, make_lvari_c(thgc, "i64", "24"));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)mcall);
	LVari* sp = make_vn(thgc, "%StringType**");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%Triptr", sp, ov, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, sp, v));
	LVari* pps = make_vn(thgc, "%SqlParamType**");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%Triptr", pps, ov, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, pps, ps));
	return ov;
}

// ============================================================
// Helper: emit coroutine queue push in local->llvm->func
// ============================================================
static inline void emit_local_coro_push(ThreadGC* thgc, Master* local, LVari* hv, LVari* rn4) {
	LVari* pushqueue1 = make_vn(thgc, "%PushQueueType");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, pushqueue1, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LVari* thgcv = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* qv = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%ThreadGCType", qv, thgcv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	LVari* qv2 = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, qv2, qv, false));
	LVari* queueptr2 = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, queueptr2, qv2));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue1);
	lcall_add(thgc, pqcall, qv2);
	lcall_add(thgc, pqcall, hv);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)pqcall);
}

// ============================================================
// Helper: emit AddString2 (string concat)
// ============================================================
static inline LVari* emit_add_string2(ThreadGC* thgc, Master* local, LVari* str1, LVari* str2) {
	LVari* thgc4 = make_lvari_c(thgc, "%ThreadGCType*", "%thgc");
	LVari* asv = make_vn(thgc, "%AddStringType2");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, asv, make_lvari_c(thgc, "%AddStringType2*", "@AddString2"), false));
	LVari* strv = make_vn(thgc, "%StringType*");
	LCall* ascall = make_lcall(thgc, strv, asv);
	lcall_add(thgc, ascall, thgc4);
	lcall_add(thgc, ascall, str1);
	lcall_add(thgc, ascall, str2);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)ascall);
	return strv;
}

// ============================================================
// Helper: emit malloc for sql params
// ============================================================
static inline LVari* emit_malloc_params(ThreadGC* thgc, Master* local, int count) {
	LVari* ps = make_lvari_c(thgc, "ptr", "%ps");
	char sizestr[32]; snprintf(sizestr, sizeof(sizestr), "%d", 24 * count);
	LCall* mcall = make_lcall(thgc, ps, make_lvari_c(thgc, "ptr", "@malloc"));
	lcall_add(thgc, mcall, make_lvari_c(thgc, "i32", sizestr));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)mcall);
	return ps;
}

// ============================================================
// Helper: build vstr from model blocks (for SELECT column list)
// ============================================================
static inline String* build_model_vstr(ThreadGC* thgc, ModelObj* modelobj, Master* local) {
	// Start with "id"
	SB* sb = sb_create(thgc);
	sb_adds(thgc, sb, "id");
	Block* blk1 = (Block*)*(Obj**)get_list(((Obj*)modelobj->draw)->children, 0);
	// iterate blk1->vmapA
	if (blk1->vmapA != NULL) {
		for (int i = 0; i < blk1->vmapA->kvs->size; i++) {
			KV* kv_entry = *(KV**)get_list(blk1->vmapA->kvs, i);
			String* key = kv_entry->key;
			Obj* val = (Obj*)kv_entry->value;
			if (val->objtype == LetterType::OT_Variable) {
				Variable* variable = (Variable*)val;
				Obj* cls = variable->cls;
				if (cls->objtype == LetterType::OT_Var) cls = ((Var*)cls)->cls;
				if (cls->objtype == LetterType::OT_ModelObj) {
					sb_adds(thgc, sb, ", ");
					sb_add_str(thgc, sb, key);
				}
			}
		}
	}
	Block* blk2 = (Block*)*(Obj**)get_list(((Obj*)modelobj->draw)->children, 1);
	if (blk2->vmapA != NULL) {
		for (int i = 0; i < blk2->vmapA->kvs->size; i++) {
			KV* kv_entry = *(KV**)get_list(blk2->vmapA->kvs, i);
			String* key = kv_entry->key;
			if (str_matchA(key, "id", 2)) continue;
			Obj* val = (Obj*)kv_entry->value;
			if (val->objtype == LetterType::OT_Variable) {
				Variable* variable = (Variable*)val;
				Obj* cls = variable->cls;
				if (cls->objtype == LetterType::OT_Var) cls = ((Var*)cls)->cls;
				if (cls->objtype == LetterType::OT_ModelObj) {
					sb_adds(thgc, sb, ", ");
					sb_add_str(thgc, sb, key);
				}
			}
		}
	}
	sb->data[sb->size] = '\0';
	return createString(thgc, sb->data, sb->size, 1);
}

// ============================================================
// Helper: process SqlString varis for sql params
// ============================================================
static inline void emit_sqlstring_params(ThreadGC* thgc, Master* local, SqlString* sqs, LVari* ps) {
	if (sqs->varis == NULL) return;
	for (int ki = 0; ki < sqs->varis->keys->size; ki++) {
		int key = *(int*)get_list(sqs->varis->keys, ki);
		Obj* kval = *(Obj**)get_list(sqs->varis->vals, ki);
		if (kval->objtype == LetterType::OT_Value) {
			Value* value = (Value*)kval;
			Obj* cls = value->cls;
			if (cls->objtype == LetterType::OT_Var) cls = value->cls = ((Var*)cls)->cls;
			if (cls->objtype == LetterType::OT_ModelObj) {
				if (cls == (Obj*)local->Int) {
					emit_sql_param_int(thgc, local, ps, sqs->varis->keys->size, key, value->vari);
				} else if (cls == (Obj*)local->StrT) {
					emit_sql_param_str(thgc, local, ps, sqs->varis->keys->size, key, value->vari);
				}
			}
		}
	}
}

// ============================================================
// Helper: emit Store type value handling in coroutine
// ============================================================
static inline LVari* emit_store_value_type(ThreadGC* thgc, Master* local, LFunc* func,
	Value* value, LVari* objp, LVari* objv,
	const char* typename_str, LTypeDec* typedec, int i, int n3, int* count_out)
{
	LVari* vx = NULL;
	if (value->cls == (Obj*)local->Int) {
		add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "i32", i));
		char n3str[16]; snprintf(n3str, sizeof(n3str), "%d", n3);
		LVari* gv = make_vn(thgc, "i32*");
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, typename_str, gv, objp, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, gv, value->vari));
		LVari* vxp = make_vn(thgc, "i32*");
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, typename_str, vxp, objv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		vx = make_vn(thgc, "i32");
		add_list(thgc, func->comps, (char*)make_lload(thgc, vx, vxp, false));
		*count_out += 8;
	} else if (value->cls == (Obj*)local->Short) {
		add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "i16", i));
		char n3str[16]; snprintf(n3str, sizeof(n3str), "%d", n3);
		LVari* gv = make_vn(thgc, "i16*");
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, typename_str, gv, objp, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, gv, value->vari));
		LVari* vxp = make_vn(thgc, "i16*");
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, typename_str, vxp, objv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		vx = make_vn(thgc, "i16");
		add_list(thgc, func->comps, (char*)make_lload(thgc, vx, vxp, false));
		*count_out += 8;
	} else if (value->cls == (Obj*)local->Bool) {
		add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "i1", i));
		char n3str[16]; snprintf(n3str, sizeof(n3str), "%d", n3);
		LVari* gv = make_vn(thgc, "i1*");
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, typename_str, gv, objp, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, gv, value->vari));
		LVari* vxp = make_vn(thgc, "i1*");
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, typename_str, vxp, objv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		vx = make_vn(thgc, "i1");
		add_list(thgc, func->comps, (char*)make_lload(thgc, vx, vxp, false));
		*count_out += 8;
	} else if (value->cls == (Obj*)local->StrT) {
		add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "%StringType*", i));
		char n3str[16]; snprintf(n3str, sizeof(n3str), "%d", n3);
		LVari* gv = make_vn(thgc, "%StringType*");
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, typename_str, gv, objp, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, gv, value->vari));
		LVari* vxp = make_vn(thgc, "%StringType**");
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, typename_str, vxp, objv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		LVari* vx0 = make_vn(thgc, "%StringType*");
		add_list(thgc, func->comps, (char*)make_lload(thgc, vx0, vxp, false));
		LVari* utf8 = make_vn(thgc, "%StringUTF8Type*");
		add_list(thgc, func->comps, (char*)make_lload(thgc, utf8, make_lvari_c(thgc, "%StringUTF8Type*", "@StringUTF8"), false));
		LVari* len = make_vn(thgc, "i32*");
		add_list(thgc, func->comps, (char*)make_lalloca(thgc, len));
		vx = make_vn(thgc, "i8*");
		LCall* ucall = make_lcall(thgc, vx, utf8);
		lcall_add(thgc, ucall, vx0);
		lcall_add(thgc, ucall, len);
		add_list(thgc, func->comps, (char*)ucall);
		*count_out += 8;
	} else {
		add_list(thgc, typedec->comps, (char*)make_ltypeval(thgc, "%GCModel*", i));
		char n3str[16]; snprintf(n3str, sizeof(n3str), "%d", n3);
		LVari* gv = make_vn(thgc, "%GCModel**");
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, typename_str, gv, objp, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lstore(thgc, gv, value->vari));
		LVari* vxp = make_vn(thgc, "%GCModel**");
		add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, typename_str, vxp, objv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", n3str)));
		vx = make_vn(thgc, "%GCModel*");
		add_list(thgc, func->comps, (char*)make_lload(thgc, vx, vxp, false));
		*count_out += 8;
	}
	return vx;
}

// ============================================================
// Helper: emit DbStore call in coro func
// ============================================================
static inline void emit_dbstore_push(ThreadGC* thgc, LFunc* func, ModelObj* model, LVari* tx, LVari* vx) {
	LVari* hv = make_vn(thgc, "%CoroFrameType*");
	char dbstore[64]; snprintf(dbstore, sizeof(dbstore), "@DbStore%d", model->identity);
	LCall* dscall = make_lcall(thgc, make_lvari_s(thgc, "ptr", hv->name), make_lvari_c(thgc, "ptr", dbstore));
	lcall_add(thgc, dscall, tx);
	lcall_add(thgc, dscall, vx);
	add_list(thgc, func->comps, (char*)dscall);
	LVari* pushqueue1 = make_vn(thgc, "%PushQueueType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, pushqueue1, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LVari* qv = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qv, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	LVari* qv2 = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, qv2, qv, false));
	LVari* queueptr2 = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, queueptr2, qv2));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue1);
	lcall_add(thgc, pqcall, qv2);
	lcall_add(thgc, pqcall, hv);
	add_list(thgc, func->comps, (char*)pqcall);

	LVari* parent2 = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", parent2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "6")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, parent2, (LVari*)get_list_val(func->draws, 0)));
}

// ============================================================
// Helper: emit DbDelete call in coro func
// ============================================================
static inline void emit_dbdelete_push(ThreadGC* thgc, LFunc* func, ModelObj* model, LVari* tx, LVari* vx) {
	LVari* hv = make_vn(thgc, "%CoroFrameType*");
	char dbdelete[64]; snprintf(dbdelete, sizeof(dbdelete), "@DbDelete%d", model->identity);
	LCall* ddcall = make_lcall(thgc, make_lvari_s(thgc, "ptr", hv->name), make_lvari_c(thgc, "ptr", dbdelete));
	lcall_add(thgc, ddcall, tx);
	lcall_add(thgc, ddcall, vx);
	add_list(thgc, func->comps, (char*)ddcall);
	LVari* pushqueue1 = make_vn(thgc, "%PushQueueType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, pushqueue1, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LVari* qv = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qv, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	LVari* qv2 = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, qv2, qv, false));
	LVari* queueptr2 = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, queueptr2, qv2));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue1);
	lcall_add(thgc, pqcall, qv2);
	lcall_add(thgc, pqcall, hv);
	add_list(thgc, func->comps, (char*)pqcall);

	LVari* parent2 = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", parent2, hv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "6")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, parent2, (LVari*)get_list_val(func->draws, 0)));
}

// ============================================================
// Helper: emit coro_fin block (async Store/Delete completion)
// ============================================================
static inline void emit_coro_fin_async(ThreadGC* thgc, LFunc* func, LVari* obj, List* list, int identity) {
	LVari* statevalptr2 = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", statevalptr2, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, statevalptr2, make_lvari_c(thgc, "i32", "-1")));
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
	LLab* lab_fin = make_llab(thgc, "coro_fin");
	add_list(thgc, list, (char*)make_lifvalue(thgc, lab_fin, make_lvari_c(thgc, "i32", "-1")));
	add_list(thgc, func->comps, (char*)lab_fin);
	LVari* stateval = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", stateval, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, stateval, make_lvari_c(thgc, "i32", "-1")));
	// Check parent != null
	LVari* parent = make_lvari_c(thgc, "%CoroFrameType**", "%parent");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", parent, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "6")));
	LVari* handle = make_vn(thgc, "%CoroFrameType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, handle, parent, false));
	LVari* comp = make_lvari_c(thgc, "i1", "%comp");
	add_list(thgc, func->comps, (char*)make_lcmp(thgc, LCMP_Ne, comp, handle, make_lvari_c(thgc, "ptr", "null")));
	char endname[64]; snprintf(endname, sizeof(endname), "coro_end%d", identity);
	LLab* lab_end = make_llab(thgc, endname);
	LLab* lab_ret = make_llab(thgc, "coro_ret");
	add_list(thgc, func->comps, (char*)make_lbr(thgc, comp, lab_end, lab_ret));
	add_list(thgc, func->comps, (char*)lab_end);
	LVari* queueptr = make_lvari_c(thgc, "%CoroutineQueueType**", "%queueptr");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	LVari* queue = make_lvari_c(thgc, "%PushQueueType*", "%queue");
	add_list(thgc, func->comps, (char*)make_lload(thgc, queue, queueptr, false));
	LVari* pushqueue = make_lvari_c(thgc, "%PushQueueType*", "%pushqueue");
	add_list(thgc, func->comps, (char*)make_lload(thgc, pushqueue, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue);
	lcall_add(thgc, pqcall, queue);
	lcall_add(thgc, pqcall, handle);
	add_list(thgc, func->comps, (char*)pqcall);
	add_list(thgc, func->comps, (char*)make_lbr(thgc, NULL, NULL, lab_ret));
	add_list(thgc, func->comps, (char*)lab_ret);
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
}

// ============================================================
// Helper: emit coro_end + TxCommit block (sync Store/Delete)
// ============================================================
static inline void emit_coro_txcommit_sync(ThreadGC* thgc, LFunc* func, LVari* obj, LVari* rn4, List* list) {
	LVari* statevalptr2 = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", statevalptr2, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	LVari* sfinp = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", sfinp, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "10")));
	LVari* sfinv = make_vn(thgc, "i32");
	add_list(thgc, func->comps, (char*)make_lload(thgc, sfinv, sfinp, false));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, statevalptr2, sfinv));
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
	LLab* lab10 = make_llab(thgc, "coro_end");
	add_list(thgc, list, (char*)make_lifvalue(thgc, lab10, make_lvari_c(thgc, "i32", "-3")));
	add_list(thgc, func->comps, (char*)lab10);
	// TxCommit
	LVari* pushqueue1 = make_vn(thgc, "%PushQueueType");
	add_list(thgc, func->comps, (char*)make_lload(thgc, pushqueue1, make_lvari_c(thgc, "%PushQueueType*", "@PushQueue"), false));
	LVari* qvn = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qvn, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	LVari* qvn2 = make_vn(thgc, "%CoroutineQueueType*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, qvn2, qvn, false));
	LVari* queueptr2 = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", queueptr2, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, queueptr2, qvn2));
	LVari* hvn = make_vn(thgc, "%CoroFrameType*");
	LVari* bt = make_lvari_c(thgc, "%TxFinishType", "%tf");
	LVari* alv = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lalloca(thgc, alv));
	add_list(thgc, func->comps, (char*)make_lload(thgc, bt, make_lvari_c(thgc, "%TxFinishType*", "@TxCommit"), false));
	LVari* txp = make_vn(thgc, "i8**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%RootNodeType", txp, rn4, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	LVari* txv = make_vn(thgc, "i8*");
	add_list(thgc, func->comps, (char*)make_lload(thgc, txv, txp, false));
	LCall* btcall = make_lcall(thgc, NULL, bt);
	lcall_add(thgc, btcall, alv);
	lcall_add(thgc, btcall, txv);
	add_list(thgc, func->comps, (char*)btcall);
	add_list(thgc, func->comps, (char*)make_lload(thgc, hvn, alv, false));
	LVari* parent2 = make_vn(thgc, "%CoroFrameType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", parent2, hvn, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "6")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, parent2, (LVari*)get_list_val(func->draws, 0)));
	LVari* qp = make_vn(thgc, "%CoroutineQueueType**");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", qp, hvn, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "4")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, qp, qvn2));
	LCall* pqcall = make_lcall(thgc, NULL, pushqueue1);
	lcall_add(thgc, pqcall, qvn2);
	lcall_add(thgc, pqcall, hvn);
	add_list(thgc, func->comps, (char*)pqcall);
	statevalptr2 = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", statevalptr2, (LVari*)get_list_val(func->draws, 0), make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, statevalptr2, make_lvari_c(thgc, "i32", "-1")));
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
	LLab* lab_fin = make_llab(thgc, "coro_fin");
	add_list(thgc, list, (char*)make_lifvalue(thgc, lab_fin, make_lvari_c(thgc, "i32", "-1")));
	add_list(thgc, func->comps, (char*)lab_fin);
	LVari* stateval = make_vn(thgc, "i32*");
	add_list(thgc, func->comps, (char*)make_lgete_idx2(thgc, "%CoroFrameType", stateval, obj, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "7")));
	add_list(thgc, func->comps, (char*)make_lstore(thgc, stateval, make_lvari_c(thgc, "i32", "-1")));
	add_list(thgc, func->comps, (char*)make_lret(thgc, make_lvari_c(thgc, "void", NULL)));
}

// ============================================================
// Helper: emit RowSet first-row extraction (FirstAwait result)
// ============================================================
static inline LVari* emit_rowset_first_row(ThreadGC* thgc, Master* local, LVari* ret, const char* model_type) {
	LVari* rowsp = make_vn(thgc, "%HashType**");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%RowSet", rowsp, ret, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	LVari* rowsv = make_vn(thgc, "%HashType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, rowsv, rowsp, false));
	LVari* kvsp = make_vn(thgc, "%ListType**");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%HashType", kvsp, rowsv, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "0")));
	LVari* kvs = make_vn(thgc, "%ListType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, kvs, kvsp, false));
	LVari* sizep = make_vn(thgc, "i32*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%ListType", sizep, kvs, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "1")));
	LVari* size = make_vn(thgc, "i32");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, size, sizep, false));
	LVari* equal = make_vn(thgc, "i1");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lcmp(thgc, LCMP_Eq, equal, size, make_lvari_c(thgc, "i32", "0")));
	char l1name[32]; snprintf(l1name, sizeof(l1name), "coro_end%d", llvm_n++);
	char l2name[32]; snprintf(l2name, sizeof(l2name), "coro_ret%d", llvm_n++);
	LLab* lab1 = make_llab(thgc, l1name);
	LLab* lab2 = make_llab(thgc, l2name);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lbr(thgc, equal, lab1, lab2));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)lab2);

	LVari* glf = make_vn(thgc, "%GetListType");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, glf, make_lvari_c(thgc, "%GetListType*", "@GetList"), false));
	LVari* op2 = make_vn(thgc, "ptr");
	LCall* glcall = make_lcall(thgc, op2, glf);
	lcall_add(thgc, glcall, kvs);
	lcall_add(thgc, glcall, make_lvari_c(thgc, "i32", "0"));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)glcall);
	LVari* ov2 = make_vn(thgc, "%KeyValueType*");
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, ov2, op2, false));
	char mt2[64]; snprintf(mt2, sizeof(mt2), "%s**", model_type);
	LVari* op3 = make_vn(thgc, mt2);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lgete_idx2(thgc, "%KeyValueType", op3, ov2, make_lvari_c(thgc, "i32", "0"), make_lvari_c(thgc, "i32", "2")));
	char mt1[64]; snprintf(mt1, sizeof(mt1), "%s*", model_type);
	LVari* ov3 = make_vn(thgc, mt1);
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lload(thgc, ov3, op3, false));

	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)make_lbr(thgc, NULL, NULL, lab1));
	add_list(thgc, llvm_get_func(local->llvm)->comps, (char*)lab1);
	// Return ov3 from lab2 or null from the other path via phi
	return ov3;
}

// ============================================================
// end of obj2c_part4.h
// ============================================================
