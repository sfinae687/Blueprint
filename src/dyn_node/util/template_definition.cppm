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


export module blueprint.dyn_node:template_definition;
import :definition;

namespace blueprint::dyn_node::util
{
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

    };


}
