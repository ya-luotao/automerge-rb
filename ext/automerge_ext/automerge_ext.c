#include "ruby.h"
#include "ruby/encoding.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct AMactorId AMactorId;
typedef struct AMchange AMchange;
typedef struct AMcursor AMcursor;
typedef struct AMdoc AMdoc;
typedef struct AMitem AMitem;
typedef struct AMmark AMmark;
typedef struct AMobjId AMobjId;
typedef struct AMresult AMresult;
typedef struct AMsyncMessage AMsyncMessage;
typedef struct AMsyncState AMsyncState;

typedef struct {
    const uint8_t *src;
    size_t count;
} AMbyteSpan;

typedef struct {
    unsigned char detail[sizeof(size_t) + sizeof(ptrdiff_t) + sizeof(void *)];
} AMitems;

typedef enum {
    AM_STATUS_OK = 0,
    AM_STATUS_ERROR = 1,
    AM_STATUS_INVALID_RESULT = 2,
} AMstatus;

typedef enum {
    AM_OBJ_TYPE_DEFAULT = 0,
    AM_OBJ_TYPE_LIST = 1,
    AM_OBJ_TYPE_MAP = 2,
    AM_OBJ_TYPE_TEXT = 3,
} AMobjType;

typedef enum {
    AM_MARK_EXPAND_DEFAULT = 0,
    AM_MARK_EXPAND_NONE = 1,
    AM_MARK_EXPAND_BEFORE = 2,
    AM_MARK_EXPAND_AFTER = 3,
    AM_MARK_EXPAND_BOTH = 4,
} AMmarkExpand;

typedef enum {
    AM_VAL_TYPE_VOID = 1 << 0,
    AM_VAL_TYPE_ACTOR_ID = 1 << 1,
    AM_VAL_TYPE_BOOL = 1 << 2,
    AM_VAL_TYPE_BYTES = 1 << 3,
    AM_VAL_TYPE_CHANGE = 1 << 4,
    AM_VAL_TYPE_CHANGE_HASH = 1 << 5,
    AM_VAL_TYPE_COUNTER = 1 << 6,
    AM_VAL_TYPE_CURSOR = 1 << 7,
    AM_VAL_TYPE_DOC = 1 << 8,
    AM_VAL_TYPE_F64 = 1 << 9,
    AM_VAL_TYPE_INT = 1 << 10,
    AM_VAL_TYPE_MARK = 1 << 11,
    AM_VAL_TYPE_NULL = 1 << 12,
    AM_VAL_TYPE_OBJ_TYPE = 1 << 13,
    AM_VAL_TYPE_STR = 1 << 14,
    AM_VAL_TYPE_SYNC_HAVE = 1 << 15,
    AM_VAL_TYPE_SYNC_MESSAGE = 1 << 16,
    AM_VAL_TYPE_SYNC_STATE = 1 << 17,
    AM_VAL_TYPE_TIMESTAMP = 1 << 18,
    AM_VAL_TYPE_UINT = 1 << 19,
    AM_VAL_TYPE_UNKNOWN = 1 << 20,
} AMvalType;

extern AMresult *AMactorIdFromStr(AMbyteSpan value);
extern AMbyteSpan AMactorIdStr(const AMactorId *actor_id);
extern AMresult *AMapplyChanges(AMdoc *doc, const AMitems *items);
extern AMresult *AMchangeFromBytes(const uint8_t *src, size_t count);
extern AMbyteSpan AMchangeRawBytes(const AMchange *change);
extern AMresult *AMclone(const AMdoc *doc);
extern AMresult *AMcommit(AMdoc *doc, AMbyteSpan message, const int64_t *timestamp);
extern AMresult *AMcreate(const AMactorId *actor_id);
extern AMbyteSpan AMcursorBytes(const AMcursor *cursor);
extern AMresult *AMcursorFromBytes(const uint8_t *src, size_t count);
extern AMresult *AMemptyChange(AMdoc *doc, AMbyteSpan message, const int64_t *timestamp);
extern bool AMequal(AMdoc *doc1, AMdoc *doc2);
extern AMresult *AMfork(AMdoc *doc, const AMitems *heads);
extern AMresult *AMgenerateSyncMessage(AMdoc *doc, AMsyncState *sync_state);
extern AMresult *AMgetActorId(const AMdoc *doc);
extern AMresult *AMgetChangeByHash(AMdoc *doc, const uint8_t *src, size_t count);
extern AMresult *AMgetChanges(AMdoc *doc, const AMitems *have_deps);
extern AMresult *AMgetChangesAdded(AMdoc *doc1, AMdoc *doc2);
extern AMresult *AMgetCursor(const AMdoc *doc, const AMobjId *obj_id, size_t position, const AMitems *heads);
extern AMresult *AMgetCursorPosition(const AMdoc *doc, const AMobjId *obj_id, const AMcursor *cursor, const AMitems *heads);
extern AMresult *AMgetHeads(AMdoc *doc);
extern AMresult *AMgetLastLocalChange(AMdoc *doc);
extern AMresult *AMgetMissingDeps(AMdoc *doc, const AMitems *heads);
extern AMitem *AMitemResult(const AMitem *item);
extern AMresult *AMitemFromBool(bool value);
extern AMresult *AMitemFromBytes(const uint8_t *src, size_t count);
extern AMresult *AMitemFromCounter(int64_t value);
extern AMresult *AMitemFromF64(double value);
extern AMresult *AMitemFromInt(int64_t value);
extern AMresult *AMitemFromNull(void);
extern AMresult *AMitemFromStr(AMbyteSpan value);
extern AMresult *AMitemFromTimestamp(int64_t value);
extern AMresult *AMitemFromUint(uint64_t value);
extern bool AMitemKey(const AMitem *item, AMbyteSpan *value);
extern const AMobjId *AMitemObjId(const AMitem *item);
extern bool AMitemPos(const AMitem *item, size_t *value);
extern AMresult *AMitemFromChangeHash(AMbyteSpan value);
extern bool AMitemToActorId(const AMitem *item, const AMactorId **value);
extern bool AMitemToBool(const AMitem *item, bool *value);
extern bool AMitemToBytes(const AMitem *item, AMbyteSpan *value);
extern bool AMitemToChange(AMitem *item, AMchange **value);
extern bool AMitemToChangeHash(const AMitem *item, AMbyteSpan *value);
extern bool AMitemToCounter(const AMitem *item, int64_t *value);
extern bool AMitemToCursor(const AMitem *item, const AMcursor **value);
extern bool AMitemToDoc(AMitem *item, AMdoc **value);
extern bool AMitemToF64(const AMitem *item, double *value);
extern bool AMitemToInt(const AMitem *item, int64_t *value);
extern bool AMitemToMark(const AMitem *item, const AMmark **value);
extern bool AMitemToStr(const AMitem *item, AMbyteSpan *value);
extern bool AMitemToSyncMessage(const AMitem *item, const AMsyncMessage **value);
extern bool AMitemToSyncState(AMitem *item, AMsyncState **value);
extern bool AMitemToTimestamp(const AMitem *item, int64_t *value);
extern bool AMitemToUint(const AMitem *item, uint64_t *value);
extern AMvalType AMitemValType(const AMitem *item);
extern void AMitemsAdvance(AMitems *items, ptrdiff_t n);
extern AMitem *AMitemsNext(AMitems *items, ptrdiff_t n);
extern size_t AMitemsSize(const AMitems *items);
extern AMresult *AMkeys(const AMdoc *doc, const AMobjId *obj_id, const AMitems *heads);
extern AMresult *AMlistDelete(AMdoc *doc, const AMobjId *obj_id, size_t pos);
extern AMresult *AMlistGet(const AMdoc *doc, const AMobjId *obj_id, size_t pos, const AMitems *heads);
extern AMresult *AMlistGetAll(const AMdoc *doc, const AMobjId *obj_id, size_t pos, const AMitems *heads);
extern AMresult *AMlistIncrement(AMdoc *doc, const AMobjId *obj_id, size_t pos, int64_t value);
extern AMresult *AMlistPutBool(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, bool value);
extern AMresult *AMlistPutBytes(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, AMbyteSpan value);
extern AMresult *AMlistPutCounter(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, int64_t value);
extern AMresult *AMlistPutF64(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, double value);
extern AMresult *AMlistPutInt(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, int64_t value);
extern AMresult *AMlistPutNull(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert);
extern AMresult *AMlistPutObject(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, AMobjType obj_type);
extern AMresult *AMlistPutStr(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, AMbyteSpan value);
extern AMresult *AMlistPutTimestamp(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, int64_t value);
extern AMresult *AMlistPutUint(AMdoc *doc, const AMobjId *obj_id, size_t pos, bool insert, uint64_t value);
extern AMresult *AMlistRange(const AMdoc *doc, const AMobjId *obj_id, size_t begin, size_t end, const AMitems *heads);
extern AMresult *AMload(const uint8_t *src, size_t count);
extern AMresult *AMloadIncremental(AMdoc *doc, const uint8_t *src, size_t count);
extern AMresult *AMmapDelete(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key);
extern AMresult *AMmapGet(const AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, const AMitems *heads);
extern AMresult *AMmapGetAll(const AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, const AMitems *heads);
extern AMresult *AMmapIncrement(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, int64_t value);
extern AMresult *AMmapPutBool(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, bool value);
extern AMresult *AMmapPutBytes(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, AMbyteSpan value);
extern AMresult *AMmapPutCounter(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, int64_t value);
extern AMresult *AMmapPutF64(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, double value);
extern AMresult *AMmapPutInt(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, int64_t value);
extern AMresult *AMmapPutNull(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key);
extern AMresult *AMmapPutObject(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, AMobjType obj_type);
extern AMresult *AMmapPutStr(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, AMbyteSpan value);
extern AMresult *AMmapPutTimestamp(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, int64_t value);
extern AMresult *AMmapPutUint(AMdoc *doc, const AMobjId *obj_id, AMbyteSpan key, uint64_t value);
extern AMresult *AMmapRange(const AMdoc *doc, const AMobjId *obj_id, AMbyteSpan begin, AMbyteSpan end, const AMitems *heads);
extern AMresult *AMmarkClear(AMdoc *doc, const AMobjId *obj_id, size_t start, size_t end, AMmarkExpand expand, AMbyteSpan name);
extern AMresult *AMmarkCreate(AMdoc *doc, const AMobjId *obj_id, size_t start, size_t end, AMmarkExpand expand, AMbyteSpan name, const AMitem *value);
extern size_t AMmarkEnd(const AMmark *mark);
extern AMbyteSpan AMmarkName(const AMmark *mark);
extern size_t AMmarkStart(const AMmark *mark);
extern AMresult *AMmarkValue(const AMmark *mark);
extern AMresult *AMmarks(const AMdoc *doc, const AMobjId *obj_id, const AMitems *heads);
extern AMresult *AMmerge(AMdoc *dest, AMdoc *src);
extern AMobjType AMobjObjType(const AMdoc *doc, const AMobjId *obj_id);
extern size_t AMobjSize(const AMdoc *doc, const AMobjId *obj_id, const AMitems *heads);
extern size_t AMpendingOps(const AMdoc *doc);
extern AMresult *AMreceiveSyncMessage(AMdoc *doc, AMsyncState *sync_state, const AMsyncMessage *sync_message);
extern AMbyteSpan AMresultError(const AMresult *result);
extern void AMresultFree(AMresult *result);
extern AMitem *AMresultItem(AMresult *result);
extern AMitems AMresultItems(AMresult *result);
extern AMresult *AMresultCat(const AMresult *dest, const AMresult *src);
extern size_t AMresultSize(const AMresult *result);
extern AMstatus AMresultStatus(const AMresult *result);
extern size_t AMrollback(AMdoc *doc);
extern AMresult *AMsave(AMdoc *doc);
extern AMresult *AMsaveIncremental(AMdoc *doc);
extern AMresult *AMsetActorId(AMdoc *doc, const AMactorId *actor_id);
extern AMresult *AMspliceText(AMdoc *doc, const AMobjId *obj_id, size_t pos, ptrdiff_t del, AMbyteSpan text);
extern AMresult *AMsyncMessageDecode(const uint8_t *src, size_t count);
extern AMresult *AMsyncMessageEncode(const AMsyncMessage *sync_message);
extern AMresult *AMsyncMessageHeads(const AMsyncMessage *sync_message);
extern AMresult *AMsyncMessageNeeds(const AMsyncMessage *sync_message);
extern AMresult *AMsyncStateDecode(const uint8_t *src, size_t count);
extern AMresult *AMsyncStateEncode(const AMsyncState *sync_state);
extern bool AMsyncStateEqual(const AMsyncState *sync_state1, const AMsyncState *sync_state2);
extern AMresult *AMsyncStateInit(void);
extern AMresult *AMsyncStateSharedHeads(const AMsyncState *sync_state);
extern AMresult *AMtext(const AMdoc *doc, const AMobjId *obj_id, const AMitems *heads);
extern AMresult *AMchangeActorId(const AMchange *change);
extern AMbyteSpan AMchangeMessage(const AMchange *change);
extern int64_t AMchangeTime(const AMchange *change);
extern AMbyteSpan AMchangeHash(const AMchange *change);
extern uint64_t AMchangeSeq(const AMchange *change);
extern uint64_t AMchangeMaxOp(const AMchange *change);
extern uint64_t AMchangeStartOp(const AMchange *change);
extern AMresult *AMchangeDeps(const AMchange *change);
extern size_t AMchangeSize(const AMchange *change);
extern bool AMchangeIsEmpty(const AMchange *change);
extern AMbyteSpan AMchangeExtraBytes(const AMchange *change);

