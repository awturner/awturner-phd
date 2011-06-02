for ( IndexType ax = 0; ax < 2; ++ax )
{
   MT calc_grad[] = { 0, 0 };

   MT shape_grad[] = { 0, 0, 0 };

   // Record the lambdas
   Vector lams[] = { Vector( params.ns-1 ), Vector( params.ns-1 ) };

   Matrix covar_grad( params.ns, params.ns );

   for ( IndexType rr = 0; rr < 2; ++rr )
   {
      // Don't want to muck up shape_image
      const ImageType shape_image_copy = shape_image_stack;

      // Create a displacement vector and perturb the A-th component it
      Vector displacement_copy = displacement[i];
      displacement_copy( A + ax*n ) += ( 1.0 - 2.0 * rr ) * params.Delta;

      // Create a copy of the shape vectors
      Vector* shape_vector_copy = new Vector[ params.ns ];
      for ( IndexType ii = 0; ii < params.ns; ++ii )
         shape_vector_copy[ii] = shape_vector[ii];

      // Deform the shape image and shape vector
      populate_shape_vector( shape_image_copy.get_shared_channel( i ), params.h, displacement_copy, shape_vector_copy[i] );

      //PRINTVBL( i  );
      //PRINTVBL( A  );
      //PRINTVBL( ax );
      //PRINTVBL( rr );
      //PRINTVBL( shape_vector_copy[i]( A + 0*n ) );
      //PRINTVBL( shape_vector_copy[i]( A + 1*n ) );
      //PRINTVBL( shape_vector_copy[i]( A + 2*n ) );

      for ( IndexType axx = 0; axx < 3; ++axx )
      {
         shape_grad[axx] += ( 1.0 - 2.0*rr ) * shape_vector_copy[i][ A + axx*n ];
      }

      Matrix Covariance_copy( params.ns, params.ns );
      calculate_covariance( shape_vector_copy, params, Covariance_copy );

      //BLANKLINE;
      //printf_matrix( Covariance_copy );
      //BLANKLINE;

      //DEBUGLINE;
      //BLANKLINE;
      //printf_matrix( ( Covariance_copy - Covariance ) / params.Delta );
      //BLANKLINE;

      if ( rr == 0 )
         covar_grad = Covariance_copy;
      else
         covar_grad = ( covar_grad - Covariance_copy ) / ( 2 * params.Delta );

      // OK, so now we need to extract the eigenvalue_copys
      Vector eigenvalue_copy( params.ns );
      Matrix eigenvector_copy( params.ns, params.ns );

      calculate_eigensystem( Covariance_copy, eigenvalue_copy, eigenvector_copy );

      for ( IndexType a = 0; a < params.ns-1; ++a )
      {
         lams[rr]( a ) = eigenvalue_copy( a );
         calc_grad[rr] += log( eigenvalue_copy( a ) );
      }

      delete [] shape_vector_copy;

   }

   // This is a finite-difference estimate of d_Lambda_a_by_d_P_A
   Vector grad_lams = ( lams[0] - lams[1] ) / ( 2 * params.Delta );

   for ( IndexType axx = 0; axx < 3; ++axx )
      shape_grad[axx] /= -2 * params.Delta;

   BLANKLINE;
   PRINTVBL( ax );
   PRINTVBL( A );
   PRINTVBL2( "Finite Difference", ( calc_grad[1]-calc_grad[0] ) / params.Delta );
   PRINTVBL2( "Calculated", gradient[i]( A + ax*n ) );
   BLANKLINE;
   PRINTVEC( shape_grad, 3 );
   //PRINTVBL( grad_lams );
   PRINTVBLMATLAB( covar_grad );
   BLANKLINE;
   std::cerr << "==============================================" << std::endl;
   BLANKLINE;
}
/*
PRINTVBL( d_lam_a_by_d_p_a_X );
PRINTVBL( d_lam_a_by_d_p_a_Y );
PRINTVBL( sum_int_area );
DEBUGLINEANDEXIT( 99 );
*/
//PAUSE;