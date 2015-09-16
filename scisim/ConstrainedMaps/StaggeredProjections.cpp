// StaggeredProjections.cpp
//
// Breannan Smith
// Last updated: 09/07/2015

#include "StaggeredProjections.h"

#include "SCISim/ConstrainedMaps/ImpactMaps/ImpactOperator.h"
#include "SCISim/ConstrainedMaps/FrictionMaps/FrictionOperator.h"
#include "SCISim/Constraints/Constraint.h"
#include "SCISim/Math/MathUtilities.h"
#include "SCISim/ConstrainedMaps/ConstrainedMapUtilities.h"
#include "SCISim/ConstrainedMaps/ImpactMaps/ImpactOperatorUtilities.h"
#include "SCISim/UnconstrainedMaps/FlowableSystem.h"
#include "SCISim/Utilities.h"
#include "SCISim/StringUtilities.h"
#include <iostream>

// For error computation
#include "SCISim/ConstrainedMaps/Sobogus.h"

StaggeredProjections::StaggeredProjections( const bool warm_start_alpha, const bool warm_start_beta, const ImpactOperator& impact_operator, const FrictionOperator& friction_operator )
: m_warm_start_alpha( warm_start_alpha )
, m_warm_start_beta( warm_start_beta )
, m_impact_operator( impact_operator.clone() )
, m_friction_operator( friction_operator.clone() )
{}

StaggeredProjections::StaggeredProjections( std::istream& input_stream )
: m_warm_start_alpha( Utilities::deserialize<bool>( input_stream ) )
, m_warm_start_beta( Utilities::deserialize<bool>( input_stream ) )
, m_impact_operator( ConstrainedMapUtilities::deserializeImpactOperator( input_stream ) )
, m_friction_operator( ConstrainedMapUtilities::deserializeFrictionOperator( input_stream ) )
{}

StaggeredProjections::~StaggeredProjections()
{}

//static scalar computeGlobalObjectiveSingleCoR( const SparseMatrixsc& Minv, const VectorXs& v0, const scalar& CoR, const VectorXs& mu, const SparseMatrixsc& N, const VectorXs& alpha, const SparseMatrixsc& D, const VectorXs& beta, const VectorXs& lambda, const VectorXs& nrel, const VectorXs& drel )
//{
//  // || N \alpha + D \beta ||^2_{M^{-1}}
//  assert( N.cols() == alpha.size() ); assert( D.cols() == beta.size() );
//  assert( N.rows() == D.rows() ); assert( Minv.cols() == N.rows() );
//  assert( Minv.cols() == Minv.rows() );
//  const scalar termA = ( N * alpha + D * beta ).dot( Minv * ( N * alpha + D * beta ) );
//
//  // ( (1+e) * N \alpha + D \beta )^T \dot{q}^p
//  assert( v0.size() == N.rows() ); assert( CoR >= 0.0 ); assert( CoR <= 1.0 );
//  const scalar termB = v0.dot( ( 1 + CoR ) * N * alpha + D * beta );
//
//  // ( 1 + e ) \alpha^T \dot{g}_N + \beta^T drel
//  assert( alpha.size() == nrel.size() ); assert( beta.size() == drel.size() );
//  const scalar termC = ( 1 + CoR ) * alpha.dot( nrel ) + beta.dot( drel );
//
//  // lambda^T diag( \mu ) \alpha
//  assert( lambda.size() == mu.size() ); assert( mu.size() == alpha.size() );
//  const scalar termD =  lambda.dot( ( mu.array() * alpha.array() ).matrix() );
//
//  return termA + termB + termC + termD;
//}

