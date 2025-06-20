//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-24.
//

module;
#include <proxy/proxy.h>

#include <string_view>
#include <memory>

export module blueprint.builtin_node:unsigned_integral;
import blueprint.dyn_node;

namespace blueprint::dyn_node
{

    // Tag

    namespace builtin
    {
        export using builtin_unsigned_type = unsigned long long;
        export constexpr char UNSIGNED_INTEGRAL_ID[] = "core.integral.unsigned";

        struct builtin_hint {};
    }

    template <>
    struct lookup_hint<builtin::builtin_unsigned_type>
    {
        using type = builtin::builtin_hint;
    };

    // Signed Integral

    namespace builtin
    {
        // Int

        export constexpr id_type type_id(builtin_hint, builtin_unsigned_type)
        {
            return UNSIGNED_INTEGRAL_ID;
        }
        export data_proxy clone(builtin_hint, builtin_unsigned_type i)
        {
            return std::make_shared<builtin_unsigned_type>(i);
        }

        export struct uint_definition
        {
            /*NOLINT*/ [[nodiscard]] text_type name() const
            {
                return "Unsigned Integral";
            }
            /*NOLINT*/ [[nodiscard]] text_type description() const
            {
                return "The builtin type of unsigned integral.";
            }
            /*NOLINT*/ [[nodiscard]] id_type id() const
            {
                return UNSIGNED_INTEGRAL_ID;
            }

            dyn_node::data_proxy load(archive::input_archive_t &ar)
            {
                builtin_unsigned_type si;
                ar(si);
                return std::make_shared<builtin_unsigned_type>(si);
            }

            void save(archive::output_archive_t &ar, dyn_node::data_proxy &p)
            {
                assert(p->type_id() == UNSIGNED_INTEGRAL_ID);

                auto si = proxy_cast<builtin_unsigned_type>(*p);
                ar(si);
            }
        };


    }
}
