#pragma once
// ============================================================
// llvm.h - LLVM IR Code Generation (ported from C# Component classes)
// ============================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
// Section A: StringBuilder（String* を内包）
// ============================================================
struct SB {
    String str;
    int capa;
};

SB* sb_create(ThreadGC* thgc, int esize = 1) {
    SB* sb = (SB*)GC_alloc(thgc, _SB);
    sb->capa = 256;
    sb->str.size = 0;
    sb->str.esize = esize;
    sb->str.data = (char*)GC_alloc_size(thgc, sb->capa);
    return sb;
}
// extra は追加する文字数
void sb_ensure(ThreadGC* thgc, SB* sb, int extra) {
    int needed = (sb->str.size + extra) * sb->str.esize;
    if (needed >= sb->capa) {
        int newcapa = sb->capa * 2;
        while (newcapa < needed + sb->str.esize) newcapa *= 2;
        char* newdata = (char*)GC_alloc_size(thgc, newcapa);
        memcpy(newdata, sb->str.data, sb->str.size * sb->str.esize);
        sb->str.data = newdata;
        sb->capa = newcapa;
    }
}
void sb_addc(ThreadGC* thgc, SB* sb, char c) {
    sb_ensure(thgc, sb, 1);
    int off = sb->str.size * sb->str.esize;
    sb->str.data[off] = c;
    if (sb->str.esize == 2) sb->str.data[off + 1] = 0;
    sb->str.size++;
}
void sb_adds(ThreadGC* thgc, SB* sb, const char* s) {
    int len = (int)strlen(s);
    sb_ensure(thgc, sb, len);
    int off = sb->str.size * sb->str.esize;
    if (sb->str.esize == 1) {
        memcpy(sb->str.data + off, s, len);
    } else {
        for (int i = 0; i < len; i++) {
            sb->str.data[off + i * 2] = s[i];
            sb->str.data[off + i * 2 + 1] = 0;
        }
    }
    sb->str.size += len;
}
// chars 文字分コピー（esize変換あり）
void sb_addn(ThreadGC* thgc, SB* sb, const char* s, int chars, int src_esize) {
    sb_ensure(thgc, sb, chars);
    int off = sb->str.size * sb->str.esize;
    if (src_esize == sb->str.esize) {
        memcpy(sb->str.data + off, s, chars * sb->str.esize);
    } else if (src_esize == 1 && sb->str.esize == 2) {
        for (int i = 0; i < chars; i++) {
            sb->str.data[off + i * 2] = s[i];
            sb->str.data[off + i * 2 + 1] = 0;
        }
    } else if (src_esize == 2 && sb->str.esize == 1) {
        for (int i = 0; i < chars; i++) {
            sb->str.data[off + i] = s[i * 2];
        }
    }
    sb->str.size += chars;
}
void sb_addi(ThreadGC* thgc, SB* sb, int n) {
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%d", n);
    sb_adds(thgc, sb, buf);
}
void sb_add_str(ThreadGC* thgc, SB* sb, String* s) {
    if (!s) return;
    sb_addn(thgc, sb, s->data, s->size, s->esize);
}
void sb_add_sb(ThreadGC* thgc, SB* sb, SB* other) {
    if (!other || other->str.size == 0) return;
    sb_add_str(thgc, sb, &other->str);
}
void sb_write(SB* sb, FILE* f) {
    int bytes = sb->str.size * sb->str.esize;
    if (bytes > 0) fwrite(sb->str.data, 1, bytes, f);
}

// ============================================================
// Section B: Enums
// ============================================================
enum LCompType {
    LC_Vari, LC_StrV, LC_IfValue, LC_Phi, LC_TypeDec, LC_TypeVal,
    LC_Func, LC_Gete, LC_Cast, LC_Call, LC_Alloca, LC_Store, LC_Load,
    LC_Br, LC_SwitchComp, LC_BinOp, LC_Cmp, LC_Lab,
    LC_LocationMarker, LC_Ret, LC_LLVM
};
enum LCastType {
    LCAST_Bitcast, LCAST_Zext, LCAST_Sext, LCAST_Trunc, LCAST_PtrToInt, LCAST_IntToPtr
};
enum LBinOpType {
    LBOP_Add, LBOP_Sub, LBOP_Mul, LBOP_SDiv, LBOP_SRem
};
enum LCmpType {
    LCMP_Eq, LCMP_Ne, LCMP_SLT, LCMP_SLE, LCMP_SGT, LCMP_SGE
};

// ============================================================
// Section B: Forward declarations
// ============================================================
struct LComp;
struct LVari;
struct LStrV;
struct LLab;
struct LIfValue;
struct LPhi;
struct LTypeDec;
struct LTypeVal;
struct LFunc;
struct LGete;
struct LCast;
struct LCall;
struct LAlloca;
struct LStore;
struct LLoad;
struct LBr;
struct LSwitchComp;
struct LBinOp;
struct LCmp;
struct LLocationMarker;
struct LRet;
struct LDebugInfo;
struct LDebugVariable;
struct LStructField;
struct LStructTypeInfo;
struct LLLVM;

// ============================================================
// Section B: Struct definitions
// ============================================================
struct LComp {
    LCompType comptype;
    int debugLocationId;
    List* comps; // List of LComp*
};

struct LVari {
    LCompType comptype; // LC_Vari
    int debugLocationId;
    List* comps;
    String* type;
    String* name;
};

struct LStrV {
    LCompType comptype; // LC_StrV
    int debugLocationId;
    List* comps;
    String* type;
    String* name;
    String* value;
    int length;
};

struct LLab {
    LCompType comptype; // LC_Lab
    int debugLocationId;
    List* comps;
    String* name;
};

struct LIfValue {
    LCompType comptype; // LC_IfValue
    int debugLocationId;
    List* comps;
    LLab* lab;
    LVari* v;
    bool check;
};

struct LPhi {
    LCompType comptype; // LC_Phi
    int debugLocationId;
    List* comps;
    LVari* y;
    Map* vmap; // String* -> LIfValue*
};

struct LTypeDec {
    LCompType comptype; // LC_TypeDec
    int debugLocationId;
    List* comps; // List of LTypeVal*
    String* name;
    String* option;
    bool func;
};

struct LTypeVal {
    LCompType comptype; // LC_TypeVal
    int debugLocationId;
    List* comps;
    String* type;
    String* name;
};

struct LFunc {
    LCompType comptype; // LC_Func
    int debugLocationId;
    List* comps; // List of LComp* (instructions)
    LVari* y; // return type + function name
    List* draws; // List of LVari* (parameters)
    List* dbinits; // List of LComp*
    LVari* vari;
    bool async_flag;
    bool optnone;
    LSwitchComp* sc;
    int debugId;
    int currentLine;
    LLLVM* llvm;
    List* debugVariables; // List of LDebugVariable*
};

struct LGete {
    LCompType comptype; // LC_Gete
    int debugLocationId;
    List* comps; // List of LVari* (indices)
    String* type;
    LVari* y;
    LVari* x;
};

struct LCast {
    LCompType comptype; // LC_Cast
    int debugLocationId;
    List* comps;
    LCastType casttype;
    LVari* y;
    LVari* x;
    int debugVarId; // -1 = none, used by Bitcast
};

struct LCall {
    LCompType comptype; // LC_Call
    int debugLocationId;
    List* comps; // List of LVari* (arguments)
    LVari* y; // return value (NULL for void)
    LVari* f; // function
};

struct LAlloca {
    LCompType comptype; // LC_Alloca
    int debugLocationId;
    List* comps;
    LVari* a;
};

struct LStore {
    LCompType comptype; // LC_Store
    int debugLocationId;
    List* comps;
    LVari* y; // destination pointer
    LVari* a; // source value
};

struct LLoad {
    LCompType comptype; // LC_Load
    int debugLocationId;
    List* comps;
    LVari* y; // result
    LVari* a; // source pointer
    bool vol;
};

struct LBr {
    LCompType comptype; // LC_Br
    int debugLocationId;
    List* comps; // List of LLab*
    LVari* a; // condition (NULL for unconditional)
};

struct LSwitchComp {
    LCompType comptype; // LC_SwitchComp
    int debugLocationId;
    List* comps;
    LVari* x;
    LLab* lab; // default label
    List* values; // List of LIfValue*
};

struct LBinOp {
    LCompType comptype; // LC_BinOp
    int debugLocationId;
    List* comps;
    LBinOpType optype;
    LVari* y;
    LVari* a;
    LVari* b;
};

struct LCmp {
    LCompType comptype; // LC_Cmp
    int debugLocationId;
    List* comps;
    LCmpType cmptype;
    LVari* y;
    LVari* a;
    LVari* b;
};

struct LLocationMarker {
    LCompType comptype; // LC_LocationMarker
    int debugLocationId;
    List* comps;
    int line;
    int column;
    int scopeId;
    int metaid;
};

struct LRet {
    LCompType comptype; // LC_Ret
    int debugLocationId;
    List* comps;
    LVari* a;
};

// ============================================================
// Debug structs (not Components)
// ============================================================
struct LDebugVariable {
    String* name;
    int typeId;
    int scopeId;
    int line;
    int metaId;
    int argNo;
};

struct LStructField {
    String* name;
    String* llvmType;
    int offsetInBits;
    int sizeInBits;
    int metaId;
};

struct LStructTypeInfo {
    int structId;
    int fileId;
    List* fields; // List of LStructField*
    String* name;
};

struct LDebugInfo {
    int metaId;
    Map* fileIds;       // String* -> int (as intptr_t)
    Map* functionIds;   // String* -> int
    Map* typeIds;       // String* -> int
    Map* structTypeIds; // String* -> int
    Map* structTypes;   // String* -> LStructTypeInfo*
    Map* locations;     // int-key -> LLocationMarker* (use List for int-keyed)
    List* locationKeys; // List of int (parallel to locations)
    List* locationVals; // List of LLocationMarker*
    Map* pointerTypes;  // int-key -> int (use List pairs)
    List* ptrBaseIds;   // List of int
    List* ptrTypeIds;   // List of int
    int compileUnitId;
    int dwarfVersionId;
    int debugInfoVersionId;
};

// ============================================================
// Main LLVM struct
// ============================================================
int llvm_n = 0; // global counter (C# LLVM.n)

struct LLLVM {
    LCompType comptype; // LC_LLVM
    int debugLocationId;
    List* comps;        // List of LComp* (top-level funcs)
    Master* local;
    List* strs;         // List of LStrV*
    List* types;        // List of LTypeDec*
    List* funcs;        // List of LFunc*
    LFunc* main_func;
    LFunc* main;        // alias used in obj2c_part3
    int cn;
    List* tagvars;      // List of String*
    List* tagvarvars;   // List of LVari*
    List* tagparents;   // List of LVari*
    List* tagparenttes; // List of LVari*
    LDebugInfo* debugInfo;
    String* sourceFile;
    String* directory;
};

static int lfunc_fn = 0; // C# Func.fn

// ============================================================
// Section C: Constructor functions
// ============================================================
LVari* make_lvari(ThreadGC* thgc, String* type, String* name) {
    LVari* v = (LVari*)GC_alloc(thgc, _LVari);
    v->comptype = LC_Vari;
    v->debugLocationId = -1;
    v->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    v->type = type;
    v->name = name;
    return v;
}

// Helper: create LVari from C string literals
LVari* make_lvari_c(ThreadGC* thgc, const char* type, const char* name) {
    return make_lvari(thgc,
        createString(thgc, (char*)type, (int)strlen(type), 1),
        createString(thgc, (char*)name, (int)strlen(name), 1));
}

LStrV* make_lstrv(ThreadGC* thgc, String* name, String* value, int length) {
    LStrV* v = (LStrV*)GC_alloc(thgc, _LStrV);
    v->comptype = LC_StrV;
    v->debugLocationId = -1;
    v->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    // type = "[N+1 x i8]*"
    char typebuf[64];
    snprintf(typebuf, sizeof(typebuf), "[%d x i8]*", length + 1);
    v->type = createString(thgc, typebuf, (int)strlen(typebuf), 1);
    v->name = name;
    v->value = value;
    v->length = length;
    return v;
}