//static scalar computeGlobalError( const SparseMatrixsc& Minv, const VectorXs& v0, const VectorXs& CoR, const VectorXs& mu, const SparseMatrixsc& N, const VectorXs& alpha, const SparseMatrixsc& D, const VectorXs& beta, const VectorXs& lambda, const VectorXs& nrel, const VectorXs& drel )
//{
//  // || N \alpha + D \beta ||^2_{M^{-1}}
//  assert( N.cols() == alpha.size() ); assert( D.cols() == beta.size() );
//  assert( N.rows() == D.rows() ); assert( Minv.cols() == N.rows() );
//  assert( Minv.cols() == Minv.rows() );
//  const scalar termA = ( N * alpha + D * beta ).dot( Minv * ( N * alpha + D * beta ) );
//
//  // (  N ( 1 + diag(e) ) \alpha )^T \dot{q}^p
//  assert( CoR.size() == alpha.size() ); assert( ( CoR.array() >= 0.0 ).all() ); assert( ( CoR.array() <= 1.0 ).all() );
//  const scalar termB = v0.dot( ( N * ( ( 1.0 + CoR.array() ) * alpha.array() ).matrix() ) );
//
//  // ( D \beta )^T \dot{q}^p
//  assert( v0.size() == D.rows() );
//  const scalar termC = v0.dot( D * beta );
//
//  // ( ( 1 + diag(e) ) \alpha )^T \dot{g}_N
//  assert( alpha.size() == nrel.size() );
//  const scalar termD = nrel.dot( ( ( 1.0 + CoR.array() ) * alpha.array() ).matrix() );
//
//  // \beta^T drel
//  assert( beta.size() == drel.size() );
//  const scalar termE = beta.dot( drel );
//
//  // lambda^T diag( \mu ) \alpha
//  assert( lambda.size() == mu.size() ); assert( mu.size() == alpha.size() );
//  const scalar termF = lambda.dot( ( mu.array() * alpha.array() ).matrix() );
//
//  // See if it reduces to the single CoR version
//  #ifndef NDEBUG
//  if( CoR.isConstant( CoR(0), 0.0 ) )
//  {
//    const scalar single_cor_obj = computeGlobalObjectiveSingleCoR( Minv, v0, CoR(0), mu, N, alpha, D, beta, lambda, nrel, drel );
//    assert( fabs( single_cor_obj - ( termA + termB + termC + termD + termE + termF ) ) <= 1.0e-6 );
//  }
//  #endif
//
//  return termA + termB + termC + termD + termE + termF;
//}

//static void computeLambda( const std::vector<std::unique_ptr<Constraint>>& active_set, const VectorXs& q0, const VectorXs& v0, const SparseMatrixsc& Minv, const SparseMatrixsc& N, const VectorXs& alpha, const SparseMatrixsc& D, const VectorXs& beta, VectorXs& lambda )
//{
//  lambda.resize( active_set.size() );
//  assert( Minv.rows() == Minv.cols() ); assert( Minv.rows() == v0.size() );
//  assert( Minv.cols() == N.rows() ); assert( Minv.cols() == D.rows() );
//  assert( N.cols() == alpha.size() ); assert( D.cols() == beta.size() );
//  const VectorXs v_temp = v0 + Minv * ( N * alpha + D * beta );
//  for( unsigned i = 0; i < lambda.size(); ++i )
//  {
//    lambda( i ) = active_set[i]->computeLambda( q0, v_temp );
//  }
//}

//static scalar globalObjective( const std::vector<std::unique_ptr<Constraint>>& active_set, const SparseMatrixsc& Minv, const VectorXs& q0, const VectorXs& v0, const VectorXs& CoR, const VectorXs& mu, const SparseMatrixsc& N, const VectorXs& alpha, const SparseMatrixsc& D, const VectorXs& beta, const VectorXs& nrel, const VectorXs& drel )
//{
//  VectorXs lambda;
//  computeLambda( active_set, q0, v0, Minv, N, alpha, D, beta, lambda );
//  return computeGlobalError( Minv, v0, CoR, mu, N, alpha, D, beta, lambda, nrel, drel );
//}


// Verify that || M^-1 N \alpha ||_M^2  <= || (1 + cor) v0 + M^-1 f0 ||_M^2
// TODO: Update for kinematic boundaries
// TODO: Pass tolerance in as option
// TODO: Check sizes
#ifndef NDEBUG
static bool impactSolutionIsContraction( const SparseMatrixsc& M, const SparseMatrixsc& Minv, const SparseMatrixsc& N, const VectorXs& CoR, const VectorXs& v0, const VectorXs& v0F, const VectorXs& alpha, const VectorXs& nrel, const VectorXs& drel )
{
  if( ( nrel.array() != 0.0 ).any() ) { return true; }
  if( ( drel.array() != 0.0 ).any() ) { return true; }
    if( ( CoR.array() != CoR(0) ).any() ) { return true; }

  // || M^-1 N \alpha ||_M^2
  const scalar first{ ( Minv * N * alpha ).dot( N * alpha ) };
  // || (1 + cor) v0 + M^-1 f0 ||_M^2
  const scalar second{ ( CoR(0) * v0 + v0F ).dot( M * ( CoR(0) * v0 + v0F ) ) };
  if( first > second + 3.0e-4 ) { return false; }
  return true;
}
#endif

