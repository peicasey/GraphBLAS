//------------------------------------------------------------------------------
// GB_conform: conform any matrix to its desired sparsity structure
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// On input, the matrix has any one of four sparsity structures: hypersparse,
// sparse, bitmap, or full.  A bitmap or full matrix never has pending work.  A
// sparse or hypersparse matrix may have pending work (zombies, jumbled, and/or
// pending tuples).  The pending work is not finished unless the matrix is
// converted to bitmap or full.  If this method fails, the matrix is cleared
// of all entries.

#include "GB.h"

#define GB_FREE_ALL ;

//------------------------------------------------------------------------------
// GB_conform_hyper_or_bitmap: ensure a matrix is either hypersparse or bitmap
//------------------------------------------------------------------------------

static inline GrB_Info GB_conform_hyper_or_bitmap
(
    bool is_hyper, bool is_sparse, bool is_bitmap, bool is_full,
    GrB_Matrix A, GB_Werk Werk
)
{
    GrB_Info info ;
    if (is_full || ((is_hyper || is_sparse) &&
        GB_convert_s2b_test (A->bitmap_switch,
            GB_nnz (A), A->vlen, A->vdim)))
    {   GB_cov[3679]++ ;
// covered (3679): 27796
        // if full or sparse/hypersparse with many entries: to bitmap
        GB_OK (GB_convert_any_to_bitmap (A, Werk)) ;
    }
    else if (is_sparse || (is_bitmap &&
        GB_convert_bitmap_to_sparse_test (A->bitmap_switch,
            GB_nnz (A), A->vlen, A->vdim)))
    {   GB_cov[3680]++ ;
// covered (3680): 116
        // if sparse or bitmap with few entries: to hypersparse
        GB_OK (GB_convert_any_to_hyper (A, Werk)) ;
    }
    return (GrB_SUCCESS) ;
}

//------------------------------------------------------------------------------
// GB_conform_sparse_or_bitmap: ensure a matrix is either sparse or bitmap
//------------------------------------------------------------------------------

static inline GrB_Info GB_conform_sparse_or_bitmap
(
    bool is_hyper, bool is_sparse, bool is_bitmap, bool is_full,
    GrB_Matrix A, GB_Werk Werk
)
{
    GrB_Info info ;
    if (is_full || ((is_hyper || is_sparse) &&
        GB_convert_s2b_test (A->bitmap_switch,
            GB_nnz (A), A->vlen, A->vdim)))
    {   GB_cov[3681]++ ;
// covered (3681): 675300
        // if full or sparse/hypersparse with many entries: to bitmap
        GB_OK (GB_convert_any_to_bitmap (A, Werk)) ;
    }
    else if (is_hyper || (is_bitmap &&
        GB_convert_bitmap_to_sparse_test (A->bitmap_switch,
            GB_nnz (A), A->vlen, A->vdim)))
    {   GB_cov[3682]++ ;
// covered (3682): 321269
        // if hypersparse or bitmap with few entries: to sparse
        GB_OK (GB_convert_any_to_sparse (A, Werk)) ;
    }
    return (GrB_SUCCESS) ;
}

//------------------------------------------------------------------------------
// GB_conform_hyper_sparse_or_bitmap: ensure matrix is hyper, sparse, or bitmap
//------------------------------------------------------------------------------

static inline GrB_Info GB_conform_hyper_sparse_or_bitmap
(
    bool is_hyper, bool is_sparse, bool is_bitmap, bool is_full,
    GrB_Matrix A, GB_Werk Werk
)
{
    GrB_Info info ;
    if (is_full || ((is_hyper || is_sparse) &&
        GB_convert_s2b_test (A->bitmap_switch,
            GB_nnz (A), A->vlen, A->vdim)))
    {   GB_cov[3683]++ ;
// covered (3683): 2920163
        // if full or sparse/hypersparse with many entries: to bitmap
        GB_OK (GB_convert_any_to_bitmap (A, Werk)) ;
    }
    else if (is_bitmap)
    {
        if (GB_convert_bitmap_to_sparse_test (A->bitmap_switch,
            GB_nnz (A), A->vlen, A->vdim))
        {   GB_cov[3684]++ ;
// covered (3684): 14228
            // if bitmap with few entries: to sparse
            GB_OK (GB_convert_bitmap_to_sparse (A, Werk)) ;
            // conform between sparse and hypersparse
            GB_OK (GB_conform_hyper (A, Werk)) ;
        }
    }
    else // is_hyper || is_sparse
    {   GB_cov[3685]++ ;
// covered (3685): 3242208
        // conform between sparse and hypersparse
        GB_OK (GB_conform_hyper (A, Werk)) ;
    }
    return (GrB_SUCCESS) ;
}

//------------------------------------------------------------------------------
// GB_conform
//------------------------------------------------------------------------------