LLab* make_llab(ThreadGC* thgc, String* name) {
    LLab* l = (LLab*)GC_alloc(thgc, _LLab);
    l->comptype = LC_Lab;
    l->debugLocationId = -1;
    l->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    l->name = name;
    return l;
}

LLab* make_llab_c(ThreadGC* thgc, const char* name) {
    return make_llab(thgc, createString(thgc, (char*)name, (int)strlen(name), 1));
}

// Overload: make_llab accepting char* (used in obj2c_part3.h)
LLab* make_llab(ThreadGC* thgc, const char* name) {
    return make_llab_c(thgc, name);
}

LIfValue* make_lifvalue(ThreadGC* thgc, LLab* lab, LVari* v) {
    LIfValue* iv = (LIfValue*)GC_alloc(thgc, _LIfValue);
    iv->comptype = LC_IfValue;
    iv->debugLocationId = -1;
    iv->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    iv->lab = lab;
    iv->v = v;
    iv->check = false;
    return iv;
}

LPhi* make_lphi(ThreadGC* thgc, LVari* y, Map* vmap) {
    LPhi* p = (LPhi*)GC_alloc(thgc, _LPhi);
    p->comptype = LC_Phi;
    p->debugLocationId = -1;
    p->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    p->y = y;
    p->vmap = vmap;
    return p;
}

LTypeDec* make_ltypedec(ThreadGC* thgc, String* name, String* option, bool func) {
    LTypeDec* t = (LTypeDec*)GC_alloc(thgc, _LTypeDec);
    t->comptype = LC_TypeDec;
    t->debugLocationId = -1;
    t->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    t->name = name;
    t->option = option;
    t->func = func;
    return t;
}

LTypeVal* make_ltypeval(ThreadGC* thgc, String* type, String* name) {
    LTypeVal* t = (LTypeVal*)GC_alloc(thgc, _LTypeVal);
    t->comptype = LC_TypeVal;
    t->debugLocationId = -1;
    t->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    t->type = type;
    t->name = name;
    return t;
}

LGete* make_lgete(ThreadGC* thgc, String* type, LVari* y, LVari* x) {
    LGete* g = (LGete*)GC_alloc(thgc, _LGete);
    g->comptype = LC_Gete;
    g->debugLocationId = -1;
    g->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    g->type = type;
    g->y = y;
    g->x = x;
    return g;
}

LCast* make_lcast(ThreadGC* thgc, LCastType casttype, LVari* y, LVari* x) {
    LCast* c = (LCast*)GC_alloc(thgc, _LCast);
    c->comptype = LC_Cast;
    c->debugLocationId = -1;
    c->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    c->casttype = casttype;
    c->y = y;
    c->x = x;
    c->debugVarId = -1;
    return c;
}

LCall* make_lcall(ThreadGC* thgc, LVari* y, LVari* f) {
    LCall* c = (LCall*)GC_alloc(thgc, _LCall);
    c->comptype = LC_Call;
    c->debugLocationId = -1;
    c->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    c->y = y;
    c->f = f;
    return c;
}

LAlloca* make_lalloca(ThreadGC* thgc, LVari* a) {
    LAlloca* al = (LAlloca*)GC_alloc(thgc, _LAlloca);
    al->comptype = LC_Alloca;
    al->debugLocationId = -1;
    al->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    al->a = a;
    return al;
}

LStore* make_lstore(ThreadGC* thgc, LVari* y, LVari* a) {
    LStore* s = (LStore*)GC_alloc(thgc, _LStore);
    s->comptype = LC_Store;
    s->debugLocationId = -1;
    s->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    s->y = y;
    s->a = a;
    return s;
}

LLoad* make_lload(ThreadGC* thgc, LVari* y, LVari* a, bool vol) {
    LLoad* l = (LLoad*)GC_alloc(thgc, _LLoad);
    l->comptype = LC_Load;
    l->debugLocationId = -1;
    l->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    l->y = y;
    l->a = a;
    l->vol = vol;
    return l;
}

LBr* make_lbr(ThreadGC* thgc, LVari* a) {
    LBr* b = (LBr*)GC_alloc(thgc, _LBr);
    b->comptype = LC_Br;
    b->debugLocationId = -1;
    b->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    b->a = a;
    return b;
}

// Conditional branch: br cond, label %true, label %false
LBr* make_lbr(ThreadGC* thgc, LVari* a, LLab* lab_true, LLab* lab_false) {
    LBr* b = make_lbr(thgc, a);
    add_list(thgc, b->comps, (char*)lab_true);
    if (lab_false) add_list(thgc, b->comps, (char*)lab_false);
    return b;
}

// Unconditional branch: br label %target
LBr* make_lbr_uncond(ThreadGC* thgc, LLab* lab) {
    LBr* b = make_lbr(thgc, (LVari*)NULL);
    add_list(thgc, b->comps, (char*)lab);
    return b;
}

LSwitchComp* make_lswitchcomp(ThreadGC* thgc, LVari* x, LLab* lab, List* values) {
    LSwitchComp* s = (LSwitchComp*)GC_alloc(thgc, _LSwitchComp);
    s->comptype = LC_SwitchComp;
    s->debugLocationId = -1;
    s->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    s->x = x;
    s->lab = lab;
    s->values = values;
    return s;
}

LBinOp* make_lbinop(ThreadGC* thgc, LBinOpType optype, LVari* y, LVari* a, LVari* b) {
    LBinOp* op = (LBinOp*)GC_alloc(thgc, _LBinOp);
    op->comptype = LC_BinOp;
    op->debugLocationId = -1;
    op->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    op->optype = optype;
    op->y = y;
    op->a = a;
    op->b = b;
    return op;
}

LCmp* make_lcmp(ThreadGC* thgc, LCmpType cmptype, LVari* y, LVari* a, LVari* b) {
    LCmp* c = (LCmp*)GC_alloc(thgc, _LCmp);
    c->comptype = LC_Cmp;
    c->debugLocationId = -1;
    c->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    c->cmptype = cmptype;
    c->y = y;
    c->a = a;
    c->b = b;
    return c;
}

LLocationMarker* make_llocmarker(ThreadGC* thgc, int line, int column, int scopeId) {
    LLocationMarker* m = (LLocationMarker*)GC_alloc(thgc, _LLocationMarker);
    m->comptype = LC_LocationMarker;
    m->debugLocationId = -1;
    m->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    m->line = line;
    m->column = column;
    m->scopeId = scopeId;
    m->metaid = -1;
    return m;
}

LRet* make_lret(ThreadGC* thgc, LVari* a) {
    LRet* r = (LRet*)GC_alloc(thgc, _LRet);
    r->comptype = LC_Ret;
    r->debugLocationId = -1;
    r->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    r->a = a;
    return r;
}

// Global counters used by obj2c code generation
static int obj_cn = 0;
static int Obj_cn = 0;

// IfValue is an alias for LIfValue (used in obj2c_part3.h)
typedef LIfValue IfValue;

// Overload: make_ltypeval accepting const char* for type
LTypeVal* make_ltypeval(ThreadGC* thgc, const char* type, String* name) {
    return make_ltypeval(thgc,
        createString(thgc, (char*)type, (int)strlen(type), 1), name);
}

// Overload: make_ltypeval with const char* type and int name
LTypeVal* make_ltypeval(ThreadGC* thgc, const char* type, int name) {
    char buf[32]; snprintf(buf, sizeof(buf), "%d", name);
    return make_ltypeval(thgc,
        createString(thgc, (char*)type, (int)strlen(type), 1),
        createString(thgc, buf, (int)strlen(buf), 1));
}

// Overload: make_ltypeval with const char* type and const char* name
LTypeVal* make_ltypeval(ThreadGC* thgc, const char* type, const char* name) {
    return make_ltypeval(thgc,
        createString(thgc, (char*)type, (int)strlen(type), 1),
        createString(thgc, (char*)name, (int)strlen(name), 1));
}

// Overload: make_lstrv with char* name/value (used in obj2c_part4.h emit_create_string_sql)
LStrV* make_lstrv(ThreadGC* thgc, const char* name, const char* value, int length) {
    return make_lstrv(thgc,
        createString(thgc, (char*)name, (int)strlen(name), 1),
        createString(thgc, (char*)value, length, 1), length);
}

// Overload: make_ltypedec with just name (no option, not func)
LTypeDec* make_ltypedec(ThreadGC* thgc, String* name) {
    return make_ltypedec(thgc, name, NULL, false);
}

// Forward declarations for virtual dispatch used in obj2c_part2/3/5.h
void vt_Dec(ThreadGC* thgc, Obj* d, Master* local);
void vt_DecC(ThreadGC* thgc, Obj* d, Master* local);

// ============================================================
// Section C: DebugInfo constructor
// ============================================================
LDebugInfo* make_ldebuginfo(ThreadGC* thgc) {
    LDebugInfo* d = (LDebugInfo*)GC_alloc(thgc, _LDebugInfo);
    d->metaId = 0;
    d->fileIds = create_mapy(thgc, CType::_KV);
    d->functionIds = create_mapy(thgc, CType::_KV);
    d->typeIds = create_mapy(thgc, CType::_KV);
    d->structTypeIds = create_mapy(thgc, CType::_KV);
    d->structTypes = create_mapy(thgc, CType::_KV);
    d->locationKeys = create_list(thgc, sizeof(int), CType::_List);
    d->locationVals = create_list(thgc, sizeof(LLocationMarker*), CType::_List);
    d->ptrBaseIds = create_list(thgc, sizeof(int), CType::_List);
    d->ptrTypeIds = create_list(thgc, sizeof(int), CType::_List);
    d->dwarfVersionId = d->metaId++;
    d->debugInfoVersionId = d->metaId++;
    d->compileUnitId = d->metaId++;
    return d;
}

// Helper: store int in map as (intptr_t) cast
void ldbg_map_set_int(ThreadGC* thgc, Map* map, String* key, int val) {
    add_mapy(thgc, map, key, (char*)(intptr_t)val);
}
int ldbg_map_get_int(Map* map, String* key) {
    char* v = get_mapy(map, key);
    return (v != NULL) ? (int)(intptr_t)v : -1;
}
bool ldbg_map_has(Map* map, String* key) {
    return get_mapy(map, key) != NULL;
}

// ============================================================
// Section C: LLLVM and LFunc constructors
// ============================================================
// Forward declaration of ldbg functions used in LFunc constructor
int ldbg_create_file(ThreadGC* thgc, LDebugInfo* dbg, String* filename, String* directory);
int ldbg_create_function(ThreadGC* thgc, LDebugInfo* dbg, String* name, int fileId, int line);

LFunc* make_lfunc(ThreadGC* thgc, LLLVM* llvm, LVari* y) {
    LFunc* f = (LFunc*)GC_alloc(thgc, _LFunc);
    f->comptype = LC_Func;
    f->debugLocationId = -1;
    f->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    f->y = y;
    f->draws = create_list(thgc, sizeof(LVari*), CType::_List);
    f->dbinits = create_list(thgc, sizeof(LComp*), CType::_List);
    f->vari = NULL;
    f->async_flag = false;
    f->optnone = false;
    f->sc = NULL;
    f->currentLine = 1;
    f->llvm = llvm;
    f->debugVariables = create_list(thgc, sizeof(LDebugVariable*), CType::_List);
    int fileId = ldbg_create_file(thgc, llvm->debugInfo, llvm->sourceFile, llvm->directory);
    f->debugId = ldbg_create_function(thgc, llvm->debugInfo, y->name, fileId, 1);
    return f;
}

// Overload: make_lfunc 4-arg (LLLVM, y, 1 param)
LFunc* make_lfunc(ThreadGC* thgc, LLLVM* lllvm, LVari* y, LVari* p1) {
    LFunc* f = make_lfunc(thgc, lllvm, y);
    add_list(thgc, f->draws, (char*)p1);
    return f;
}

// Overload: make_lfunc 5-arg (LLLVM, y, 2 params)
LFunc* make_lfunc(ThreadGC* thgc, LLLVM* llvm, LVari* y, LVari* p1, LVari* p2) {
    LFunc* f = make_lfunc(thgc, llvm, y);
    add_list(thgc, f->draws, (char*)p1);
    add_list(thgc, f->draws, (char*)p2);
    return f;
}

