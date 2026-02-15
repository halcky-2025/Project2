#pragma once

struct String {
    char* data;
    int size;    // 文字数（文字列の長さ）
    int esize;   // 1文字あたりのバイト数
};
String* createString(ThreadGC* thgc, char* tex, int size, int esize) {
	String* str = (String*)GC_alloc(thgc, _String);
	str->data = (char*)GC_alloc_size(thgc, size * esize);
	str->size = size;
    str->esize = esize;
	memcpy(str->data, tex, size * esize);
	return str;
}
String* createStringant(ThreadGC* thgc, char* tex, int size, int esize) {
    String* str = (String*)GC_alloc_ant(thgc, _String);
    if (str == NULL) return NULL;
    str->data = (char*)GC_alloc_size_ant(thgc, size * esize);
    if (str == NULL) return NULL;
    str->size = size;
    str->esize = esize;
    memcpy(str->data, tex, size * esize);
    return str;
}
String* StringAdd(ThreadGC* thgc, String* str, char* value, int len, int esize) {
    String* ret = (String*)GC_alloc(thgc, _String);
    if (str->esize == esize) {
        ret->esize = esize;
        ret->size = str->size + len;
        ret->data = (char*)GC_alloc_size(thgc, ret->size * ret->esize);
        memcpy(ret->data, str->data, str->size * str->esize);
        memcpy((char*)(ret->data + str->size * str->esize), value, len * esize);
    }
    else {
        ret->esize = 2;
        ret->size = str->size + len;
        ret->data = (char*)GC_alloc_size(thgc, ret->size * 2);
        if (str->esize == 1) {
            for (int i = 0; i < str->size; i++) {
				ret->data[i * 2] = str->data[i];
				ret->data[i * 2 + 1] = 0;
            }
            memcpy((char*)(ret->data + str->size * 2), value, len * 2);
        }
        else {
            memcpy(ret->data, str->data, str->size * 2);
            for (int i = 0; i < len; i++) {
                ret->data[(str->size + i) * 2] = value[i];
                ret->data[(str->size + i) * 2 + 1] = 0;
            }
        }
    }
    return ret;
}
String* StringAdd2(ThreadGC* thgc, String* str, String* value) {
    String* ret = (String*)GC_alloc(thgc, _String);
    if (str->esize == value->esize) {
        ret->esize = str->esize;
        ret->size = str->size + value->size;
        ret->data = (char*)GC_alloc_size(thgc, ret->size * ret->esize);
        memcpy(ret->data, str->data, str->size * str->esize);
        memcpy((char*)(ret->data + str->size * str->esize), value->data, value->size * value->esize);
    }
    else {
        ret->esize = 2;
        ret->size = str->size + value->size;
        ret->data = (char*)GC_alloc_size(thgc, ret->size * 2);
        if (str->esize == 1) {
            for (int i = 0; i < str->size; i++) {
                ret->data[i * 2] = str->data[i];
                ret->data[i * 2 + 1] = 0;
            }
            memcpy((char*)(ret->data + str->size * 2), value->data, value->size * 2);
        }
        else {
            memcpy(ret->data, str->data, str->size * 2);
            for (int i = 0; i < value->size; i++) {
                ret->data[(str->size + i) * 2] = value->data[i];
                ret->data[(str->size + i) * 2 + 1] = 0;
            }
        }
    }
    return ret;
}
String* StringRemove(ThreadGC* thgc, String* str, int n) {
	String* ret = (String*)GC_alloc(thgc, _String);
    ret->esize = str->esize;
    ret->size = str->size - 1;
	ret->data = (char*)GC_alloc_size(thgc, ret->size * ret->esize);
    memcpy(ret->data, str->data, n * str->esize);
    memcpy((char*)(ret->data + n * str->esize), str->data + (n + 1) * str->esize, (str->size - (n + 1)) * str->esize);
    return ret;
}
String* SubString(ThreadGC* thgc, String* str, int n1, int length) {
	String* ret = (String*)GC_alloc(thgc, _String);
	ret->esize = str->esize;
	ret->size = length;
	ret->data = (char*)GC_alloc_size(thgc, ret->size * ret->esize);
    memcpy(ret->data, str->data + n1 * str->esize, length * str->esize);
	return ret;
}
wchar_t GetChar(String* str, int n) {
    if (str->esize == 1) {
        wchar_t wc = (unsigned char)(*(str->data + n));
        return wc;
    }
    else {
        wchar_t wc = ((unsigned char)*(str->data + n * 2 + 1) << 8) |
            (unsigned char)*(str->data + n * 2);
        return wc;
    }
}
static inline size_t utf8_len_from_codepoint(uint32_t cp) {
    if (cp <= 0x7F)   return 1;
    if (cp <= 0x7FF)  return 2;
    if (cp <= 0xFFFF) return 3;
    return 4;
}

