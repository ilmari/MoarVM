#include "moar.h"

/* copied from perl5/pp.c:S_iv_shift */
static MVMint64 S_int_shift(MVMint64 value, MVMint64 shift, MVMint64 left) {
   if (shift < 0) {
       shift = -shift;
       left = !left;
   }
   if (shift >= sizeof(MVMint64) * CHAR_BIT) {
       return value < 0 && !left ? -1 : 0;
   }
   return left ? value << shift : value >> shift;
}

MVMint64 MVM_int_shl(MVMThreadContext *tc, MVMint64 value, MVMint64 shift) {
    return S_int_shift(value, shift, 1);
}

MVMint64 MVM_int_shr(MVMThreadContext *tc, MVMint64 value, MVMint64 shift) {
    return S_int_shift(value, shift, 0);
}
