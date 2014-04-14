/*

    Copyright (C) 2014, The University of Texas at Austin

    This file is part of libflame and is available under the 3-Clause
    BSD license, which can be found in the LICENSE file at the top-level
    directory, or at http://opensource.org/licenses/BSD-3-Clause

*/
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "FLAME.h"
#include "Gemm_prototypes.h"

int main(int argc, char *argv[])
{
  int 
    n, nfirst, nlast, ninc, i, irep,
    nrepeats, nb_alg;

  double
    dtime,
    dtime_best,
    gflops,
    max_gflops,
    diff;

  FLA_Obj
    A, B, C, Cref, Cold;
  
  /* Initialize FLAME */
  FLA_Init( );

  /* Every time trial is repeated "repeat" times and the fastest run in recorded */
  printf( "%% number of repeats:" );
  scanf( "%d", &nrepeats );
  printf( "%% %d\n", nrepeats );

  /* Enter the max GFLOPS attainable 
     This is used to set the y-axis range for the graphs. Here is how
     you figure out what to enter (on Linux machines):
     1) more /proc/cpuinfo   (this lists the contents of this file).
     2) read through this and figure out the clock rate of the machine (in GHz).
     3) Find out (from an expert of from the web) the number of floating point
        instructions that can be performed per core per clock cycle.
     4) Figure out if you are using "multithreaded BLAS" which automatically
        parallelize calls to the Basic Linear Algebra Subprograms.  If so,
        check how many cores are available.
     5) Multiply 2) x 3) x 4) and enter this in response to the below.
  */

  printf( "%% enter max GFLOPS:" );
  scanf( "%lf", &max_gflops );
  printf( "%% %lf\n", max_gflops );

  /* Enter the algorithmic block size */
  printf( "%% enter nb_alg:" );
  scanf( "%d", &nb_alg );
  printf( "%% %d\n", nb_alg );

  /* Turn on parameter checking */
  FLA_Check_error_level_set( FLA_FULL_ERROR_CHECKING );

  /* Timing trials for matrix sizes n=nfirst to nlast in increments 
     of ninc will be performed */
  printf( "%% enter nfirst, nlast, ninc:" );
  scanf( "%d%d%d", &nfirst, &nlast, &ninc );
  printf( "%% %d %d %d\n", nfirst, nlast, ninc );

  i = 1;
  for ( n=nfirst; n<= nlast; n+=ninc ){
   
    /* Allocate space for the matrices */
    FLA_Obj_create( FLA_DOUBLE, n, n, 0, 0, &A );
    FLA_Obj_create( FLA_DOUBLE, n, n, 0, 0, &B );
    FLA_Obj_create( FLA_DOUBLE, n, n, 0, 0, &C );
    FLA_Obj_create( FLA_DOUBLE, n, n, 0, 0, &Cref );
    FLA_Obj_create( FLA_DOUBLE, n, n, 0, 0, &Cold );

    /* Generate random matrix A and save in Aold */
    FLA_Random_matrix( A );
    FLA_Random_matrix( B );
    FLA_Random_matrix( Cold );

    /* Set gflops = billions of floating point operations that will be performed */
    gflops = 2.0 * n * n * n * 1.0e-09;

    /* Time the reference implementation */

    for ( irep=0; irep<nrepeats; irep++ ){
      FLA_Copy( Cold, Cref );

      dtime = FLA_Clock();

      FLA_Gemm( FLA_NO_TRANSPOSE, FLA_NO_TRANSPOSE,
                FLA_ONE, A, B, FLA_ONE, Cref );

      dtime = FLA_Clock() - dtime;

      if ( irep == 0 ) 
        dtime_best = dtime;
      else
        dtime_best = ( dtime < dtime_best ? dtime : dtime_best );
    }

    printf( "data_FLAME( %d, 1:2 ) = [ %d %le ];\n", i, n,
            gflops / dtime_best );
    fflush( stdout );


    /* Time the your implementations */


    /* Variant 1 unblocked */

    for ( irep=0; irep<nrepeats; irep++ ){

      FLA_Copy( Cold, C );
    
      dtime = FLA_Clock();

      Gemm_unb_var1( A, B, C );

      dtime = FLA_Clock() - dtime;

      if ( irep == 0 ) 
        dtime_best = dtime;
      else
        dtime_best = ( dtime < dtime_best ? dtime : dtime_best );
    }    

    diff = FLA_Max_elemwise_diff( C, Cref );

    printf( "data_unb_var1( %d, 1:3 ) = [ %d %le  %le];\n", i, n,
            gflops / dtime_best, diff );
    fflush( stdout );

    /* Variant 1 blocked */

    for ( irep=0; irep<nrepeats; irep++ ){
      FLA_Copy( Cold, C );
    
      dtime = FLA_Clock();

      Gemm_blk_var1( A, B, C, nb_alg );

      dtime = FLA_Clock() - dtime;

      if ( irep == 0 ) 
        dtime_best = dtime;
      else
        dtime_best = ( dtime < dtime_best ? dtime : dtime_best );
    }

    diff = FLA_Max_elemwise_diff( C, Cref );

    printf( "data_blk_var1( %d, 1:3 ) = [ %d %le  %le];\n", i, n,
            gflops / dtime_best, diff );
    fflush( stdout );

    /* Variant 2 unblocked */

    for ( irep=0; irep<nrepeats; irep++ ){

      FLA_Copy( Cold, C );
    
      dtime = FLA_Clock();

      Gemm_unb_var2( A, B, C );

      dtime = FLA_Clock() - dtime;

      if ( irep == 0 ) 
        dtime_best = dtime;
      else
        dtime_best = ( dtime < dtime_best ? dtime : dtime_best );
    }    

    diff = FLA_Max_elemwise_diff( C, Cref );

    printf( "data_unb_var2( %d, 1:3 ) = [ %d %le  %le];\n", i, n,
            gflops / dtime_best, diff );
    fflush( stdout );

    /* Variant 2 blocked */

    for ( irep=0; irep<nrepeats; irep++ ){
      FLA_Copy( Cold, C );
    
      dtime = FLA_Clock();

      Gemm_blk_var2( A, B, C, nb_alg );

      dtime = FLA_Clock() - dtime;

      if ( irep == 0 ) 
        dtime_best = dtime;
      else
        dtime_best = ( dtime < dtime_best ? dtime : dtime_best );
    }

    diff = FLA_Max_elemwise_diff( C, Cref );

    printf( "data_blk_var2 %d, 1:3 ) = [ %d %le  %le];\n", i, n,
            gflops / dtime_best, diff );
    fflush( stdout );

    /* Variant 3 unblocked */

    for ( irep=0; irep<nrepeats; irep++ ){

      FLA_Copy( Cold, C );
    
      dtime = FLA_Clock();

      Gemm_unb_var3( A, B, C );

      dtime = FLA_Clock() - dtime;

      if ( irep == 0 ) 
        dtime_best = dtime;
      else
        dtime_best = ( dtime < dtime_best ? dtime : dtime_best );
    }    

    diff = FLA_Max_elemwise_diff( C, Cref );

    printf( "data_unb_var3( %d, 1:3 ) = [ %d %le  %le];\n", i, n,
            gflops / dtime_best, diff );
    fflush( stdout );

    /* Variant 3 blocked */

    for ( irep=0; irep<nrepeats; irep++ ){
      FLA_Copy( Cold, C );
    
      dtime = FLA_Clock();

      Gemm_blk_var3( A, B, C, nb_alg );

      dtime = FLA_Clock() - dtime;

      if ( irep == 0 ) 
        dtime_best = dtime;
      else
        dtime_best = ( dtime < dtime_best ? dtime : dtime_best );
    }

    diff = FLA_Max_elemwise_diff( C, Cref );

    printf( "data_blk_var3( %d, 1:3 ) = [ %d %le  %le];\n", i, n,
            gflops / dtime_best, diff );
    fflush( stdout );


    FLA_Obj_free( &A );
    FLA_Obj_free( &B );
    FLA_Obj_free( &C );
    FLA_Obj_free( &Cold );
    FLA_Obj_free( &Cref );
    printf( "\n" );

    i++;
  }

  /* Print the MATLAB commands to plot the data */

  /* Delete all existing figures */
  printf( "close all\n" );

  /* Plot the performance of FLAME */
  printf( "plot( data_FLAME( :,1 ), data_FLAME( :, 2 ), 'k--' ); \n" );

  /* Indicate that you want to add to the existing plot */
  printf( "hold on\n" );

  /* Plot the performance of your implementations */

  printf( "plot( data_unb_var1( :,1 ), data_unb_var1( :, 2 ), 'r-.' ); \n" );
  printf( "plot( data_blk_var1( :,1 ), data_blk_var1( :, 2 ), 'r--' ); \n" );
  printf( "plot( data_unb_var2( :,1 ), data_unb_var2( :, 2 ), 'r-.' ); \n" );
  printf( "plot( data_blk_var2( :,1 ), data_blk_var2( :, 2 ), 'r--' ); \n" );
  printf( "plot( data_unb_var3( :,1 ), data_unb_var3( :, 2 ), 'r-.' ); \n" );
  printf( "plot( data_blk_var3( :,1 ), data_blk_var3( :, 2 ), 'r--' ); \n" );

  printf( "hold on \n");

  printf( "xlabel( 'matrix dimension m=n' );\n");
  printf( "ylabel( 'GFLOPS/sec.' );\n");
  printf( "axis( [ 0 %d 0 %3.1f ] ); \n", nlast, max_gflops );
  printf( "legend( 'FLA Chol nopiv', ...\n");
  printf( "        'unb var1', ...\n");
  printf( "        'unb var2', ...\n");
  printf( "        'unb var3', ...\n");
  printf( "        'blk var1', ...\n");
  printf( "        'blk var2', ...\n");
  printf( "        'blk var3', 2);\n");

  printf( "print -r100 -depsc Gemm.eps\n");

  FLA_Finalize( );

  return 0;
}