typedef struct {
    AMresult *result;
    AMdoc *doc;
} amrb_doc_t;

typedef struct {
    AMresult *result;
    AMsyncState *state;
} amrb_sync_state_t;

typedef struct {
    AMresult **items;
    long len;
    long capacity;
} amrb_result_stack_t;

static VALUE mAutomerge;
static VALUE cDocument;
static VALUE cSyncState;
static VALUE cError;
static VALUE cCounter;
static VALUE cTimestamp;
static VALUE cBytes;
static VALUE cText;
static VALUE cUint;
static ID id_actor_id;
static ID id_message;
static ID id_timestamp;
static ID id_value;
static ID id_value_key;

static AMbyteSpan null_span(void) {
    AMbyteSpan span;
    span.src = NULL;
    span.count = 0;
    return span;
}

static AMbyteSpan string_span(VALUE value) {
    StringValue(value);
    AMbyteSpan span;
    span.src = (const uint8_t *)RSTRING_PTR(value);
    span.count = (size_t)RSTRING_LEN(value);
    return span;
}

static VALUE utf8_string_from_span(AMbyteSpan span) {
    VALUE string = rb_str_new((const char *)span.src, (long)span.count);
    rb_enc_associate(string, rb_utf8_encoding());
    return string;
}

static VALUE binary_string_from_span(AMbyteSpan span) {
    VALUE string = rb_str_new((const char *)span.src, (long)span.count);
    rb_enc_associate(string, rb_ascii8bit_encoding());
    return string;
}

static void raise_with_message(VALUE error_class, VALUE message) {
    rb_exc_raise(rb_exc_new_str(error_class, message));
}

static void raise_result(AMresult *result) {
    if (!result) {
        rb_raise(cError, "Automerge returned a null result");
    }

    AMstatus status = AMresultStatus(result);
    if (status == AM_STATUS_OK) {
        return;
    }

    VALUE message;
    if (status == AM_STATUS_ERROR) {
        message = utf8_string_from_span(AMresultError(result));
    } else {
        message = rb_sprintf("invalid Automerge result status %d", status);
    }
    AMresultFree(result);
    raise_with_message(cError, message);
}

static void check_result(AMresult *result) {
    raise_result(result);
}

static void checked_free_result(AMresult *result) {
    check_result(result);
    AMresultFree(result);
}

static VALUE scalar_object(VALUE klass, VALUE value) {
    VALUE obj = rb_obj_alloc(klass);
    rb_ivar_set(obj, id_value, value);
    return obj;
}

static VALUE path_array(VALUE path) {
    if (NIL_P(path)) {
        return rb_ary_new();
    }
    if (RB_TYPE_P(path, T_ARRAY)) {
        return path;
    }
    VALUE ary = rb_ary_new2(1);
    rb_ary_push(ary, path);
    return ary;
}

static VALUE path_key_string(VALUE key) {
    if (RB_TYPE_P(key, T_SYMBOL)) {
        key = rb_sym2str(key);
    }
    StringValue(key);
    return key;
}

static size_t path_index(VALUE index) {
    if (!RB_INTEGER_TYPE_P(index)) {
        rb_raise(rb_eTypeError, "list path segment must be an Integer");
    }
    long value = NUM2LONG(index);
    if (value < 0) {
        rb_raise(rb_eArgError, "list index must be non-negative");
    }
    return (size_t)value;
}

static void stack_init(amrb_result_stack_t *stack, long capacity) {
    stack->len = 0;
    stack->capacity = capacity > 0 ? capacity : 1;
    stack->items = ALLOC_N(AMresult *, stack->capacity);
}

static void stack_push(amrb_result_stack_t *stack, AMresult *result) {
    if (stack->len == stack->capacity) {
        REALLOC_N(stack->items, AMresult *, stack->capacity * 2);
        stack->capacity *= 2;
    }
    stack->items[stack->len++] = result;
}

static void stack_free(amrb_result_stack_t *stack) {
    for (long i = 0; i < stack->len; i++) {
        AMresultFree(stack->items[i]);
    }
    xfree(stack->items);
    stack->items = NULL;
    stack->len = 0;
    stack->capacity = 0;
}

static void doc_free(void *ptr) {
    amrb_doc_t *doc = (amrb_doc_t *)ptr;
    if (doc) {
        if (doc->result) {
            AMresultFree(doc->result);
        }
        xfree(doc);
    }
}

static size_t doc_memsize(const void *ptr) {
    (void)ptr;
    return sizeof(amrb_doc_t);
}

