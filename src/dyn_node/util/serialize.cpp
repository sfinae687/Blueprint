//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/8.
//

module;

#include <functional>
#include <string>

module blueprint.dyn_node;
import :host_api;
import :byte_archive;
import :serialization;

namespace blueprint::dyn_node::util
{
    using host_api::host;

    binary_archive save_data_with_type(const data_proxy& data)
    {
        binary_archive ar;
        id_type tid = data->type_id();
        ar.save_string(tid);
        int has_v = data.has_value();
        ar.save_byte(has_v);
        if (has_v)
        {
            ar |= data->serialize();
        }
        return ar;
    }

    data_proxy load_data_with_type(binary_archive &ar)
    {
        std::string tid = ar.load_string();
        auto ty_def = host->get_type_definition(tid);
        if (int has_v = ar.load_byte<int>())
        {
            return ty_def->load(ar);
        }
        return nullptr;
    }
}