static inline char* utf8_write_codepoint(uint32_t cp, char* w) {
    if (cp <= 0x7F) {
        *w++ = static_cast<char>(cp);
    }
    else if (cp <= 0x7FF) {
        *w++ = static_cast<char>(0xC0 | (cp >> 6));
        *w++ = static_cast<char>(0x80 | (cp & 0x3F));
    }
    else if (cp <= 0xFFFF) {
        *w++ = static_cast<char>(0xE0 | (cp >> 12));
        *w++ = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        *w++ = static_cast<char>(0x80 | (cp & 0x3F));
    }
    else {
        *w++ = static_cast<char>(0xF0 | (cp >> 18));
        *w++ = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        *w++ = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        *w++ = static_cast<char>(0x80 | (cp & 0x3F));
    }
    return w;
}

/* Latin-1 → UTF-8（正確な長さでmalloc） */
char* latin1_to_utf8_malloc_exact(const char* data, int size, int* out_len = nullptr) {
    if (!data || size < 0) throw std::runtime_error("latin1_to_utf8: invalid input");
    const unsigned char* s = reinterpret_cast<const unsigned char*>(data);

    // 1st pass: 必要バイト数
    size_t need = 0;
    for (int i = 0; i < size; ++i)
        need += (s[i] < 0x80) ? 1u : 2u;

    // allocate
    char* buf = static_cast<char*>(std::malloc(need + 1));
    if (!buf) throw std::bad_alloc();

    // 2nd pass: 実書き込み
    char* w = buf;
    for (int i = 0; i < size; ++i) {
        unsigned c = s[i];
        if (c < 0x80) {
            *w++ = static_cast<char>(c);
        }
        else {
            *w++ = static_cast<char>(0xC0 | (c >> 6));
            *w++ = static_cast<char>(0x80 | (c & 0x3F));
        }
    }
    *w = '\0';
    if (out_len) *out_len = static_cast<int>(w - buf);
    return buf;
}

/* UTF-16LE（charCountは文字数）→ UTF-8（正確な長さでmalloc） */
char* utf16le_to_utf8_malloc_exact(const char* data, int charCount, int* out_len = nullptr) {
    int byteSize = charCount * 2;
    if (!data || charCount < 0)
        throw std::runtime_error("utf16le_to_utf8: invalid input size");
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    const size_t len16 = static_cast<size_t>(charCount);

    // 1st pass: 必要バイト数
    size_t need = 0;
    for (size_t i = 0; i < len16; ++i) {
        uint16_t w1 = static_cast<uint16_t>(p[2 * i]) | (static_cast<uint16_t>(p[2 * i + 1]) << 8);
        if (0xD800 <= w1 && w1 <= 0xDBFF) { // high surrogate
            if (i + 1 >= len16) throw std::runtime_error("Unpaired high surrogate");
            uint16_t w2 = static_cast<uint16_t>(p[2 * (i + 1)]) | (static_cast<uint16_t>(p[2 * (i + 1) + 1]) << 8);
            if (!(0xDC00 <= w2 && w2 <= 0xDFFF)) throw std::runtime_error("Invalid low surrogate");
            uint32_t cp = ((uint32_t(w1) - 0xD800) << 10) + (uint32_t(w2) - 0xDC00) + 0x10000;
            need += utf8_len_from_codepoint(cp);
            ++i; // 低位を消費
        }
        else if (0xDC00 <= w1 && w1 <= 0xDFFF) {
            throw std::runtime_error("Unpaired low surrogate");
        }
        else {
            need += utf8_len_from_codepoint(w1);
        }
    }

    // allocate
    char* buf = static_cast<char*>(std::malloc(need + 1));
    if (!buf) throw std::bad_alloc();

    // 2nd pass: 実書き込み
    char* w = buf;
    for (size_t i = 0; i < len16; ++i) {
        uint16_t w1 = static_cast<uint16_t>(p[2 * i]) | (static_cast<uint16_t>(p[2 * i + 1]) << 8);
        uint32_t cp;
        if (0xD800 <= w1 && w1 <= 0xDBFF) {
            uint16_t w2 = static_cast<uint16_t>(p[2 * (i + 1)]) | (static_cast<uint16_t>(p[2 * (i + 1) + 1]) << 8);
            cp = ((uint32_t(w1) - 0xD800) << 10) + (uint32_t(w2) - 0xDC00) + 0x10000;
            ++i;
        }
        else {
            cp = w1;
        }
        w = utf8_write_codepoint(cp, w);
    }
    *w = '\0';
    if (out_len) *out_len = static_cast<int>(w - buf);
    return buf;
}

