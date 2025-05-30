//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <compare>
#include <unordered_map>
#include <vector>
#include <shared_mutex>


export module blueprint.flow:node_instance_manage;
import blueprint.dyn_node;
import :NOID;

namespace blueprint::flow
{

    export class node_instance_handler;
    export class node_instance_manager;

    using element_type = std::pair<dyn_node::node_instance_proxy, no_id>;
    using list_type = std::unordered_map<no_id, element_type>;


    /**
     * The class represents the node instance stored in the node_instance_manager.
     */
    export class node_instance_handler
    {
        friend class node_instance_manager;
    public:
        /*NOLINT*/ node_instance_handler(std::nullptr_t = nullptr)
            : manager_(nullptr), it_({}) {}
        node_instance_handler(const node_instance_handler&) = default;
        node_instance_handler& operator=(const node_instance_handler&) = default;

        bool operator== (const node_instance_handler &) const = default;
        bool operator== (std::nullptr_t) const {return manager_ == nullptr || it_ == list_type::iterator{};}

        [[nodiscard]] const dyn_node::node_instance_proxy& node_instance() const noexcept;
        [[nodiscard]] dyn_node::node_instance_proxy& node_instance() noexcept;

        [[nodiscard]] no_id node_id() const noexcept;

        void remove();

    private:
        node_instance_handler(node_instance_manager& mg, list_type::iterator it) : manager_(&mg), it_(it) {}

        node_instance_manager* manager_;
        list_type::iterator it_{};
    };

    /**
     * The class owns and manages the node instances, and provide id form node instance.
     */
    class node_instance_manager
    {
        friend node_instance_handler;
    public:
        node_instance_manager() = default;
        node_instance_manager(const node_instance_manager& other) = delete;
        node_instance_manager& operator=(const node_instance_manager& other) = delete;

        node_instance_handler add_instance(dyn_node::node_instance_proxy p);

        [[nodiscard]] node_instance_handler get_handler(no_id) noexcept;

        std::vector<node_instance_handler> dump_handler() noexcept;

        // remove a node need a node_instance_handler.

    private:
        std::shared_mutex lock_{};
        no_id last_id_ = 0;
        list_type list_{};

    };


}
