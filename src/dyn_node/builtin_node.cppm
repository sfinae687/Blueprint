//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-2.
//
module;
#include <proxy.h>

#include <memory>
#include <vector>
#include <optional>

export module blueprint.dyn_node:builtin_node;
import :definition;
import :builtin_type;

namespace blueprint::dyn_node::builtin
{

    // Constant

    export constexpr std::string_view IDENTITY_ID = "core.identity";

    // Identity

    export struct identity_node_instance
    {
        friend id_type type_id(const identity_node_instance&) noexcept
        {
            return IDENTITY_ID;
        }
        [[nodiscard]] std::size_t current_variant() const noexcept
        {
            return 0;
        }
        bool set_variant(std::size_t) noexcept
        {
            return false;
        }
        [[nodiscard]] std::span<signature_t> signatures() const noexcept
        {
            static signature_t sig = {{INT_ID}, {INT_ID}};
            return {&sig, 1};
        }

        bool compute(data_sequence seq) noexcept
        {
            if (seq.size() != 1 || seq[0]->type_id() != INT_ID)
            {
                return false;
            }
            val = proxy_cast<int>(seq[0]);
            return true;
        }
        [[nodiscard]] data_sequence output() const noexcept
        {
            return {std::make_shared<int>(*val)};
        }

        std::optional<int> val = std::nullopt;
    };
    static_assert(pro::proxiable<std::unique_ptr<identity_node_instance>, node_instance_facade>);

    export struct identity_node_definition
    {
        [[nodiscard]] text_type name() const
        {
            return "Identity";
        }
        [[nodiscard]] text_type description() const
        {
            return "The node output the data passed in without changing. It can also used as display data.";
        }
        [[nodiscard]] id_type id() const
        {
            return IDENTITY_ID;
        }
        [[nodiscard]] node_instance_proxy create_node() const
        {
            return std::make_unique<identity_node_instance>();
        }
    };

    // utility

    export node_definitions_t builtin_definitions()
    {
        node_definitions_t rt{};

        auto idn = std::make_unique<identity_node_definition>();

        rt[IDENTITY_ID] = std::move(idn);
        return rt;
    }
}
