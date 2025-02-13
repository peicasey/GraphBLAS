//------------------------------------------------------------------------------
// GrB_Row_assign: C<M'>(row,Cols) = accum (C(row,Cols),u')
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2024, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// Compare with GxB_Row_subassign, which uses M and C_replace differently

#include "assign/GB_assign.h"
#include "assign/GB_bitmap_assign.h"
#include "mask/GB_get_mask.h"

GrB_Info GrB_Row_assign             // C<M'>(row,Cols) += u'
(
    GrB_Matrix C,                   // input/output matrix for results
    const GrB_Vector M_in,          // M for C(row,:), unused if NULL
    const GrB_BinaryOp accum,       // optional accum for z=accum(C(row,Cols),t)
    const GrB_Vector u,             // input vector
    GrB_Index row,                  // row index
    const GrB_Index *Cols,          // column indices
    GrB_Index nCols,                // number of column indices
    const GrB_Descriptor desc       // descriptor for C(row,:) and M
)
{ 

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE (C, "GrB_Row_assign (C, M, accum, u, row, Cols, nCols, desc)") ;
    GB_BURBLE_START ("GrB_assign") ;
    GB_RETURN_IF_NULL_OR_FAULTY (C) ;
    GB_RETURN_IF_FAULTY (M_in) ;
    GB_RETURN_IF_NULL_OR_FAULTY (u) ;
    ASSERT (M_in == NULL || GB_VECTOR_OK (M_in)) ;
    ASSERT (GB_VECTOR_OK (u)) ;

    // get the descriptor
    GB_GET_DESCRIPTOR (info, desc, C_replace, Mask_comp, Mask_struct,
        xx1, xx2, xx3, xx7) ;

    // get the mask
    GrB_Matrix M = GB_get_mask ((GrB_Matrix) M_in, &Mask_comp, &Mask_struct) ;

    //--------------------------------------------------------------------------
    // C<M'>(row,Cols) = accum (C(row,Cols), u')
    //--------------------------------------------------------------------------

    // construct the row index list Rows = [ row ] of length nRows = 1
    GrB_Index Rows [1] ;
    Rows [0] = row ;

    info = GB_assign (
        C, C_replace,                   // C matrix and its descriptor
        M, Mask_comp, Mask_struct,      // mask and its descriptor
        true,                           // transpose the mask
        accum,                          // for accum (C(Rows,col),u)
        (GrB_Matrix) u, true,           // u as a matrix; always transposed
        Rows, 1,                        // a single row index
        Cols, nCols,                    // column indices
        false, NULL, GB_ignore_code,    // no scalar expansion
        GB_ROW_ASSIGN,
        Werk) ;

    GB_BURBLE_END ;
    return (info) ;
}

