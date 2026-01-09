#pragma once
struct MemTable {
	Map* table;
};
struct ModelVal {
	char* blk;
	int id;
};
RowSet* MemInsert(ThreadGC* thgc, MemTable* mt, enum CType type, ModelVal* val) {
	RowSet* rs = (RowSet*)malloc(sizeof(RowSet));
	if (val->id == 0) {
		val->id = thgc->staticid = (thgc->staticid % 1000000) - 1;
	}
	Map* t = (Map*)get_mapy(mt->table, thgc->class_table[type].name);
	if (t == NULL) {
		t = create_mapy(thgc, true);
		add_mapy(thgc, mt->table, thgc->class_table[type].name, (char*)t);
	}
	add_mapy(thgc, t, NumberString(thgc, val->id), (char*)val);
	rs->last_rowid = val->id;
	return rs;
}
RowSet* MemSelect(ThreadGC* thgc, MemTable* mt, enum CType type, char* (*func)(ModelVal*)) {
	RowSet* rs = (RowSet*)malloc(sizeof(RowSet));
	Map* t = (Map*)get_mapy(mt->table, thgc->class_table[type].name);
	if (t == NULL) {
		t = create_mapy(thgc, true);
		add_mapy(thgc, mt->table, thgc->class_table[type].name, (char*)t);
	}
	rs->rows = create_mapy(thgc, true);
	for (int i = 0; i < t->kvs->size; i++) {
		KV* kv = (KV*)*get_list2(t->kvs, i);
		ModelVal* val = (ModelVal*)kv->value;
		char* o = func(val);
		add_mapy(thgc, rs->rows, kv->key, o);
	}
	rs->has_data = true;
	return rs;
}
String* MemString(ThreadGC* thgc, MemTable* mt) {
	for (int i = 0; i < mt->table->kvs->size; i++) {
		KV* kv = (KV*)*get_list2(mt->table->kvs, i);
		Map* t = (Map*)kv->value;
		//model kv->key le\n
		for (int j = 0; j < t->kvs->size; j++) {
			KV* kv2 = (KV*)*get_list2(t->kvs, j);
			ModelVal* val = (ModelVal*)kv2->value;
			// 
		}
	}
	return NULL;
}
MemTable* StringMem(ThreadGC* thgc, String* str) {
	int len = str->size / str->esize;
	for (int i = 0; i < len; i++) {
		wchar_t c = GetChar(str, i);
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					MemTable* mt = (MemTable*)GC_malloc(thgc, sizeof(MemTable));
					mt->table = create_mapy(thgc, true);
					return mt;
				}
				c = GetChar(str, j);
				if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c == '_') {
					continue;
				}
				else {
					MemTable* mt = (MemTable*)GC_malloc(thgc, sizeof(MemTable));
					mt->table = create_mapy(thgc, true);
					return mt;
				}
			}
			i = j - 1;
		}
		else if ('0' <= c && c <= '9') {
			int j = i + 1;
			for (; ; j++) {
				if (j >= len) {
					return NULL;
				}
				c = GetChar(str, j);
				if ('0' <= c && c <= '9') {
					continue;
				}
				else {
					return NULL;
				}
			}
			i = j - 1;
		}
		else {
			return NULL;
		}
	}
	return NULL;
}