//------------------------------------------------------------------------------
// GB_AxB:  hard-coded functions for semiring: C<M>=A*B or A'*B
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// If this file is in the Generated/ folder, do not edit it (auto-generated).

#include "GB.h"
#ifndef GBCOMPACT
#include "GB_control.h"
#include "GB_bracket.h"
#include "GB_sort.h"
#include "GB_atomics.h"
#include "GB_AxB_saxpy.h"
#include "GB_AxB__include.h"
#include "GB_unused.h"
#include "GB_bitmap_assign_methods.h"
#include "GB_ek_slice_search.c"

// The C=A*B semiring is defined by the following types and operators:

// A'*B function (dot2):     GB_Adot2B__max_isge_fp32
// A'*B function (dot3):     GB_Adot3B__max_isge_fp32
// C+=A'*B function (dot4):  GB_Adot4B__max_isge_fp32
// A*B function (saxpy):     GB_AsaxpyB__max_isge_fp32

// C type:   float
// A type:   float
// B type:   float

// Multiply: z = (aik >= bkj)
// Add:      if (!isgreaterequal (cij, z)) cij = z
//           'any' monoid?  0
//           atomic?        1
//           OpenMP atomic? 0
// MultAdd:  cij = fmaxf (cij, (aik >= bkj))
// Identity: (-INFINITY)
// Terminal: if (cij == INFINITY) break ;

#define GB_ATYPE \
    float

#define GB_BTYPE \
    float

#define GB_CTYPE \
    float

#define GB_ASIZE (sizeof (GB_BTYPE))
#define GB_BSIZE (sizeof (GB_BTYPE))
#define GB_CSIZE (sizeof (GB_CTYPE))

// true for int64, uint64, float, double, float complex, and double complex 
#define GB_CTYPE_IGNORE_OVERFLOW \
    1

// aik = Ax [pA]
#define GB_GETA(aik,Ax,pA) \
    float aik = Ax [pA]

// bkj = Bx [pB]
#define GB_GETB(bkj,Bx,pB) \
    float bkj = Bx [pB]

// Gx [pG] = Ax [pA]
#define GB_LOADA(Gx,pG,Ax,pA) \
    Gx [pG] = Ax [pA]

// Gx [pG] = Bx [pB]
#define GB_LOADB(Gx,pG,Bx,pB) \
    Gx [pG] = Bx [pB]

#define GB_CX(p) Cx [p]

// multiply operator
#define GB_MULT(z, x, y, i, k, j) \
    z = (x >= y)

// cast from a real scalar (or 2, if C is complex) to the type of C
#define GB_CTYPE_CAST(x,y) \
    ((float) x)

// cast from a real scalar (or 2, if A is complex) to the type of A
#define GB_ATYPE_CAST(x,y) \
    ((float) x)

// multiply-add
#define GB_MULTADD(z, x, y, i, k, j) \
    z = fmaxf (z, (x >= y))

// monoid identity value
#define GB_IDENTITY \
    (-INFINITY)

// 1 if the identity value can be assigned via memset, with all bytes the same
#define GB_HAS_IDENTITY_BYTE \
    0

// identity byte, for memset
#define GB_IDENTITY_BYTE \
    (none)

// break if cij reaches the terminal value (dot product only)
#define GB_DOT_TERMINAL(cij) \
    if (cij == INFINITY) break ;

// simd pragma for dot-product loop vectorization
#define GB_PRAGMA_SIMD_DOT(cij) \
    ;

// simd pragma for other loop vectorization
#define GB_PRAGMA_SIMD_VECTORIZE GB_PRAGMA_SIMD

// 1 for the PLUS_PAIR_(real) semirings, not for the complex case
#define GB_IS_PLUS_PAIR_REAL_SEMIRING \
    0

// 1 for performance-critical semirings, which get extra optimization
#define GB_IS_PERFORMANCE_CRITICAL_SEMIRING \
    0

// declare the cij scalar
#if GB_IS_PLUS_PAIR_REAL_SEMIRING
    // also initialize cij to zero
    #define GB_CIJ_DECLARE(cij) \
        float cij = 0