static const rb_data_type_t amrb_doc_type = {
    "Automerge::Document",
    {NULL, doc_free, doc_memsize},
    NULL, NULL,
    RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE doc_alloc(VALUE klass) {
    amrb_doc_t *doc = ALLOC(amrb_doc_t);
    doc->result = NULL;
    doc->doc = NULL;
    return TypedData_Wrap_Struct(klass, &amrb_doc_type, doc);
}

static amrb_doc_t *get_doc(VALUE self) {
    amrb_doc_t *doc;
    TypedData_Get_Struct(self, amrb_doc_t, &amrb_doc_type, doc);
    if (!doc || !doc->doc) {
        rb_raise(cError, "uninitialized Automerge document");
    }
    return doc;
}

static VALUE doc_from_result(VALUE klass, AMresult *result) {
    check_result(result);
    AMitem *item = AMresultItem(result);
    AMdoc *native_doc = NULL;
    if (!item || !AMitemToDoc(item, &native_doc) || !native_doc) {
        AMresultFree(result);
        rb_raise(cError, "Automerge result did not contain a document");
    }

    VALUE obj = doc_alloc(klass);
    amrb_doc_t *doc;
    TypedData_Get_Struct(obj, amrb_doc_t, &amrb_doc_type, doc);
    doc->result = result;
    doc->doc = native_doc;
    return obj;
}

static void sync_state_free(void *ptr) {
    amrb_sync_state_t *state = (amrb_sync_state_t *)ptr;
    if (state) {
        if (state->result) {
            AMresultFree(state->result);
        }
        xfree(state);
    }
}

static size_t sync_state_memsize(const void *ptr) {
    (void)ptr;
    return sizeof(amrb_sync_state_t);
}

static const rb_data_type_t amrb_sync_state_type = {
    "Automerge::SyncState",
    {NULL, sync_state_free, sync_state_memsize},
    NULL, NULL,
    RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE sync_state_alloc(VALUE klass) {
    amrb_sync_state_t *state = ALLOC(amrb_sync_state_t);
    state->result = NULL;
    state->state = NULL;
    return TypedData_Wrap_Struct(klass, &amrb_sync_state_type, state);
}

static amrb_sync_state_t *get_sync_state(VALUE self) {
    amrb_sync_state_t *state;
    TypedData_Get_Struct(self, amrb_sync_state_t, &amrb_sync_state_type, state);
    if (!state || !state->state) {
        rb_raise(cError, "uninitialized Automerge sync state");
    }
    return state;
}

static VALUE sync_state_from_result(VALUE klass, AMresult *result) {
    check_result(result);
    AMitem *item = AMresultItem(result);
    AMsyncState *native_state = NULL;
    if (!item || !AMitemToSyncState(item, &native_state) || !native_state) {
        AMresultFree(result);
        rb_raise(cError, "Automerge result did not contain a sync state");
    }

    VALUE obj = sync_state_alloc(klass);
    amrb_sync_state_t *state;
    TypedData_Get_Struct(obj, amrb_sync_state_t, &amrb_sync_state_type, state);
    state->result = result;
    state->state = native_state;
    return obj;
}

static AMresult *item_result_from_bytes(VALUE bytes, int change) {
    StringValue(bytes);
    AMbyteSpan span = string_span(bytes);
    return change ? AMchangeFromBytes(span.src, span.count) : AMitemFromChangeHash(span);
}

static AMresult *items_result_from_array(VALUE values, int change) {
    Check_Type(values, T_ARRAY);
    long len = RARRAY_LEN(values);
    AMresult *acc = NULL;
    for (long i = 0; i < len; i++) {
        VALUE bytes = rb_ary_entry(values, i);
        AMresult *next_item = item_result_from_bytes(bytes, change);
        check_result(next_item);
        if (!acc) {
            acc = next_item;
        } else {
            AMresult *joined = AMresultCat(acc, next_item);
            AMresultFree(acc);
            AMresultFree(next_item);
            check_result(joined);
            acc = joined;
        }
    }
    return acc;
}

static VALUE bytes_from_change_item(AMitem *item) {
    AMchange *change = NULL;
    if (!AMitemToChange(item, &change) || !change) {
        rb_raise(cError, "expected Automerge change");
    }
    return binary_string_from_span(AMchangeRawBytes(change));
}

static VALUE bytes_from_item(AMitem *item, int change) {
    AMbyteSpan span = null_span();
    bool ok = change ? AMitemToChangeHash(item, &span) : AMitemToBytes(item, &span);
    if (!ok) {
        rb_raise(cError, "expected Automerge byte value");
    }
    return binary_string_from_span(span);
}

static VALUE array_from_items_result(AMresult *result, int item_kind) {
    check_result(result);
    AMitems items = AMresultItems(result);
    VALUE ary = rb_ary_new2((long)AMitemsSize(&items));
    AMitem *item = NULL;
    while ((item = AMitemsNext(&items, 1)) != NULL) {
        if (item_kind == AM_VAL_TYPE_CHANGE) {
            rb_ary_push(ary, bytes_from_change_item(item));
        } else if (item_kind == AM_VAL_TYPE_CHANGE_HASH) {
            rb_ary_push(ary, bytes_from_item(item, 1));
        } else {
            AMbyteSpan span = null_span();
            if (!AMitemToStr(item, &span)) {
                AMresultFree(result);
                rb_raise(cError, "expected Automerge string item");
            }
            rb_ary_push(ary, utf8_string_from_span(span));
        }
    }
    AMresultFree(result);
    return ary;
}

static VALUE value_from_object(amrb_doc_t *doc, const AMobjId *obj, const AMitems *heads);

static VALUE value_from_item(amrb_doc_t *doc, AMitem *item, const AMitems *heads);

static VALUE value_from_item(amrb_doc_t *doc, AMitem *item, const AMitems *heads) {
    if (!item) {
        return Qnil;
    }

    switch (AMitemValType(item)) {
        case AM_VAL_TYPE_VOID:
        case AM_VAL_TYPE_NULL:
            return Qnil;
        case AM_VAL_TYPE_BOOL: {
            bool value = false;
            if (!AMitemToBool(item, &value)) rb_raise(cError, "expected boolean item");
            return value ? Qtrue : Qfalse;
        }
        case AM_VAL_TYPE_BYTES: {
            AMbyteSpan span = null_span();
            if (!AMitemToBytes(item, &span)) rb_raise(cError, "expected bytes item");
            return scalar_object(cBytes, binary_string_from_span(span));
        }
        case AM_VAL_TYPE_COUNTER: {
            int64_t value = 0;
            if (!AMitemToCounter(item, &value)) rb_raise(cError, "expected counter item");
            return scalar_object(cCounter, LL2NUM(value));
        }
        case AM_VAL_TYPE_F64: {
            double value = 0.0;
            if (!AMitemToF64(item, &value)) rb_raise(cError, "expected float item");
            return DBL2NUM(value);
        }
        case AM_VAL_TYPE_INT: {
            int64_t value = 0;
            if (!AMitemToInt(item, &value)) rb_raise(cError, "expected int item");
            return LL2NUM(value);
        }
        case AM_VAL_TYPE_UINT: {
            uint64_t value = 0;
            if (!AMitemToUint(item, &value)) rb_raise(cError, "expected uint item");
            return scalar_object(cUint, ULL2NUM(value));
        }
        case AM_VAL_TYPE_TIMESTAMP: {
            int64_t value = 0;
            if (!AMitemToTimestamp(item, &value)) rb_raise(cError, "expected timestamp item");
            return scalar_object(cTimestamp, LL2NUM(value));
        }
        case AM_VAL_TYPE_STR: {
            AMbyteSpan span = null_span();
            if (!AMitemToStr(item, &span)) rb_raise(cError, "expected string item");
            return utf8_string_from_span(span);
        }
        case AM_VAL_TYPE_OBJ_TYPE:
            return value_from_object(doc, AMitemObjId(item), heads);
        case AM_VAL_TYPE_CHANGE:
            return bytes_from_change_item(item);
        case AM_VAL_TYPE_CHANGE_HASH:
            return bytes_from_item(item, 1);
        default:
            rb_raise(cError, "unsupported Automerge value type %d", AMitemValType(item));
    }
}

static VALUE value_from_object(amrb_doc_t *doc, const AMobjId *obj, const AMitems *heads) {
    AMobjType type = AMobjObjType(doc->doc, obj);
    if (type == AM_OBJ_TYPE_TEXT) {
        AMresult *result = AMtext(doc->doc, obj, heads);
        check_result(result);
        AMitem *item = AMresultItem(result);
        AMbyteSpan span = null_span();
        if (!item || !AMitemToStr(item, &span)) {
            AMresultFree(result);
            rb_raise(cError, "expected Automerge text value");
        }
        VALUE text = scalar_object(cText, utf8_string_from_span(span));
        AMresultFree(result);
        return text;
    }

    if (type == AM_OBJ_TYPE_MAP) {
        VALUE hash = rb_hash_new();
        AMresult *result = AMmapRange(doc->doc, obj, null_span(), null_span(), heads);
        check_result(result);
        AMitems items = AMresultItems(result);
        AMitem *item = NULL;
        while ((item = AMitemsNext(&items, 1)) != NULL) {
            AMbyteSpan key = null_span();
            if (AMitemKey(item, &key)) {
                rb_hash_aset(hash, utf8_string_from_span(key), value_from_item(doc, item, heads));
            }
        }
        AMresultFree(result);
        return hash;
    }

    if (type == AM_OBJ_TYPE_LIST) {
        size_t size = AMobjSize(doc->doc, obj, heads);
        VALUE ary = rb_ary_new2((long)size);
        AMresult *result = AMlistRange(doc->doc, obj, 0, size, heads);
        check_result(result);
        AMitems items = AMresultItems(result);
        AMitem *item = NULL;
        while ((item = AMitemsNext(&items, 1)) != NULL) {
            rb_ary_push(ary, value_from_item(doc, item, heads));
        }
        AMresultFree(result);
        return ary;
    }

    rb_raise(cError, "unsupported Automerge object type %d", type);
}

static AMresult *get_child_result(amrb_doc_t *doc, const AMobjId *obj, VALUE segment, const AMitems *heads) {
    AMobjType type = AMobjObjType(doc->doc, obj);
    if (type == AM_OBJ_TYPE_MAP) {
        VALUE key = path_key_string(segment);
        return AMmapGet(doc->doc, obj, string_span(key), heads);
    }
    if (type == AM_OBJ_TYPE_LIST) {
        return AMlistGet(doc->doc, obj, path_index(segment), heads);
    }
    rb_raise(cError, "cannot descend into Automerge object type %d", type);
}

static const AMobjId *resolve_object(amrb_doc_t *doc, VALUE path, amrb_result_stack_t *stack, const AMitems *heads) {
    VALUE ary = path_array(path);
    long len = RARRAY_LEN(ary);
    const AMobjId *obj = NULL;

    for (long i = 0; i < len; i++) {
        AMresult *result = get_child_result(doc, obj, rb_ary_entry(ary, i), heads);
        check_result(result);
        AMitem *item = AMresultItem(result);
        if (!item || AMitemValType(item) != AM_VAL_TYPE_OBJ_TYPE) {
            AMresultFree(result);
            stack_free(stack);
            rb_raise(cError, "path does not resolve to an Automerge object");
        }
        obj = AMitemObjId(item);
        stack_push(stack, result);
    }
    return obj;
}

static const AMobjId *resolve_parent(amrb_doc_t *doc, VALUE path, VALUE *last_segment, amrb_result_stack_t *stack, const AMitems *heads) {
    VALUE ary = path_array(path);
    long len = RARRAY_LEN(ary);
    if (len == 0) {
        rb_raise(rb_eArgError, "path must not be empty");
    }
    *last_segment = rb_ary_entry(ary, len - 1);

    const AMobjId *obj = NULL;
    for (long i = 0; i < len - 1; i++) {
        AMresult *result = get_child_result(doc, obj, rb_ary_entry(ary, i), heads);
        check_result(result);
        AMitem *item = AMresultItem(result);
        if (!item || AMitemValType(item) != AM_VAL_TYPE_OBJ_TYPE) {
            AMresultFree(result);
            stack_free(stack);
            rb_raise(cError, "path parent does not resolve to an Automerge object");
        }
        obj = AMitemObjId(item);
        stack_push(stack, result);
    }
    return obj;
}

/* Convert a Ruby heads array (array of binary 32-byte change hashes) into a
 * heap-allocated AMresult containing AMchangeHash items. Returns NULL when the
 * Ruby value is nil. The caller owns the returned AMresult and must call
 * AMresultFree. */
static AMresult *heads_result_from_ruby(VALUE heads) {
    if (NIL_P(heads)) return NULL;
    return items_result_from_array(heads, 0);
}

/* Helper to extract a borrowed AMitems view from an AMresult, or set the
 * provided AMitems * to NULL when the result is NULL. */
static const AMitems *heads_items_view(AMresult *head_result, AMitems *out) {
    if (!head_result) return NULL;
    *out = AMresultItems(head_result);
    return out;
}

static void put_ruby_value_map(amrb_doc_t *doc, const AMobjId *obj, VALUE key_value, VALUE value);
static void put_ruby_value_list(amrb_doc_t *doc, const AMobjId *obj, size_t pos, bool insert, VALUE value);

static int fill_hash_i(VALUE key, VALUE value, VALUE payload) {
    VALUE *values = (VALUE *)payload;
    amrb_doc_t *doc = (amrb_doc_t *)values[0];
    const AMobjId *obj = (const AMobjId *)values[1];
    put_ruby_value_map(doc, obj, key, value);
    return ST_CONTINUE;
}

static void fill_hash(amrb_doc_t *doc, const AMobjId *obj, VALUE hash) {
    VALUE payload[2];
    payload[0] = (VALUE)doc;
    payload[1] = (VALUE)obj;
    rb_hash_foreach(hash, fill_hash_i, (VALUE)payload);
}

static void fill_array(amrb_doc_t *doc, const AMobjId *obj, VALUE array) {
    long len = RARRAY_LEN(array);
    for (long i = 0; i < len; i++) {
        put_ruby_value_list(doc, obj, (size_t)i, true, rb_ary_entry(array, i));
    }
}

static void fill_text(amrb_doc_t *doc, const AMobjId *obj, VALUE text) {
    VALUE value = rb_ivar_get(text, id_value);
    StringValue(value);
    checked_free_result(AMspliceText(doc->doc, obj, 0, 0, string_span(value)));
}

static AMresult *scalar_item_result_from_ruby(VALUE value) {
    if (NIL_P(value)) {
        return AMitemFromNull();
    }
    if (value == Qtrue || value == Qfalse) {
        return AMitemFromBool(value == Qtrue);
    }
    if (RB_INTEGER_TYPE_P(value)) {
        return AMitemFromInt(NUM2LL(value));
    }
    if (RB_FLOAT_TYPE_P(value)) {
        return AMitemFromF64(NUM2DBL(value));
    }
    if (RB_TYPE_P(value, T_STRING)) {
        return AMitemFromStr(string_span(value));
    }
    if (rb_obj_is_kind_of(value, cCounter)) {
        return AMitemFromCounter(NUM2LL(rb_ivar_get(value, id_value)));
    }
    if (rb_obj_is_kind_of(value, cTimestamp)) {
        return AMitemFromTimestamp(NUM2LL(rb_ivar_get(value, id_value)));
    }
    if (rb_obj_is_kind_of(value, cUint)) {
        return AMitemFromUint(NUM2ULL(rb_ivar_get(value, id_value)));
    }
    if (rb_obj_is_kind_of(value, cBytes)) {
        VALUE bytes = rb_ivar_get(value, id_value);
        StringValue(bytes);
        AMbyteSpan span = string_span(bytes);
        return AMitemFromBytes(span.src, span.count);
    }
    rb_raise(rb_eTypeError, "mark values must be Automerge scalar values");
}

static const AMobjId *obj_id_from_result(AMresult *result) {
    check_result(result);
    AMitem *item = AMresultItem(result);
    const AMobjId *obj = item ? AMitemObjId(item) : NULL;
    if (!obj) {
        AMresultFree(result);
        rb_raise(cError, "Automerge result did not contain an object id");
    }
    return obj;
}

static void put_ruby_value_map(amrb_doc_t *doc, const AMobjId *obj, VALUE key_value, VALUE value) {
    VALUE key = path_key_string(key_value);
    AMbyteSpan key_span = string_span(key);
    AMresult *result = NULL;

    if (NIL_P(value)) {
        checked_free_result(AMmapPutNull(doc->doc, obj, key_span));
    } else if (value == Qtrue || value == Qfalse) {
        checked_free_result(AMmapPutBool(doc->doc, obj, key_span, value == Qtrue));
    } else if (RB_INTEGER_TYPE_P(value)) {
        checked_free_result(AMmapPutInt(doc->doc, obj, key_span, NUM2LL(value)));
    } else if (RB_FLOAT_TYPE_P(value)) {
        checked_free_result(AMmapPutF64(doc->doc, obj, key_span, NUM2DBL(value)));
    } else if (RB_TYPE_P(value, T_STRING)) {
        checked_free_result(AMmapPutStr(doc->doc, obj, key_span, string_span(value)));
    } else if (rb_obj_is_kind_of(value, cCounter)) {
        checked_free_result(AMmapPutCounter(doc->doc, obj, key_span, NUM2LL(rb_ivar_get(value, id_value))));
    } else if (rb_obj_is_kind_of(value, cTimestamp)) {
        checked_free_result(AMmapPutTimestamp(doc->doc, obj, key_span, NUM2LL(rb_ivar_get(value, id_value))));
    } else if (rb_obj_is_kind_of(value, cUint)) {
        checked_free_result(AMmapPutUint(doc->doc, obj, key_span, NUM2ULL(rb_ivar_get(value, id_value))));
    } else if (rb_obj_is_kind_of(value, cBytes)) {
        VALUE bytes = rb_ivar_get(value, id_value);
        StringValue(bytes);
        checked_free_result(AMmapPutBytes(doc->doc, obj, key_span, string_span(bytes)));
    } else if (rb_obj_is_kind_of(value, cText)) {
        result = AMmapPutObject(doc->doc, obj, key_span, AM_OBJ_TYPE_TEXT);
        const AMobjId *child = obj_id_from_result(result);
        fill_text(doc, child, value);
        AMresultFree(result);
    } else if (RB_TYPE_P(value, T_HASH)) {
        result = AMmapPutObject(doc->doc, obj, key_span, AM_OBJ_TYPE_MAP);
        const AMobjId *child = obj_id_from_result(result);
        fill_hash(doc, child, value);
        AMresultFree(result);
    } else if (RB_TYPE_P(value, T_ARRAY)) {
        result = AMmapPutObject(doc->doc, obj, key_span, AM_OBJ_TYPE_LIST);
        const AMobjId *child = obj_id_from_result(result);
        fill_array(doc, child, value);
        AMresultFree(result);
    } else {
        rb_raise(rb_eTypeError, "unsupported Automerge value type: %"PRIsVALUE, rb_obj_class(value));
    }
}

static void put_ruby_value_list(amrb_doc_t *doc, const AMobjId *obj, size_t pos, bool insert, VALUE value) {
    AMresult *result = NULL;
    if (NIL_P(value)) {
        checked_free_result(AMlistPutNull(doc->doc, obj, pos, insert));
    } else if (value == Qtrue || value == Qfalse) {
        checked_free_result(AMlistPutBool(doc->doc, obj, pos, insert, value == Qtrue));
    } else if (RB_INTEGER_TYPE_P(value)) {
        checked_free_result(AMlistPutInt(doc->doc, obj, pos, insert, NUM2LL(value)));
    } else if (RB_FLOAT_TYPE_P(value)) {
        checked_free_result(AMlistPutF64(doc->doc, obj, pos, insert, NUM2DBL(value)));
    } else if (RB_TYPE_P(value, T_STRING)) {
        checked_free_result(AMlistPutStr(doc->doc, obj, pos, insert, string_span(value)));
    } else if (rb_obj_is_kind_of(value, cCounter)) {
        checked_free_result(AMlistPutCounter(doc->doc, obj, pos, insert, NUM2LL(rb_ivar_get(value, id_value))));
    } else if (rb_obj_is_kind_of(value, cTimestamp)) {
        checked_free_result(AMlistPutTimestamp(doc->doc, obj, pos, insert, NUM2LL(rb_ivar_get(value, id_value))));
    } else if (rb_obj_is_kind_of(value, cUint)) {
        checked_free_result(AMlistPutUint(doc->doc, obj, pos, insert, NUM2ULL(rb_ivar_get(value, id_value))));
    } else if (rb_obj_is_kind_of(value, cBytes)) {
        VALUE bytes = rb_ivar_get(value, id_value);
        StringValue(bytes);
        checked_free_result(AMlistPutBytes(doc->doc, obj, pos, insert, string_span(bytes)));
    } else if (rb_obj_is_kind_of(value, cText)) {
        result = AMlistPutObject(doc->doc, obj, pos, insert, AM_OBJ_TYPE_TEXT);
        const AMobjId *child = obj_id_from_result(result);
        fill_text(doc, child, value);
        AMresultFree(result);
    } else if (RB_TYPE_P(value, T_HASH)) {
        result = AMlistPutObject(doc->doc, obj, pos, insert, AM_OBJ_TYPE_MAP);
        const AMobjId *child = obj_id_from_result(result);
        fill_hash(doc, child, value);
        AMresultFree(result);
    } else if (RB_TYPE_P(value, T_ARRAY)) {
        result = AMlistPutObject(doc->doc, obj, pos, insert, AM_OBJ_TYPE_LIST);
        const AMobjId *child = obj_id_from_result(result);
        fill_array(doc, child, value);
        AMresultFree(result);
    } else {
        rb_raise(rb_eTypeError, "unsupported Automerge value type: %"PRIsVALUE, rb_obj_class(value));
    }
}

static VALUE doc_initialize(int argc, VALUE *argv, VALUE self) {
    VALUE arg = Qnil;
    rb_scan_args(argc, argv, "01", &arg);

    VALUE actor = Qnil;
    if (RB_TYPE_P(arg, T_HASH)) {
        actor = rb_hash_aref(arg, ID2SYM(id_actor_id));
    } else {
        actor = arg;
    }

    AMactorId *actor_id = NULL;
    AMresult *actor_result = NULL;
    if (!NIL_P(actor)) {
        StringValue(actor);
        actor_result = AMactorIdFromStr(string_span(actor));
        check_result(actor_result);
        AMitem *actor_item = AMresultItem(actor_result);
        const AMactorId *actor_ptr = NULL;
        if (!actor_item || !AMitemToActorId(actor_item, &actor_ptr)) {
            AMresultFree(actor_result);
            rb_raise(cError, "invalid actor id");
        }
        actor_id = (AMactorId *)actor_ptr;
    }

    AMresult *result = AMcreate(actor_id);
    if (actor_result) {
        AMresultFree(actor_result);
    }

    VALUE obj = doc_from_result(CLASS_OF(self), result);
    amrb_doc_t *src;
    amrb_doc_t *dst;
    TypedData_Get_Struct(obj, amrb_doc_t, &amrb_doc_type, src);
    TypedData_Get_Struct(self, amrb_doc_t, &amrb_doc_type, dst);
    dst->result = src->result;
    dst->doc = src->doc;
    src->result = NULL;
    src->doc = NULL;
    return self;
}

static VALUE doc_s_load(VALUE klass, VALUE bytes) {
    StringValue(bytes);
    AMbyteSpan span = string_span(bytes);
    return doc_from_result(klass, AMload(span.src, span.count));
}

static VALUE doc_clone(VALUE self) {
    amrb_doc_t *doc = get_doc(self);
    return doc_from_result(CLASS_OF(self), AMclone(doc->doc));
}

static VALUE doc_equal(VALUE self, VALUE other) {
    amrb_doc_t *doc = get_doc(self);
    amrb_doc_t *other_doc = get_doc(other);
    return AMequal(doc->doc, other_doc->doc) ? Qtrue : Qfalse;
}

static VALUE doc_get(int argc, VALUE *argv, VALUE self) {
    VALUE path = Qnil;
    VALUE opts = Qnil;
    rb_scan_args(argc, argv, "1:", &path, &opts);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NULL;
    if (!NIL_P(opts)) {
        VALUE heads = rb_hash_aref(opts, ID2SYM(rb_intern2("heads", 5)));
        head_result = heads_result_from_ruby(heads);
    }
    AMitems head_items;
    const AMitems *heads = heads_items_view(head_result, &head_items);

    VALUE ary = path_array(path);
    VALUE value;
    if (RARRAY_LEN(ary) == 0) {
        value = value_from_object(doc, NULL, heads);
    } else {
        amrb_result_stack_t stack;
        stack_init(&stack, RARRAY_LEN(ary));
        const AMobjId *parent = NULL;
        VALUE last = Qnil;
        parent = resolve_parent(doc, ary, &last, &stack, heads);
        AMresult *result = get_child_result(doc, parent, last, heads);
        check_result(result);
        value = value_from_item(doc, AMresultItem(result), heads);
        AMresultFree(result);
        stack_free(&stack);
    }
    if (head_result) AMresultFree(head_result);
    return value;
}

static VALUE values_array_from_result(amrb_doc_t *doc, AMresult *result, const AMitems *heads) {
    check_result(result);
    AMitems items = AMresultItems(result);
    VALUE ary = rb_ary_new2((long)AMitemsSize(&items));
    AMitem *item = NULL;
    while ((item = AMitemsNext(&items, 1)) != NULL) {
        rb_ary_push(ary, value_from_item(doc, item, heads));
    }
    AMresultFree(result);
    return ary;
}

static VALUE doc_get_all(int argc, VALUE *argv, VALUE self) {
    VALUE path = Qnil;
    VALUE opts = Qnil;
    rb_scan_args(argc, argv, "1:", &path, &opts);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NULL;
    if (!NIL_P(opts)) {
        head_result = heads_result_from_ruby(rb_hash_aref(opts, ID2SYM(rb_intern2("heads", 5))));
    }
    AMitems head_items;
    const AMitems *heads = heads_items_view(head_result, &head_items);

    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    VALUE last = Qnil;
    const AMobjId *parent = resolve_parent(doc, ary, &last, &stack, heads);
    AMobjType type = AMobjObjType(doc->doc, parent);
    VALUE values;
    if (type == AM_OBJ_TYPE_MAP) {
        VALUE key = path_key_string(last);
        values = values_array_from_result(doc, AMmapGetAll(doc->doc, parent, string_span(key), heads), heads);
    } else if (type == AM_OBJ_TYPE_LIST) {
        values = values_array_from_result(doc, AMlistGetAll(doc->doc, parent, path_index(last), heads), heads);
    } else {
        stack_free(&stack);
        if (head_result) AMresultFree(head_result);
        rb_raise(cError, "cannot read conflicts from Automerge object type %d", type);
    }
    stack_free(&stack);
    if (head_result) AMresultFree(head_result);
    return values;
}

static VALUE doc_put(VALUE self, VALUE path, VALUE value) {
    amrb_doc_t *doc = get_doc(self);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    VALUE last = Qnil;
    const AMobjId *parent = resolve_parent(doc, ary, &last, &stack, NULL);
    AMobjType type = AMobjObjType(doc->doc, parent);
    if (type == AM_OBJ_TYPE_MAP) {
        put_ruby_value_map(doc, parent, last, value);
    } else if (type == AM_OBJ_TYPE_LIST) {
        put_ruby_value_list(doc, parent, path_index(last), false, value);
    } else {
        stack_free(&stack);
        rb_raise(cError, "cannot put into Automerge object type %d", type);
    }
    stack_free(&stack);
    return self;
}

static VALUE doc_insert(VALUE self, VALUE path, VALUE index, VALUE value) {
    amrb_doc_t *doc = get_doc(self);
    amrb_result_stack_t stack;
    VALUE ary = path_array(path);
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, NULL);
    if (AMobjObjType(doc->doc, obj) != AM_OBJ_TYPE_LIST) {
        stack_free(&stack);
        rb_raise(cError, "insert target must be an Automerge list");
    }
    put_ruby_value_list(doc, obj, path_index(index), true, value);
    stack_free(&stack);
    return self;
}

static VALUE doc_delete(VALUE self, VALUE path) {
    amrb_doc_t *doc = get_doc(self);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    VALUE last = Qnil;
    const AMobjId *parent = resolve_parent(doc, ary, &last, &stack, NULL);
    AMobjType type = AMobjObjType(doc->doc, parent);
    if (type == AM_OBJ_TYPE_MAP) {
        VALUE key = path_key_string(last);
        checked_free_result(AMmapDelete(doc->doc, parent, string_span(key)));
    } else if (type == AM_OBJ_TYPE_LIST) {
        checked_free_result(AMlistDelete(doc->doc, parent, path_index(last)));
    } else {
        stack_free(&stack);
        rb_raise(cError, "cannot delete from Automerge object type %d", type);
    }
    stack_free(&stack);
    return self;
}

static VALUE doc_increment(VALUE self, VALUE path, VALUE amount) {
    amrb_doc_t *doc = get_doc(self);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    VALUE last = Qnil;
    const AMobjId *parent = resolve_parent(doc, ary, &last, &stack, NULL);
    int64_t delta = NUM2LL(amount);
    AMobjType type = AMobjObjType(doc->doc, parent);
    if (type == AM_OBJ_TYPE_MAP) {
        VALUE key = path_key_string(last);
        checked_free_result(AMmapIncrement(doc->doc, parent, string_span(key), delta));
    } else if (type == AM_OBJ_TYPE_LIST) {
        checked_free_result(AMlistIncrement(doc->doc, parent, path_index(last), delta));
    } else {
        stack_free(&stack);
        rb_raise(cError, "cannot increment in Automerge object type %d", type);
    }
    stack_free(&stack);
    return self;
}

static VALUE doc_splice_text(VALUE self, VALUE path, VALUE index, VALUE delete_count, VALUE text) {
    amrb_doc_t *doc = get_doc(self);
    StringValue(text);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, NULL);
    if (AMobjObjType(doc->doc, obj) != AM_OBJ_TYPE_TEXT) {
        stack_free(&stack);
        rb_raise(cError, "splice_text target must be Automerge text");
    }
    checked_free_result(AMspliceText(doc->doc, obj, (size_t)NUM2LONG(index), (ptrdiff_t)NUM2LONG(delete_count), string_span(text)));
    stack_free(&stack);
    return self;
}

