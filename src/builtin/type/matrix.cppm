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

    export constexpr id_type type_id(builtin_hint, builtin_matrix_t &)
    {
        return matrix_id;
    }
    export dyn_node::data_proxy clone(builtin_hint, builtin_matrix_t &d)
    {
        return std::make_shared<builtin_matrix_t>(d);
    }

    export void save_matrix(archive::output_archive_t &ar, builtin_matrix_t &tb)
    {
        auto rows = tb.rows();
        auto cols = tb.cols();

        ar(rows, cols);
        ar.saveBinary(tb.data(), rows * cols * sizeof(double));
    }
    export builtin_matrix_t load_matrix(archive::input_archive_t &ar)
    {
        Eigen::Index r, c;
        ar(r, c);
        builtin_matrix_t tb(r, c);
        ar.loadBinary(tb.data(), r*c*sizeof(double));
        return tb;
    }

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

        dyn_node::data_proxy load(archive::input_archive_t &ar)
        {
            auto tb = load_matrix(ar);

            return std::make_shared<builtin_matrix_t>(std::move(tb));
        }

        void save(archive::output_archive_t &ar, dyn_node::data_proxy &p)
        {
            assert(id() == p->type_id());
            auto &&tb = proxy_cast<builtin_matrix_t&>(*p);
            save_matrix(ar, tb);
        }
    };
    static_assert(dyn_node::type_definition<matrix_definition>);

}
