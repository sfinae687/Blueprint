//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/4.
//

module;

#include <Eigen/Dense>

export module blueprint.builtin_node:matrix_arithmetic;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    using def_t = dyn_node::node_definition_proxy;

    export std::optional<builtin_matrix_t> matrix_plus(builtin_matrix_t lhs, builtin_matrix_t rhs)
    {
        if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
        {
            return std::nullopt;
        }

        return lhs + rhs;
    }

    export std::optional<builtin_matrix_t> matrix_subtract(builtin_matrix_t lhs, builtin_matrix_t rhs)
    {
        if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
        {
            return std::nullopt;
        }

        return lhs - rhs;
    }

    export std::optional<builtin_matrix_t> matrix_multiplies(builtin_matrix_t lhs, builtin_matrix_t rhs)
    {
        if (lhs.cols() != rhs.rows())
        {
            return std::nullopt;
        }

        return lhs * rhs;
    }

    export std::optional<builtin_matrix_t> matrix_array_multiplies(builtin_matrix_t lhs, builtin_matrix_t rhs)
    {
        if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
        {
            return std::nullopt;
        }
        return lhs.array() * rhs.array();
    }

    export std::optional<builtin_matrix_t> matrix_array_divide(builtin_matrix_t lhs, builtin_matrix_t rhs)
    {
        if (lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
        {
            return std::nullopt;
        }
        if (rhs.array().any() == 0.0)
        {
            return std::nullopt; // Division by zero
        }
        return lhs.array() / rhs.array();
    }

    export builtin_matrix_t matrix_scalar_plus(builtin_matrix_t lhs, double rhs)
    {
        return lhs.array() + rhs;
    }

    export builtin_matrix_t matrix_scalar_subtract(builtin_matrix_t lhs, double rhs)
    {
        return lhs.array() - rhs;
    }

    export builtin_matrix_t matrix_scalar_multiplies(builtin_matrix_t lhs, double rhs)
    {
        return lhs.array() * rhs;
    }

    export std::optional<builtin_matrix_t> matrix_scalar_divide(builtin_matrix_t lhs, double rhs)
    {
        if (rhs == 0.0)
        {
            return std::nullopt;
        }
        return lhs.array() / rhs;
    }


    export builtin_matrix_t matrix_negate(builtin_matrix_t x)
    {
        return -x;
    }

    export std::optional<builtin_matrix_t> matrix_hypot(builtin_matrix_t x, builtin_matrix_t y, builtin_matrix_t z)
    {
        if (x.size() != y.size() || x.size() != z.size())
        {
            return std::nullopt;
        }

        auto &&xa = x.array();
        auto &&ya = y.array();
        auto &&za = z.array();
        return Eigen::sqrt(xa * xa + ya * ya + za * za);
    }

    export def_t def_matrix_plus()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_plus,
            "core.arithmetic.plus.matrix", "Matrix Plus", "Plus two matrix"
        );
    }
    export def_t def_matrix_subtract()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_subtract,
            "core.arithmetic.subtract.matrix", "Matrix Subtract", "Subtract two matrix"
        );
    }

    export def_t def_matrix_multiplies()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_multiplies,
            "core.arithmetic.multiply.matrix", "Matrix Multiply", "Multiply two matrix"
        );
    }

    export def_t def_matrix_array_multiplies()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_array_multiplies,
            "core.arithmetic.array-multiply.matrix", "Matrix Array Multiply",
            "Multiply two matrix with array semantics"
        );
    }

    export def_t def_matrix_scalar_plus()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_scalar_plus,
            "core.arithmetic.plus.scalar", "Scalar Plus", "Add a scalar to a matrix"
        );
    }

    export def_t def_matrix_scalar_subtract()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_scalar_subtract,
            "core.arithmetic.subtract.scalar", "Scalar Subtract", "Subtract a scalar from a matrix"
        );
    }

    export def_t def_matrix_scalar_multiplies()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_scalar_multiplies,
            "core.arithmetic.multiply.scalar", "Scalar Multiply", "Multiply a matrix by a scalar"
        );
    }

    export def_t def_matrix_negate()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_negate,
            "core.arithmetic.negate.matrix", "Matrix Negate", "Negate a matrix"
        );
    }

    export def_t def_matrix_array_divide()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_array_divide,
            "core.arithmetic.array-divide.matrix", "Matrix Array Divide",
            "Divide two matrix with array semantics"
        );
    }

    export def_t def_matrix_scalar_divide()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_scalar_divide,
            "core.arithmetic.divide.scalar", "Scalar Divide", "Divide a matrix by a scalar"
        );
    }

    export def_t def_matrix_hypot()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&matrix_hypot,
            "core.arithmetic.hypot.matrix", "Matrix Hypot", "Compute the hypotenuse of three matrix"
        );
    }


}
