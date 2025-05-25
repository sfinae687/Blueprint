//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-24.
//

module;
#include <proxy.h>
#include <boost/hana.hpp>

#include <string_view>
#include <string>
#include <memory>

export module blueprint.dyn_node:constant_factory;
import :definition;

namespace blueprint::dyn_node::util
{
    namespace hana = boost::hana;

    constexpr std::string constant_id_base = "util.constant";

    export class constant_node_definition
    {
    public:
        explicit constant_node_definition(data_proxy d);
        constant_node_definition(data_proxy d, std::string id);

        [[nodiscard]] id_type id() const noexcept;
        [[nodiscard]] text_type name() const noexcept;
        [[nodiscard]] text_type description() const noexcept;

        node_instance_proxy create_node();
    private:
        std::string id_;
        data_proxy data_;
    };
    static_assert(pro::proxiable<std::shared_ptr<constant_node_definition>, node_definition_facade>);

    export class constant_node_instance
    {
        friend constant_node_definition;
    public:
        constant_node_instance(id_type id, data_proxy d);

        [[nodiscard]] id_type type_id() const noexcept;
        [[nodiscard]] std::span<const signature_t> signatures() const noexcept;
        [[nodiscard]] std::size_t current_variant() const noexcept;
        bool set_variant(std::size_t) noexcept;
        bool compute(data_sequence) noexcept;
        [[nodiscard]] data_sequence output() const noexcept;

    private:

        id_type id_;
        data_proxy data_;
        signature_t sig_;
    };
    static_assert(pro::proxiable<std::unique_ptr<constant_node_instance>, node_instance_facade>);

}