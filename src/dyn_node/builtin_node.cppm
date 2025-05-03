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
        [[nodiscard]] std::vector<id_type> channels() const
        {
            return {INT_ID};
        }
        [[nodiscard]] std::vector<id_type> outputs() const
        {
            return {INT_ID};
        }
        bool set_channel(std::size_t i, data_proxy p)
        {
            if (i > 0)
            {
                return false;
            }

            try
            {
                if (p->type_id() != INT_ID)
                {
                    return false;
                }
                else
                {
                    val = proxy_cast<int>(*p);
                    return true;
                }
            }
            catch (pro::bad_proxy_cast &e)
            {
                return false;
            }
            __builtin_unreachable();
        }
        [[nodiscard]] data_proxy get_channel(std::size_t i) const
        {
            if (val)
            {
                return std::make_unique<int>(*val);
            } else
            {
                return nullptr;
            }
        }
        void compute() const
        {
        }

        [[nodiscard]] data_proxy get_output(std::size_t i) const
        {
            if (val)
            {
                return std::make_unique<int>(*val);
            }
            else
            {
                return nullptr;
            }
        }

        std::optional<int> val = std::nullopt;
    };

    node_instance_proxy p = std::make_unique<identity_node_instance>();

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

    node_definitions_t builtin_definitions()
    {
        node_definitions_t rt{};

        auto idn = std::make_unique<identity_node_definition>();

        rt[IDENTITY_ID] = std::move(idn);
        return rt;
    }
}