static VALUE doc_splice_list(VALUE self, VALUE path, VALUE index, VALUE delete_count, VALUE values) {
    amrb_doc_t *doc = get_doc(self);
    Check_Type(values, T_ARRAY);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, NULL);
    if (AMobjObjType(doc->doc, obj) != AM_OBJ_TYPE_LIST) {
        stack_free(&stack);
        rb_raise(cError, "splice target must be an Automerge list");
    }
    size_t pos = (size_t)NUM2LONG(index);
    long del = NUM2LONG(delete_count);
    for (long i = 0; i < del; i++) {
        checked_free_result(AMlistDelete(doc->doc, obj, pos));
    }
    for (long i = 0; i < RARRAY_LEN(values); i++) {
        put_ruby_value_list(doc, obj, pos + (size_t)i, true, rb_ary_entry(values, i));
    }
    stack_free(&stack);
    return self;
}

static VALUE doc_keys(int argc, VALUE *argv, VALUE self) {
    VALUE path = Qnil;
    VALUE opts = Qnil;
    rb_scan_args(argc, argv, "01:", &path, &opts);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NULL;
    if (!NIL_P(opts)) {
        head_result = heads_result_from_ruby(rb_hash_aref(opts, ID2SYM(rb_intern2("heads", 5))));
    }
    AMitems head_items;
    const AMitems *heads = heads_items_view(head_result, &head_items);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, heads);
    VALUE keys = array_from_items_result(AMkeys(doc->doc, obj, heads), AM_VAL_TYPE_STR);
    stack_free(&stack);
    if (head_result) AMresultFree(head_result);
    return keys;
}

