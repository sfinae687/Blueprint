//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//

module;
#include <memory>

export module blueprint.dyn_node:builtin_type;
import :definition;

namespace blueprint::dyn_node
{

    // Tag

    namespace builtin
    {
        struct builtin_hint {};
    }

    template <>
    struct lookup_hint<int>
    {
        using type = builtin::builtin_hint;
    };

    // Builtin Type

    namespace builtin
    {
        // Int

        export constexpr std::string_view INT_ID = "core.integral";

        export struct int_definition
        {
            text_type name() const
            {
                return "Integral";
            }
            text_type description() const
            {
                return "The builtin type of integral.";
            }
            id_type id() const
            {
                return INT_ID;
            }
        };

        export constexpr id_type type_id(builtin_hint, int)
        {
            return INT_ID;
        }
        export data_proxy clone(builtin_hint, int i)
        {
            return std::make_unique<int>(i);
        }

        // utility

        template <typename T>
        auto definition_pair()
        {
            auto t = std::make_unique<T>();
            return std::pair(t->name(), std::move(t));
        }

        export type_definitions_t builtin_type_definitions()
        {
            type_definitions_t rt{};
            rt.insert(definition_pair<int_definition>());
            return rt;
        }

    }

}
