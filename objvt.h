// ============================================================
// objvt.h - 仮想関数テーブル (Virtual Table) 管理
// ObjVTの初期化とディスパッチヘルパー
// ============================================================

// ============================================================
// デフォルト関数 (C#のObj基底クラスに対応)
// ============================================================
Obj* _vt_exeZ_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_exepZ_default(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* _vt_GetterZ_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_SelfZ_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_opeZ_default(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) { return self; }
Obj* _vt_PrimaryZ_default(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) { (*n)++; return self; }
Obj* _vt_exeA_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_exepA_default(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* _vt_GetterA_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_SelfA_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_opeA_default(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) { return self; }
Obj* _vt_PrimaryA_default(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) { (*n)++; return self; }
Obj* _vt_exeB_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_exepB_default(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) { return self; }
Obj* _vt_GetterB_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_SelfB_default(ThreadGC* thgc, Obj* self, Master* local) { return self; }
Obj* _vt_opeB_default(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) { return self; }
Obj* _vt_PrimaryB_default(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) { (*n)++; return self; }

// ============================================================
// デフォルトVTable
// ============================================================
ObjVT vt_Obj_default = {
	_vt_exeZ_default, _vt_exepZ_default, _vt_GetterZ_default, _vt_SelfZ_default, _vt_opeZ_default, _vt_PrimaryZ_default,
	_vt_exeA_default, _vt_exepA_default, _vt_GetterA_default, _vt_SelfA_default, _vt_opeA_default, _vt_PrimaryA_default,
	_vt_exeB_default, _vt_exepB_default, _vt_GetterB_default, _vt_SelfB_default, _vt_opeB_default, _vt_PrimaryB_default
};

// ============================================================
// 各型用VTableインスタンス
// ============================================================
ObjVT vt_Operator;
ObjVT vt_Primary;
ObjVT vt_Block;
ObjVT vt_Block_Array;
ObjVT vt_CallBlock;
ObjVT vt_Word;
ObjVT vt_Number;
ObjVT vt_FloatVal;
ObjVT vt_StrObj;
ObjVT vt_BoolVal;
ObjVT vt_VoiVal;
ObjVT vt_Variable;
ObjVT vt_Function;
ObjVT vt_GenericFunction;
ObjVT vt_Comment;
ObjVT vt_Comment2;
ObjVT vt_TagBlock;
ObjVT vt_Signal;
ObjVT vt_SignalFunction;
ObjVT vt_ServerClient;
ObjVT vt_ServerFunction;
ObjVT vt_Dolor;
ObjVT vt_Mountain;
ObjVT vt_HtmObj;
ObjVT vt_ElemType;
ObjVT vt_ElemObj;
ObjVT vt_LinearFunction;
ObjVT vt_AnimationFunction;
ObjVT vt_Connect;
ObjVT vt_ConnectStock;
ObjVT vt_Address;
ObjVT vt_Sum;
ObjVT vt_Question;
ObjVT vt_Clones;
ObjVT vt_Class;
ObjVT vt_Type;
ObjVT vt_Var;
ObjVT vt_ClassObj;
ObjVT vt_Generic;
ObjVT vt_Model;
ObjVT vt_ModelObj;
ObjVT vt_ModelVal;
ObjVT vt_Gene;
ObjVT vt_GeneObj;
ObjVT vt_GeneLabel;
ObjVT vt_If;
ObjVT vt_While;
ObjVT vt_For;
ObjVT vt_Switch;
ObjVT vt_Break;
ObjVT vt_Continue;
ObjVT vt_Return;
ObjVT vt_Goto;
ObjVT vt_Print;
ObjVT vt_SqlString;
ObjVT vt_Null;
ObjVT vt_ArrayConstructor;
ObjVT vt_Constructor;
ObjVT vt_Base;
ObjVT vt_GeneChild;
ObjVT vt_GeneMutate;
ObjVT vt_GeneNew;
ObjVT vt_GeneCross;
ObjVT vt_GeneVal;
ObjVT vt_GeneStore;
ObjVT vt_GeneSelect;
ObjVT vt_GeneSort;
ObjVT vt_GeneLabelValue;
ObjVT vt_IfBlock;
ObjVT vt_Iterator;
ObjVT vt_NewBlock;
ObjVT vt_CrossBlock;
ObjVT vt_MigrateBlock;

// ============================================================
// LetterType → VTable ルックアップテーブル (objtypeから引く)
// ============================================================
#define VT_LOOKUP_SIZE 128
ObjVT* vt_lookup[VT_LOOKUP_SIZE];

// ============================================================
// ヘルパー: vtを安全に呼ぶマクロ
// ============================================================
#define VT_CALL(self, fn, ...) \
	((self)->vt ? (self)->vt->fn(__VA_ARGS__) : _vt_##fn##_default(__VA_ARGS__))

// ============================================================
// ディスパッチ関数 (self->vtがNULLでも安全に動く)
// ============================================================
// lazy vtable assignment: vtがNULLならvt_lookupから自動割り当て
static inline void _vt_lazy_assign(Obj* self) {
	if (self->vt == NULL) {
		int idx = (int)self->objtype;
		if (idx >= 0 && idx < VT_LOOKUP_SIZE) self->vt = vt_lookup[idx];
	}
}

Obj* vt_exeA(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->exeA) return self->vt->exeA(thgc, self, local);
	return self;
}
Obj* vt_exepA(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->exepA) return self->vt->exepA(thgc, self, n, local, primary);
	return self;
}
Obj* vt_GetterA(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->GetterA) return self->vt->GetterA(thgc, self, local);
	return self;
}
Obj* vt_SelfA(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->SelfA) return self->vt->SelfA(thgc, self, local);
	return self;
}
Obj* vt_opeA(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->opeA) return self->vt->opeA(thgc, self, key, local, val2);
	return self;
}
Obj* vt_PrimaryA(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (self == NULL) { (*n)++; return self; }
	_vt_lazy_assign(self);
	if (self->vt && self->vt->PrimaryA) return self->vt->PrimaryA(thgc, self, n, local, primary, val2);
	(*n)++; return self;
}
Obj* vt_exeZ(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->exeZ) return self->vt->exeZ(thgc, self, local);
	return self;
}
Obj* vt_exepZ(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->exepZ) return self->vt->exepZ(thgc, self, n, local, primary);
	return self;
}
Obj* vt_GetterZ(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->GetterZ) return self->vt->GetterZ(thgc, self, local);
	return self;
}
Obj* vt_SelfZ(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->SelfZ) return self->vt->SelfZ(thgc, self, local);
	return self;
}
Obj* vt_PrimaryZ(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (self == NULL) { (*n)++; return self; }
	_vt_lazy_assign(self);
	if (self->vt && self->vt->PrimaryZ) return self->vt->PrimaryZ(thgc, self, n, local, primary, val2);
	(*n)++; return self;
}
Obj* vt_exeB(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->exeB) return self->vt->exeB(thgc, self, local);
	return self;
}
Obj* vt_exepB(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->exepB) return self->vt->exepB(thgc, self, n, local, primary);
	return self;
}
Obj* vt_GetterB(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->GetterB) return self->vt->GetterB(thgc, self, local);
	return self;
}
Obj* vt_SelfB(ThreadGC* thgc, Obj* self, Master* local) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->SelfB) return self->vt->SelfB(thgc, self, local);
	return self;
}
Obj* vt_opeB(ThreadGC* thgc, Obj* self, String* key, Master* local, Obj* val2) {
	if (self == NULL) return self;
	_vt_lazy_assign(self);
	if (self->vt && self->vt->opeB) return self->vt->opeB(thgc, self, key, local, val2);
	return self;
}
Obj* vt_PrimaryB(ThreadGC* thgc, Obj* self, int* n, Master* local, Primary* primary, Obj* val2) {
	if (self == NULL) { (*n)++; return self; }
	_vt_lazy_assign(self);
	if (self->vt && self->vt->PrimaryB) return self->vt->PrimaryB(thgc, self, n, local, primary, val2);
	(*n)++; return self;
}

// ============================================================
// VTable割り当てヘルパー
// ============================================================
void vt_assign(Obj* obj, ObjVT* vt) {
	if (obj) obj->vt = vt;
}
void vt_assign_block(Block* blk, ObjVT* vt) {
	if (blk) blk->vt = vt;
}

// ============================================================
// initVTables - 起動時にVTableルックアップを初期化
// (obj2a.h, obj2a2.h, obj2a3.h, objz.h の関数実装後に呼ぶ)
// ============================================================
void initVTables();
void initVTables_A2();
void initVTables_A3();
void initVTables_B();
void initVTables_B2();
void initVTables_B3();
