//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

module;
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/key.hpp>

#include <optional>
#include <utility>

export module blueprint.flow:link_manage;
import :node_instance_manage;

namespace blueprint::flow
{

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
                    ordered_unique<key<get<1>>>,
                    ordered_non_unique<key<get<2>>>
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

        // query

        [[nodiscard]] bool empty() const noexcept;

        /// Get all link id.
        [[nodiscard]] std::vector<link_t> all_link() const noexcept;

        /// Query the two side of the given link.
        [[nodiscard]] std::optional<std::pair<input_t, output_t>> query_link(link_t) const noexcept;

        /// Query all input connected to the given output
        [[nodiscard]] std::vector<input_t> to_input(output_t) const noexcept;

        /// Query the output connected to given input.
        [[nodiscard]] std::optional<output_t> to_output(input_t) const noexcept;

        // Link operation

        /**
         * Create a link between output and input.
         * create_link will performance the type checking and link existence test.
         * If all tests are passed, do_connect is called to process the link create.
         *
         * @pre The input channel hasn't been to a output channel, or connected to the given output channel.
         *
         * @param output The output channel
         * @param input The input channel
         *
         * @return The link id between output and link, or nullopt if fail.
         * If the link has existed, the link id for the current link is returned.
         */
        std::optional<link_t> create_link(output_t output, input_t input) noexcept;

        /**
         * Remove the link.
         *
         * @pre The link has existed.
         *
         * @param output The output channel
         * @param input The input channel
         * @return True if success.
         */
        bool remove_link(output_t output, input_t input);

        bool remove_link(link_t) noexcept;

        /// Remove all link about the given node.
        bool detach_node(no_id) noexcept;


    protected:
        virtual std::optional<no_id> do_connect(output_t output, input_t input);
        virtual bool do_remove(link_index_type::iterator);

        node_instance_manager& instance_info_;

        // relational

        index_type index_{};
        no_id last_id_ = 0;

    private:
        void remove_node_input(no_id) noexcept;
        void remove_node_output(no_id) noexcept;
    };
} // namespace blueprint::flow