char* StringUTF8(String* s, int* out_len) {
    if (!s) throw std::runtime_error("StringUTF8: null");
    if (s->esize == 1) return latin1_to_utf8_malloc_exact(s->data, s->size, out_len);
    if (s->esize == 2) return utf16le_to_utf8_malloc_exact(s->data, s->size, out_len);
    throw std::runtime_error("Unsupported esize: " + std::to_string(s->esize));
}
String* NumberString(ThreadGC* thgc, int n) {
    String* ret = (String*)GC_alloc(thgc, _String);
    ret->esize = 1;
    int count = 0;
    int n2 = n;
    for (;;) {
        count++;
        n2 /= 10;
        if (n2 == 0) break;
    }
    if (n < 0) {
        ret->size = count + 1;
        ret->data = (char*)GC_alloc_size(thgc, ret->size);
        ret->data[0] = '-';
        n2 = -n;
    }
    else {
        ret->size = count;
        ret->data = (char*)GC_alloc_size(thgc, ret->size);
        n2 = n;
    }
    for (int i = 1; ;i++) {
        ret->data[ret->size - i] = '0' + n2 % 10;
        n2 /= 10;
        if (n2 == 0) break;
    }
    return ret;
}
String* FloatString(ThreadGC* thgc, float n, int dlength) {
    String* ret = (String*)GC_alloc(thgc, _String);
    ret->esize = 1;
    int count = 0;
    int n2 = (int)n;
    for (;;) {
        count++;
        n2 /= 10;
    }
    count += dlength;
    if (n < 0) {
        ret->size = count + 2;
        ret->data = (char*)GC_alloc_size(thgc, ret->size);
        ret->data[0] = '-';
        n = -n;
    }
    else {
        ret->size = count + 1;
        ret->data = (char*)GC_alloc_size(thgc, ret->size);
    }
    n2 = (int)n;
    float f2 = n - (float)(int)n2;
    for (int i = 0; i < dlength;) {
        f2 *= 10;
        ret->data[ret->size - dlength + i] = (int)f2 + '0';
        f2 = f2 - (float)(int)f2;
    }
    ret->data[ret->size - dlength - 1] = '.';
    for (int i = dlength + 2; ; i++) {
        ret->data[ret->size - i] = '0' + n2 % 10;
        n2 /= 10;
        if (n2 == 0) break;
    }
    return ret;
}
void PrintString(String* str) {
    if (str->esize == 1) {
        for (int i = 0; i < str->size; i++) {
            putchar(str->data[i]);
        }
    }
    else {
        for (int i = 0; i < str->size; i++) {
            wchar_t result;
            memcpy(&result, str->data + i * 2, sizeof(wchar_t));
            putwchar(result);
        }
    }
}
int StringNumber(String* str) {
	int n = 0;
	if (str->esize == 1) {
		for (int i = 0; i < str->size; i++) {
			n = n * 10 + (str->data[i] - '0');
		}
	}
	else {
		for (int i = 0; i < str->size; i++) {
			wchar_t wc;
			memcpy(&wc, str->data + i * 2, sizeof(wchar_t));
			n = n * 10 + (wc - '0');
		}
	}
	return n;
}
bool StringEqual(char* str1, int len1, int esize1,
    char* str2, int len2, int esize2) {
    if (str1 == str2) return true;
    // NULLポインタチェック
    if (str1 == NULL || str2 == NULL) {
        return false;
    }
    if (len1 != len2) return false;
    // 同じエンコーディングの場合
    if (esize1 == esize2) {
        for (int i = 0; i < len1 * esize1; i++) {
            if (str1[i] != str2[i]) return false;
        }
        return true;
    }
    else {

        char* ascii_str;
        uint16_t* utf16_str;
        size_t ascii_len, utf16_len_words;

        if (esize1 == 1) {
            ascii_str = str1;
            utf16_str = (uint16_t*)str2;
        }
        else {
            ascii_str = str2;
            utf16_str = (uint16_t*)str1;
        }
        for (int i = 0; i < len1; i++) {
            // UTF-16文字列から16ビット文字を取得
            uint16_t utf16_char = (utf16_str)[i];

            // UTF-16文字が ASCII範囲外（0-127）なら一致しない
            if (utf16_char > 0xFF) {
                return false;
            }

            // UTF-16の下位バイトとASCII文字を比較
            if ((char)utf16_char != ascii_str[i]) {
                return false;
            }
        }
        return true;
    }

}
bool StringEqual2(String* str1, String* str2) {
	return StringEqual(str1->data, str1->size, str1->esize,
		str2->data, str2->size, str2->esize);
}
typedef struct {
    char** data;
    int size;
    int capa;
    int esize;
    CType type;
} List;
void ListCheck(ThreadGC* thgc, char* data) {
    List* list = (List*)data;
    list->data = (char**)GC_clone(thgc, (char*)list->data);
    if (list->type) {
        for (int i = 0; i < list->size; i++) {
            char** value = (char**)(list->data + i * list->esize);
            *value = (char*)GC_clone(thgc, *value);
        }
    }
}
List* create_list(ThreadGC* thgc, int esize, CType type) {
    List* list = (List*)GC_alloc(thgc, _List);
    list->type = type;
    list->data = (char **)GC_alloc_size(thgc, 4 * esize);
    list->size = 0;
    list->capa = 4;
    list->esize = esize;
    return list;
}
List* create_list2(ThreadGC* thgc, List* list, int esize, CType type) {
    list->data = (char**)GC_alloc(thgc, _List);
    list->size = 0;
    list->capa = 4;
    list->esize = esize;
	list->type = type;
    return list;
}
List* create_list_ant(ThreadGC* thgc, int esize, CType type) {
    List* list = (List*)GC_alloc_ant(thgc, _List);
	if (list == NULL) return NULL;
    list->type = type;
    list->data = (char**)GC_alloc_size_ant(thgc, 4 * esize);
	if (list->data == NULL) return NULL;
    list->size = 0;
    list->capa = 4;
    list->esize = esize;
    return list;
}
List* create_list2_ant(ThreadGC* thgc, List* list, int esize, CType type) {
    list->data = (char**)GC_alloc_ant(thgc, _List);
	if (list->data == NULL) return NULL;
    list->size = 0;
    list->capa = 4;
    list->esize = esize;
    list->type = type;
    return list;
}
void add_list(ThreadGC* thgc, List* list, char* value) {
    if (list->size == list->capa) {
        list->capa *= 2;
        char** data = (char**)GC_alloc_size(thgc, list->capa * list->esize);
        memcpy(data, list->data, list->size * list->esize);
        list->data = data;
    }
    char* target = (char*)list->data + list->size * list->esize;
    memcpy(target, &value, list->esize);
    list->size++;
}
char* add_list_ant(ThreadGC* thgc, List* list, char* value) {
    if (list->size == list->capa) {
        char** data = (char**)GC_alloc_size_ant(thgc, list->capa * list->esize * 2);
        if (data == NULL) return NULL;
        list->capa *= 2;
        memcpy(data, list->data, list->size * list->esize);
        list->data = data;
    }
    char* target = (char*)list->data + list->size * list->esize;
    memcpy(target, &value, list->esize);
    list->size++;
    return (char*)list;
}
void remove_list(ThreadGC* thgc, List* list, int index) {
    if (index < 0 || index >= list->size) return;

    // 前詰め（index 以降を1つ左へ）
    char* dst = (char*)list->data + (size_t)index * (size_t)list->esize;
    char* src = (char*)list->data + (size_t)(index + 1) * (size_t)list->esize;
    size_t tail_bytes = (size_t)(list->size - index - 1) * (size_t)list->esize;
    if (tail_bytes > 0) {
        memmove(dst, src, tail_bytes);
    }
    // 要素数を減らす
    list->size--;

    // 小さくしすぎない範囲で容量縮小（capa は最小 4 を維持）
    if (list->capa > 4 && list->size <= list->capa / 4) {
        int new_capa = list->capa / 4;
        if (new_capa < 4) new_capa = 4;
        // esize * new_capa バイト確保してコピー
        char** new_data = (char**)GC_alloc_size(thgc, (size_t)new_capa * (size_t)list->esize);
        // 縮小確保に失敗しても動作継続（容量変更だけ諦める）
        if (new_data) {
            memcpy(new_data, list->data, (size_t)list->size * (size_t)list->esize);
            list->data = new_data;
            list->capa = new_capa;
        }
    }
}
char** get_list(List* list, int index) {
    if (index < 0 || index >= list->size) {
        return NULL;
    }
    return (char**)((char*)list->data + index * list->esize);
}
void remove_list_val(ThreadGC* thgc, List* list, char* val) {
    for (int i = 0; i < list->size; i++) {
        char* gv = *get_list(list, i);
        if (gv == val) {
            remove_list(thgc, list, i);
            return;
        }
    }
}
char** get_last(List* list) {
    return get_list(list, list->size - 1);
}
char** get_list2(List* list, int index) {
    return (char**)((char*)list->data + index * list->esize);
}
void set_list(List* list, int index, char* value) {
    if (index < 0 || index >= list->size) {
        return;
    }
    char** set = (char**)((char*)list->data + index * list->esize);
    *set = value;
}
void set_list2(List* list, int index, char* value) {
    char** set = (char**)((char*)list->data + index * list->esize);
    *set = value;
}
void set_last(List* list, char* value) {
    set_list(list, list->size - 1, value);
}
typedef struct {
    int n;
    String* key;
    char* value;
    CType type;
}KV;
void KVCheck(ThreadGC* thgc, char* data) {
	KV* kv = (KV*)data;
	if (kv->type) {
		kv->key = (String*)GC_clone(thgc, (char*)kv->key);
		kv->value = (char*)GC_clone(thgc, kv->value);
	}
	else kv->key = (String*)GC_clone(thgc, (char*)kv->key);
}
typedef struct {
    List** _map;
    int capa;
} MapData;
void MapDataCheck(ThreadGC* thgc, char* data) {
	MapData* map = (MapData*)data;
    map->_map = (List**)GC_clone (thgc, (char*)map->_map);
	for (int i = 0; i < map->capa; i++) {
        map->_map[i] = (List*)GC_clone(thgc, (char*)map->_map[i]);
	}
}
struct Map{
    List* kvs;
    MapData* md;
    CType type;
};
void MapCheck(ThreadGC* thgc, char* data) {
	Map* map = (Map*)data;
	map->md = (MapData*)GC_clone(thgc, (char*)map->md);
}
MapData* create_map_data(ThreadGC* thgc, int capa, CType type) {
    MapData* md = (MapData*)GC_alloc(thgc, _MapData);
    md->capa = capa;
    md->_map = (List**)GC_alloc_size(thgc, capa * sizeof(List*));
    for (int i = 0; i < capa; i++) {
        md->_map[i] = (List*)GC_alloc(thgc, _List);
    }
    for (int i = 0; i < md->capa; i++) {
        create_list2(thgc, (List*) md->_map[i], sizeof(KV*), type);
    }
    return md;
}
MapData* create_map_data_ant(ThreadGC* thgc, int capa, CType type) {
    MapData* map = (MapData*)GC_alloc_ant(thgc, _MapData);
    if (map == NULL) return NULL;
    map->capa = capa;
    map->_map = (List**)GC_alloc_size_ant(thgc, capa * sizeof(List*));
	if (map->_map == NULL) return NULL;
    for (int i = 0; i < capa; i++) {
        map->_map[i] = (List*)GC_alloc_ant(thgc, _List);
		if (map->_map[i] == NULL) return NULL;
    }
    for (int i = 0; i < map->capa; i++) {
        create_list2_ant(thgc, (List*)map->_map[i], sizeof(KV*), type);
    }
    return map;
}
void PrintArray(Map* map, int depth, int type) {
    putchar('[');
	List* list = map->kvs;
    for (int i = 0; i < list->size; i++) {
		if (i != 0) putchar(',');
        KV* kv = (KV*)*get_list2(list, i);
        if (kv->key != NULL) {
            putchar('#');
            PrintString(kv->key);
        }
        putchar(' ');
        if (depth == 0) {
			if (type == 0) {
				printf("%s", kv->value);
			}
			else if (type == 1) {
				printf("%d", kv->value);
			}
			else if (type == 2) {
				printf("%f", kv->value);
			}
		}
        else {
            if (kv->value == NULL) printf("null");
			else PrintArray((Map*)kv->value, depth - 1, type);
        }
    }
    putchar(']');
    putchar('\n');
}
Map* create_mapy(ThreadGC* thgc, CType type) {
    Map* map = (Map*)GC_alloc(thgc, _Map);
	map->type = type;
    map->md = create_map_data(thgc, 16, type);
    map->kvs = create_list(thgc, sizeof(KV*), type);
    return map;
}
Map* create_mapy_ant(ThreadGC* thgc, CType type) {
    Map* map = (Map*)GC_alloc_ant(thgc, _Map);
    if (map == NULL) return NULL;
    map->type = type;
    map->md = create_map_data_ant(thgc, 16, type);
    if (map->md == NULL) return NULL;
    map->kvs = create_list_ant(thgc, sizeof(KV*), type);
	if (map->kvs == NULL) return NULL;
    return map;
}
int get_hashvalue(String* key, int size) {
    int hash = 10000;
    int byteSize = key->size * key->esize;
    for (int i = 0; i < byteSize; i++) {
        hash ^= (unsigned char)key->data[i];
        hash *= 1619;
        hash %= size;
    }
    return hash;
}
void add_mapy(ThreadGC* thgc, Map* map, String* key, char* value) {
    List* mdl;
head:
    if (key != NULL) {
        mdl = map->md->_map[get_hashvalue(key, map->md->capa)];
        if (mdl->size > 3) {
            map->md = create_map_data(thgc, map->md->capa * 4, map->type);
            for (int i = 0; i < map->kvs->size; i++) {
                KV* kv = (KV*)*get_list2(map->kvs, i);
                if (kv->key == NULL) continue;
                List* md2 = map->md->_map[get_hashvalue(kv->key, map->md->capa)];
                add_list(thgc, md2, (char*)kv);
                goto head;
            }
        }
        for (int i = 0; i < mdl->size; i++) {
            KV* kv = (KV*)*get_list2(mdl, i);
            if (kv->key->size != key->size) continue;
            int byteSize = key->size * key->esize;
            for (int j = 0; ; j++) {
                if (j >= byteSize) {
                    kv->value = value;
                    return;
                }
                if (kv->key->data[j] != key->data[j]) break;
            }
        }
        KV* kv2 = (KV*)GC_alloc(thgc, _KV);
        kv2->type = map->type;
        kv2->key = key;
        kv2->value = value;
        kv2->n = map->kvs->size;
        add_list(thgc, mdl, (char*)kv2);
        add_list(thgc, map->kvs, (char*)kv2);
    }
    else {
        KV* kv2 = (KV*)GC_alloc(thgc, _KV);
        kv2->type = map->type;
        kv2->key = NULL;
        kv2->value = value;
        kv2->n = map->kvs->size;
        add_list(thgc, map->kvs, (char*)kv2);
    }
    return;
}
char* add_mapy_ant(ThreadGC* thgc, Map* map, String* key, char* value) {
    List* mdl;
head:
    if (key != NULL) {
        mdl = map->md->_map[get_hashvalue(key, map->md->capa)];
        if (mdl->size > 3) {
            map->md = create_map_data_ant(thgc, map->md->capa * 4, map->type);
            if (map->md == NULL) return NULL;
            for (int i = 0; i < map->kvs->size; i++) {
                KV* kv = (KV*)*get_list2(map->kvs, i);
                if (kv->key == NULL) continue;
                List* md2 = map->md->_map[get_hashvalue(kv->key, map->md->capa)];
                char *o2 = add_list_ant(thgc, md2, (char*)kv);
                if (o2 == NULL) return NULL;
                goto head;
            }
        }
        for (int i = 0; i < mdl->size; i++) {
            KV* kv = (KV*)*get_list2(mdl, i);
            if (kv->key->size != key->size) continue;
            int byteSize = key->size * key->esize;
            for (int j = 0; ; j++) {
                if (j >= byteSize) {
                    kv->value = value;
                    return (char*)map;
                }
                if (kv->key->data[j] != key->data[j]) break;
            }
        }
        KV* kv2 = (KV*)GC_alloc_ant(thgc, _KV);
        if (kv2 == NULL) return NULL;
        kv2->type = map->type;
        kv2->key = key;
        kv2->value = value;
        kv2->n = map->kvs->size;
        char * o = add_list_ant(thgc, mdl, (char*)kv2);
		if (o == NULL) return NULL;
        o = add_list_ant(thgc, map->kvs, (char*)kv2);
		if (o == NULL) return NULL;
    }
    else {
        KV* kv2 = (KV*)GC_alloc_ant(thgc, _KV);
		if (kv2 == NULL) return NULL;
        kv2->type = map->type;
        kv2->key = NULL;
        kv2->value = value;
        kv2->n = map->kvs->size;
        char *o = add_list_ant(thgc, map->kvs, (char*)kv2);
		if (o == NULL) return NULL;
    }
    return (char*)map;
}
char* get_mapy(Map* map, String* key) {
    List* mdl = map->md->_map[get_hashvalue(key, map->md->capa)];
    for (int i = 0; i < mdl->size; i++) {
        KV* kv = (KV*)*get_list2(mdl, i);
        if (kv->key->size != key->size) continue;
        int byteSize = key->size * key->esize;
        for (int j = 0; ; j++) {
            if (j >= byteSize) {
                return kv->value;
            }
            if (kv->key->data[j] != key->data[j]) break;
        }
    }
    return NULL;
}
int remove_mapy(ThreadGC* thgc, Map* map, String* key) {
    // 通常キー：まずハッシュバケットで該当 KV を探す
    List* mdl = map->md->_map[get_hashvalue(key, map->md->capa)];
    KV* target = NULL;
    int md_index = -1;

    for (int i = 0; i < mdl->size; ++i) {
        KV* kv = (KV*)*get_list2(mdl, i);
        if (!kv || !kv->key) continue;
        if (kv->key->size != key->size) continue;

        // 生バイト比較（既存コード準拠）
        int byteSize = key->size * key->esize;
        int j = 0;
        for (;; ++j) {
            if (j >= byteSize) {
                target = kv;
                md_index = i;
                break;
            }
            if (kv->key->data[j] != key->data[j]) break;
        }
        if (target) break;
        else return 0;
    }

    if (!target) return 0; // 見つからない

    // バケットから削除
    remove_list(thgc, mdl, md_index);

    // kvs からも同じポインタを削除（n を信用してもよいが、安全に線形探索）
    List* kvs = map->kvs;
    remove_list(thgc, kvs, target->n);
    // 後続の n を更新
    for (int j = target->n; j < kvs->size; ++j) {
        KV* kvj = (KV*)*get_list2(kvs, j);
        kvj->n = j;
    }
    return 1;
}
int get_hashvaluen(char* n, int size) {
    long hash = (long)10000;
    hash ^= (long)n;
    hash *= 1619;
    hash %= size;
    return hash;
}
void add_mapyn(ThreadGC* thgc, Map* map, char* key, char* value) {
    List* mdl;
head:
    if (key != NULL) {
        mdl = map->md->_map[get_hashvaluen(key, map->md->capa)];
        if (mdl->size > 3) {
            map->md = create_map_data(thgc, map->md->capa * 4, map->type);
            for (int i = 0; i < map->kvs->size; i++) {
                KV* kv = (KV*)*get_list2(map->kvs, i);
                if (kv->key == NULL) continue;
                List* md2 = map->md->_map[get_hashvaluen((char*)kv->key, map->md->capa)];
                add_list(thgc, md2, (char*)kv);
                goto head;
            }
        }
        for (int i = 0; i < mdl->size; i++) {
            KV* kv = (KV*)*get_list2(mdl, i);
            if ((char*)kv->key != key) continue;
            else {
                kv->value = value;
                return;
            }
        }
        KV* kv2 = (KV*)GC_alloc(thgc, _KV);
        kv2->type = map->type;
        kv2->key = (String*)key;
        kv2->value = value;
        kv2->n = map->kvs->size;
        add_list(thgc, mdl, (char*)kv2);
        add_list(thgc, map->kvs, (char*)kv2);
    }
    else {
        KV* kv2 = (KV*)GC_alloc(thgc, _KV);
        kv2->type = map->type;
        kv2->key = NULL;
        kv2->value = value;
        kv2->n = map->kvs->size;
        add_list(thgc, map->kvs, (char*)kv2);
    }
    return;
}
char* add_mapyn_ant(ThreadGC* thgc, Map* map, char* key, char* value) {
    List* mdl;
head:
    if (key != NULL) {
        mdl = map->md->_map[get_hashvaluen(key, map->md->capa)];
        if (mdl->size > 3) {
            map->md = create_map_data_ant(thgc, map->md->capa * 4, map->type);
            if (map->md == NULL) return NULL;
            for (int i = 0; i < map->kvs->size; i++) {
                KV* kv = (KV*)*get_list2(map->kvs, i);
                if (kv->key == NULL) continue;
                List* md2 = map->md->_map[get_hashvaluen((char*)kv->key, map->md->capa)];
                char* o2 = add_list_ant(thgc, md2, (char*)kv);
                if (o2 == NULL) return NULL;
                goto head;
            }
        }
        for (int i = 0; i < mdl->size; i++) {
            KV* kv = (KV*)*get_list2(mdl, i);
            if ((char*)kv->key != key) continue;
            else {
                kv->value = value;
				return (char*)map;
            }
        }
        KV* kv2 = (KV*)GC_alloc_ant(thgc, _KV);
        if (kv2 == NULL) return NULL;
        kv2->type = map->type;
        kv2->key = (String*)key;
        kv2->value = value;
        kv2->n = map->kvs->size;
        char* o = add_list_ant(thgc, mdl, (char*)kv2);
        if (o == NULL) return NULL;
        o = add_list_ant(thgc, map->kvs, (char*)kv2);
        if (o == NULL) return NULL;
    }
    else {
        KV* kv2 = (KV*)GC_alloc_ant(thgc, _KV);
        if (kv2 == NULL) return NULL;
        kv2->type = map->type;
        kv2->key = NULL;
        kv2->value = value;
        kv2->n = map->kvs->size;
        char* o = add_list_ant(thgc, map->kvs, (char*)kv2);
        if (o == NULL) return NULL;
    }
    return (char*)map;
}
char* get_mapyn(Map* map, char* key) {
    List* mdl = map->md->_map[get_hashvaluen(key, map->md->capa)];
    for (int i = 0; i < mdl->size; i++) {
        KV* kv = (KV*)*get_list2(mdl, i);
        if ((char*)kv->key != key) continue;
        else {
            return kv->value;
		}
    }
    return NULL;
}
int remove_mapyn(ThreadGC* thgc, Map* map, char* key) {
    // 通常キー：まずハッシュバケットで該当 KV を探す
    List* mdl = map->md->_map[get_hashvaluen(key, map->md->capa)];
    KV* target = NULL;
    int md_index = -1;

    for (int i = 0; i < mdl->size; ++i) {
        KV* kv = (KV*)*get_list2(mdl, i);
        if (!kv || !kv->key) continue;
        if ((char*)kv->key != key) continue;
        else {
            target = kv;
            md_index = i;
            break;
        }
    }

    if (!target) return 0; // 見つからない

    // バケットから削除
    remove_list(thgc, mdl, md_index);

    // kvs からも同じポインタを削除（n を信用してもよいが、安全に線形探索）
    List* kvs = map->kvs;
    remove_list(thgc, kvs, target->n);
    // 後続の n を更新
    for (int j = target->n; j < kvs->size; ++j) {
        KV* kvj = (KV*)*get_list2(kvs, j);
        kvj->n = j;
    }
    return 1;
}