// Overload: make_lfunc5 7-arg (LLLVM, y, 4 params)
LFunc* make_lfunc5(ThreadGC* thgc, LLLVM* llvm, LVari* y, LVari* p1, LVari* p2, LVari* p3, LVari* p4) {
    LFunc* f = make_lfunc(thgc, llvm, y);
    add_list(thgc, f->draws, (char*)p1);
    add_list(thgc, f->draws, (char*)p2);
    add_list(thgc, f->draws, (char*)p3);
    add_list(thgc, f->draws, (char*)p4);
    return f;
}

// Overload: make_lvari_s with String* name (for calls that pass String* as name)
LVari* make_lvari_s(ThreadGC* thgc, const char* type, String* name) {
    return make_lvari(thgc,
        createString(thgc, (char*)type, (int)strlen(type), 1), name);
}

// Helper to get last func from LLLVM
LFunc* llvm_get_func(LLLVM* llvm) {
    return *(LFunc**)get_list(llvm->funcs, llvm->funcs->size - 1);
}

LLLVM* make_lllvm(ThreadGC* thgc, Master* local) {
    LLLVM* l = (LLLVM*)GC_alloc(thgc, _LLLVM);
    l->comptype = LC_LLVM;
    l->debugLocationId = -1;
    l->comps = create_list(thgc, sizeof(LComp*), CType::_List);
    l->local = local;
    l->strs = create_list(thgc, sizeof(LStrV*), CType::_List);
    l->types = create_list(thgc, sizeof(LTypeDec*), CType::_List);
    l->funcs = create_list(thgc, sizeof(LFunc*), CType::_List);
    l->main_func = NULL;
    l->cn = 0;
    l->tagvars = create_list(thgc, sizeof(String*), CType::_List);
    l->tagvarvars = create_list(thgc, sizeof(LVari*), CType::_List);
    l->tagparents = create_list(thgc, sizeof(LVari*), CType::_List);
    l->tagparenttes = create_list(thgc, sizeof(LVari*), CType::_List);
    l->debugInfo = make_ldebuginfo(thgc);
    l->sourceFile = createString(thgc, (char*)"foo.cy", 6, 1);
    // directory: cwd + "/clang2" (simplified)
    l->directory = createString(thgc, (char*)".", 1, 1);

    // Create initial test functions (matching C# constructor)
    LVari* testY = make_lvari_c(thgc, "%torch.nn.ModuleImpl*", "@test");
    LFunc* testFunc = make_lfunc(thgc, l, testY);
    add_list(thgc, l->comps, (char*)&testFunc);

    LVari* fwdY = make_lvari_c(thgc, "i8*", "@forward");
    LFunc* fwdFunc = make_lfunc(thgc, l, fwdY);
    LVari* fwdP1 = make_lvari_c(thgc, "i8*", "%obj");
    LVari* fwdP2 = make_lvari_c(thgc, "i8*", "%input");
    add_list(thgc, fwdFunc->draws, (char*)&fwdP1);
    add_list(thgc, fwdFunc->draws, (char*)&fwdP2);
    add_list(thgc, l->comps, (char*)&fwdFunc);

    LVari* test2Y = make_lvari_c(thgc, "i32", "@test2");
    LFunc* test2Func = make_lfunc(thgc, l, test2Y);
    add_list(thgc, l->comps, (char*)&test2Func);
    add_list(thgc, l->funcs, (char*)&test2Func);

    return l;
}

// ============================================================
// Section D: Output helper
// ============================================================
// Forward declaration needed by output_lfunc
int ldbg_create_location(ThreadGC* thgc, LDebugInfo* dbg, LLocationMarker* lm);

void output_debugloc(ThreadGC* thgc, SB* sb, int debugLocationId) {
    if (debugLocationId >= 0) {
        sb_adds(thgc, sb, ", !dbg !");
        sb_addi(thgc, sb, debugLocationId);
    }
}

// ============================================================
// Section D: Output functions
// ============================================================
// Forward declaration of dispatcher
void lcomp_output(ThreadGC* thgc, LComp* comp, SB* sb);

void output_lvari(ThreadGC* thgc, LVari* v, SB* sb) {
    sb_add_str(thgc, sb, v->type);
    sb_addc(thgc, sb, ' ');
    sb_add_str(thgc, sb, v->name);
}

void output_lstrv_dec(ThreadGC* thgc, LStrV* v, SB* sb) {
    // name = private unnamed_addr constant [N x i8] c"value\00", align 1
    sb_add_str(thgc, sb, v->name);
    sb_adds(thgc, sb, " = private unnamed_addr constant ");
    // type without trailing *
    String* t = v->type;
    if (t->size > 0 && t->data[t->size - 1] == '*') {
        sb_addn(thgc, sb, t->data, t->size - 1, t->esize);
    } else {
        sb_add_str(thgc, sb, t);
    }
    sb_adds(thgc, sb, " c\"");
    sb_add_str(thgc, sb, v->value);
    sb_adds(thgc, sb, "\\00\", align 1\n");
}

