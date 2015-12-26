/* A GC sync point is a point where we can check if we're being signalled
 * to stop to do a GC run. This is placed at points where it is safe to
 * do such a thing, and hopefully so that it happens often enough; note
 * that every call down to the allocator is also a sync point, so this
 * really only means we need to do this enough to make sure tight native
 * loops trigger it. */
/* Don't use a MVM_load(&tc->gc_status) here for performance, it's okay
 * if the interrupt is delayed a bit. */
#define GC_SYNC_POINT(tc) \
    if (tc->gc_status) { \
        MVM_gc_enter_from_interrupt(tc); \
    }

/* Different views of a register. */
union MVMRegister {
    MVMObject         *o;
    MVMString *s;
    MVMint8            i8;
    MVMuint8           u8;
    MVMint16           i16;
    MVMuint16          u16;
    MVMint32           i32;
    MVMuint32          u32;
    MVMint64           i64;
    MVMuint64          u64;
    MVMnum32           n32;
    MVMnum64           n64;
};

/* Most operands an operation will have. */
#define MVM_MAX_OPERANDS 8

/* Kind of de-opt mark. */
#define MVM_DEOPT_MARK_ONE 1
#define MVM_DEOPT_MARK_ALL 2
#define MVM_DEOPT_MARK_OSR 4

/* Information about an opcode. */
struct MVMOpInfo {
    MVMuint16   opcode;
    const char *name;
    char        mark[2];
    MVMuint16   num_operands;
    MVMuint8    pure;
    MVMuint8    deopt_point;
    MVMuint8    no_inline;
    MVMuint8    jittivity;
    MVMuint8    operands[MVM_MAX_OPERANDS];
};

/* Operand read/write/literal flags. */
#define MVM_operand_literal     0
#define MVM_operand_read_reg    1
#define MVM_operand_write_reg   2
#define MVM_operand_read_lex    3
#define MVM_operand_write_lex   4
#define MVM_operand_rw_mask     7

/* Register data types. */
#define MVM_reg_int8            1
#define MVM_reg_int16           2
#define MVM_reg_int32           3
#define MVM_reg_int64           4
#define MVM_reg_num32           5
#define MVM_reg_num64           6
#define MVM_reg_str             7
#define MVM_reg_obj             8
#define MVM_reg_uint8           17
#define MVM_reg_uint16          18
#define MVM_reg_uint32          19
#define MVM_reg_uint64          20

/* Operand data types. */
#define MVM_operand_int8        (MVM_reg_int8 << 3)
#define MVM_operand_int16       (MVM_reg_int16 << 3)
#define MVM_operand_int32       (MVM_reg_int32 << 3)
#define MVM_operand_int64       (MVM_reg_int64 << 3)
#define MVM_operand_num32       (MVM_reg_num32 << 3)
#define MVM_operand_num64       (MVM_reg_num64 << 3)
#define MVM_operand_str         (MVM_reg_str << 3)
#define MVM_operand_obj         (MVM_reg_obj << 3)
#define MVM_operand_ins         (9 << 3)
#define MVM_operand_type_var    (10 << 3)
#define MVM_operand_coderef     (12 << 3)
#define MVM_operand_callsite    (13 << 3)
#define MVM_operand_spesh_slot  (16 << 3)
#define MVM_operand_uint8       (MVM_reg_uint8 << 3)
#define MVM_operand_uint16      (MVM_reg_uint16 << 3)
#define MVM_operand_uint32      (MVM_reg_uint32 << 3)
#define MVM_operand_uint64      (MVM_reg_uint64 << 3)
#define MVM_operand_type_mask   (31 << 3)

/* Functions. */
void MVM_interp_run(MVMThreadContext *tc, void (*initial_invoke)(MVMThreadContext *, void *), void *invoke_data);
MVM_PUBLIC void MVM_interp_enable_tracing();

#define DECLARE_MVM_BC_GET_ALIGNED(short_type, long_type)                          \
    MVM_STATIC_INLINE long_type MVM_BC_get_ ## short_type (const MVMuint8 *cur_op, int offset) {  \
        long_type temp;                                                                      \
        memmove(&temp, (cur_op + offset), sizeof(long_type));      \
        return temp;                                                                     \
    }

#define DECLARE_MVM_BC_GET_UNALIGNED(short_type, long_type)               \
    MVM_STATIC_INLINE long_type MVM_BC_get_ ## short_type (const MVMuint8 *cur_op, int offset) {  \
        return *(long_type *) (cur_op + offset);                        \
    } \

#ifdef MVM_CAN_UNALIGNED_INT32
DECLARE_MVM_BC_GET_UNALIGNED(I32, MVMint32)
DECLARE_MVM_BC_GET_UNALIGNED(UI32, MVMuint32)
DECLARE_MVM_BC_GET_UNALIGNED(N32, MVMnum32)
#else
DECLARE_MVM_BC_GET_ALIGNED(I32, MVMint32)
DECLARE_MVM_BC_GET_ALIGNED(UI32, MVMuint32)
DECLARE_MVM_BC_GET_ALIGNED(N32, MVMnum32)
#endif

#define GET_I32  MVM_BC_get_I32
#define GET_UI32 MVM_BC_get_UI32
#define GET_N32  MVM_BC_get_N32

#ifdef MVM_CAN_UNALIGNED_INT64
DECLARE_MVM_BC_GET_UNALIGNED(I64, MVMint64)
#else
DECLARE_MVM_BC_GET_ALIGNED(I64, MVMint64)
#endif

#ifdef MVM_CAN_UNALIGNED_NUM64
DECLARE_MVM_BC_GET_UNALIGNED(N64, MVMnum64)
#else
DECLARE_MVM_BC_GET_ALIGNED(N64, MVMnum64)
#endif

#define GET_I64 MVM_BC_get_I64
#define GET_N64 MVM_BC_get_N64