static VALUE doc_length(int argc, VALUE *argv, VALUE self) {
    VALUE path = Qnil;
    VALUE opts = Qnil;
    rb_scan_args(argc, argv, "01:", &path, &opts);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NULL;
    if (!NIL_P(opts)) {
        head_result = heads_result_from_ruby(rb_hash_aref(opts, ID2SYM(rb_intern2("heads", 5))));
    }
    AMitems head_items;
    const AMitems *heads = heads_items_view(head_result, &head_items);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, heads);
    size_t size = AMobjSize(doc->doc, obj, heads);
    stack_free(&stack);
    if (head_result) AMresultFree(head_result);
    return SIZET2NUM(size);
}

static VALUE doc_cursor(int argc, VALUE *argv, VALUE self) {
    VALUE path = Qnil;
    VALUE position = Qnil;
    VALUE opts = Qnil;
    rb_scan_args(argc, argv, "20:", &path, &position, &opts);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NULL;
    if (!NIL_P(opts)) {
        head_result = heads_result_from_ruby(rb_hash_aref(opts, ID2SYM(rb_intern2("heads", 5))));
    }
    AMitems head_items;
    const AMitems *heads = heads_items_view(head_result, &head_items);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, heads);
    AMresult *result = AMgetCursor(doc->doc, obj, (size_t)NUM2LONG(position), heads);
    check_result(result);
    AMitem *item = AMresultItem(result);
    const AMcursor *cursor = NULL;
    if (!item || !AMitemToCursor(item, &cursor)) {
        AMresultFree(result);
        stack_free(&stack);
        if (head_result) AMresultFree(head_result);
        rb_raise(cError, "expected Automerge cursor");
    }
    VALUE bytes = binary_string_from_span(AMcursorBytes(cursor));
    AMresultFree(result);
    stack_free(&stack);
    if (head_result) AMresultFree(head_result);
    return bytes;
}