// Verify that || M^-1 f ||_M^2 <= || vinit + M^-1 N \alpha ||_M^2
// TODO: Update for kinematic boundaries
// TODO: Pass tolerance in as option
// TODO: Check sizes
#ifndef NDEBUG
static bool frictionSolutinIsContractionA( const SparseMatrixsc& M, const SparseMatrixsc& Minv, const VectorXs& f, const VectorXs& v2, const VectorXs& nrel, const VectorXs& drel )
{
  if( ( nrel.array() != 0.0 ).any() ) { return true; }
  if( ( drel.array() != 0.0 ).any() ) { return true; }

  // || M^-1 f ||_M^2
  const scalar first{ ( Minv * f ).dot( f ) };
  // || vinit + M^-1 N \alpha ||_M^2
  const scalar second{ ( v2 ).dot( M * v2 ) };
  if( first > second + 1.0e-5 ) { return false; }
  return true;
}
#endif

// Verify that || vinit + M^-1 f + M^-1 N \alpha ||_M^2 <= || vinit + M^-1 N \alpha ||_M^2
// TODO: Update for kinematic boundaries
// TODO: Pass tolerance in as option
// TODO: Check sizes
#ifndef NDEBUG
static bool frictionSolutinIsContractionB( const SparseMatrixsc& M, const SparseMatrixsc& Minv, const VectorXs& f, const VectorXs& v2, const VectorXs& nrel, const VectorXs& drel )
{
  if( ( nrel.array() != 0.0 ).any() ) { return true; }
  if( ( drel.array() != 0.0 ).any() ) { return true; }

  // || vinit + M^-1 f + M^-1 N \alpha ||_M^2
  const scalar T_impulse_friction{ ( v2 + Minv * f ).dot( M * ( v2 + Minv * f ) ) };
  // || vinit + M^-1 N \alpha ||_M^2
  const scalar T_impulse{ v2.dot( M * v2 ) };
  // The friction solve shouldn't add kinetic energy
  // Moved from 1.0e-6 when external warm starting enabled...
  if( T_impulse_friction > T_impulse + 1.0e-3 ) { return false; }
  return true;
}
#endif

static void computePerContactForce( const VectorXs& alpha, const VectorXs& beta, const MatrixXXsc& contact_bases, VectorXs& per_contact_force )
{
  assert( alpha.size() + beta.size() == contact_bases.cols() );
  assert( beta.size() % alpha.size() == 0 );

  const unsigned ncontacts{ static_cast<unsigned>( alpha.size() ) };
  const unsigned num_friction_samples{ static_cast<unsigned>( beta.size() ) / ncontacts };
  const unsigned dofs_per_body{ static_cast<unsigned>( contact_bases.rows() ) };

  per_contact_force.setZero( dofs_per_body * ncontacts );

  for( unsigned clsn_idx = 0; clsn_idx < ncontacts; ++clsn_idx )
  {
    const unsigned base_normal_index{ ( num_friction_samples + 1 ) * clsn_idx };
    assert( base_normal_index < contact_bases.cols() );
    // Grab the contact basis
    const MatrixXXsc basis{ contact_bases.block( 0, base_normal_index, dofs_per_body, dofs_per_body ) };
    assert( ( basis * basis.transpose() - MatrixXXsc::Identity( basis.rows(), basis.cols() ) ).lpNorm<Eigen::Infinity>() <= 1.0e-6 );
    assert( fabs( basis.determinant() - 1.0 ) <= 1.0e-6 );
    // Add in the impact contribution
    const unsigned base_force_index{ dofs_per_body * clsn_idx };
    assert( base_force_index + dofs_per_body - 1 < per_contact_force.size() );
    per_contact_force.segment( base_force_index, dofs_per_body ) += alpha( clsn_idx ) * basis.col( 0 );
    // Add in the friction contributions
    for( unsigned sample_num = 0; sample_num < num_friction_samples; ++sample_num )
    {
      const unsigned base_friction_index{ num_friction_samples * clsn_idx + sample_num };
      assert( base_friction_index < beta.size() );
      assert( fabs( basis.col( sample_num + 1 ).dot( basis.col( 0 ) ) ) <= 1.0e-6 );
      per_contact_force.segment( base_force_index, dofs_per_body ) += beta( base_friction_index ) * basis.col( sample_num + 1 );
    }
  }
}

