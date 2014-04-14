/*

    Copyright (C) 2014, The University of Texas at Austin

    This file is part of libflame and is available under the 3-Clause
    BSD license, which can be found in the LICENSE file at the top-level
    directory, or at http://opensource.org/licenses/BSD-3-Clause

*/

#include "FLAME.h"

FLA_Error FLA_Trinv_ln_blk_var3( FLA_Obj A, fla_trinv_t* cntl )
{
  FLA_Obj ATL,   ATR,      A00, A01, A02, 
          ABL,   ABR,      A10, A11, A12,
                           A20, A21, A22;

  dim_t b;

  FLA_Part_2x2( A,    &ATL, &ATR,
                      &ABL, &ABR,     0, 0, FLA_TL );

  while ( FLA_Obj_length( ATL ) < FLA_Obj_length( A ) ){

    b = FLA_Determine_blocksize( ABR, FLA_BR, FLA_Cntl_blocksize( cntl ) );

    FLA_Repart_2x2_to_3x3( ATL, /**/ ATR,       &A00, /**/ &A01, &A02,
                        /* ************* */   /* ******************** */
                                                &A10, /**/ &A11, &A12,
                           ABL, /**/ ABR,       &A20, /**/ &A21, &A22,
                           b, b, FLA_BR );

    /*------------------------------------------------------------*/

    // A21 = -A21 / tril( A11 );
    FLA_Trsm_internal( FLA_RIGHT, FLA_LOWER_TRIANGULAR, FLA_NO_TRANSPOSE, FLA_NONUNIT_DIAG, 
                       FLA_MINUS_ONE, A11, A21, 
                       FLA_Cntl_sub_trsm1( cntl ) );

    // A20 = A21 * A10 + A20;
    FLA_Gemm_internal( FLA_NO_TRANSPOSE, FLA_NO_TRANSPOSE,
                       FLA_ONE, A21, A10, FLA_ONE, A20, 
                       FLA_Cntl_sub_gemm( cntl ) );

    // A10 = tril( A11 ) \ A10;
    FLA_Trsm_internal( FLA_LEFT, FLA_LOWER_TRIANGULAR, FLA_NO_TRANSPOSE, FLA_NONUNIT_DIAG, 
                       FLA_ONE, A11, A10, 
                       FLA_Cntl_sub_trsm2( cntl ) );

    // A11 = inv( A11 );
    FLA_Trinv_internal( FLA_LOWER_TRIANGULAR, FLA_NONUNIT_DIAG, A11,
                        FLA_Cntl_sub_trinv( cntl ) );

    /*------------------------------------------------------------*/

    FLA_Cont_with_3x3_to_2x2( &ATL, /**/ &ATR,       A00, A01, /**/ A02,
                                                     A10, A11, /**/ A12,
                            /* ************** */  /* ****************** */
                              &ABL, /**/ &ABR,       A20, A21, /**/ A22,
                              FLA_TL );

  }

  return FLA_SUCCESS;
}
