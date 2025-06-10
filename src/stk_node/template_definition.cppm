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

    template <typename T, typename CTX>
    concept context_aware_node = archive::loadable<CTX> && archive::saveable<CTX> && std::constructible_from<T, CTX>
        && requires (T t, CTX ctx) {
            {t.context()} -> archive::saveable;
        };

    export template <const char Id[], const char Name[], const char Description[], typename T, typename CTX = void>
        requires std::same_as<CTX, void> || context_aware_node<T, CTX>
    class template_definition {};

    export template <const char Id[], const char Name[], const char Description[], node_instance T, typename CTX>
        requires context_aware_node<T, CTX>
    class template_definition<Id, Name, Description, T, CTX>
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

        node_instance_proxy load(archive::input_archive_t &ar)
        {
            CTX ctx;
            archive::load(ctx, ar);
            return std::make_shared<T>(std::move(ctx));
        }
        void save(archive::output_archive_t &ar, node_instance_proxy &p)
        {
            auto &&nt = proxy_cast<T&>(*p);
            auto &&ctx = nt.context();
            archive::save(ar, ctx);
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

        node_instance_proxy load(archive::input_archive_t &)
        {
            return create_node();
        }
        void save(archive::output_archive_t &, node_instance_proxy &p)
        {
            assert(id() == p->type_id());
        }

    };


}
