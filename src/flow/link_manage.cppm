//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/key.hpp>

#include <optional>
#include <utility>

export module blueprint.flow:link_manage;
import :node_instance_manage;

namespace blueprint::flow
{


    // utility


    // link_manager

    namespace details
    {
        using namespace boost::multi_index;

        /// Helper class to build boost.multi_index container for link_manager
        template <typename Link, typename Input, typename Output>
        struct index_type_helper
        {
            using element_type = std::tuple<Link, Input, Output>;

            template<std::size_t i>
            static constexpr const std::tuple_element_t<i, element_type>& (*get)(const element_type &)
                = std::get<i, Link, Input, Output>;

            static_assert(std::invocable<decltype(get<0>), const element_type&>);
            static_assert(std::invocable<decltype(get<1>), const element_type&>);
            static_assert(std::invocable<decltype(get<2>), const element_type&>);

            using index_type = multi_index_container<
                element_type,
                indexed_by<
                    hashed_unique<key<get<0>>>,
                    hashed_unique<key<get<1>>>,
                    hashed_non_unique<key<get<2>>>
                >
            >;
        };
    }

    /**
     * The class stored the link information, and provided the necessary diagnostic operation needed by link
     * operation.
     */
    export class link_manager
    {
    public:
        using input_t = no_id;
        using output_t = no_id;
        using link_t = no_id;

    private:
        using index_helper = details::index_type_helper<link_t, input_t, output_t>;

    public:
        using element_type = index_helper::element_type;
        using index_type = index_helper::index_type;
        using link_index_type = index_type::nth_index<0>::type;
        using input_index_type = index_type::nth_index<1>::type;
        using output_index_type = index_type::nth_index<2>::type;

        explicit link_manager(node_instance_manager&);
        virtual ~link_manager() = default;

        // Copy control: Not copyable.

        link_manager(const link_manager&) = delete;
        link_manager& operator=(const link_manager&) = delete;

        // Connection Test

        [[nodiscard]] bool is_connected(output_t output, input_t input) const noexcept;
        [[nodiscard]] bool have_connection(input_t ch) const noexcept;

        // TODO Connection Test with direction

        // Link operation

        [[nodiscard("Lost the only reference to link")]]
        std::optional<link_t> create_link(output_t output, input_t input);

        void remove_link(output_t output, input_t input);


    protected:
        virtual no_id do_connect(output_t output, input_t input);
        virtual void do_remove(link_index_type::iterator);

        node_instance_manager& instance_info_;

        // relational

        index_type index_{};
        no_id last_id_ = 0;
    };
} // namespace blueprint::flow
