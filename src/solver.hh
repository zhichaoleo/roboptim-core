// Copyright (C) 2009 by Thomas Moulard, FIXME.
//
// This file is part of the liboptimization.
//
// liboptimization is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// liboptimization is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with liboptimization.  If not, see <http://www.gnu.org/licenses/>.

/**
 * \file solver.hh
 *
 * \brief Declaration of the Solver class.
 */

#ifndef OPTIMIZATION_SOLVER_HH
# define OPTIMIZATION_SOLVER_HH

# include <boost/function.hpp>
# include <boost/optional.hpp>
# include <boost/variant.hpp>
# include <boost/utility.hpp>

# include <optimization-fwd.hh>

namespace optimization
{
  /// Generic solver error.
  class SolverError
  {
    //FIXME: make exception.
  };

  /// Abstract solver class.
  template <typename F>
  class Solver : public boost::noncopyable
  {
  public:
    typedef F function_t;
    typedef double functionResult_t; // boost::result_of<function_t>
    typedef boost::variant<functionResult_t, SolverError> result_t;

    typedef function_t gradient_t;


    Solver (const function_t&) throw ();
    virtual ~Solver () throw ();

    virtual result_t getMinimum () throw () = 0;

  private:
    boost::optional<gradient_t> gradient_;
    result_t result_;

    //FIXME: More?
    //boost::optional<jacobian_t> jacobian_;
    //boost::optional<lagrangian_t> lagrangian_;
  };

} // end of namespace optimization

# include <solver.hxx>
#endif //! OPTIMIZATION_SOLVER_HH
