//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <proxy/proxy.h>

#include <boost/hana.hpp>

#include <concepts>
#include <type_traits>

export module blueprint.stk_node:template_definition;
import blueprint.dyn_node;

namespace blueprint::stk_node
{

    using dyn_node::text_type;
    using dyn_node::node_instance;
    using dyn_node::id_type;
    using dyn_node::node_instance_proxy;
    using dyn_node::binary_archive;

    export template <const char Id[], const char Name[], const char Description[], node_instance T, typename CTX = void>
    class template_definition
    {
        using context_t = std::remove_cvref_t<CTX>;
    public:
        template_definition(CTX &&ctx)
            : context_(std::forward<CTX>(ctx))
        {}


        text_type name() const noexcept
        {
            return Name;
        }
        text_type description() const noexcept
        {
            return Description;
        }
        id_type id() const noexcept
        {
            return Id;
        }

        node_instance_proxy create_node()
        {
            return std::make_shared<T>(context_);
        }

        node_instance_proxy load(binary_archive &ar)
        {
            return std::make_shared<T>(load_binary<CTX>(ar));
        }

    private:
        context_t context_;
    };

    export template <const char Id[], const char Name[], const char Description[], node_instance T>
    class template_definition<Id, Name, Description, T, void>
    {
    public:
        template_definition() = default;

        text_type name() const noexcept
        {
            return Name;
        }
        text_type description() const noexcept
        {
            return Description;
        }
        id_type id() const noexcept
        {
            return Id;
        }

        node_instance_proxy create_node()
        {
            return std::make_shared<T>();
        }

        node_instance_proxy load(binary_archive &)
        {
            return create_node();
        }

    };


}
