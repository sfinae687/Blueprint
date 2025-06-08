//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/8.
//

module;

#include <utility>
#include <functional>

export module blueprint.dyn_node:serialization;
import :byte_archive;
import :definition;

namespace blueprint::dyn_node::util
{
    using byte_archive::binary_archive;

    export binary_archive save_data_with_type(const data_proxy&);
    export data_proxy load_data_with_type(binary_archive &);
}