GrB_Info GB_conform     // conform a matrix to its desired sparsity structure
(
    GrB_Matrix A,       // matrix to conform
    GB_Werk Werk
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GrB_Info info ;
    ASSERT_MATRIX_OK (A, "A to conform", GB0) ;
    ASSERT (GB_ZOMBIES_OK (A)) ;
    ASSERT (GB_JUMBLED_OK (A)) ;
    ASSERT (GB_PENDING_OK (A)) ;
    bool is_hyper = GB_IS_HYPERSPARSE (A) ;
    bool is_sparse = GB_IS_SPARSE (A) ;
    bool is_full = GB_IS_FULL (A) ;
    bool is_bitmap = GB_IS_BITMAP (A) ;
    bool as_if_full = GB_as_if_full (A) ;
    if (A->nvec_nonempty < 0)
    {   GB_cov[3686]++ ;
// covered (3686): 3124505
        A->nvec_nonempty = GB_nvec_nonempty (A) ;
    }
    if (A->no_hyper_hash)
    {   GB_cov[3687]++ ;
// NOT COVERED (3687):
        // A does not want the hyper_hash, so free A->Y if present
        GB_hyper_hash_free (A) ;
    }

    //--------------------------------------------------------------------------
    // select the sparsity structure
    //--------------------------------------------------------------------------

    switch (GB_sparsity_control (A->sparsity_control, A->vdim))
    {

        //----------------------------------------------------------------------
        // (1) always hypersparse
        //----------------------------------------------------------------------

        case GxB_HYPERSPARSE  : GB_cov[3688]++ ;  
// covered (3688): 3562737

            GB_OK (GB_convert_any_to_hyper (A, Werk)) ;
            break ;

        //----------------------------------------------------------------------
        // (2) always sparse
        //----------------------------------------------------------------------

        case GxB_SPARSE  : GB_cov[3689]++ ;  
// covered (3689): 1905072

            GB_OK (GB_convert_any_to_sparse (A, Werk)) ;
            break ;

        //----------------------------------------------------------------------
        // (3) sparse or hypersparse
        //----------------------------------------------------------------------

        case GxB_HYPERSPARSE + GxB_SPARSE  : GB_cov[3690]++ ;  
// covered (3690): 34020

            if (is_full || is_bitmap)
            {   GB_cov[3691]++ ;
// covered (3691): 1060
                // if full or bitmap: to sparse
                GB_OK (GB_convert_any_to_sparse (A, Werk)) ;
            }
            // conform between sparse and hypersparse
            GB_OK (GB_conform_hyper (A, Werk)) ;
            break ;

        //----------------------------------------------------------------------
        // (4) always bitmap
        //----------------------------------------------------------------------

        case GxB_BITMAP  : GB_cov[3692]++ ;  
// covered (3692): 1615897

            GB_OK (GB_convert_any_to_bitmap (A, Werk)) ;
            break ;

        //----------------------------------------------------------------------
        // (5) hypersparse or bitmap
        //----------------------------------------------------------------------

        case GxB_HYPERSPARSE + GxB_BITMAP  : GB_cov[3693]++ ;  
// covered (3693): 25270

            // ensure the matrix is hypersparse or bitmap
            GB_OK (GB_conform_hyper_or_bitmap (is_hyper, is_sparse, is_bitmap,
                is_full, A, Werk)) ;
            break ;

        //----------------------------------------------------------------------
        // (6) sparse or bitmap
        //----------------------------------------------------------------------

        case GxB_SPARSE + GxB_BITMAP  : GB_cov[3694]++ ;  
// covered (3694): 29107

            // ensure the matrix is sparse or bitmap
            GB_OK (GB_conform_sparse_or_bitmap (is_hyper, is_sparse, is_bitmap,
                is_full, A, Werk)) ;
            break ;

        //----------------------------------------------------------------------
        // (7) hypersparse, sparse, or bitmap
        //----------------------------------------------------------------------

        case GxB_HYPERSPARSE + GxB_SPARSE + GxB_BITMAP  : GB_cov[3695]++ ;  
// covered (3695): 25270

            // ensure the matrix is hypersparse, sparse, or bitmap
            GB_OK (GB_conform_hyper_sparse_or_bitmap (is_hyper, is_sparse,
                is_bitmap, is_full, A, Werk)) ;
            break ;

        //----------------------------------------------------------------------
        // (8): full
        //----------------------------------------------------------------------

        case GxB_FULL  : GB_cov[3696]++ ;  
// covered (3696): 1023771

            if (as_if_full)
            {   GB_cov[3697]++ ;
// covered (3697): 455425
                // if full or all entries present: to full
                GB_convert_any_to_full (A) ;
            }
            else
            {   GB_cov[3698]++ ;
// covered (3698): 568346
                // otherwise: to bitmap
                GB_OK (GB_convert_any_to_bitmap (A, Werk)) ;
            }
            break ;

        //----------------------------------------------------------------------
        // (9) hypersparse or full
        //----------------------------------------------------------------------

        case GxB_HYPERSPARSE + GxB_FULL  : GB_cov[3699]++ ;  
// covered (3699): 36630

            if (as_if_full)
            {   GB_cov[3700]++ ;
// covered (3700): 3750
                // if all entries present: to full
                GB_convert_any_to_full (A) ;
            }
            else
            {   GB_cov[3701]++ ;
// covered (3701): 32880
                // otherwise: to hypersparse
                GB_OK (GB_convert_any_to_hyper (A, Werk)) ;
            }
            break ;

        //----------------------------------------------------------------------
        // (10) sparse or full
        //----------------------------------------------------------------------

        case GxB_SPARSE + GxB_FULL  : GB_cov[3702]++ ;   
// covered (3702): 4947564

            if (as_if_full)
            {   GB_cov[3703]++ ;
// covered (3703): 1142721
                // if full or all entries present: to full
                GB_convert_any_to_full (A) ;
            }
            else
            {   GB_cov[3704]++ ;
// covered (3704): 3804843
                // otherwise: to sparse
                GB_OK (GB_convert_any_to_sparse (A, Werk)) ;
            }
            break ;

        //----------------------------------------------------------------------
        // (11) hypersparse, sparse, or full
        //----------------------------------------------------------------------

        case GxB_HYPERSPARSE + GxB_SPARSE + GxB_FULL  : GB_cov[3705]++ ;  
// covered (3705): 61600

            if (as_if_full)
            {   GB_cov[3706]++ ;
// covered (3706): 15388
                // if full or all entries present: to full
                GB_convert_any_to_full (A) ;
            }
            else if (is_bitmap)
            {   GB_cov[3707]++ ;
// covered (3707): 9324
                // if bitmap: to sparse
                GB_OK (GB_convert_bitmap_to_sparse (A, Werk)) ;
                // conform between sparse and hypersparse
                GB_OK (GB_conform_hyper (A, Werk)) ;
            }
            else
            {   GB_cov[3708]++ ;
// covered (3708): 36888
                // conform between sparse and hypersparse
                GB_OK (GB_conform_hyper (A, Werk)) ;
            }
            break ;

        //----------------------------------------------------------------------
        // (12): bitmap or full
        //----------------------------------------------------------------------

        case GxB_BITMAP + GxB_FULL  : GB_cov[3709]++ ;  
// covered (3709): 29275

            if (as_if_full)
            {   GB_cov[3710]++ ;
// covered (3710): 4260
                // if full or all entries present: to full
                GB_convert_any_to_full (A) ;
            }
            else
            {   GB_cov[3711]++ ;
// covered (3711): 25015
                // otherwise: to bitmap
                GB_OK (GB_convert_any_to_bitmap (A, Werk)) ;
            }
            break ;

        //----------------------------------------------------------------------
        // (13) hypersparse, bitmap, or full
        //----------------------------------------------------------------------

        case GxB_HYPERSPARSE + GxB_BITMAP + GxB_FULL  : GB_cov[3712]++ ;  
// covered (3712): 25270

            if (as_if_full)
            {   GB_cov[3713]++ ;
// covered (3713): 3750
                // if full or all entries present: to full
                GB_convert_any_to_full (A) ;
            }
            else
            {   GB_cov[3714]++ ;
// covered (3714): 21520
                // ensure the matrix is hypersparse or bitmap
                GB_OK (GB_conform_hyper_or_bitmap (is_hyper, is_sparse,
                    is_bitmap, is_full, A, Werk)) ;
            }
            break ;

        //----------------------------------------------------------------------
        // (14) sparse, bitmap, or full
        //----------------------------------------------------------------------

        case GxB_SPARSE + GxB_BITMAP + GxB_FULL  : GB_cov[3715]++ ;  
// covered (3715): 6143279

            if (as_if_full)
            {   GB_cov[3716]++ ;
// covered (3716): 4571125
                // if full or all entries present: to full
                GB_convert_any_to_full (A) ;
            }
            else
            {   GB_cov[3717]++ ;
// covered (3717): 1572154
                // ensure the matrix is sparse or bitmap
                GB_OK (GB_conform_sparse_or_bitmap (is_hyper, is_sparse,
                    is_bitmap, is_full, A, Werk)) ;
            }
            break ;

        //----------------------------------------------------------------------
        // (15) hypersparse, sparse, bitmap, or full
        //----------------------------------------------------------------------

        default:
        case GxB_HYPERSPARSE + GxB_SPARSE + GxB_BITMAP + GxB_FULL  : GB_cov[3718]++ ;  
// covered (3718): 8953421

            if (as_if_full)
            {   GB_cov[3719]++ ;
// covered (3719): 414630
                // if full or all entries present: to full
                GB_convert_any_to_full (A) ;
            }
            else
            {   GB_cov[3720]++ ;
// covered (3720): 8538791
                // ensure the matrix is hypersparse, sparse, or bitmap
                GB_OK (GB_conform_hyper_sparse_or_bitmap (is_hyper, is_sparse,
                    is_bitmap, is_full, A, Werk)) ;
            }
            break ;
    }

    //--------------------------------------------------------------------------
    // return result
    //--------------------------------------------------------------------------

    ASSERT_MATRIX_OK (A, "A conformed", GB0) ;
    return (GrB_SUCCESS) ;
}

