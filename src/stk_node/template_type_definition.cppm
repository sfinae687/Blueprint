//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/2.
//

module;

#include <proxy/proxy.h>

#include <concepts>
#include <cassert>
#include <memory>

export module blueprint.stk_node:template_type_definition;
import blueprint.dyn_node;

namespace blueprint::stk_node
{
    export template <const char *Id, const char *Name, const char *Description, typename D>
        requires archive::loadable<D> && archive::saveable<D>
    struct template_type_definition
    {
        /*NOLINT*/ [[nodiscard]] dyn_node::text_type name() const noexcept
        {
            return Name;
        }
        /*NOLINT*/ [[nodiscard]] dyn_node::text_type description() const noexcept
        {
            return Description;
        }
        /*NOLINT*/ [[nodiscard]] dyn_node::id_type id() const noexcept
        {
            return Id;
        }


        dyn_node::data_proxy load(archive::input_archive_t &ar)
        {
            D d;
            archive::load(d, ar);
            return std::make_shared<D>(std::move(d));
        }

        void save(archive::output_archive_t &ar, dyn_node::data_proxy &p)
        {
            auto &&d = proxy_cast<D&>(*p);
            archive::save(d, ar);
        }
    };
}
