//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/9.
//

module;

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

#include <functional>
#include <utility>
#include <string>

export module blueprint.dyn_node:serialization;
export import blueprint.archive;
import :definition;
import :host_api;

namespace blueprint::dyn_node::util
{
    export template <archive::saveable T>
    auto save_wrapper()
    {
        return [] (archive::output_archive_t &ar, T &t)
        {
            archive::save(t, ar);
        };
    }

    export template <archive::loadable T>
    auto load_wrapper()
    {
        return [] (archive::input_archive_t &ar, T &t)
        {
            archive::load(t, ar);
        };
    }

    // Data //

    export void save_data(archive::output_archive_t &ar, data_proxy &p)
    {
        using host_api::host;
        int vv = p.has_value();
        ar(vv);
        if (vv)
        {
            auto &&d_ty = host->get_type_definition(p->type_id());
            d_ty->save(ar, p);
        }
    }

    export data_proxy load_data(archive::input_archive_t &ar, id_type id)
    {
        using host_api::host;
        int vv;
        ar(vv);
        if (vv)
        {
            auto &&d_ty = host->get_type_definition(id);
            return d_ty->load(ar);
        }
        return nullptr;
    }

    export void save_data_with_type(archive::output_archive_t &ar, data_proxy &p)
    {

        std::string tid{p->type_id()};
        ar(tid);
        save_data(ar, p);
    }

    export data_proxy load_data_with_type(archive::input_archive_t &ar)
    {
        using host_api::host;

        std::string str;
        ar(str);
        return load_data(ar, str);
    }

    // Node //

    export void save_node(archive::output_archive_t &ar, node_instance_proxy &p)
    {
        using host_api::host;

        int vv = p.has_value();
        ar(vv);
        if (vv)
        {
            auto &&tid = p->type_id();
            auto &&n_ty = host->get_node_definition(tid);
            n_ty->save(ar, p);
        }
    }

    export node_instance_proxy load_node(archive::input_archive_t &ar, id_type id)
    {
        using host_api::host;

        int vv;
        ar(vv);
        if (vv)
        {
            auto &&n_ty = host->get_node_definition(id);
            return n_ty->load(ar);
        }
        return nullptr;
    }

    export void save_node_with_type(archive::output_archive_t &ar, node_instance_proxy &p)
    {
        std::string tid{p->type_id()};
        ar(tid);
        save_node(ar, p);
    }

    export node_instance_proxy load_node_with_type(archive::input_archive_t &ar)
    {
        std::string str;
        ar(str);
        return load_node(ar, str);
    }
}