static VALUE doc_cursor_position(int argc, VALUE *argv, VALUE self) {
    VALUE path = Qnil;
    VALUE cursor_bytes = Qnil;
    VALUE opts = Qnil;
    rb_scan_args(argc, argv, "20:", &path, &cursor_bytes, &opts);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NULL;
    if (!NIL_P(opts)) {
        head_result = heads_result_from_ruby(rb_hash_aref(opts, ID2SYM(rb_intern2("heads", 5))));
    }
    AMitems head_items;
    const AMitems *heads = heads_items_view(head_result, &head_items);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, heads);
    StringValue(cursor_bytes);
    AMbyteSpan span = string_span(cursor_bytes);
    AMresult *cursor_result = AMcursorFromBytes(span.src, span.count);
    check_result(cursor_result);
    AMitem *cursor_item = AMresultItem(cursor_result);
    const AMcursor *cursor = NULL;
    if (!cursor_item || !AMitemToCursor(cursor_item, &cursor)) {
        AMresultFree(cursor_result);
        stack_free(&stack);
        if (head_result) AMresultFree(head_result);
        rb_raise(cError, "invalid Automerge cursor");
    }
    AMresult *result = AMgetCursorPosition(doc->doc, obj, cursor, heads);
    check_result(result);
    AMitem *item = AMresultItem(result);
    uint64_t pos = 0;
    if (!AMitemToUint(item, &pos)) {
        AMresultFree(result);
        AMresultFree(cursor_result);
        stack_free(&stack);
        if (head_result) AMresultFree(head_result);
        rb_raise(cError, "expected cursor position");
    }
    AMresultFree(result);
    AMresultFree(cursor_result);
    stack_free(&stack);
    if (head_result) AMresultFree(head_result);
    return ULL2NUM(pos);
}

static AMmarkExpand mark_expand_from_value(VALUE value) {
    if (NIL_P(value)) return AM_MARK_EXPAND_NONE;
    if (RB_TYPE_P(value, T_SYMBOL)) {
        VALUE str = rb_sym2str(value);
        value = str;
    }
    StringValue(value);
    if (RSTRING_LEN(value) == 4 && strncmp(RSTRING_PTR(value), "none", 4) == 0) return AM_MARK_EXPAND_NONE;
    if (RSTRING_LEN(value) == 6 && strncmp(RSTRING_PTR(value), "before", 6) == 0) return AM_MARK_EXPAND_BEFORE;
    if (RSTRING_LEN(value) == 5 && strncmp(RSTRING_PTR(value), "after", 5) == 0) return AM_MARK_EXPAND_AFTER;
    if (RSTRING_LEN(value) == 4 && strncmp(RSTRING_PTR(value), "both", 4) == 0) return AM_MARK_EXPAND_BOTH;
    rb_raise(rb_eArgError, "mark expand must be one of: none, before, after, both");
}

static VALUE mark_hash_from_item(amrb_doc_t *doc, AMitem *item, const AMitems *heads) {
    const AMmark *mark = NULL;
    if (!item || !AMitemToMark(item, &mark) || !mark) {
        rb_raise(cError, "expected Automerge mark");
    }
    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, ID2SYM(rb_intern2("name", 4)), utf8_string_from_span(AMmarkName(mark)));
    rb_hash_aset(hash, ID2SYM(rb_intern2("start", 5)), SIZET2NUM(AMmarkStart(mark)));
    rb_hash_aset(hash, ID2SYM(rb_intern2("end", 3)), SIZET2NUM(AMmarkEnd(mark)));
    AMresult *value_result = AMmarkValue(mark);
    check_result(value_result);
    rb_hash_aset(hash, ID2SYM(id_value_key), value_from_item(doc, AMresultItem(value_result), heads));
    AMresultFree(value_result);
    return hash;
}

static VALUE doc_marks(int argc, VALUE *argv, VALUE self) {
    VALUE path = Qnil;
    VALUE opts = Qnil;
    rb_scan_args(argc, argv, "1:", &path, &opts);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NULL;
    if (!NIL_P(opts)) {
        head_result = heads_result_from_ruby(rb_hash_aref(opts, ID2SYM(rb_intern2("heads", 5))));
    }
    AMitems head_items;
    const AMitems *heads = heads_items_view(head_result, &head_items);
    VALUE ary = path_array(path);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, heads);
    AMresult *result = AMmarks(doc->doc, obj, heads);
    check_result(result);
    AMitems items = AMresultItems(result);
    VALUE marks = rb_ary_new2((long)AMitemsSize(&items));
    AMitem *item = NULL;
    while ((item = AMitemsNext(&items, 1)) != NULL) {
        rb_ary_push(marks, mark_hash_from_item(doc, item, heads));
    }
    AMresultFree(result);
    stack_free(&stack);
    if (head_result) AMresultFree(head_result);
    return marks;
}

static VALUE doc_mark(int argc, VALUE *argv, VALUE self) {
    VALUE path, start, end, name, value, expand;
    rb_scan_args(argc, argv, "51", &path, &start, &end, &name, &value, &expand);
    amrb_doc_t *doc = get_doc(self);
    VALUE ary = path_array(path);
    VALUE mark_name = path_key_string(name);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, NULL);
    AMresult *value_result = scalar_item_result_from_ruby(value);
    check_result(value_result);
    AMitem *value_item = AMresultItem(value_result);
    checked_free_result(AMmarkCreate(doc->doc, obj, (size_t)NUM2LONG(start), (size_t)NUM2LONG(end), mark_expand_from_value(expand), string_span(mark_name), value_item));
    AMresultFree(value_result);
    stack_free(&stack);
    return self;
}

static VALUE doc_unmark(int argc, VALUE *argv, VALUE self) {
    VALUE path, start, end, name, expand;
    rb_scan_args(argc, argv, "41", &path, &start, &end, &name, &expand);
    amrb_doc_t *doc = get_doc(self);
    VALUE ary = path_array(path);
    VALUE mark_name = path_key_string(name);
    amrb_result_stack_t stack;
    stack_init(&stack, RARRAY_LEN(ary));
    const AMobjId *obj = resolve_object(doc, ary, &stack, NULL);
    checked_free_result(AMmarkClear(doc->doc, obj, (size_t)NUM2LONG(start), (size_t)NUM2LONG(end), mark_expand_from_value(expand), string_span(mark_name)));
    stack_free(&stack);
    return self;
}

static VALUE result_hash_or_nil(AMresult *result) {
    check_result(result);
    AMitem *item = AMresultItem(result);
    if (!item || AMitemValType(item) == AM_VAL_TYPE_VOID) {
        AMresultFree(result);
        return Qnil;
    }
    AMbyteSpan hash = null_span();
    if (!AMitemToChangeHash(item, &hash)) {
        AMresultFree(result);
        rb_raise(cError, "expected change hash");
    }
    VALUE value = binary_string_from_span(hash);
    AMresultFree(result);
    return value;
}

static VALUE doc_commit(int argc, VALUE *argv, VALUE self) {
    VALUE message = Qnil;
    VALUE timestamp = Qnil;
    rb_scan_args(argc, argv, "02", &message, &timestamp);
    if (RB_TYPE_P(message, T_HASH)) {
        VALUE opts = message;
        message = rb_hash_aref(opts, ID2SYM(id_message));
        timestamp = rb_hash_aref(opts, ID2SYM(id_timestamp));
    }

    AMbyteSpan span = null_span();
    if (!NIL_P(message)) {
        StringValue(message);
        span = string_span(message);
    }

    int64_t ts = 0;
    int64_t *ts_ptr = NULL;
    if (!NIL_P(timestamp)) {
        ts = NUM2LL(timestamp);
        ts_ptr = &ts;
    }

    amrb_doc_t *doc = get_doc(self);
    return result_hash_or_nil(AMcommit(doc->doc, span, ts_ptr));
}

