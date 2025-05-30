//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-24.
//

#include <gtest/gtest.h>

#include <utility>
#include <string_view>

import blueprint.constraint;
import blueprint.flow;
import blueprint.dyn_node;

using namespace blueprint;
using dyn_node::node_instance_proxy;
using dyn_node::node_definition_proxy;
using dyn_node::data_proxy;
using dyn_node::type_definition_proxy;
using dyn_node::util::trivial_node_definition;
using dyn_node::util::trivial_node_instance;
using flow::node_instance_handler;
using dyn_node::id_type;

using std::string_view;

class ConstraintTest : public testing::Test
{
public:
    ConstraintTest()
        : nd_(), cf_(nd_)
    {

    }
protected:

    void SetUp() override
    {
        o1 = gen_inst({}, {type1});
        o2 = gen_inst({}, {type2});
        i1o1 = gen_inst({type1}, {type1});
        i1o2 = gen_inst({type1}, {type2});
        i12o2 = gen_inst({type1, type2}, {type2});
        i22o2 = gen_inst({type2, type2}, {type2});
    }

    node_instance_handler gen_inst(std::initializer_list<id_type> in, std::initializer_list<id_type> out)
    {
        trivial_node_definition nd_def("Test.node", "Test.node", "Test.node", in, out);
        return nd_.add_instance(nd_def.create_node());
    }

    constraint::node_state state_of(node_instance_handler hd)
    {
        return cf_.state(hd.node_id());
    }

    flow::node_instance_manager nd_{};
    constraint::constraint_flow cf_;

    static constexpr string_view type1 = "Test.t1";
    static constexpr string_view type2 = "Test.t2";
    static constexpr string_view type3 = "Test.t3";

    node_instance_handler o1;
    node_instance_handler o2;
    node_instance_handler i1o1;
    node_instance_handler i1o2;
    node_instance_handler i12o2;
    node_instance_handler i22o2;
};

TEST_F(ConstraintTest, DefaultStateTest)
{
    using enum constraint::node_state;

    EXPECT_EQ(state_of(o1), DIRTY);
    EXPECT_EQ(state_of(o2), DIRTY);
    EXPECT_EQ(state_of(i1o1), NOT_PREPARED);
    EXPECT_EQ(state_of(i1o2), NOT_PREPARED);
    EXPECT_EQ(state_of(i12o2), NOT_PREPARED);
    EXPECT_EQ(state_of(i22o2), NOT_PREPARED);
}

TEST_F(ConstraintTest, LinkTest)
{
    using enum constraint::node_state;

    const auto o1_o1 = flow::output_channel_id(o1.node_id(), 0);
    const auto i1o1_i1 = flow::input_channel_id(i1o1.node_id(), 0);

    // Basic link

    auto lk1 = cf_.create_link(o1_o1, i1o1_i1);
    ASSERT_TRUE(lk1);
    EXPECT_EQ(state_of(i1o1), DIRTY);

    auto i1o2_i1 = flow::input_channel_id(i1o2.node_id(), 0);
    EXPECT_EQ(state_of(i1o2), NOT_PREPARED);
    auto lk2 = cf_.create_link(o1_o1, i1o2_i1);
    ASSERT_TRUE(lk2);
    EXPECT_EQ(state_of(i1o2), DIRTY);

    cf_.remove_link(*lk1);
    EXPECT_EQ(state_of(i1o1), NOT_PREPARED);
    cf_.remove_link(*lk2);
    EXPECT_EQ(state_of(i1o2), NOT_PREPARED);

    // Relink

    auto lk3 = cf_.create_link(o1_o1, i1o1_i1);
    ASSERT_TRUE(lk3);
    EXPECT_EQ(state_of(i1o1), DIRTY);

    // Conflict Link
    auto i1o1_o1 = flow::output_channel_id(i1o1.node_id(), 0);
    auto lk4 = cf_.create_link(o1_o1, i1o2_i1);
    ASSERT_TRUE(lk4);
    EXPECT_EQ(state_of(i1o2), DIRTY);
    auto lk5 = cf_.create_link(i1o1_o1, i1o2_i1);
    ASSERT_FALSE(lk5);
    EXPECT_EQ(state_of(i1o2), DIRTY);

    cf_.remove_link(*lk3);
    cf_.remove_link(*lk4);
}

TEST_F(ConstraintTest, MarkTest)
{
    using enum constraint::node_state;

    // Basic Mark

    EXPECT_EQ(state_of(o1), DIRTY);
    EXPECT_FALSE(cf_.mark_clean(o1.node_id()));
    EXPECT_TRUE(cf_.mark_computing(o1.node_id()));
    EXPECT_EQ(state_of(o1), COMPUTING);
    EXPECT_TRUE(cf_.mark_clean(o1.node_id()));
    EXPECT_EQ(state_of(o1), CLEAN);

    // Mark with dependence

    EXPECT_FALSE(cf_.mark_computing(i1o1.node_id())) << "Shouldn't success mark a unprepared node to be computing";
    auto o1_o1 = flow::output_channel_id(o1.node_id(), 0);
    auto i1o1_i1 = flow::input_channel_id(i1o1.node_id(), 0);
    auto lk1 = cf_.create_link(o1_o1, i1o1_i1);
    ASSERT_TRUE(lk1);
    EXPECT_EQ(state_of(o1), CLEAN);
    EXPECT_EQ(state_of(i1o1), DIRTY);
    EXPECT_TRUE(cf_.mark_computing(i1o1.node_id()));
    EXPECT_EQ(state_of(i1o1), COMPUTING);
    EXPECT_TRUE(cf_.mark_clean(i1o1.node_id()));
    EXPECT_EQ(state_of(i1o1), CLEAN);

    // Revision test

    auto o1_id = o1.node_id();
    EXPECT_TRUE(cf_.mark_dirty(o1_id));
    EXPECT_EQ(state_of(o1), DIRTY);
    EXPECT_EQ(state_of(i1o1), DIRTY);
    EXPECT_TRUE(cf_.mark_computing(o1_id));
    EXPECT_TRUE(cf_.mark_clean(o1_id));
    EXPECT_EQ(state_of(o1), CLEAN);
    EXPECT_EQ(state_of(i1o1), DIRTY);

}

TEST_F(ConstraintTest, SetTest)
{

}