//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-26.
//


module;
#define IMGUI_DEFINE_MATH_OPERATORS

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <imgui.h>
#include <imnodes.h>

#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>

#include <proxy/proxy.h>

#include <iomanip>
#include <ranges>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include <expected>

module blueprint;
import :style;
import blueprint.gui;
import blueprint.dyn_node;
import blueprint.draw_node;
import blueprint.builtin_node;
import blueprint.constraint;

namespace blueprint
{

    namespace ranges = std::ranges;
    namespace views = std::views;
    using namespace std::string_literals;

    /// Constructor, it defines the all GUI, and connect another necessary part.
    blueprint_application::blueprint_application()
        : gui_("Blueprint Node editor", 1080, 720)
        , imnodes_context_(gui_)
        , link_(node_instance_)
        , to_finish_compute_(64)
    {
        using namespace GUI;

        setup_logger();
        load_builtin();

        gui_.set_update_operation([this] {update();});
        gui_.set_draw_operation([this] {draw();});
    }

    int blueprint_application::run()
    {
        gui_.render_loop();
        return 0;
    }

    void blueprint_application::setup_logger()
    {
        using namespace boost::log;
        logger.add_attribute("Module", attributes::constant<std::string>("Application"));
    }

    void blueprint_application::update()
    {

        if (begin_compute_flag)
        {
            auto all_ready = link_.dump_ready();
            if (all_ready.empty() && in_computing_.empty())
            {
                // begin_compute_flag = false;
            }
            else
            {
                for (auto id : all_ready)
                {
                    link_.mark_computing(id);
                    to_computing(id);
                }
            }
        }

        to_finish_compute_.consume_all([this] (auto item)
        {
            auto [id, ans] = item;
            do_finish_computing(id, ans);
        });

        while (! to_remove_nodes_.empty())
        {
            auto &&cur_ent = to_remove_nodes_.front();
            do_remove_node(cur_ent);
            to_remove_nodes_.pop();
        }
        while (! to_create_nodes_.empty())
        {
            auto &&cur_ent = to_create_nodes_.front();
            do_create_node(cur_ent.first, cur_ent.second);
            to_create_nodes_.pop();
        }
        process_link();
    }
    void blueprint_application::draw()
    {
        using namespace GUI;
        if (begin_main_window("main", &main_open))
        {

            ImGui::BeginChild("NodeEditor",
                ImGui::GetContentRegionAvail() - ImVec2(0, ImGui::GetTextLineHeight() * 1.3F));

            ImNodes::BeginNodeEditor();

            for (auto &&hd : node_instance_.dump_handler())
            {
                draw_node(hd);
            }

            draw_link();

            draw_editor_menu();

            ImNodes::EndNodeEditor();

            ImGui::EndChild();

            if (ImGui::Button("Compute"))
            {
                begin_compute_flag = true;
            }
        }
        ImGui::End();

    }