static VALUE doc_empty_change(int argc, VALUE *argv, VALUE self) {
    VALUE message = Qnil;
    VALUE timestamp = Qnil;
    rb_scan_args(argc, argv, "02", &message, &timestamp);
    if (RB_TYPE_P(message, T_HASH)) {
        VALUE opts = message;
        message = rb_hash_aref(opts, ID2SYM(id_message));
        timestamp = rb_hash_aref(opts, ID2SYM(id_timestamp));
    }

    AMbyteSpan span = null_span();
    if (!NIL_P(message)) {
        StringValue(message);
        span = string_span(message);
    }

    int64_t ts = 0;
    int64_t *ts_ptr = NULL;
    if (!NIL_P(timestamp)) {
        ts = NUM2LL(timestamp);
        ts_ptr = &ts;
    }

    amrb_doc_t *doc = get_doc(self);
    return result_hash_or_nil(AMemptyChange(doc->doc, span, ts_ptr));
}

static VALUE doc_save(VALUE self) {
    amrb_doc_t *doc = get_doc(self);
    AMresult *result = AMsave(doc->doc);
    check_result(result);
    VALUE bytes = bytes_from_item(AMresultItem(result), 0);
    AMresultFree(result);
    return bytes;
}

static VALUE doc_save_incremental(VALUE self) {
    amrb_doc_t *doc = get_doc(self);
    AMresult *result = AMsaveIncremental(doc->doc);
    check_result(result);
    VALUE bytes = bytes_from_item(AMresultItem(result), 0);
    AMresultFree(result);
    return bytes;
}

static VALUE doc_load_incremental(VALUE self, VALUE bytes) {
    amrb_doc_t *doc = get_doc(self);
    StringValue(bytes);
    AMbyteSpan span = string_span(bytes);
    AMresult *result = AMloadIncremental(doc->doc, span.src, span.count);
    check_result(result);
    AMitem *item = AMresultItem(result);
    uint64_t count = 0;
    if (!AMitemToUint(item, &count)) {
        AMresultFree(result);
        rb_raise(cError, "expected loaded change count");
    }
    AMresultFree(result);
    return ULL2NUM(count);
}

static VALUE doc_heads(VALUE self) {
    amrb_doc_t *doc = get_doc(self);
    return array_from_items_result(AMgetHeads(doc->doc), AM_VAL_TYPE_CHANGE_HASH);
}

static VALUE doc_missing_deps(int argc, VALUE *argv, VALUE self) {
    VALUE heads = Qnil;
    rb_scan_args(argc, argv, "01", &heads);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NIL_P(heads) ? NULL : items_result_from_array(heads, 0);
    AMitems items;
    AMitems *items_ptr = NULL;
    if (head_result) {
        items = AMresultItems(head_result);
        items_ptr = &items;
    }
    VALUE missing = array_from_items_result(AMgetMissingDeps(doc->doc, items_ptr), AM_VAL_TYPE_CHANGE_HASH);
    if (head_result) AMresultFree(head_result);
    return missing;
}

static VALUE change_metadata_hash(AMchange *change) {
    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, ID2SYM(rb_intern2("hash", 4)), binary_string_from_span(AMchangeHash(change)));
    AMbyteSpan msg = AMchangeMessage(change);
    if (msg.count > 0) {
        rb_hash_aset(hash, ID2SYM(id_message), utf8_string_from_span(msg));
    } else {
        rb_hash_aset(hash, ID2SYM(id_message), Qnil);
    }
    rb_hash_aset(hash, ID2SYM(id_timestamp), LL2NUM(AMchangeTime(change)));
    rb_hash_aset(hash, ID2SYM(rb_intern2("seq", 3)), ULL2NUM(AMchangeSeq(change)));
    rb_hash_aset(hash, ID2SYM(rb_intern2("max_op", 6)), ULL2NUM(AMchangeMaxOp(change)));
    rb_hash_aset(hash, ID2SYM(rb_intern2("start_op", 8)), ULL2NUM(AMchangeStartOp(change)));
    rb_hash_aset(hash, ID2SYM(rb_intern2("size", 4)), SIZET2NUM(AMchangeSize(change)));
    rb_hash_aset(hash, ID2SYM(rb_intern2("empty", 5)), AMchangeIsEmpty(change) ? Qtrue : Qfalse);

    AMresult *actor_result = AMchangeActorId(change);
    check_result(actor_result);
    AMitem *actor_item = AMresultItem(actor_result);
    const AMactorId *actor = NULL;
    if (actor_item && AMitemToActorId(actor_item, &actor) && actor) {
        rb_hash_aset(hash, ID2SYM(id_actor_id), utf8_string_from_span(AMactorIdStr(actor)));
    }
    AMresultFree(actor_result);

    AMresult *deps_result = AMchangeDeps(change);
    rb_hash_aset(hash, ID2SYM(rb_intern2("deps", 4)),
                 array_from_items_result(deps_result, AM_VAL_TYPE_CHANGE_HASH));

    AMbyteSpan extra = AMchangeExtraBytes(change);
    rb_hash_aset(hash, ID2SYM(rb_intern2("extra_bytes", 11)),
                 extra.count > 0 ? binary_string_from_span(extra) : rb_str_new("", 0));
    return hash;
}

static VALUE doc_s_decode_change(VALUE klass, VALUE bytes) {
    (void)klass;
    StringValue(bytes);
    AMbyteSpan span = string_span(bytes);
    AMresult *result = AMchangeFromBytes(span.src, span.count);
    check_result(result);
    AMitem *item = AMresultItem(result);
    AMchange *change = NULL;
    if (!item || !AMitemToChange(item, &change) || !change) {
        AMresultFree(result);
        rb_raise(cError, "expected Automerge change bytes");
    }
    VALUE meta = change_metadata_hash(change);
    AMresultFree(result);
    return meta;
}

static VALUE doc_change_by_hash(VALUE self, VALUE hash) {
    amrb_doc_t *doc = get_doc(self);
    StringValue(hash);
    AMbyteSpan span = string_span(hash);
    AMresult *result = AMgetChangeByHash(doc->doc, span.src, span.count);
    check_result(result);
    VALUE change = bytes_from_change_item(AMresultItem(result));
    AMresultFree(result);
    return change;
}

static VALUE doc_get_changes(int argc, VALUE *argv, VALUE self) {
    VALUE heads = Qnil;
    rb_scan_args(argc, argv, "01", &heads);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NIL_P(heads) ? NULL : items_result_from_array(heads, 0);
    AMitems items;
    AMitems *items_ptr = NULL;
    if (head_result) {
        items = AMresultItems(head_result);
        items_ptr = &items;
    }
    VALUE changes = array_from_items_result(AMgetChanges(doc->doc, items_ptr), AM_VAL_TYPE_CHANGE);
    if (head_result) AMresultFree(head_result);
    return changes;
}

static VALUE doc_apply_changes(VALUE self, VALUE changes) {
    amrb_doc_t *doc = get_doc(self);
    AMresult *change_result = items_result_from_array(changes, 1);
    if (!change_result) return self;
    AMitems items = AMresultItems(change_result);
    checked_free_result(AMapplyChanges(doc->doc, &items));
    AMresultFree(change_result);
    return self;
}

static VALUE doc_last_local_change(VALUE self) {
    amrb_doc_t *doc = get_doc(self);
    AMresult *result = AMgetLastLocalChange(doc->doc);
    check_result(result);
    AMitem *item = AMresultItem(result);
    VALUE value = Qnil;
    if (item && AMitemValType(item) == AM_VAL_TYPE_CHANGE) {
        value = bytes_from_change_item(item);
    }
    AMresultFree(result);
    return value;
}

static VALUE doc_merge(VALUE self, VALUE other) {
    amrb_doc_t *doc = get_doc(self);
    amrb_doc_t *other_doc = get_doc(other);
    return array_from_items_result(AMmerge(doc->doc, other_doc->doc), AM_VAL_TYPE_CHANGE_HASH);
}

static VALUE doc_changes_added(VALUE self, VALUE other) {
    amrb_doc_t *doc = get_doc(self);
    amrb_doc_t *other_doc = get_doc(other);
    return array_from_items_result(AMgetChangesAdded(doc->doc, other_doc->doc), AM_VAL_TYPE_CHANGE);
}

static VALUE doc_fork(int argc, VALUE *argv, VALUE self) {
    VALUE heads = Qnil;
    rb_scan_args(argc, argv, "01", &heads);
    amrb_doc_t *doc = get_doc(self);
    AMresult *head_result = NIL_P(heads) ? NULL : items_result_from_array(heads, 0);
    AMitems items;
    AMitems *items_ptr = NULL;
    if (head_result) {
        items = AMresultItems(head_result);
        items_ptr = &items;
    }
    VALUE forked = doc_from_result(CLASS_OF(self), AMfork(doc->doc, items_ptr));
    if (head_result) AMresultFree(head_result);
    return forked;
}

static VALUE doc_actor_id(VALUE self) {
    amrb_doc_t *doc = get_doc(self);
    AMresult *result = AMgetActorId(doc->doc);
    check_result(result);
    AMitem *item = AMresultItem(result);
    const AMactorId *actor = NULL;
    if (!item || !AMitemToActorId(item, &actor)) {
        AMresultFree(result);
        rb_raise(cError, "expected actor id");
    }
    VALUE actor_id = utf8_string_from_span(AMactorIdStr(actor));
    AMresultFree(result);
    return actor_id;
}

static VALUE doc_set_actor_id(VALUE self, VALUE actor_id_value) {
    amrb_doc_t *doc = get_doc(self);
    StringValue(actor_id_value);
    AMresult *actor_result = AMactorIdFromStr(string_span(actor_id_value));
    check_result(actor_result);
    AMitem *item = AMresultItem(actor_result);
    const AMactorId *actor = NULL;
    if (!item || !AMitemToActorId(item, &actor)) {
        AMresultFree(actor_result);
        rb_raise(cError, "invalid actor id");
    }
    checked_free_result(AMsetActorId(doc->doc, actor));
    AMresultFree(actor_result);
    return self;
}