void output_lphi(ThreadGC* thgc, LPhi* p, SB* sb) {
    sb_add_str(thgc, sb, p->y->name);
    sb_adds(thgc, sb, " = phi ");
    sb_add_str(thgc, sb, p->y->type);
    // iterate vmap values
    bool first = true;
    for (int i = 0; i < p->vmap->kvs->size; i++) {
        KV* kv = *(KV**)get_list(p->vmap->kvs, i);
        LIfValue* iv = (LIfValue*)(kv->value);
        if (!iv || !iv->v) continue;
        if (!first) sb_addc(thgc, sb, ',');
        first = false;
        sb_adds(thgc, sb, " [");
        sb_add_str(thgc, sb, iv->v->name);
        sb_adds(thgc, sb, ", %");
        sb_add_str(thgc, sb, iv->lab->name);
        sb_addc(thgc, sb, ']');
    }
    output_debugloc(thgc, sb, p->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_ltypedec(ThreadGC* thgc, LTypeDec* t, SB* sb) {
    sb_add_str(thgc, sb, t->name);
    sb_adds(thgc, sb, " = type ");
    if (!t->func) {
        sb_addc(thgc, sb, '{');
        for (int i = 0; i < t->comps->size; i++) {
            if (i != 0) sb_addc(thgc, sb, ',');
            LTypeVal* tv = *(LTypeVal**)get_list(t->comps, i);
            sb_add_str(thgc, sb, tv->type);
        }
        sb_addc(thgc, sb, '}');
        if (t->option) sb_add_str(thgc, sb, t->option);
        sb_addc(thgc, sb, '\n');
    } else {
        LTypeVal* tv0 = *(LTypeVal**)get_list(t->comps, 0);
        sb_add_str(thgc, sb, tv0->type);
        sb_addc(thgc, sb, '(');
        for (int i = 1; i < t->comps->size; i++) {
            if (i != 1) sb_addc(thgc, sb, ',');
            LTypeVal* tv = *(LTypeVal**)get_list(t->comps, i);
            sb_add_str(thgc, sb, tv->type);
        }
        sb_addc(thgc, sb, ')');
        if (t->option) sb_add_str(thgc, sb, t->option);
        sb_addc(thgc, sb, '\n');
    }
}

void output_lgete(ThreadGC* thgc, LGete* g, SB* sb) {
    sb_add_str(thgc, sb, g->y->name);
    sb_adds(thgc, sb, " = getelementptr ");
    sb_add_str(thgc, sb, g->type);
    sb_addc(thgc, sb, ',');
    sb_add_str(thgc, sb, g->x->type);
    sb_addc(thgc, sb, ' ');
    sb_add_str(thgc, sb, g->x->name);
    sb_addc(thgc, sb, '\n');
    for (int i = 0; i < g->comps->size; i++) {
        LVari* idx = *(LVari**)get_list(g->comps, i);
        sb_adds(thgc, sb, ", ");
        sb_add_str(thgc, sb, idx->type);
        sb_addc(thgc, sb, ' ');
        sb_add_str(thgc, sb, idx->name);
    }
    output_debugloc(thgc, sb, g->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_lcast(ThreadGC* thgc, LCast* c, SB* sb) {
    const char* opname = "bitcast";
    switch (c->casttype) {
        case LCAST_Bitcast:  opname = "bitcast"; break;
        case LCAST_Zext:     opname = "zext"; break;
        case LCAST_Sext:     opname = "sext"; break;
        case LCAST_Trunc:    opname = "trunc"; break;
        case LCAST_PtrToInt: opname = "ptrtoint"; break;
        case LCAST_IntToPtr: opname = "inttoptr"; break;
    }
    sb_add_str(thgc, sb, c->y->name);
    sb_adds(thgc, sb, " = ");
    sb_adds(thgc, sb, opname);
    sb_addc(thgc, sb, ' ');
    output_lvari(thgc, c->x, sb);
    sb_adds(thgc, sb, " to ");
    sb_add_str(thgc, sb, c->y->type);
    // Bitcast debug var
    if (c->casttype == LCAST_Bitcast && c->debugVarId >= 0) {
        sb_addc(thgc, sb, '\n');
        sb_adds(thgc, sb, "call void @llvm.dbg.value(");
        sb_adds(thgc, sb, "metadata ");
        sb_add_str(thgc, sb, c->y->type);
        sb_addc(thgc, sb, ' ');
        sb_add_str(thgc, sb, c->y->name);
        sb_adds(thgc, sb, ", metadata !");
        sb_addi(thgc, sb, c->debugVarId);
        sb_adds(thgc, sb, ", metadata !DIExpression())");
    }
    output_debugloc(thgc, sb, c->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_lcall(ThreadGC* thgc, LCall* c, SB* sb) {
    if (c->y != NULL) {
        if (c->y->name != NULL) {
            sb_add_str(thgc, sb, c->y->name);
            sb_adds(thgc, sb, " = call ");
            sb_add_str(thgc, sb, c->y->type);
        } else {
            sb_adds(thgc, sb, "call ");
            sb_add_str(thgc, sb, c->y->type);
        }
    } else {
        sb_adds(thgc, sb, "call void");
    }
    sb_addc(thgc, sb, ' ');
    sb_add_str(thgc, sb, c->f->name);
    sb_addc(thgc, sb, '(');
    for (int i = 0; i < c->comps->size; i++) {
        if (i != 0) sb_adds(thgc, sb, ", ");
        LVari* arg = *(LVari**)get_list(c->comps, i);
        output_lvari(thgc, arg, sb);
    }
    sb_addc(thgc, sb, ')');
    output_debugloc(thgc, sb, c->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_lalloca(ThreadGC* thgc, LAlloca* a, SB* sb) {
    sb_add_str(thgc, sb, a->a->name);
    sb_adds(thgc, sb, " = alloca ");
    // type without trailing *
    String* t = a->a->type;
    if (t->size > 0 && t->data[(t->size - 1) * t->esize] == '*') {
        sb_addn(thgc, sb, t->data, t->size - 1, t->esize);
    } else {
        sb_add_str(thgc, sb, t);
    }
    output_debugloc(thgc, sb, a->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_lstore(ThreadGC* thgc, LStore* s, SB* sb) {
    sb_adds(thgc, sb, "store ");
    output_lvari(thgc, s->a, sb);
    sb_adds(thgc, sb, ", ");
    sb_add_str(thgc, sb, s->y->type);
    sb_addc(thgc, sb, ' ');
    sb_add_str(thgc, sb, s->y->name);
    output_debugloc(thgc, sb, s->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_lload(ThreadGC* thgc, LLoad* l, SB* sb) {
    sb_add_str(thgc, sb, l->y->name);
    sb_adds(thgc, sb, " = ");
    if (l->vol) sb_adds(thgc, sb, "load volatile ");
    else sb_adds(thgc, sb, "load ");
    sb_add_str(thgc, sb, l->y->type);
    sb_adds(thgc, sb, ", ");
    sb_add_str(thgc, sb, l->a->type);
    sb_addc(thgc, sb, ' ');
    sb_add_str(thgc, sb, l->a->name);
    output_debugloc(thgc, sb, l->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_lbr(ThreadGC* thgc, LBr* b, SB* sb) {
    sb_adds(thgc, sb, "br ");
    if (b->a != NULL) {
        output_lvari(thgc, b->a, sb);
        sb_adds(thgc, sb, ", ");
    }
    for (int i = 0; i < b->comps->size; i++) {
        if (i != 0) sb_adds(thgc, sb, ", ");
        LLab* lab = *(LLab**)get_list(b->comps, i);
        sb_adds(thgc, sb, "label %");
        sb_add_str(thgc, sb, lab->name);
    }
    output_debugloc(thgc, sb, b->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_lswitchcomp(ThreadGC* thgc, LSwitchComp* s, SB* sb) {
    sb_adds(thgc, sb, "switch ");
    output_lvari(thgc, s->x, sb);
    sb_adds(thgc, sb, ", label %");
    sb_add_str(thgc, sb, s->lab->name);
    sb_adds(thgc, sb, " [\n");
    for (int i = 0; i < s->values->size; i++) {
        LIfValue* iv = *(LIfValue**)get_list(s->values, i);
        output_lvari(thgc, iv->v, sb);
        sb_adds(thgc, sb, ", label %");
        sb_add_str(thgc, sb, iv->lab->name);
        output_debugloc(thgc, sb, s->debugLocationId);
        sb_addc(thgc, sb, '\n');
    }
    sb_adds(thgc, sb, "]\n");
}

void output_lbinop(ThreadGC* thgc, LBinOp* op, SB* sb) {
    const char* opname = "add";
    switch (op->optype) {
        case LBOP_Add:  opname = "add"; break;
        case LBOP_Sub:  opname = "sub"; break;
        case LBOP_Mul:  opname = "mul"; break;
        case LBOP_SDiv: opname = "sdiv"; break;
        case LBOP_SRem: opname = "srem"; break;
    }
    sb_add_str(thgc, sb, op->y->name);
    sb_adds(thgc, sb, " = ");
    sb_adds(thgc, sb, opname);
    sb_addc(thgc, sb, ' ');
    output_lvari(thgc, op->a, sb);
    sb_adds(thgc, sb, ", ");
    sb_add_str(thgc, sb, op->b->name);
    output_debugloc(thgc, sb, op->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_lcmp(ThreadGC* thgc, LCmp* c, SB* sb) {
    const char* cmpname = "eq";
    switch (c->cmptype) {
        case LCMP_Eq:  cmpname = "eq"; break;
        case LCMP_Ne:  cmpname = "ne"; break;
        case LCMP_SLT: cmpname = "slt"; break;
        case LCMP_SLE: cmpname = "sle"; break;
        case LCMP_SGT: cmpname = "sgt"; break;
        case LCMP_SGE: cmpname = "sge"; break;
    }
    sb_add_str(thgc, sb, c->y->name);
    sb_adds(thgc, sb, " = icmp ");
    sb_adds(thgc, sb, cmpname);
    sb_addc(thgc, sb, ' ');
    output_lvari(thgc, c->a, sb);
    sb_adds(thgc, sb, ", ");
    sb_add_str(thgc, sb, c->b->name);
    output_debugloc(thgc, sb, c->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

void output_llab(ThreadGC* thgc, LLab* l, SB* sb) {
    sb_add_str(thgc, sb, l->name);
    sb_adds(thgc, sb, ":\n");
}

void output_lret(ThreadGC* thgc, LRet* r, SB* sb) {
    // Check for "void" type
    bool is_void = false;
    if (r->a->type->esize == 1 && r->a->type->size == 4 &&
        memcmp(r->a->type->data, "void", 4) == 0) {
        is_void = true;
    }
    if (is_void) {
        sb_adds(thgc, sb, "ret void");
    } else {
        sb_adds(thgc, sb, "ret ");
        sb_add_str(thgc, sb, r->a->type);
        sb_addc(thgc, sb, ' ');
        sb_add_str(thgc, sb, r->a->name);
    }
    output_debugloc(thgc, sb, r->debugLocationId);
    sb_addc(thgc, sb, '\n');
}

// ============================================================
// Section D: Func output
// ============================================================
// Helper: check if LComp is a Br/SwitchComp/Phi/Ret
bool lcomp_is_control(LComp* c) {
    return c->comptype == LC_Br || c->comptype == LC_SwitchComp ||
           c->comptype == LC_Phi || c->comptype == LC_Ret;
}
// Helper: check if LCall's function is @llvm.coro.save
bool lcall_is_coro_save(LCall* c) {
    if (!c->f || !c->f->name) return false;
    const char* target = "@llvm.coro.save";
    int tlen = 15;
    if (c->f->name->esize == 1 && c->f->name->size >= tlen) {
        return memcmp(c->f->name->data, target, tlen) == 0;
    }
    return false;
}

void output_lfunc(ThreadGC* thgc, LFunc* func, SB* sb) {
    sb_adds(thgc, sb, "define dllexport ");
    sb_add_str(thgc, sb, func->y->type);
    sb_addc(thgc, sb, ' ');
    sb_add_str(thgc, sb, func->y->name);
    sb_addc(thgc, sb, '(');
    for (int i = 0; i < func->draws->size; i++) {
        if (i != 0) sb_adds(thgc, sb, ", ");
        LVari* p = *(LVari**)get_list(func->draws, i);
        output_lvari(thgc, p, sb);
    }
    sb_adds(thgc, sb, ") ");
    if (func->optnone) sb_adds(thgc, sb, "optnone noinline ");
    if (func->debugId >= 0) {
        sb_adds(thgc, sb, "!dbg !");
        sb_addi(thgc, sb, func->debugId);
        sb_addc(thgc, sb, ' ');
    }
    sb_adds(thgc, sb, "{\nentry:\n");

    int debugLocationId = -1;

    // dbinits
    for (int i = 0; i < func->dbinits->size; i++) {
        LComp* c = *(LComp**)get_list(func->dbinits, i);
        lcomp_output(thgc, c, sb);
        if (i != func->dbinits->size - 1) {
            LComp* next = *(LComp**)get_list(func->dbinits, i + 1);
            if (lcomp_is_control(next)) continue;
            if (debugLocationId >= 0) output_debugloc(thgc, sb, debugLocationId);
            sb_addc(thgc, sb, '\n');
        }
    }

    // comps (instructions)
    for (int i = 0; i < func->comps->size; i++) {
        LComp* c = *(LComp**)get_list(func->comps, i);
        if (c->comptype == LC_LocationMarker) {
            LLocationMarker* marker = (LLocationMarker*)c;
            debugLocationId = ldbg_create_location(thgc, func->llvm->debugInfo, marker);
            continue;
        } else if (debugLocationId >= 0) {
            c->debugLocationId = debugLocationId;
        }
        lcomp_output(thgc, c, sb);
        // Skip print_int after coro.save, br, switch, ret
        if (c->comptype == LC_Call && lcall_is_coro_save((LCall*)c)) continue;
        if (c->comptype == LC_Br || c->comptype == LC_SwitchComp || c->comptype == LC_Ret) continue;
        if (i != func->comps->size - 1) {
            LComp* next = *(LComp**)get_list(func->comps, i + 1);
            if (lcomp_is_control(next)) continue;
            // print_int debug call
            sb_adds(thgc, sb, "                                                                                                                       call void @print_int(i32 ");
            sb_addi(thgc, sb, lfunc_fn++);
            sb_addc(thgc, sb, ')');
            if (debugLocationId >= 0) output_debugloc(thgc, sb, c->debugLocationId);
            sb_addc(thgc, sb, '\n');
        }
    }
    sb_adds(thgc, sb, "}\n");
}

// ============================================================
// Section D: Dispatcher
// ============================================================
void lcomp_output(ThreadGC* thgc, LComp* comp, SB* sb) {
    if (!comp) return;
    switch (comp->comptype) {
        case LC_Vari:           output_lvari(thgc, (LVari*)comp, sb); break;
        case LC_StrV:           output_lstrv_dec(thgc, (LStrV*)comp, sb); break;
        case LC_Phi:            output_lphi(thgc, (LPhi*)comp, sb); break;
        case LC_TypeDec:        output_ltypedec(thgc, (LTypeDec*)comp, sb); break;
        case LC_Func:           output_lfunc(thgc, (LFunc*)comp, sb); break;
        case LC_Gete:           output_lgete(thgc, (LGete*)comp, sb); break;
        case LC_Cast:           output_lcast(thgc, (LCast*)comp, sb); break;
        case LC_Call:           output_lcall(thgc, (LCall*)comp, sb); break;
        case LC_Alloca:         output_lalloca(thgc, (LAlloca*)comp, sb); break;
        case LC_Store:          output_lstore(thgc, (LStore*)comp, sb); break;
        case LC_Load:           output_lload(thgc, (LLoad*)comp, sb); break;
        case LC_Br:             output_lbr(thgc, (LBr*)comp, sb); break;
        case LC_SwitchComp:     output_lswitchcomp(thgc, (LSwitchComp*)comp, sb); break;
        case LC_BinOp:          output_lbinop(thgc, (LBinOp*)comp, sb); break;
        case LC_Cmp:            output_lcmp(thgc, (LCmp*)comp, sb); break;
        case LC_Lab:            output_llab(thgc, (LLab*)comp, sb); break;
        case LC_LocationMarker: /* marker only, no output */ break;
        case LC_Ret:            output_lret(thgc, (LRet*)comp, sb); break;
        default: break;
    }
}

// ============================================================
// Section E: DebugInfo functions
// ============================================================
int ldbg_create_file(ThreadGC* thgc, LDebugInfo* dbg, String* filename, String* directory) {
    // key = "filename#directory"
    String* key = StringAdd(thgc, filename, (char*)"#", 1, 1);
    key = StringAdd2(thgc, key, directory);
    int existing = ldbg_map_get_int(dbg->fileIds, key);
    if (existing >= 0) return existing;
    int id = dbg->metaId++;
    ldbg_map_set_int(thgc, dbg->fileIds, key, id);
    return id;
}

int ldbg_create_function(ThreadGC* thgc, LDebugInfo* dbg, String* name, int fileId, int line) {
    int id = dbg->metaId++;
    ldbg_map_set_int(thgc, dbg->functionIds, name, id);
    return id;
}

// Forward declaration
int ldbg_get_or_create_type(ThreadGC* thgc, LDebugInfo* dbg, String* llvmType);

// Helper: normalize type key
String* ldbg_normalize(ThreadGC* thgc, String* s) {
    // Trim leading/trailing spaces (simplified: just return as-is for LLVM types)
    return s;
}

int ldbg_create_basic_type(ThreadGC* thgc, LDebugInfo* dbg, String* llvmType) {
    String* key = ldbg_normalize(thgc, llvmType);
    int existing = ldbg_map_get_int(dbg->typeIds, key);
    if (existing >= 0) return existing;
    int id = dbg->metaId++;
    ldbg_map_set_int(thgc, dbg->typeIds, key, id);
    return id;
}

// String helper: check startsWith
bool str_starts_with(String* s, const char* prefix) {
    int plen = (int)strlen(prefix);
    if (!s || s->size < plen) return false;
    if (s->esize == 1) return memcmp(s->data, prefix, plen) == 0;
    // UTF-16
    for (int i = 0; i < plen; i++) {
        if (s->data[i * 2] != prefix[i]) return false;
    }
    return true;
}

bool str_ends_with(String* s, const char* suffix) {
    int slen = (int)strlen(suffix);
    if (!s || s->size < slen) return false;
    if (s->esize == 1) return memcmp(s->data + s->size - slen, suffix, slen) == 0;
    for (int i = 0; i < slen; i++) {
        if (s->data[(s->size - slen + i) * 2] != suffix[i]) return false;
    }
    return true;
}

bool str_eq_c(String* s, const char* c) {
    int clen = (int)strlen(c);
    if (!s || s->size != clen) return false;
    if (s->esize == 1) return memcmp(s->data, c, clen) == 0;
    for (int i = 0; i < clen; i++) {
        if (s->data[i * 2] != c[i]) return false;
    }
    return true;
}

bool str_contains(String* s, const char* sub) {
    int sublen = (int)strlen(sub);
    if (!s || s->size < sublen) return false;
    if (s->esize == 1) {
        for (int i = 0; i <= s->size - sublen; i++) {
            if (memcmp(s->data + i, sub, sublen) == 0) return true;
        }
    }
    return false;
}

// TrimStart('%') equivalent
String* str_trim_start(ThreadGC* thgc, String* s, char c) {
    if (!s || s->size == 0) return s;
    int start = 0;
    if (s->esize == 1) {
        while (start < s->size && s->data[start] == c) start++;
    } else {
        while (start < s->size && s->data[start * 2] == c && s->data[start * 2 + 1] == 0) start++;
    }
    if (start == 0) return s;
    return SubString(thgc, s, start, s->size - start);
}

// TrimEnd('*') equivalent
String* str_trim_end(ThreadGC* thgc, String* s, char c) {
    if (!s || s->size == 0) return s;
    int end = s->size;
    if (s->esize == 1) {
        while (end > 0 && s->data[end - 1] == c) end--;
    } else {
        while (end > 0 && s->data[(end - 1) * 2] == c && s->data[(end - 1) * 2 + 1] == 0) end--;
    }
    if (end == s->size) return s;
    return SubString(thgc, s, 0, end);
}

int ldbg_get_or_create_type(ThreadGC* thgc, LDebugInfo* dbg, String* llvmType) {
    String* typeKey = ldbg_normalize(thgc, llvmType);
    int existing = ldbg_map_get_int(dbg->typeIds, typeKey);
    if (existing >= 0) return existing;

    // Pointer type
    if (str_ends_with(llvmType, "*")) {
        String* baseType = str_trim_end(thgc, llvmType, '*');
        // trim trailing space too
        baseType = str_trim_end(thgc, baseType, ' ');
        int baseTypeId = ldbg_get_or_create_type(thgc, dbg, baseType);
        int ptrId = dbg->metaId++;
        ldbg_map_set_int(thgc, dbg->typeIds, typeKey, ptrId);
        return ptrId;
    }

    // Array type [N x type]
    if (str_starts_with(llvmType, "[")) {
        int arrayId = dbg->metaId++;
        ldbg_map_set_int(thgc, dbg->typeIds, typeKey, arrayId);
        return arrayId;
    }

    // Struct type %TypeName
    if (str_starts_with(llvmType, "%")) {
        String* structName = str_trim_start(thgc, llvmType, '%');
        int sid = ldbg_map_get_int(dbg->structTypeIds, structName);
        if (sid >= 0) return sid;
        int structId = dbg->metaId++;
        ldbg_map_set_int(thgc, dbg->structTypeIds, structName, structId);
        return structId;
    }

    // Basic type
    return ldbg_create_basic_type(thgc, dbg, llvmType);
}

int ldbg_get_type_id(ThreadGC* thgc, LDebugInfo* dbg, String* llvmType) {
    String* stripped = str_trim_end(thgc, llvmType, '*');
    if (str_eq_c(stripped, "i32") || str_eq_c(stripped, "i16") ||
        str_eq_c(stripped, "i8") || str_eq_c(stripped, "i1") ||
        str_eq_c(stripped, "i64") || str_eq_c(stripped, "float") ||
        str_eq_c(stripped, "double") || str_eq_c(stripped, "ptr")) {
        return ldbg_get_or_create_type(thgc, dbg, stripped);
    }
    if (str_starts_with(stripped, "%")) {
        return ldbg_get_or_create_type(thgc, dbg, stripped);
    }
    return ldbg_get_or_create_type(thgc, dbg, createString(thgc, (char*)"ptr", 3, 1));
}

int ldbg_get_pointer_type(ThreadGC* thgc, LDebugInfo* dbg, int baseTypeId) {
    for (int i = 0; i < dbg->ptrBaseIds->size; i++) {
        int bid = *(int*)get_list(dbg->ptrBaseIds, i);
        if (bid == baseTypeId) return *(int*)get_list(dbg->ptrTypeIds, i);
    }
    int ptrTypeId = dbg->metaId++;
    add_list(thgc, dbg->ptrBaseIds, (char*)&baseTypeId);
    add_list(thgc, dbg->ptrTypeIds, (char*)&ptrTypeId);
    return ptrTypeId;
}

int ldbg_create_location(ThreadGC* thgc, LDebugInfo* dbg, LLocationMarker* lm) {
    int id = dbg->metaId++;
    add_list(thgc, dbg->locationKeys, (char*)&id);
    add_list(thgc, dbg->locationVals, (char*)&lm);
    lm->metaid = id;
    return id;
}

void lfunc_register_param(ThreadGC* thgc, LFunc* func, LVari* param, int argNo, int line) {
    String* stripped = str_trim_end(thgc, param->type, '*');
    int typeId = ldbg_get_type_id(thgc, func->llvm->debugInfo, stripped);
    LDebugVariable* dv = (LDebugVariable*)GC_alloc(thgc, _LDebugVariable);
    dv->name = str_trim_start(thgc, param->name, '%');
    dv->typeId = typeId;
    dv->scopeId = func->debugId;
    dv->line = line;
    dv->metaId = func->llvm->debugInfo->metaId++;
    dv->argNo = argNo;
    add_list(thgc, func->debugVariables, (char*)&dv);
}

void lfunc_register_local(ThreadGC* thgc, LFunc* func, LVari* var, int line) {
    String* stripped = str_trim_end(thgc, var->type, '*');
    int typeId = ldbg_get_type_id(thgc, func->llvm->debugInfo, stripped);
    LDebugVariable* dv = (LDebugVariable*)GC_alloc(thgc, _LDebugVariable);
    dv->name = str_trim_start(thgc, var->name, '%');
    dv->typeId = typeId;
    dv->scopeId = func->debugId;
    dv->line = line;
    dv->metaId = func->llvm->debugInfo->metaId++;
    dv->argNo = 0;
    add_list(thgc, func->debugVariables, (char*)&dv);
}

// ============================================================
// Section F: LLVM type size helper
// ============================================================
int llvm_get_type_size(LLLVM* llvm, String* llvmType) {
    if (str_contains(llvmType, "*") || str_eq_c(llvmType, "ptr")) return 64;
    if (str_eq_c(llvmType, "i1")) return 1;
    if (str_eq_c(llvmType, "i8")) return 8;
    if (str_eq_c(llvmType, "i16")) return 16;
    if (str_eq_c(llvmType, "i32")) return 32;
    if (str_eq_c(llvmType, "i64")) return 64;
    if (str_eq_c(llvmType, "float")) return 32;
    if (str_eq_c(llvmType, "double")) return 64;
    // Array type [N x type] - simplified
    if (str_starts_with(llvmType, "[")) {
        // parse count and element type
        // simplified: return 64 as default
        return 64;
    }
    // Struct type
    if (str_starts_with(llvmType, "%")) {
        String* typeName = str_trim_start(NULL, llvmType, '%');
        // Look up in structTypes - simplified
        return 64;
    }
    return 64; // default
}

// ============================================================
// Section F: LLVM debug metadata output
// ============================================================
void llvm_output_debug_metadata(ThreadGC* thgc, LLLVM* llvm, SB* sb) {
    LDebugInfo* dbg = llvm->debugInfo;

    // Module flags
    sb_adds(thgc, sb, "!llvm.dbg.cu = !{!");
    sb_addi(thgc, sb, dbg->compileUnitId);
    sb_adds(thgc, sb, "}\n");
    sb_adds(thgc, sb, "!llvm.module.flags = !{!");
    sb_addi(thgc, sb, dbg->dwarfVersionId);
    sb_adds(thgc, sb, ", !");
    sb_addi(thgc, sb, dbg->debugInfoVersionId);
    sb_adds(thgc, sb, "}\n\n");

    // DWARF Version
    sb_addc(thgc, sb, '!');
    sb_addi(thgc, sb, dbg->dwarfVersionId);
    sb_adds(thgc, sb, " = !{i32 2, !\"Dwarf Version\", i32 4}\n");

    // Debug Info Version
    sb_addc(thgc, sb, '!');
    sb_addi(thgc, sb, dbg->debugInfoVersionId);
    sb_adds(thgc, sb, " = !{i32 2, !\"Debug Info Version\", i32 3}\n\n");

    // DICompileUnit
    int mainFileId = ldbg_create_file(thgc, dbg, llvm->sourceFile, llvm->directory);
    sb_addc(thgc, sb, '!');
    sb_addi(thgc, sb, dbg->compileUnitId);
    sb_adds(thgc, sb, " = distinct !DICompileUnit(\n");
    sb_adds(thgc, sb, "  language: DW_LANG_C99,\n");
    sb_adds(thgc, sb, "  file: !");
    sb_addi(thgc, sb, mainFileId);
    sb_adds(thgc, sb, ",\n");
    sb_adds(thgc, sb, "  producer: \"Cyclon Compiler\",\n");
    sb_adds(thgc, sb, "  isOptimized: false,\n");
    sb_adds(thgc, sb, "  runtimeVersion: 0,\n");
    sb_adds(thgc, sb, "  emissionKind: FullDebug)\n\n");

    // DIFile entries
    for (int i = 0; i < dbg->fileIds->kvs->size; i++) {
        KV* kv = *(KV**)get_list(dbg->fileIds->kvs, i);
        int fid = (int)(intptr_t)(kv->value);
        // Parse filename#directory from key
        String* key = kv->key;
        // Find '#' separator
        int sep = -1;
        for (int j = 0; j < key->size; j++) {
            char c = (key->esize == 1) ? key->data[j] : key->data[j * 2];
            if (c == '#') { sep = j; break; }
        }
        sb_addc(thgc, sb, '!');
        sb_addi(thgc, sb, fid);
        sb_adds(thgc, sb, " = !DIFile(filename: \"");
        if (sep > 0) {
            String* fname = SubString(thgc, key, 0, sep);
            sb_add_str(thgc, sb, fname);
        }
        sb_adds(thgc, sb, "\", directory: \"");
        if (sep >= 0 && sep + 1 < key->size) {
            String* dir = SubString(thgc, key, sep + 1, key->size - sep - 1);
            sb_add_str(thgc, sb, dir);
        }
        sb_adds(thgc, sb, "\")\n");
    }
    sb_addc(thgc, sb, '\n');

    // DISubprogram (function info)
    for (int i = 0; i < dbg->functionIds->kvs->size; i++) {
        KV* kv = *(KV**)get_list(dbg->functionIds->kvs, i);
        int funcId = (int)(intptr_t)(kv->value);
        sb_addc(thgc, sb, '!');
        sb_addi(thgc, sb, funcId);
        sb_adds(thgc, sb, " = distinct !DISubprogram(\n");
        sb_adds(thgc, sb, "  name: \"");
        sb_add_str(thgc, sb, kv->key);
        sb_adds(thgc, sb, "\",\n");
        sb_adds(thgc, sb, "  scope: !");
        sb_addi(thgc, sb, mainFileId);
        sb_adds(thgc, sb, ",\n  file: !");
        sb_addi(thgc, sb, mainFileId);
        sb_adds(thgc, sb, ",\n  line: 1,\n");
        sb_adds(thgc, sb, "  type: !99,\n");
        sb_adds(thgc, sb, "  scopeLine: 1,\n");
        sb_adds(thgc, sb, "  spFlags: DISPFlagDefinition,\n");
        sb_adds(thgc, sb, "  unit: !");
        sb_addi(thgc, sb, dbg->compileUnitId);
        sb_adds(thgc, sb, ")\n");
    }

    // Basic function type
    sb_adds(thgc, sb, "!99 = !DISubroutineType(types: !100)\n");
    sb_adds(thgc, sb, "!100 = !{}\n");
}

void llvm_output_basic_type(ThreadGC* thgc, LLLVM* llvm, String* typeKey, int typeId, SB* sb) {
    // Pointer type
    if (str_contains(typeKey, "*")) {
        String* baseName = str_trim_end(thgc, typeKey, '*');
        baseName = str_trim_end(thgc, baseName, ' ');
        int baseTypeId = ldbg_map_get_int(llvm->debugInfo->typeIds, baseName);
        if (baseTypeId < 0) baseTypeId = ldbg_get_or_create_type(thgc, llvm->debugInfo, baseName);
        sb_addc(thgc, sb, '!');
        sb_addi(thgc, sb, typeId);
        sb_adds(thgc, sb, " = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !");
        sb_addi(thgc, sb, baseTypeId);
        sb_adds(thgc, sb, ", size: 64)\n");
        return;
    }
    // Struct type - skip (output separately)
    if (str_starts_with(typeKey, "%")) return;

    // Basic type
    int size = 0, encoding = 0;
    const char* name = "unknown";
    if (str_eq_c(typeKey, "i1"))     { size = 1; encoding = 2; name = "bool"; }
    else if (str_eq_c(typeKey, "i8"))     { size = 8; encoding = 6; name = "char"; }
    else if (str_eq_c(typeKey, "i16"))    { size = 16; encoding = 5; name = "short"; }
    else if (str_eq_c(typeKey, "i32"))    { size = 32; encoding = 5; name = "int"; }
    else if (str_eq_c(typeKey, "i64"))    { size = 64; encoding = 5; name = "long"; }
    else if (str_eq_c(typeKey, "float"))  { size = 32; encoding = 4; name = "float"; }
    else if (str_eq_c(typeKey, "double")) { size = 64; encoding = 4; name = "double"; }
    else if (str_eq_c(typeKey, "ptr"))    { size = 64; encoding = 7; name = "ptr"; }
    else if (str_eq_c(typeKey, "void"))   { size = 0; encoding = 0; name = "void"; }

    sb_addc(thgc, sb, '!');
    sb_addi(thgc, sb, typeId);
    sb_adds(thgc, sb, " = !DIBasicType(name: \"");
    sb_adds(thgc, sb, name);
    sb_adds(thgc, sb, "\", size: ");
    sb_addi(thgc, sb, size);
    sb_adds(thgc, sb, ", encoding: ");
    sb_addi(thgc, sb, encoding);
    sb_adds(thgc, sb, ")\n");
}

void llvm_output_struct_type(ThreadGC* thgc, LLLVM* llvm, String* structName, int structId, SB* sb) {
    char* val = get_mapy(llvm->debugInfo->structTypes, structName);
    if (!val) return;
    LStructTypeInfo* info = (LStructTypeInfo*)val;

    // Compute total size
    int totalSize = 0;
    for (int i = 0; i < info->fields->size; i++) {
        LStructField* f = *(LStructField**)get_list(info->fields, i);
        totalSize += f->sizeInBits;
    }

    // DICompositeType
    sb_addc(thgc, sb, '!');
    sb_addi(thgc, sb, structId);
    sb_adds(thgc, sb, " = distinct !DICompositeType(\n");
    sb_adds(thgc, sb, "  tag: DW_TAG_structure_type,\n");
    sb_adds(thgc, sb, "  name: \"");
    sb_add_str(thgc, sb, structName);
    sb_adds(thgc, sb, "\",\n  file: !");
    sb_addi(thgc, sb, info->fileId);
    sb_adds(thgc, sb, ",\n  line: 1,\n  size: ");
    sb_addi(thgc, sb, totalSize);
    sb_adds(thgc, sb, ",\n  elements: !{");
    for (int i = 0; i < info->fields->size; i++) {
        if (i > 0) sb_adds(thgc, sb, ", ");
        LStructField* f = *(LStructField**)get_list(info->fields, i);
        sb_addc(thgc, sb, '!');
        sb_addi(thgc, sb, f->metaId);
    }
    sb_adds(thgc, sb, "})\n");

    // Each field
    for (int i = 0; i < info->fields->size; i++) {
        LStructField* f = *(LStructField**)get_list(info->fields, i);
        int fieldTypeId = ldbg_get_or_create_type(thgc, llvm->debugInfo, f->llvmType);
        sb_addc(thgc, sb, '!');
        sb_addi(thgc, sb, f->metaId);
        sb_adds(thgc, sb, " = !DIDerivedType(\n");
        sb_adds(thgc, sb, "  tag: DW_TAG_member,\n");
        sb_adds(thgc, sb, "  name: \"");
        sb_add_str(thgc, sb, f->name);
        sb_adds(thgc, sb, "\",\n  scope: !");
        sb_addi(thgc, sb, structId);
        sb_adds(thgc, sb, ",\n  file: !");
        sb_addi(thgc, sb, info->fileId);
        sb_adds(thgc, sb, ",\n  line: 1,\n  baseType: !");
        sb_addi(thgc, sb, fieldTypeId);
        sb_adds(thgc, sb, ",\n  size: ");
        sb_addi(thgc, sb, f->sizeInBits);
        sb_adds(thgc, sb, ",\n  offset: ");
        sb_addi(thgc, sb, f->offsetInBits);
        sb_adds(thgc, sb, ")\n");
    }
}

void llvm_output_local_variable(ThreadGC* thgc, LLLVM* llvm, LDebugVariable* dv, SB* sb) {
    sb_addc(thgc, sb, '!');
    sb_addi(thgc, sb, dv->metaId);
    sb_adds(thgc, sb, " = !DILocalVariable(name: \"");
    sb_add_str(thgc, sb, dv->name);
    sb_adds(thgc, sb, "\", ");
    if (dv->argNo > 0) {
        sb_adds(thgc, sb, "arg: ");
        sb_addi(thgc, sb, dv->argNo);
        sb_adds(thgc, sb, ", ");
    }
    sb_adds(thgc, sb, "scope: !");
    sb_addi(thgc, sb, dv->scopeId);
    sb_adds(thgc, sb, ", ");
    // file: first file
    if (llvm->debugInfo->fileIds->kvs->size > 0) {
        KV* firstFile = *(KV**)get_list(llvm->debugInfo->fileIds->kvs, 0);
        sb_adds(thgc, sb, "file: !");
        sb_addi(thgc, sb, (int)(intptr_t)(firstFile->value));
        sb_adds(thgc, sb, ", ");
    }
    sb_adds(thgc, sb, "line: ");
    sb_addi(thgc, sb, dv->line);
    sb_adds(thgc, sb, ", type: !");
    sb_addi(thgc, sb, dv->typeId);
    sb_addc(thgc, sb, ')');
    sb_addc(thgc, sb, '\n');
}

void llvm_output_debug_metadata2(ThreadGC* thgc, LLLVM* llvm, SB* sb) {
    LDebugInfo* dbg = llvm->debugInfo;

    // Struct types
    for (int i = 0; i < dbg->structTypeIds->kvs->size; i++) {
        KV* kv = *(KV**)get_list(dbg->structTypeIds->kvs, i);
        llvm_output_struct_type(thgc, llvm, kv->key, (int)(intptr_t)(kv->value), sb);
    }

    // Pointer types
    for (int i = 0; i < dbg->ptrBaseIds->size; i++) {
        int baseId = *(int*)get_list(dbg->ptrBaseIds, i);
        int ptrId = *(int*)get_list(dbg->ptrTypeIds, i);
        sb_addc(thgc, sb, '!');
        sb_addi(thgc, sb, ptrId);
        sb_adds(thgc, sb, " = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !");
        sb_addi(thgc, sb, baseId);
        sb_adds(thgc, sb, ", size: 64)\n");
    }

    // Type info
    for (int i = 0; i < dbg->typeIds->kvs->size; i++) {
        KV* kv = *(KV**)get_list(dbg->typeIds->kvs, i);
        llvm_output_basic_type(thgc, llvm, kv->key, (int)(intptr_t)(kv->value), sb);
    }
    sb_addc(thgc, sb, '\n');

    // Function debug variables
    for (int i = 0; i < llvm->funcs->size; i++) {
        LFunc* func = *(LFunc**)get_list(llvm->funcs, i);
        for (int j = 0; j < func->debugVariables->size; j++) {
            LDebugVariable* dv = *(LDebugVariable**)get_list(func->debugVariables, j);
            llvm_output_local_variable(thgc, llvm, dv, sb);
        }
    }

    // Locations
    for (int i = 0; i < dbg->locationKeys->size; i++) {
        int locId = *(int*)get_list(dbg->locationKeys, i);
        LLocationMarker* lm = *(LLocationMarker**)get_list(dbg->locationVals, i);
        sb_addc(thgc, sb, '!');
        sb_addi(thgc, sb, locId);
        sb_adds(thgc, sb, " = !DILocation(line: ");
        sb_addi(thgc, sb, lm->line);
        sb_adds(thgc, sb, ", column: ");
        sb_addi(thgc, sb, lm->column);
        sb_adds(thgc, sb, ", scope: !");
        sb_addi(thgc, sb, lm->scopeId);
        sb_adds(thgc, sb, ")\n");
    }
}

// ============================================================
// Section F: Main LLVM output (IR preamble + all content)
// ============================================================
void llvm_output(ThreadGC* thgc, LLLVM* llvm, SB* sb) {
    // Preamble
    sb_adds(thgc, sb, "source_filename = \"foo.ll\"\n");
    sb_adds(thgc, sb, "target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n");
    sb_adds(thgc, sb, "target triple = \"x86_64-pc-windows-msvc19.43.34808\"\n");
    sb_adds(thgc, sb, "@.strnum = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\", align 1\n");
    sb_adds(thgc, sb, "@.str.conv1 = private unnamed_addr constant [6 x i8] c\"conv1\\00\", align 1\n");
    sb_adds(thgc, sb, "@.str.relu1 = private unnamed_addr constant [6 x i8] c\"relu1\\00\", align 1\n");
    sb_adds(thgc, sb, "@.str.flat1 = private unnamed_addr constant [6 x i8] c\"flat1\\00\", align 1\n");
    sb_adds(thgc, sb, "@.str.line1 = private unnamed_addr constant [6 x i8] c\"line1\\00\", align 1\n");

    // String constants
    for (int i = 0; i < llvm->strs->size; i++) {
        LStrV* sv = *(LStrV**)get_list(llvm->strs, i);
        output_lstrv_dec(thgc, sv, sb);
    }

    // Declarations
    sb_adds(thgc, sb, "declare void @llvm.dbg.declare(metadata, metadata, metadata)\n");
    sb_adds(thgc, sb, "declare void @llvm.dbg.value(metadata, metadata, metadata)\n");
    sb_adds(thgc, sb, "declare ptr @calloc(i64, i64)\n");
    sb_adds(thgc, sb, "declare ptr @malloc(i64)\n");
    sb_adds(thgc, sb, "declare void @free(ptr)\n");
    sb_adds(thgc, sb, "declare i32 @printf(i8*, ...)\n");
    sb_adds(thgc, sb, "declare i32 @putchar(i32)\n");
    sb_adds(thgc, sb, "declare i32 @fflush(i8 *)\n");

    // Type declarations
    for (int i = 0; i < llvm->types->size; i++) {
        LTypeDec* td = *(LTypeDec**)get_list(llvm->types, i);
        output_ltypedec(thgc, td, sb);
    }

    // Global variables and type aliases (large preamble from C# LLVM.output())
    sb_adds(thgc, sb, "@cnp = dllexport global i32 0\n");
    sb_adds(thgc, sb, "@cnl = dllexport global i32 ");
    sb_addi(thgc, sb, llvm->cn);
    sb_addc(thgc, sb, '\n');
    sb_adds(thgc, sb, "%SDL_Color = type {i8, i8, i8, i8}\n");
    sb_adds(thgc, sb, "%MarginType = type {i32, i32, i32, i32}\n");
    sb_adds(thgc, sb, "%PointType = type {i32, i32}\n");
    sb_adds(thgc, sb, "%ElementType = type {ptr, ptr, ptr, ptr, %MarginType, %MarginType, %MarginType, %PointType, %MarginType, %MarginType, i1, ptr, i1, i1, i1, %SDL_Color, %SDL_Color, i32, i32, i32, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr}\n");
    sb_adds(thgc, sb, "%CoroFrameType = type {ptr, ptr, ptr, ptr, ptr, ptr, ptr, i32, i8*, i8*}\n");
    sb_adds(thgc, sb, "%ThreadGCType = type {i8*, i8*}\n");
    sb_adds(thgc, sb, "%torch.nn.Conv2dImpl = type opaque\n");
    sb_adds(thgc, sb, "%CreateConv2dType = type %torch.nn.Conv2dImpl* (i64, i64, i64)*;\n");
    sb_adds(thgc, sb, "%GetInChannelsType = type i64 (%torch.nn.Conv2dImpl*)*;\n");
    sb_adds(thgc, sb, "%GetWeightDataType = type float* (%torch.nn.Conv2dImpl*)*;\n");
    sb_adds(thgc, sb, "%ForwardConv2dType = type i8* (%torch.nn.Conv2dImpl*, i8*)*;\n");
    sb_adds(thgc, sb, "@CreateConv2d = dllexport global %CreateConv2dType null\n");
    sb_adds(thgc, sb, "@GetInChannels = dllexport global %GetInChannelsType null\n");
    sb_adds(thgc, sb, "@GetOutChannels = dllexport global %GetInChannelsType null\n");
    sb_adds(thgc, sb, "@GetKernelSize = dllexport global %GetInChannelsType null\n");
    sb_adds(thgc, sb, "@GetWeightData = dllexport global %GetWeightDataType null\n");
    sb_adds(thgc, sb, "@GetWeightSize = dllexport global %GetInChannelsType null\n");
    sb_adds(thgc, sb, "@ForwardConv2d = dllexport global %ForwardConv2dType null\n");
    sb_adds(thgc, sb, "%torch.nn.MaxPool2dImpl = type opaque\n");
    sb_adds(thgc, sb, "%CreateMaxPool2dType = type %torch.nn.MaxPool2dImpl* (i64, i64, i64)*\n");
    sb_adds(thgc, sb, "%GetMaxPoolKernelSizeType = type i64 (%torch.nn.MaxPool2dImpl*)*\n");
    sb_adds(thgc, sb, "%ForwardMaxPool2dType = type i8* (%torch.nn.MaxPool2dImpl*, i8*)*\n");
    sb_adds(thgc, sb, "@CreateMaxPool2d = dllexport global %CreateMaxPool2dType null\n");
    sb_adds(thgc, sb, "@GetMaxPoolKernelSize = dllexport global %GetMaxPoolKernelSizeType null\n");
    sb_adds(thgc, sb, "@GetMaxPoolStride = dllexport global %GetMaxPoolKernelSizeType null\n");
    sb_adds(thgc, sb, "@GetMaxPoolPadding = dllexport global %GetMaxPoolKernelSizeType null\n");
    sb_adds(thgc, sb, "@ForwardMaxPool2d = dllexport global %ForwardMaxPool2dType null\n");
    sb_adds(thgc, sb, "%torch.nn.ReLUImpl = type opaque\n");
    sb_adds(thgc, sb, "%CreateReLUType = type %torch.nn.ReLUImpl* (i1)*\n");
    sb_adds(thgc, sb, "%GetReLUInplaceType = type i1 (%torch.nn.ReLUImpl*)*\n");
    sb_adds(thgc, sb, "%ForwardReLUType = type i8* (%torch.nn.ReLUImpl*, i8*)*\n");
    sb_adds(thgc, sb, "@CreateReLU = dllexport global %CreateReLUType null\n");
    sb_adds(thgc, sb, "@GetReLUInplace = dllexport global %GetReLUInplaceType null\n");
    sb_adds(thgc, sb, "@ForwardReLU = dllexport global %ForwardReLUType null\n");
    sb_adds(thgc, sb, "%torch.nn.FlattenImpl = type opaque\n");
    sb_adds(thgc, sb, "%CreateFlattenType = type %torch.nn.FlattenImpl* (i64, i64)*\n");
    sb_adds(thgc, sb, "%GetFlattenStartDimType = type i64 (%torch.nn.FlattenImpl*)*\n");
    sb_adds(thgc, sb, "%ForwardFlattenType = type i8* (%torch.nn.FlattenImpl*, i8*)*\n");
    sb_adds(thgc, sb, "@CreateFlatten = dllexport global %CreateFlattenType null\n");
    sb_adds(thgc, sb, "@GetFlattenStartDim = dllexport global %GetFlattenStartDimType null\n");
    sb_adds(thgc, sb, "@GetFlattenEndDim = dllexport global %GetFlattenStartDimType null\n");
    sb_adds(thgc, sb, "@ForwardFlatten = dllexport global %ForwardFlattenType null\n");
    sb_adds(thgc, sb, "%torch.nn.LinearImpl = type opaque\n");
    sb_adds(thgc, sb, "%CreateLinearType = type %torch.nn.LinearImpl* (i64, i64, i1)*\n");
    sb_adds(thgc, sb, "%GetLinearInFeaturesType = type i64 (%torch.nn.LinearImpl*)*\n");
    sb_adds(thgc, sb, "%GetLinearWeightDataType = type float* (%torch.nn.LinearImpl*)*\n");
    sb_adds(thgc, sb, "%ForwardLinearType = type i8* (%torch.nn.LinearImpl*, i8*)*\n");
    sb_adds(thgc, sb, "@CreateLinear = dllexport global %CreateLinearType null\n");
    sb_adds(thgc, sb, "@GetLinearInFeatures = dllexport global %GetLinearInFeaturesType null\n");
    sb_adds(thgc, sb, "@GetLinearOutFeatures = dllexport global %GetLinearInFeaturesType null\n");
    sb_adds(thgc, sb, "@GetLinearBias = dllexport global i1 (%torch.nn.LinearImpl*)* null\n");
    sb_adds(thgc, sb, "@GetLinearWeightData = dllexport global %GetLinearWeightDataType null\n");
    sb_adds(thgc, sb, "@GetLinearWeightSize = dllexport global %GetLinearInFeaturesType null\n");
    sb_adds(thgc, sb, "@GetLinearBiasData = dllexport global %GetLinearWeightDataType null\n");
    sb_adds(thgc, sb, "@GetLinearBiasSize = dllexport global %GetLinearInFeaturesType null\n");
    sb_adds(thgc, sb, "@ForwardLinear = dllexport global %ForwardLinearType null\n");
    sb_adds(thgc, sb, "%torch.nn.SequentialImpl = type opaque\n");
    sb_adds(thgc, sb, "%CreateSequentialType = type %torch.nn.SequentialImpl* ()*\n");
    sb_adds(thgc, sb, "%AddModuleToSequentialType = type void (%torch.nn.SequentialImpl*, i8*, i8*)*\n");
    sb_adds(thgc, sb, "%GetSequentialSizeType = type i64 (%torch.nn.SequentialImpl*)*\n");
    sb_adds(thgc, sb, "%GetSequentialModuleType = type i8* (%torch.nn.SequentialImpl*, i64)*\n");
    sb_adds(thgc, sb, "%ForwardSequentialType = type i8* (%torch.nn.SequentialImpl*, i8*)*\n");
    sb_adds(thgc, sb, "@CreateSequential = dllexport global %CreateSequentialType null\n");
    sb_adds(thgc, sb, "@AddModuleToSequential = dllexport global %AddModuleToSequentialType null\n");
    sb_adds(thgc, sb, "@GetSequentialSize = dllexport global %GetSequentialSizeType null\n");
    sb_adds(thgc, sb, "@GetSequentialModule = dllexport global %GetSequentialModuleType null\n");
    sb_adds(thgc, sb, "@ForwardSequential = dllexport global %ForwardSequentialType null\n");
    sb_adds(thgc, sb, "%torch.nn.ModuleImpl = type opaque\n");
    sb_adds(thgc, sb, "%RegisterModuleType = type void (%torch.nn.ModuleImpl*, i8*, %torch.nn.ModuleImpl*)*\n");
    sb_adds(thgc, sb, "@RegisterModule = dllexport global %RegisterModuleType null\n");
    sb_adds(thgc, sb, "%ForwardType = type i8*(i8*, i8*)*\n");
    sb_adds(thgc, sb, "%CreateTorchModuleType = type %torch.nn.ModuleImpl* (i8*, %ForwardType)*\n");
    sb_adds(thgc, sb, "@CreateTorchModule = dllexport global %CreateTorchModuleType null\n");
    sb_adds(thgc, sb, "%GCObject = type {ptr}\n");
    sb_adds(thgc, sb, "%GCModel = type {ptr, i32}\n");
    sb_adds(thgc, sb, "%GCObjectPtr = type %GCObject*\n");
    sb_adds(thgc, sb, "%GCCheckFuncType = type void (i8*)*\n");
    sb_adds(thgc, sb, "%GCFinalizeFuncType = type void (i8*)*\n");
    sb_adds(thgc, sb, "%GC_AddClassType = type i32 (%ThreadGCType*, i32, i32, i8*, %GCCheckFuncType, %GCFinalizeFuncType)*\n");
    sb_adds(thgc, sb, "%RootNodeType = type {ptr, i32, ptr, ptr, ptr}\n");
    sb_adds(thgc, sb, "%GC_AddRootType = type %RootNodeType* (%ThreadGCType*)*\n");
    sb_adds(thgc, sb, "%GC_ReleaseRootType = type void ( %RootNodeType*)*\n");
    sb_adds(thgc, sb, "%GC_SetRootType = type void (%RootNodeType*, i8**)*\n");
    sb_adds(thgc, sb, "%GC_BackRootType = type void (%RootNodeType*, i32)*\n");
    sb_adds(thgc, sb, "%GC_mallocType = type %GCObjectPtr (%ThreadGCType*, i32)*\n");
    sb_adds(thgc, sb, "@GC_AddClass = dllexport global %GC_AddClassType null\n");
    sb_adds(thgc, sb, "@GC_AddRoot = dllexport global %GC_AddRootType null\n");
    sb_adds(thgc, sb, "@GC_ReleaseRoot = dllexport global %GC_ReleaseRootType null\n");
    sb_adds(thgc, sb, "@GC_SetRoot = dllexport global %GC_SetRootType null\n");
    sb_adds(thgc, sb, "@GC_BackRoot = dllexport global %GC_BackRootType null\n");
    sb_adds(thgc, sb, "@GC_malloc = dllexport global %GC_mallocType null\n");
    sb_adds(thgc, sb, "%TestType = type { %torch.nn.Conv2dImpl*, %torch.nn.ReLUImpl*, %torch.nn.FlattenImpl*, %torch.nn.LinearImpl*}\n");
    sb_adds(thgc, sb, "%torch.optim.Adam = type opaque\n");
    sb_adds(thgc, sb, "%AdamOptions = type opaque\n");
    sb_adds(thgc, sb, "%CreateAdamType = type %torch.optim.Adam* (%torch.nn.ModuleImpl**, i64, %AdamOptions*)*\n");
    sb_adds(thgc, sb, "@CreateAdam = dllexport global %CreateAdamType null\n");
    sb_adds(thgc, sb, "%CreateAdamOptionsType = type %AdamOptions* (float)*\n");
    sb_adds(thgc, sb, "@CreateAdamOptions = dllexport global %CreateAdamOptionsType null\n");
    sb_adds(thgc, sb, "%OptimizerZeroGradType = type void (%torch.optim.Adam*)*\n");
    sb_adds(thgc, sb, "@OptimizerZeroGrad = dllexport global %OptimizerZeroGradType null\n");
    sb_adds(thgc, sb, "%TensorBackwardType = type void (i8*)*\n");
    sb_adds(thgc, sb, "@TensorBackward = dllexport global %TensorBackwardType null\n");
    sb_adds(thgc, sb, "%OptimizerStepType = type void (%torch.optim.Adam*)*\n");
    sb_adds(thgc, sb, "@OptimizerStep = dllexport global %OptimizerStepType null\n");
    sb_adds(thgc, sb, "%CrossEntropyType = type i8* (i8*, i8*)*\n");
    sb_adds(thgc, sb, "@CrossEntropy = dllexport global %CrossEntropyType null\n");
    sb_adds(thgc, sb, "%TensorToCUDAType = type i8* (i8*)*\n");
    sb_adds(thgc, sb, "@TensorToCUDA = dllexport global %TensorToCUDAType null\n");
    sb_adds(thgc, sb, "%TensorRequiresGradType = type i8* (i8*, i1)*\n");
    sb_adds(thgc, sb, "@TensorRequiresGrad = dllexport global %TensorRequiresGradType null\n");
    sb_adds(thgc, sb, "%StringType = type {i8**, i32, i32}\n");
    sb_adds(thgc, sb, "%NumberStringType = type %StringType* (%ThreadGCType*, i32)*\n");
    sb_adds(thgc, sb, "@NumberString = dllexport global %NumberStringType null\n");
    sb_adds(thgc, sb, "%CreateStringType = type %StringType* (%ThreadGCType*, i8*, i32, i32)*\n");
    sb_adds(thgc, sb, "@CreateString = dllexport global %CreateStringType null\n");
    sb_adds(thgc, sb, "%AddStringType = type %StringType* (%ThreadGCType*, %StringType*, i8*, i32, i32)*\n");
    sb_adds(thgc, sb, "@AddString = dllexport global %AddStringType null\n");
    sb_adds(thgc, sb, "%AddStringType2 = type %StringType* (%ThreadGCType*, %StringType*, %StringType*)*\n");
    sb_adds(thgc, sb, "@AddString2 = dllexport global %AddStringType2 null\n");
    sb_adds(thgc, sb, "%RemoveStringType = type %StringType* (%ThreadGCType*, %StringType*, i32)*\n");
    sb_adds(thgc, sb, "@RemoveString = dllexport global %RemoveStringType null\n");
    sb_adds(thgc, sb, "%SubStringType = type %StringType* (%ThreadGCType*, %StringType*, i32, i32)*\n");
    sb_adds(thgc, sb, "@SubString = dllexport global %SubStringType null\n");
    sb_adds(thgc, sb, "%GetCharType = type i16 (%StringType*, i32)*\n");
    sb_adds(thgc, sb, "@GetChar = dllexport global %GetCharType null\n");
    sb_adds(thgc, sb, "%PrintStringType = type void (%StringType*)*\n");
    sb_adds(thgc, sb, "@PrintString = dllexport global %PrintStringType null\n");
    sb_adds(thgc, sb, "%CopyObjectType = type i8* (%ThreadGCType*, i8*)*\n");
    sb_adds(thgc, sb, "@CopyObject = dllexport global %CopyObjectType null\n");
    sb_adds(thgc, sb, "@CloneObject = dllexport global %CopyObjectType null\n");
    sb_adds(thgc, sb, "@thgcp = dllexport global %ThreadGCType * null\n");
    sb_adds(thgc, sb, "%GetHashValueType = type %StringType* (%StringType*, i32)*\n");
    sb_adds(thgc, sb, "@GetHashValue = dllexport global %GetHashValueType null\n");
    sb_adds(thgc, sb, "%FuncType = type {i8*, i8*, i8*, i8*}\n");
    sb_adds(thgc, sb, "%MemFuncType = type {i8*, i8*, i8*}\n");
    sb_adds(thgc, sb, "%ListType = type {i8**, i32, i32, i32, i1}\n");
    sb_adds(thgc, sb, "%CreateListType = type %ListType* (%ThreadGCType*, i32, i1)*\n");
    sb_adds(thgc, sb, "@CreateList = dllexport global %CreateListType null\n");
    sb_adds(thgc, sb, "%AddListType = type void (%ThreadGCType*, %ListType*, i8*)*\n");
    sb_adds(thgc, sb, "@AddList = dllexport global %AddListType null\n");
    sb_adds(thgc, sb, "%GetListType = type i8** (%ListType*, i32)*\n");
    sb_adds(thgc, sb, "@GetList = dllexport global %GetListType null\n");
    sb_adds(thgc, sb, "%GetLastType = type i8** (%ListType*)*\n");
    sb_adds(thgc, sb, "@GetLast = dllexport global %GetLastType null\n");
    sb_adds(thgc, sb, "%SetListType = type void (%ListType*, i32, i8*)*\n");
    sb_adds(thgc, sb, "@SetList = dllexport global %SetListType null\n");
    sb_adds(thgc, sb, "%SetLastType = type void (%ListType*, i8*)*\n");
    sb_adds(thgc, sb, "@SetLast = dllexport global %SetLastType null\n");
    sb_adds(thgc, sb, "%KeyValueType = type {i32, %StringType*, i8*, i1}\n");
    sb_adds(thgc, sb, "%HashType = type {%ListType*, i8*, i1}\n");
    sb_adds(thgc, sb, "%CreateHashType = type %HashType* (%ThreadGCType*, i1)*\n");
    sb_adds(thgc, sb, "@CreateHash = dllexport global %CreateHashType null\n");
    sb_adds(thgc, sb, "%AddHashType = type void (%ThreadGCType*, %HashType*, %StringType*, i8*)*\n");
    sb_adds(thgc, sb, "@AddHash = dllexport global %AddHashType null\n");
    sb_adds(thgc, sb, "%GetHashType = type i8* (%HashType*, %StringType*)*\n");
    sb_adds(thgc, sb, "@GetHash = dllexport global %GetHashType null\n");
    sb_adds(thgc, sb, "%PrintArrayType = type void (%HashType*, i32, i32)*\n");
    sb_adds(thgc, sb, "@PrintArray = dllexport global %PrintArrayType null\n");
    sb_adds(thgc, sb, "%DbInitType = type ptr (%ThreadGCType*, %StringType*, i32, i32, i32, %StringType)*\n");
    sb_adds(thgc, sb, "@DbInit = dllexport global %DbInitType null\n");
    sb_adds(thgc, sb, "%CoroutineQueueType = type {i8*}\n");
    sb_adds(thgc, sb, "%PushQueueType = type void (%CoroutineQueueType*, i8*)*\n");
    sb_adds(thgc, sb, "@PushQueue = dllexport global %PushQueueType null\n");
    sb_adds(thgc, sb, "%WaitHandleType = type i8* (i8*)*\n");
    sb_adds(thgc, sb, "@WaitHandle = dllexport global %WaitHandleType null\n");
    sb_adds(thgc, sb, "@sqlp = dllexport global ptr null\n");
    sb_adds(thgc, sb, "%BeginTransactionType = type void (ptr, %ThreadGCType*, ptr, %CoroutineQueueType*)*\n");
    sb_adds(thgc, sb, "@BeginTransaction = dllexport global %BeginTransactionType null\n");
    sb_adds(thgc, sb, "%StringUTF8Type = type i8* (%StringType*, i32*)*\n");
    sb_adds(thgc, sb, "@StringUTF8 = dllexport global %StringUTF8Type null\n");
    sb_adds(thgc, sb, "%TextType = type { i8*, i32 }\n");
    sb_adds(thgc, sb, "%SqlParamType = type {i32, i32, [16 x i8]}\n");
    sb_adds(thgc, sb, "%ExecSqlType = type void (ptr, i8*, %StringType, %SqlParamType*, i32, ptr, ptr)*\n");
    sb_adds(thgc, sb, "@ExecSql = dllexport global %ExecSqlType null\n");
    sb_adds(thgc, sb, "%TxFinishType = type void (ptr, i8*)*\n");
    sb_adds(thgc, sb, "@TxCommit = dllexport global %TxFinishType null\n");
    sb_adds(thgc, sb, "@TxRollback = dllexport global %TxFinishType null\n");
    sb_adds(thgc, sb, "%SQP = type {%StringType*, %SqlParamType*}\n");
    sb_adds(thgc, sb, "%Triptr = type {%StringType*, %SqlParamType*, ptr}\n");
    sb_adds(thgc, sb, "%Vector = type {ptr, ptr, ptr}\n");
    sb_adds(thgc, sb, "%RowSet = type {%ListType*, %ListType*, i64, i32, i8}\n");
    sb_adds(thgc, sb, "%SqlIntType = type i32 (i8*, i32)*\n");
    sb_adds(thgc, sb, "%SqlStringType = type i8* (i8*, i32)*\n");
    sb_adds(thgc, sb, "@SqlInt = dllexport global %SqlIntType null\n");
    sb_adds(thgc, sb, "@SqlString = dllexport global %SqlStringType null\n");
    sb_adds(thgc, sb, "@SqlLength = dllexport global %SqlIntType null\n");
    sb_adds(thgc, sb, "@CreateStringANT = dllexport global %CreateStringType null\n");
    sb_adds(thgc, sb, "@CloneObjectANT = dllexport global %CopyObjectType null\n");
    sb_adds(thgc, sb, "%GCANTType = type void(%ThreadGCType*, %CoroutineQueueType*, i8*)*\n");
    sb_adds(thgc, sb, "@GCANT = dllexport global %GCANTType null\n");
    sb_adds(thgc, sb, "%CreateDivType = type %ElementType* (%ThreadGCType*)*\n");
    sb_adds(thgc, sb, "@CreateDiv = dllexport global %CreateDivType null\n");
    sb_adds(thgc, sb, "@CreateSheet = dllexport global %CreateDivType null\n");
    sb_adds(thgc, sb, "%TreeElementType = type { %StringType*, %ElementType*, %TreeElementType*, %ListType*, i1}\n");
    sb_adds(thgc, sb, "%CheckTreeElementType = type %TreeElementType* (%ThreadGCType*, %TreeElementType*, %ElementType*, %ListType*, %StringType*, i32, i32, ptr)*\n");
    sb_adds(thgc, sb, "@CheckTreeElement = dllexport global %CheckTreeElementType null\n");
    sb_adds(thgc, sb, "%ElementAddType = type %ElementType* (%ThreadGCType*, %ElementType*, %ElementType*)*\n");
    sb_adds(thgc, sb, "@ElementAdd = dllexport global %ElementAddType null\n");
    sb_adds(thgc, sb, "%CreateLetterType = type void (%ThreadGCType*, %ElementType*, %StringType*)*\n");
    sb_adds(thgc, sb, "@CreateLetter = dllexport global %CreateLetterType null\n");
    sb_adds(thgc, sb, "%CreateKaigyouType = type void (%ThreadGCType*, %ElementType*)*\n");
    sb_adds(thgc, sb, "@CreateKaigyou = dllexport global %CreateKaigyouType null\n");
    sb_adds(thgc, sb, "%CreateEndType = type void (%ThreadGCType*, %ElementType*)*\n");
    sb_adds(thgc, sb, "@CreateEnd = dllexport global %CreateEndType null\n");
    sb_adds(thgc, sb, "%MakeFrameType = type ptr (%RootNodeType*, i8*, i8*)*\n");
    sb_adds(thgc, sb, "@MakeFrame = dllexport global %MakeFrameType null\n");
    sb_adds(thgc, sb, "@client = dllexport global ptr null\n");
    sb_adds(thgc, sb, "%CheckType = type void(ptr)*\n");
    sb_adds(thgc, sb, "@Check = dllexport global %CheckType null\n");

    // print_int function
    sb_adds(thgc, sb, "define void @print_int(i32 %n) {\nentry:\n");
    sb_adds(thgc, sb, "  %fmt_ptr = getelementptr [4 x i8], [4 x i8]* @.strnum, i32 0, i32 0\n");
    sb_adds(thgc, sb, "  call i32 (i8*, ...) @printf(i8* %fmt_ptr, i32 %n)\n");
    sb_adds(thgc, sb, "  call i32 @fflush(i8 * null)\n");
    sb_adds(thgc, sb, "  ret void\n}\n");

    // Output all components (functions)
    for (int i = 0; i < llvm->comps->size; i++) {
        LComp* comp = *(LComp**)get_list(llvm->comps, i);
        lcomp_output(thgc, comp, sb);
    }

    // Debug metadata
    llvm_output_debug_metadata(thgc, llvm, sb);
    llvm_output_debug_metadata2(thgc, llvm, sb);
}

// ============================================================
// Section G: Compile function
// ============================================================
void llvm_compile(ThreadGC* thgc, LLLVM* llvm, const char* name) {
    char path[512];
    snprintf(path, sizeof(path), "clang3/%s.ll", name);
    FILE* f = fopen(path, "w");
    if (!f) return;
    SB* sb = sb_create(thgc);
    llvm_output(thgc, llvm, sb);
    sb_write(sb, f);
    fclose(f);

    // Run clang to produce DLL
    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
        "clang3/clang++ -std=c++20 -g -gdwarf-4 -O0 -fuse-ld=lld -shared "
        "%s.ll -o %s.dll -Wl,/IMPLIB:%s",
        name, name, name);
    // Note: working directory should be clang3/
    // This is simplified - the full C# version also builds main.exe
    system(cmd);
}

// ============================================================
// Section H: Bitcast.Cast utility
// ============================================================
LVari* lcast_cast(ThreadGC* thgc, Master* local, Obj* t1, Obj* t2, LVari* vari) {
    if (t1 == t2) return vari;
    // Simplified type casting between int/short/bool
    // Full implementation would need Type* comparisons
    return vari;
}