    void blueprint_application::draw_node(flow::node_instance_handler hd)
    {
        namespace ranges = std::ranges;
        namespace views = std::views;
        using namespace blueprint::dyn_node;

        auto &&p = hd.node_instance();
        const auto node_id = hd.node_id();
        const auto type_id = p->type_id();
        const auto name = node_def_[type_id]->name();
        const auto all_sig = p->signatures();
        const auto sig_ind = p->current_variant();
        const auto [inputs, outputs] = all_sig[sig_ind];
        const auto node_st = link_.state(node_id);

        auto title_col = node_state_color(node_st);
        auto select_mix = ImColor(0x00, 0xa6, 0xfb, 124);

        ImNodes::PushColorStyle(ImNodesCol_TitleBar, title_col);
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, mix_color(title_col, select_mix));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, mix_color(title_col, select_mix));
        ImNodes::BeginNode(node_id);
        if (new_node_.contains(node_id))
        {
            ImNodes::SetNodeScreenSpacePos(node_id, new_node_[node_id].pos);
            new_node_.erase(node_id);
        }

        // TitleBar

        ImNodes::BeginNodeTitleBar();
        // const auto title_id = std::format("NodeTitle##{}", type_id);

        if (all_sig.size() > 1)
        {
            auto left_button = std::format("<##node-left##{}", node_id);
            auto right_button = std::format(">##node-right##{}", node_id);
            if (ImGui::Button(left_button.c_str()))
            {
                if (sig_ind > 0)
                {
                    to_switch_variant(node_id, sig_ind -1);
                }
            }
            ImGui::SameLine();
            auto variant_hint = std::format("{}/{}", sig_ind+1, all_sig.size());
            ImGui::Text("%s", variant_hint.c_str());
            ImGui::SameLine();
            if (ImGui::Button(right_button.c_str()))
            {
                if (sig_ind+1 < all_sig.size())
                {
                    to_switch_variant(node_id, sig_ind+1);
                }
            }
            ImGui::SameLine(0, 8);
        }

        ImGui::Text("%s", name.data());
        ImGui::SameLine(0, 8.0);

        const auto remove_label = std::format("[x]##{}##", type_id, node_id);
        if (ImGui::Button(remove_label.c_str()))
        {
            to_remove_node(node_id);
        }
        ImNodes::EndNodeTitleBar();

        // Attribute
        auto try_draw_type = [&](dyn_node::id_type tid, flow::no_id id)
        {
            if (type_draw_.contains(tid))
            {
                auto &&ctx = make_draw_context(id);
                type_draw_[tid](ctx);
                if (flow::channel_type(id) == flow::channel_type_t::INPUT &&
                    ! ctx.is_connected
                    && ctx.set_data)
                {
                    link_.set_date(id, ctx.data);
                }
            }
            else
            {
                auto type_name = type_def_[tid]->name();
                ImGui::Text("%s", type_name.data());
            }
        };

        ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
        for (auto &&[ind, ip] : inputs | views::enumerate)
        {
            auto input_id = flow::input_channel_id(node_id, ind);
            ImNodes::BeginInputAttribute(input_id);
            try_draw_type(ip, input_id);
            ImNodes::EndInputAttribute();
        }
        ImNodes::PopAttributeFlag();

        for (auto &&[ind, ip] : outputs | views::enumerate)
        {
            auto output_id = flow::output_channel_id(node_id, ind);
            assert(type_def_.contains(ip));
            auto atx_name = type_def_[ip]->name();
            ImNodes::BeginOutputAttribute(output_id);
            ImGui::Text("%s", atx_name.data());
            ImNodes::EndOutputAttribute();
        }

        // Node Content
        if (node_draw_.contains(type_id))
        {
            auto id_text = std::format("{}", node_id);
            draw_node::node_draw_context ctx{
                .node = p,
                .id = id_text,
                .set_dirty = false
            };
            node_draw_[type_id](ctx);
            if (ctx.set_dirty)
            {
                link_.mark_dirty(node_id);
            }
        }

        ImNodes::EndNode();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();

    }


    void blueprint_application::load_builtin()
    {
        BOOST_LOG_SEV(logger, info) << "Loading builtin Components";
        load_component(dyn_node::builtin::builtin_components());


        BOOST_LOG_SEV(logger, info) << "Loading builtin Nodes draw rule";
        node_draw_ = draw_node::builtin_node_draw_map();

        BOOST_LOG_SEV(logger, info) << "Loading builtin Types draw rule";
        type_draw_ = draw_node::builtin_type_draw_map();

        BOOST_LOG_SEV(logger, info) << "Finish to load builtin node information";
    }
    void blueprint_application::load_component(plugin::component_package pkg)
    {
        for (auto ty : pkg.types | views::as_rvalue)
        {
            if (type_def_.contains(ty->id()))
            {
                BOOST_LOG_SEV(logger, error) << "Load a type has be loaded. The later definition will be ignored";
            }
            else
            {
                auto id = ty->id();
                type_def_[id] = std::move(ty);
            }
        }

        for (auto nd : pkg.nodes | views::as_rvalue)
        {
            if (node_def_.contains(nd->id()))
            {
                BOOST_LOG_SEV(logger, error) << "Load a node has be loaded. The later definition will be ignored";
            }
            else
            {
                auto id = nd->id();
                node_def_[id] = std::move(nd);
            }
        }

        for (auto [name, ids] : pkg.groups | views::as_rvalue)
        {
            std::string cname{name.begin(), name.end()};
            auto&& cur_list = menu_def_[cname];
            for (auto id : ids)
            {
                if (cur_list.contains(id))
                {
                    BOOST_LOG_SEV(logger, error) << "Create a menu entry has exists";
                }
                else
                {
                    cur_list.insert(id);
                }
            }
        }
    }

    void blueprint_application::draw_editor_menu()
    {
        if (ImNodes::IsEditorHovered()
            && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(editor_menu_id.data());
        }

        if (ImGui::BeginPopup(editor_menu_id.data()))
        {
            for (auto&& [group_name, members] : menu_def_)
            {
                auto group_menu_id = std::format(" {}##GroupMenu", group_name);
                if (ImGui::BeginMenu(group_menu_id.c_str()))
                {
                    for (auto&& ent : members)
                    {
                        auto node_name = node_def_[ent]->name();
                        auto item_id = std::format(" {}##{}", node_name, group_menu_id);
                        if (ImGui::MenuItem(item_id.c_str()))
                        {
                            BOOST_LOG_SEV(logger, trace) << "Create node: " << ent;
                            to_create_node(ent, {
                                .pos = ImGui::GetCursorScreenPos()
                            });
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndPopup();
        }
    }
    draw_node::data_draw_context& blueprint_application::make_draw_context(flow::no_id id)
    {
        using draw_node::data_channel_type_t;
        using flow::channel_type_t;

        // TODO Rewrite ?

        auto&& ct = draw_contexts_[id];
        ct.channel =
            flow::channel_type(id) == channel_type_t::INPUT ? data_channel_type_t::input : data_channel_type_t::output;

        if (ct.channel == data_channel_type_t::input)
        {
            ct.is_connected = link_.have_connection(id);
        }
        else
        {
            ct.is_connected = !link_.to_input(id).empty();
        }

        if (auto sk_data = link_.seek_data(id))
        {
            ct.data = sk_data.value();
        }
        else
        {
            ct.data = nullptr;
        }
        ct.set_data = false;
        // todo ct.id = ?;
        return ct;
    }

    void blueprint_application::process_link()
    {
        int d_id;
        if (ImNodes::IsLinkDestroyed(&d_id))
        {
            link_.remove_link(d_id);
        }

        int new_out, new_in;
        if (ImNodes::IsLinkCreated(&new_out, &new_in))
        {
            link_.create_link(new_out, new_in);
        }
    }

    void blueprint_application::draw_link()
    {
        auto all_link = link_.all_link();

        for (auto id : all_link)
        {
            auto [in, out] = *link_.query_link(id);
            ImNodes::Link(id, out, in);
        }
    }
    void blueprint_application::draw_node_menu()
    {
        int selected_node;
        std::string node_menu_id;
        if (ImNodes::IsNodeHovered(&selected_node)
            && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            node_menu_id = std::format("node-menu##{}", node_menu_id);
            ImGui::OpenPopup(node_menu_id.c_str());
        }

        if (ImGui::BeginPopup(node_menu_id.data()))
        {
            auto delete_id = "remove##" + node_menu_id;
            if (ImGui::MenuItem(delete_id.c_str()))
            {
                to_remove_node(selected_node);
            }
            ImGui::EndPopup();
        }

    }

    void blueprint_application::to_remove_node(flow::no_id id)
    {
        to_remove_nodes_.emplace(id);
    }

    void blueprint_application::do_remove_node(flow::no_id id)
    {
        auto handler = node_instance_.get_handler(id);

        auto min_input = flow::min_input_channel_id(id);
        auto max_input = flow::max_input_channel_id(id);
        auto min_output = flow::min_output_channel_id(id);
        auto max_output = flow::max_output_channel_id(id);

        auto min_input_iter = draw_contexts_.lower_bound(min_input);
        auto max_input_iter = draw_contexts_.upper_bound(max_input);
        auto min_output_iter = draw_contexts_.lower_bound(min_output);
        auto max_output_iter = draw_contexts_.upper_bound(max_output);

        draw_contexts_.erase(min_input_iter, max_input_iter);
        draw_contexts_.erase(min_output_iter, max_output_iter);

        link_.detach_node(id);

        handler.remove();

        if (new_node_.contains(id))
        {
            new_node_.erase(id);
        }
    }

    void blueprint_application::to_create_node(dyn_node::id_type id, new_node_context ctx)
    {
        to_create_nodes_.emplace(std::move(id), std::move(ctx));
    }

    void blueprint_application::do_create_node(dyn_node::id_type id, new_node_context ctx)
    {
        assert(node_def_.contains(id));
        auto&& def = node_def_[id];
        auto new_instance = def->create_node();
        auto hd = node_instance_.add_instance(std::move(new_instance));
        new_node_[hd.node_id()] = std::move(ctx);
        link_.flush_node(hd.node_id());
    }

    void blueprint_application::to_computing(flow::no_id id)
    {
        executor_.enqueue_detach(
            [this, id]
            {
                auto&& inst = node_instance_.get_handler(id).node_instance();
                auto inputs = link_.gather_input(id);
                assert(inputs.has_value());
                auto rt = inst->compute(std::move(inputs.value()));
                to_finish_computing(id, rt);
            });

        in_computing_.insert(id);
    }

    void blueprint_application::to_finish_computing(flow::no_id id, bool res) { to_finish_compute_.push({id, res}); }
    void blueprint_application::do_finish_computing(flow::no_id id, bool res)
    {
        if (res)
        {
            link_.mark_clean(id);
        }
        else
        {
            link_.mark_error(id);
        }
        in_computing_.erase(id);
    }

    void blueprint_application::to_switch_variant(flow::no_id id, std::size_t ind)
    {
        auto &&inst = node_instance_.get_handler(id).node_instance();
        auto current_ind = inst->current_variant();
        if (current_ind != ind)
        {
            auto sz = inst->signatures().size();
            link_.detach_node(id);
            assert(ind < sz);
            inst->set_variant(ind);
        }

    }
} // namespace blueprint