static scalar computeSobogusError( const VectorXs& alpha, const VectorXs& beta, const MatrixXXsc& contact_bases, SobogusFrictionProblem& sobogus_problem )
{
  VectorXs per_contact_force;
  computePerContactForce( alpha, beta, contact_bases, per_contact_force );
  const scalar error{ sobogus_problem.computeError( per_contact_force ) };
  return error;
}

// NOTE: Can't precompute linear terms as they change during the solve
void StaggeredProjections::solve( const unsigned iteration, const scalar& dt, const FlowableSystem& fsys, const SparseMatrixsc& M, const SparseMatrixsc& Minv, const VectorXs& CoR, const VectorXs& mu, const VectorXs& q0, const VectorXs& v0, std::vector<std::unique_ptr<Constraint>>& active_set, const MatrixXXsc& contact_bases, const unsigned max_iters, const scalar& tol, VectorXs& f, VectorXs& alpha, VectorXs& beta, VectorXs& vout, bool& solve_succeeded, scalar& error )
{
  assert( MathUtilities::isSquare( M ) ); assert( MathUtilities::isSquare( Minv ) ); assert( M.rows() == Minv.rows() );
  assert( CoR.size() == alpha.size() ); assert( CoR.size() == mu.size() );
  assert( active_set.size() == std::vector<std::unique_ptr<Constraint>>::size_type( mu.size() ) );
  assert( v0.size() == M.cols() ); assert( vout.size() == v0.size() );
  assert( contact_bases.cols() % contact_bases.rows() == 0 );
  assert( active_set.size() == unsigned( contact_bases.cols() / contact_bases.rows() ) );

  // Friction basis
  SparseMatrixsc D;
  FrictionOperator::formGeneralizedSmoothFrictionBasis( v0.size(), alpha.size(), q0, active_set, contact_bases, D );

  // Impact basis
  SparseMatrixsc N{ static_cast<SparseMatrixsc::Index>( v0.size() ), static_cast<SparseMatrixsc::Index>( alpha.size() ) };
  ImpactOperatorUtilities::computeN( fsys, active_set, q0, N );

  VectorXs nrel{ alpha.size() };
  VectorXs drel{ beta.size() };
  Constraint::evalKinematicRelVelGivenBases( q0, v0, active_set, contact_bases, nrel, drel );

  // For computing the error
  // TODO: Make a constructor for this that takes the sparse matrix directly!
  assert( fsys.numVelDoFsPerBody() == 2 || fsys.numVelDoFsPerBody() == 3 || fsys.numVelDoFsPerBody() == 6 );
  SobogusFrictionProblem sbfp( fsys.numVelDoFsPerBody() == 2 ? SobogusSolverType::Balls2D : fsys.numVelDoFsPerBody() == 3 ? SobogusSolverType::RigidBody2D : SobogusSolverType::RigidBodies3D );
  {
    VectorXs flat_masses;
    sbfp.flattenMass( M, flat_masses );
    sbfp.initialize( active_set, contact_bases, flat_masses, q0, v0, CoR, mu, nrel, drel );
  }
  scalar global_error{ computeSobogusError( alpha, beta, contact_bases, sbfp ) };
  error = global_error;
  // Absolute tolerance
  if( global_error <= tol )
  {
    f = D * beta;
    vout = v0 + Minv * ( N * alpha + D * beta );
    solve_succeeded = true;
    return;
  }

  // Quadratic term in LCP QP
  const SparseMatrixsc QN{ N.transpose() * Minv * N };

  // Quadratic term in MDP QP
  const SparseMatrixsc QD{ D.transpose() * Minv * D };

  // Track the 'best' friction result as progress is not always monotonic
  VectorXs best_alpha{ alpha };
  VectorXs best_beta{ beta };
  unsigned best_iteration{ 0 };

  // Staggered projections loop to compute coupled impact/friction
  solve_succeeded = false;
  for( unsigned itr = 0; itr < max_iters; ++itr )
  {
    // Impact solve
    {
      // Incoming velocity with the friction impulses applied
      const VectorXs vbeta{ v0 + Minv * f };
      // Solve for the impact impulses given the total friction impulse
      if( !m_warm_start_alpha )
      {
        alpha.setZero();
      }
      m_impact_operator->flow( active_set, M, Minv, q0, v0, vbeta, N, QN, nrel, CoR, alpha );
      // Verify that || M^-1 N \alpha ||_M^2  <= || (1 + cor) v0 + M^-1 f0 ||_M^2
      assert( impactSolutionIsContraction( M, Minv, N, CoR, v0, vbeta, alpha, nrel, drel ) );
    }

    // Compute the new global error
    global_error = computeSobogusError( alpha, beta, contact_bases, sbfp );
    //fabs( globalObjective( active_set, Minv, q0, v0, CoR, mu, N, alpha, D, beta, nrel, drel ) );

    // If the current solution is the best yet, cache it
    if( global_error < error )
    {
      error = global_error;
      best_beta = beta;
      best_alpha = alpha;
      best_iteration = itr;
    }

    // TODO: If we bail here on the first iteration, D \beta might not equal f

    // Absolute tolerance
    if( global_error <= tol )
    {
      assert( global_error == error );
      assert( ( beta.array() == best_beta.array() ).all() );
      assert( ( alpha.array() == best_alpha.array() ).all() );
      solve_succeeded = true;
      f = D * beta;
      vout = v0 + Minv * ( N * alpha + D * beta );
      break;
    }

    // Friction solve
    {
      // Incoming velocity with the impact impulses applied
      const VectorXs vaplha{ v0 + Minv * N * alpha };

      // Solve for a new estimate of the friction impulse
      if( !m_warm_start_beta )
      {
        beta.setZero();
      }
      {
        // TODO: Get rid of lambda from the friction operator
        VectorXs temp_lambda( mu.size() );
        m_friction_operator->flow( iteration * dt, Minv, vaplha, D, QD, drel, mu, alpha, beta, temp_lambda );
      }
      f = D * beta;
      // Verify that || M^-1 f ||_M^2 <= || vinit + M^-1 N \alpha ||_M^2
      assert( frictionSolutinIsContractionA( M, Minv, f, vaplha, nrel, drel ) );
      // Verify that || vinit + M^-1 f + M^-1 N \alpha ||_M^2 <= || vinit + M^-1 N \alpha ||_M^2
      assert( frictionSolutinIsContractionB( M, Minv, f, vaplha, nrel, drel ) );
    }
  }
  // If the solve failed, current iteration might not be the best, so fall back on the best solution
  if( !solve_succeeded )
  {
    assert( error > tol );
    f = D * best_beta;
    beta = best_beta;
    alpha = best_alpha;
    vout = v0 + Minv * ( N * alpha + D * beta );
    std::cerr << "Warning, staggered projections solve failed, falling back to solution at iteration " << best_iteration << std::endl;
  }
  assert( ( beta.array() == best_beta.array() ).all() );
  assert( ( f - D * beta ).lpNorm<Eigen::Infinity>() <= 1.0e-10 );
  assert( ( alpha.array() == best_alpha.array() ).all() );
  assert( error == computeSobogusError( alpha, beta, contact_bases, sbfp ) );
}

unsigned StaggeredProjections::numFrictionImpulsesPerNormal( const unsigned ambient_space_dimensions ) const
{
  return m_friction_operator->numFrictionImpulsesPerNormal();
}

void StaggeredProjections::serialize( std::ostream& output_stream ) const
{
  Utilities::serializeBuiltInType( m_warm_start_alpha, output_stream );
  Utilities::serializeBuiltInType( m_warm_start_beta, output_stream );
  ConstrainedMapUtilities::serialize( m_impact_operator, output_stream );
  ConstrainedMapUtilities::serialize( m_friction_operator, output_stream );
}

std::string StaggeredProjections::name() const
{
  return "staggered_projections";
}