#else
    // all other semirings: just declare cij, do not initialize it
    #define GB_CIJ_DECLARE(cij) \
        float cij
#endif

// cij = Cx [pC]
#define GB_GETC(cij,p) cij = Cx [p]

// Cx [pC] = cij
#define GB_PUTC(cij,p) Cx [p] = cij

// Cx [p] = t
#define GB_CIJ_WRITE(p,t) Cx [p] = t

// C(i,j) += t
#define GB_CIJ_UPDATE(p,t) \
    if (!isgreaterequal (Cx [p], t)) Cx [p] = t

// x + y
#define GB_ADD_FUNCTION(x,y) \
    fmaxf (x, y)

// bit pattern for bool, 8-bit, 16-bit, and 32-bit integers
#define GB_CTYPE_BITS \
    0

// 1 if monoid update can skipped entirely (the ANY monoid)
#define GB_IS_ANY_MONOID \
    0

// 1 if monoid update is EQ
#define GB_IS_EQ_MONOID \
    0

// 1 if monoid update can be done atomically, 0 otherwise
#define GB_HAS_ATOMIC \
    1

// 1 if monoid update can be done with an OpenMP atomic update, 0 otherwise
#if GB_MICROSOFT
    #define GB_HAS_OMP_ATOMIC \
        0
#else
    #define GB_HAS_OMP_ATOMIC \
        0
#endif

// 1 for the ANY_PAIR semirings
#define GB_IS_ANY_PAIR_SEMIRING \
    0

// 1 if PAIR is the multiply operator 
#define GB_IS_PAIR_MULTIPLIER \
    0

// 1 if monoid is PLUS_FC32
#define GB_IS_PLUS_FC32_MONOID \
    0

// 1 if monoid is PLUS_FC64
#define GB_IS_PLUS_FC64_MONOID \
    0

// 1 if monoid is ANY_FC32
#define GB_IS_ANY_FC32_MONOID \
    0

// 1 if monoid is ANY_FC64
#define GB_IS_ANY_FC64_MONOID \
    0

// 1 if monoid is MIN for signed or unsigned integers
#define GB_IS_IMIN_MONOID \
    0

// 1 if monoid is MAX for signed or unsigned integers
#define GB_IS_IMAX_MONOID \
    0

// 1 if monoid is MIN for float or double
#define GB_IS_FMIN_MONOID \
    0

// 1 if monoid is MAX for float or double
#define GB_IS_FMAX_MONOID \
    1

// 1 for the FIRSTI or FIRSTI1 multiply operator
#define GB_IS_FIRSTI_MULTIPLIER \
    0

// 1 for the FIRSTJ or FIRSTJ1 multiply operator
#define GB_IS_FIRSTJ_MULTIPLIER \
    0

// 1 for the SECONDJ or SECONDJ1 multiply operator
#define GB_IS_SECONDJ_MULTIPLIER \
    0

// atomic compare-exchange
#define GB_ATOMIC_COMPARE_EXCHANGE(target, expected, desired) \
    GB_ATOMIC_COMPARE_EXCHANGE_32 (target, expected, desired)

#if GB_IS_ANY_PAIR_SEMIRING

    // result is purely symbolic; no numeric work to do.  Hx is not used.
    #define GB_HX_WRITE(i,t)
    #define GB_CIJ_GATHER(p,i)
    #define GB_CIJ_GATHER_UPDATE(p,i)
    #define GB_HX_UPDATE(i,t)
    #define GB_CIJ_MEMCPY(p,i,len)

#else

    // Hx [i] = t
    #define GB_HX_WRITE(i,t) Hx [i] = t

    // Hx [i] = identity
    #define GB_HX_CLEAR(i) Hx [i] = GB_IDENTITY

    // Cx [p] = Hx [i]
    #define GB_CIJ_GATHER(p,i) Cx [p] = Hx [i]

    // Cx [p] += Hx [i]
    #define GB_CIJ_GATHER_UPDATE(p,i) \
        if (!isgreaterequal (Cx [p], Hx [i])) Cx [p] = Hx [i]

    // Hx [i] += t
    #define GB_HX_UPDATE(i,t) \
        if (!isgreaterequal (Hx [i], t)) Hx [i] = t

    // memcpy (&(Cx [p]), &(Hx [i]), len)
    #define GB_CIJ_MEMCPY(p,i,len) \
        memcpy (Cx +(p), Hx +(i), (len) * sizeof(float))

