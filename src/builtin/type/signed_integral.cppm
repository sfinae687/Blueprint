//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-24.
//

module;

#include <proxy/proxy.h>

#include <cassert>
#include <memory>

export module blueprint.builtin_node:signed_integral;
import blueprint.dyn_node;

namespace blueprint::dyn_node
{

    // Tag

    namespace builtin
    {
        export using builtin_signed_type = long long;
        export constexpr char SIGNED_INTEGRAL_ID[] = "core.integral.signed";
        struct builtin_hint {};
    }

    template <>
    struct lookup_hint<builtin::builtin_signed_type>
    {
        using type = builtin::builtin_hint;
    };

    // Signed Integral

    namespace builtin
    {
        // Int
        export constexpr id_type type_id(builtin_hint, builtin_signed_type)
        {
            return SIGNED_INTEGRAL_ID;
        }
        export data_proxy clone(builtin_hint, builtin_signed_type i)
        {
            return std::make_shared<builtin_signed_type>(i);
        }


        export struct sint_definition
        {
            /*NOLINT*/ [[nodiscard]] text_type name() const noexcept
            {
                return "Signed Integral";
            }
            /*NOLINT*/ [[nodiscard]] text_type description() const noexcept
            {
                return "The builtin type of signed integral.";
            }
            /*NOLINT*/ [[nodiscard]] id_type id() const noexcept
            {
                return SIGNED_INTEGRAL_ID;
            }

            dyn_node::data_proxy load(archive::input_archive_t &ar)
            {
                builtin_signed_type si;
                ar(si);
                return std::make_shared<builtin_signed_type>(si);
            }

            void save(archive::output_archive_t &ar, dyn_node::data_proxy &p)
            {
                assert(p->type_id() == SIGNED_INTEGRAL_ID);

                auto si = proxy_cast<builtin_signed_type>(*p);
                ar(si);
            }
        };
    }
}