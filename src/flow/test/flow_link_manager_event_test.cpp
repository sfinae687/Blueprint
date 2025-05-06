//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-6.
//

#include "test_node_definition.h"

#include <gtest/gtest.h>
#include <proxy.h>

import blueprint.flow;
import blueprint.dyn_node;
import blueprint.scheduler;

using namespace blueprint::flow;
using namespace blueprint::dyn_node;
using namespace blueprint::scheduler;

TEST(BlueprintFlowEvent, Event)
{
    event_queue<link_event> que;

    link_event_context ct {
        link_event_type::ADD,
        0, 0, nullptr, nullptr
    };

    que.push(std::move(ct));

    auto re_ct = que.pop<link_event>();

    ASSERT_TRUE(re_ct);
    EXPECT_EQ(*re_ct, ct);

}

TEST(BlueprintFlowEvent, EventEmitter)
{
    event_queue<link_event> que;
    node_instance_manager mg;
    link_manager_with_event lk(que, mg);

    node_definition_proxy node_def = std::make_unique<test_node_definition>();
    auto aInst = node_def->create_node();
    auto inst_handler = mg.add_instance(std::move(aInst));
    auto inst_id = inst_handler.node_id();

    auto lk_id = lk.create_link(output_channel_id(inst_id, 0), input_channel_id(inst_id, 0));

    ASSERT_TRUE(lk_id);

    auto the_event = que.pop<link_event>();

    ASSERT_TRUE(the_event);

    EXPECT_EQ(the_event->ty, link_event_type::ADD);
    EXPECT_EQ(the_event->input, input_channel_id(inst_id, 0));
    EXPECT_EQ(the_event->output, output_channel_id(inst_id, 0));
    EXPECT_EQ(the_event->link_manager, static_cast<link_manager*>(&lk));
    EXPECT_EQ(the_event->node_manger, &mg);

    lk.remove_link(output_channel_id(inst_id, 0), input_channel_id(inst_id, 0));

    the_event = que.pop<link_event>();

    ASSERT_TRUE(the_event);

    EXPECT_EQ(the_event->ty, link_event_type::REMOVE);
    EXPECT_EQ(the_event->input, input_channel_id(inst_id, 0));
    EXPECT_EQ(the_event->output, output_channel_id(inst_id, 0));
    EXPECT_EQ(the_event->link_manager, static_cast<link_manager*>(&lk));
    EXPECT_EQ(the_event->node_manger, &mg);


}