#endif

// 1 if the semiring has a concise bitmap multiply-add
#define GB_HAS_BITMAP_MULTADD \
    1

// concise statement(s) for the bitmap case:
//  if (exists)
//      if (cb == 0)
//          cx = ax * bx
//          cb = 1
//      else
//          cx += ax * bx
#define GB_BITMAP_MULTADD(cb,cx,exists,ax,bx) \
    float t = ((ax >= bx)) ; if (exists && !isgreaterequal (cx, t)) cx = t ; cb |= exists

// define X for bitmap multiply-add
#define GB_XINIT \
    ;

// load X [1] = bkj for bitmap multiply-add
#define GB_XLOAD(bkj) \
    ;

// disable this semiring and use the generic case if these conditions hold
#define GB_DISABLE \
    (GxB_NO_MAX || GxB_NO_ISGE || GxB_NO_FP32 || GxB_NO_MAX_FP32 || GxB_NO_ISGE_FP32 || GxB_NO_MAX_ISGE_FP32)

//------------------------------------------------------------------------------
// C=A'*B, C<M>=A'*B, or C<!M>=A'*B: dot product method where C is bitmap
//------------------------------------------------------------------------------

GrB_Info GB_Adot2B__max_isge_fp32
(
    GrB_Matrix C,
    const GrB_Matrix M, const bool Mask_comp, const bool Mask_struct,
    const GrB_Matrix A, bool A_is_pattern, int64_t *GB_RESTRICT A_slice,
    const GrB_Matrix B, bool B_is_pattern, int64_t *GB_RESTRICT B_slice,
    int nthreads, int naslice, int nbslice
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_AxB_dot2_meta.c"
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// C<M>=A'*B: masked dot product method (phase 2) where C is sparse or hyper
//------------------------------------------------------------------------------

GrB_Info GB_Adot3B__max_isge_fp32
(
    GrB_Matrix C,
    const GrB_Matrix M, const bool Mask_struct,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    const GB_task_struct *GB_RESTRICT TaskList,
    const int ntasks,
    const int nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_AxB_dot3_meta.c"
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// C+=A'*B: dense dot product
//------------------------------------------------------------------------------

GrB_Info GB_Adot4B__max_isge_fp32
(
    GrB_Matrix C,
    const GrB_Matrix A, bool A_is_pattern,
    int64_t *GB_RESTRICT A_slice, int naslice,
    const GrB_Matrix B, bool B_is_pattern,
    int64_t *GB_RESTRICT B_slice, int nbslice,
    const int nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_AxB_dot4_meta.c"
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// C=A*B, C<M>=A*B, C<!M>=A*B: saxpy method (Gustavson + Hash)
//------------------------------------------------------------------------------

#include "GB_AxB_saxpy3_template.h"

GrB_Info GB_AsaxpyB__max_isge_fp32
(
    GrB_Matrix C,
    const GrB_Matrix M, const bool Mask_comp, const bool Mask_struct,
    const bool M_dense_in_place,
    const GrB_Matrix A, bool A_is_pattern,
    const GrB_Matrix B, bool B_is_pattern,
    const int saxpy_method,
    // for saxpy3 method only:
    GB_saxpy3task_struct *GB_RESTRICT SaxpyTasks,
    int ntasks, int nfine,
    // for saxpy3 and saxpy4 methods only:
    int nthreads,
    const int do_sort,
    // for saxpy4 method only:
    int8_t  *GB_RESTRICT Wf,
    int64_t **Wi_handle,
    size_t Wi_size,
    GB_void *GB_RESTRICT Wx,
    int64_t *GB_RESTRICT kfirst_Bslice,
    int64_t *GB_RESTRICT klast_Bslice,
    int64_t *GB_RESTRICT pstart_Bslice,
    GB_Context Context
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_AxB_saxpy_template.c"
    return (GrB_SUCCESS) ;
    #endif
}

#endif

