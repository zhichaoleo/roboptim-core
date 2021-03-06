// Copyright (C) 2014 by Benjamin Chrétien, CNRS.
//
// This file is part of the roboptim.
//
// roboptim is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// roboptim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with roboptim.  If not, see <http://www.gnu.org/licenses/>.

#include "shared-tests/fixture.hh"

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <roboptim/core/fwd.hh>
#include <roboptim/core/deprecated.hh>
#include <roboptim/core/io.hh>
#include <roboptim/core/solver.hh>
#include <roboptim/core/result.hh>
#include <roboptim/core/result-with-warnings.hh>
#include <roboptim/core/solver-error.hh>

using namespace roboptim;

// Specify the solver that will be used.
typedef Solver<EigenMatrixDense> parent_solver_t;

typedef boost::shared_ptr<boost::test_tools::output_test_stream>
  outputStreamPtr;
outputStreamPtr output;

// Define a simple function.
struct F : public Function
{
  F () : Function (1, 1, "x")
  {}

  void impl_compute (result_ref res,
                     const_argument_ref x) const
  {
    res (0) = x[0];
  }

  // No gradient, hessian.
};

class NullSolver : public parent_solver_t
{
public:
  /// Define parent's type.
  typedef parent_solver_t parent_t;
  typedef Function::argument_t argument_t;

  NullSolver (const problem_t& pb)
    : parent_t (pb)
  {
  }

  ~NullSolver ()
  {
  }

  void
  solve ()
  {
    (*output) << "solve ()" << std::endl;

    SolverError error ("the null solver always fails.");
    error.lastState () = Result (1, 1);

    result_ = error;
  }
};

class ResultVisitor : public boost::static_visitor<void>
{
public:
  ResultVisitor (outputStreamPtr o) : output_ (o) {}

  void operator() (const roboptim::Result& res)
  {
    (*output_) << "Visitor (Result):" << incindent
               << iendl << res << decindent << iendl;
  }

  ROBOPTIM_ALLOW_DEPRECATED_ON
  void operator() (const roboptim::ResultWithWarnings& res)
  {
    (*output_) << "Visitor (ResultWithWarnings):" << incindent
               << iendl << res << decindent << iendl;
  }
  ROBOPTIM_ALLOW_DEPRECATED_OFF

  void operator() (const roboptim::SolverError& res)
  {
    (*output_) << "Visitor (SolverError):" << incindent
               << iendl << res << decindent << iendl;
  }

  void operator() (const roboptim::NoSolution&)
  {
    (*output_) << "Visitor (NoSolution)" << std::endl;
  }

  template <typename R>
  void operator() (const R&)
  {
    (*output_) << "Unknown result type" << std::endl;
  }

private:
  outputStreamPtr output_;
};

BOOST_FIXTURE_TEST_SUITE (core, TestSuiteConfiguration)

BOOST_AUTO_TEST_CASE (solver)
{
  typedef NullSolver solver_t;

  output = retrievePattern ("solver");

  // Instantiate the function, the problem, and solve it.
  boost::shared_ptr<F> f = boost::make_shared<F> ();
  solver_t::problem_t pb (f);
  solver_t::argument_t x (f->inputSize ());
  x.setZero ();
  pb.startingPoint () = x;

  ResultVisitor vis (output);

  solver_t solver (pb);
  (*output) << solver << std::endl << std::endl;
  solver.parameters ()["data.string"].value = std::string ("dummy data");
  solver.parameters ()["data.string"].description = "dummy string";
  solver.parameters ()["data.int"].value = int (10);
  solver.parameters ()["data.int"].description = "dummy integer";
  solver.parameters ()["data.value_type"].value = F::value_type (42.);
  solver.parameters ()["data.value_type"].description = "dummy value_type";
  (*output) << solver << std::endl << std::endl;
  boost::apply_visitor (vis, solver.minimum ());
  solver.solve ();
  (*output) << solver << std::endl << std::endl;
  boost::apply_visitor (vis, solver.minimum ());

  SolverError error = solver.getMinimum<SolverError> ();
  (*output) << error << std::endl;
  if (error.lastState ())
    (*output) << *(error.lastState ()) << std::endl;
  (*output) << std::endl;

  // Test solver copy.
  solver_t solver2 (solver);
  (*output) << solver2 << std::endl << std::endl;
  solver2.solve ();
  (*output) << solver2 << std::endl << std::endl;

  // Test solver reset.
  solver2.reset ();
  (*output) << solver2 << std::endl << std::endl;
  solver2.solve ();
  (*output) << solver2 << std::endl << std::endl;

  // Test iteration callback throw.
  BOOST_CHECK_THROW (solver.setIterationCallback (solver_t::callback_t ()),
                     std::runtime_error);

  // Test get minimum throw.
  BOOST_CHECK_THROW (solver.getMinimum<Result> (),
                     boost::bad_get);

  std::cout << output->str () << std::endl;
  BOOST_CHECK (output->match_pattern ());
}

BOOST_AUTO_TEST_SUITE_END ()
