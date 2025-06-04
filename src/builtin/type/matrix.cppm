//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/5/31.
//

module;
#include <Eigen/Dense>

#include <proxy/proxy.h>

#include <memory>

export module blueprint.builtin_node:matrix;
import blueprint.dyn_node;

namespace blueprint::builtin
{
    export using builtin_matrix_t = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
    export constexpr char matrix_id[] = "core.matrix.int";

    export struct builtin_hint{};
}

namespace /*NOLINT*/ blueprint::dyn_node
{
    template <>
    struct lookup_hint<builtin::builtin_matrix_t>
    {
        using type = builtin::builtin_hint;
    };
}

namespace blueprint::builtin
{

    using dyn_node::text_type;
    using dyn_node::id_type;


    export struct matrix_definition
    {
        /* NOLINT */ [[nodiscard]] text_type name() const
        {
            return "Matrix";
        }
        /* NOLINT */ [[nodiscard]] text_type description() const
        {
            return "The matrix of double";
        }
        /* NOLINT */ [[nodiscard]] id_type id() const
        {
            return matrix_id;
        }
    };

    export constexpr id_type type_id(builtin_hint, builtin_matrix_t &)
    {
        return matrix_id;
    }
    export dyn_node::data_proxy clone(builtin_hint, builtin_matrix_t &d)
    {
        return std::make_shared<builtin_matrix_t>(d);
    }

    static_assert(pro::proxiable<std::shared_ptr<builtin_matrix_t>, dyn_node::data_interface_facade>);

}