static VALUE doc_pending_ops(VALUE self) {
    amrb_doc_t *doc = get_doc(self);
    return SIZET2NUM(AMpendingOps(doc->doc));
}

static VALUE doc_rollback(VALUE self) {
    amrb_doc_t *doc = get_doc(self);
    return SIZET2NUM(AMrollback(doc->doc));
}

static VALUE doc_generate_sync_message(VALUE self, VALUE sync_state_value) {
    amrb_doc_t *doc = get_doc(self);
    amrb_sync_state_t *sync_state = get_sync_state(sync_state_value);
    AMresult *result = AMgenerateSyncMessage(doc->doc, sync_state->state);
    check_result(result);
    AMitem *item = AMresultItem(result);
    if (!item || AMitemValType(item) == AM_VAL_TYPE_VOID) {
        AMresultFree(result);
        return Qnil;
    }
    const AMsyncMessage *message = NULL;
    if (!AMitemToSyncMessage(item, &message) || !message) {
        AMresultFree(result);
        rb_raise(cError, "expected sync message");
    }
    AMresult *encoded = AMsyncMessageEncode(message);
    check_result(encoded);
    VALUE bytes = bytes_from_item(AMresultItem(encoded), 0);
    AMresultFree(encoded);
    AMresultFree(result);
    return bytes;
}

static VALUE doc_receive_sync_message(VALUE self, VALUE sync_state_value, VALUE bytes) {
    amrb_doc_t *doc = get_doc(self);
    amrb_sync_state_t *sync_state = get_sync_state(sync_state_value);
    StringValue(bytes);
    AMbyteSpan span = string_span(bytes);
    AMresult *message_result = AMsyncMessageDecode(span.src, span.count);
    check_result(message_result);
    AMitem *item = AMresultItem(message_result);
    const AMsyncMessage *message = NULL;
    if (!item || !AMitemToSyncMessage(item, &message)) {
        AMresultFree(message_result);
        rb_raise(cError, "invalid sync message");
    }
    checked_free_result(AMreceiveSyncMessage(doc->doc, sync_state->state, message));
    AMresultFree(message_result);
    return self;
}

static VALUE sync_state_initialize(VALUE self) {
    VALUE obj = sync_state_from_result(CLASS_OF(self), AMsyncStateInit());
    amrb_sync_state_t *src;
    amrb_sync_state_t *dst;
    TypedData_Get_Struct(obj, amrb_sync_state_t, &amrb_sync_state_type, src);
    TypedData_Get_Struct(self, amrb_sync_state_t, &amrb_sync_state_type, dst);
    dst->result = src->result;
    dst->state = src->state;
    src->result = NULL;
    src->state = NULL;
    return self;
}

static VALUE sync_state_s_decode(VALUE klass, VALUE bytes) {
    StringValue(bytes);
    AMbyteSpan span = string_span(bytes);
    return sync_state_from_result(klass, AMsyncStateDecode(span.src, span.count));
}

static VALUE sync_state_encode(VALUE self) {
    amrb_sync_state_t *state = get_sync_state(self);
    AMresult *result = AMsyncStateEncode(state->state);
    check_result(result);
    VALUE bytes = bytes_from_item(AMresultItem(result), 0);
    AMresultFree(result);
    return bytes;
}

static VALUE sync_state_equal(VALUE self, VALUE other) {
    amrb_sync_state_t *state = get_sync_state(self);
    amrb_sync_state_t *other_state = get_sync_state(other);
    return AMsyncStateEqual(state->state, other_state->state) ? Qtrue : Qfalse;
}

static VALUE sync_state_shared_heads(VALUE self) {
    amrb_sync_state_t *state = get_sync_state(self);
    return array_from_items_result(AMsyncStateSharedHeads(state->state), AM_VAL_TYPE_CHANGE_HASH);
}

/* Decode the wire bytes of a sync message and return a Hash describing it.
 * Only `heads` and `needs` are surfaced — those are sufficient for
 * automerge-repo to drive its sync state machine (peer document status
 * inference, see DocSynchronizer). */
static VALUE automerge_s_decode_sync_message(VALUE klass, VALUE bytes) {
    (void)klass;
    StringValue(bytes);
    AMbyteSpan span = string_span(bytes);
    AMresult *msg_result = AMsyncMessageDecode(span.src, span.count);
    check_result(msg_result);
    AMitem *msg_item = AMresultItem(msg_result);
    const AMsyncMessage *message = NULL;
    if (!msg_item || !AMitemToSyncMessage(msg_item, &message) || !message) {
        AMresultFree(msg_result);
        rb_raise(cError, "expected Automerge sync message bytes");
    }
    VALUE heads = array_from_items_result(AMsyncMessageHeads(message), AM_VAL_TYPE_CHANGE_HASH);
    VALUE needs = array_from_items_result(AMsyncMessageNeeds(message), AM_VAL_TYPE_CHANGE_HASH);
    AMresultFree(msg_result);

    VALUE info = rb_hash_new();
    rb_hash_aset(info, ID2SYM(rb_intern2("heads", 5)), heads);
    rb_hash_aset(info, ID2SYM(rb_intern2("needs", 5)), needs);
    return info;
}

static VALUE ensure_class_under(VALUE parent, const char *name, VALUE superclass) {
    ID id = rb_intern2(name, (long)strlen(name));
    if (rb_const_defined(parent, id)) {
        return rb_const_get(parent, id);
    }
    return rb_define_class_under(parent, name, superclass);
}

void Init_automerge_ext(void) {
    id_actor_id = rb_intern2("actor_id", 8);
    id_message = rb_intern2("message", 7);
    id_timestamp = rb_intern2("timestamp", 9);
    id_value = rb_intern2("@value", 6);
    id_value_key = rb_intern2("value", 5);

    mAutomerge = rb_define_module("Automerge");
    cError = ensure_class_under(mAutomerge, "Error", rb_eStandardError);
    cCounter = ensure_class_under(mAutomerge, "Counter", rb_cObject);
    cTimestamp = ensure_class_under(mAutomerge, "Timestamp", rb_cObject);
    cBytes = ensure_class_under(mAutomerge, "Bytes", rb_cObject);
    cText = ensure_class_under(mAutomerge, "Text", rb_cObject);
    cUint = ensure_class_under(mAutomerge, "Uint", rb_cObject);

    cDocument = ensure_class_under(mAutomerge, "Document", rb_cObject);
    rb_define_alloc_func(cDocument, doc_alloc);
    rb_define_method(cDocument, "initialize", doc_initialize, -1);
    rb_define_singleton_method(cDocument, "load", doc_s_load, 1);
    rb_define_singleton_method(cDocument, "decode_change", doc_s_decode_change, 1);
    rb_define_method(cDocument, "clone", doc_clone, 0);
    rb_define_method(cDocument, "dup", doc_clone, 0);
    rb_define_method(cDocument, "==", doc_equal, 1);
    rb_define_method(cDocument, "get", doc_get, -1);
    rb_define_method(cDocument, "get_all", doc_get_all, -1);
    rb_define_method(cDocument, "put", doc_put, 2);
    rb_define_method(cDocument, "insert", doc_insert, 3);
    rb_define_method(cDocument, "delete", doc_delete, 1);
    rb_define_method(cDocument, "increment", doc_increment, 2);
    rb_define_method(cDocument, "splice_text", doc_splice_text, 4);
    rb_define_method(cDocument, "splice", doc_splice_list, 4);
    rb_define_method(cDocument, "keys", doc_keys, -1);
    rb_define_method(cDocument, "length", doc_length, -1);
    rb_define_method(cDocument, "cursor", doc_cursor, -1);
    rb_define_method(cDocument, "cursor_position", doc_cursor_position, -1);
    rb_define_method(cDocument, "marks", doc_marks, -1);
    rb_define_method(cDocument, "mark", doc_mark, -1);
    rb_define_method(cDocument, "unmark", doc_unmark, -1);
    rb_define_method(cDocument, "commit", doc_commit, -1);
    rb_define_method(cDocument, "empty_change", doc_empty_change, -1);
    rb_define_method(cDocument, "save", doc_save, 0);
    rb_define_method(cDocument, "save_incremental", doc_save_incremental, 0);
    rb_define_method(cDocument, "load_incremental", doc_load_incremental, 1);
    rb_define_method(cDocument, "heads", doc_heads, 0);
    rb_define_method(cDocument, "missing_deps", doc_missing_deps, -1);
    rb_define_method(cDocument, "change_by_hash", doc_change_by_hash, 1);
    rb_define_method(cDocument, "get_changes", doc_get_changes, -1);
    rb_define_method(cDocument, "apply_changes", doc_apply_changes, 1);
    rb_define_method(cDocument, "last_local_change", doc_last_local_change, 0);
    rb_define_method(cDocument, "merge", doc_merge, 1);
    rb_define_method(cDocument, "changes_added", doc_changes_added, 1);
    rb_define_method(cDocument, "fork", doc_fork, -1);
    rb_define_method(cDocument, "actor_id", doc_actor_id, 0);
    rb_define_method(cDocument, "actor_id=", doc_set_actor_id, 1);
    rb_define_method(cDocument, "pending_ops", doc_pending_ops, 0);
    rb_define_method(cDocument, "rollback", doc_rollback, 0);
    rb_define_method(cDocument, "generate_sync_message", doc_generate_sync_message, 1);
    rb_define_method(cDocument, "receive_sync_message", doc_receive_sync_message, 2);

    cSyncState = ensure_class_under(mAutomerge, "SyncState", rb_cObject);
    rb_define_alloc_func(cSyncState, sync_state_alloc);
    rb_define_method(cSyncState, "initialize", sync_state_initialize, 0);
    rb_define_singleton_method(cSyncState, "decode", sync_state_s_decode, 1);
    rb_define_method(cSyncState, "encode", sync_state_encode, 0);
    rb_define_method(cSyncState, "==", sync_state_equal, 1);
    rb_define_method(cSyncState, "shared_heads", sync_state_shared_heads, 0);

    rb_define_module_function(mAutomerge, "decode_sync_message", automerge_s_decode_sync_message, 1);
}
