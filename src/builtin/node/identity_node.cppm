//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-25.
//

module;
#include <proxy.h>

#include <string_view>
#include <string>
#include <span>

export module blueprint.builtin_node:identity_node;
import blueprint.dyn_node;

namespace blueprint::dyn_node::builtin
{
    using std::string;
    using std::string_view;

    export class identity_node_definition
    {
    public:
        explicit identity_node_definition(type_definition_proxy ty);

        id_type id() const noexcept;
        text_type name() const noexcept;
        text_type description() const noexcept;

        node_instance_proxy create_node() noexcept;

    private:
        type_definition_proxy ty_;
        string name_;
        string id_;
    };
    static_assert(pro::proxiable<std::shared_ptr<identity_node_definition>, node_definition_facade>);

    export class identity_node_instance
    {
    public:
        identity_node_instance(string_view id, const type_definition_proxy &ty);

        [[nodiscard]] id_type type_id() const noexcept;
        [[nodiscard]] std::span<const signature_t> signatures() const noexcept;
        [[nodiscard]] std::size_t current_variant() const noexcept;
        bool set_variant(std::size_t) noexcept;
        bool compute(data_sequence) noexcept;
        [[nodiscard]] data_sequence output() const noexcept;

    private:
        id_type id_;
        signature_t sig_;
        data_proxy data_;
    };
    static_assert(pro::proxiable<std::unique_ptr<identity_node_instance>, node_instance_facade>);
}
