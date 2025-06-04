//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-25.
//

module;
#include <proxy/proxy.h>

#include <string_view>
#include <string>
#include <vector>
#include <utility>

export module blueprint.plugin:compoent_package;
import blueprint.dyn_node;

namespace blueprint::plugin
{
    using std::pmr::vector;
    using std::pmr::string;
    using std::string_view;

    export struct group_descriptor
    {
        string group_name;
        vector<string_view> members;
    };

    export struct component_package
    {
        vector<dyn_node::type_definition_proxy> types; ///< The types provided by package.
        vector<dyn_node::node_definition_proxy> nodes; ///< Nodes provided by package.

        /// The group of nodes. The nodes that aren't in any group will not display in GUI.
        vector<group_descriptor> groups;
    };
}
