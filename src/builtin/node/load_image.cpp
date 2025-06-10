//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//
module;

#include <proxy/proxy.h>

#include <cereal/types/string.hpp>

#include <opencv2/imgcodecs.hpp>

#include <memory>
#include <string>
#include <cassert>
#include <filesystem>

module blueprint.builtin_node;
import blueprint.dyn_node;
import :load_image;
import :image;

namespace blueprint::builtin
{
    using dyn_node::util::constant_node_definition;

    dyn_node::node_definition_proxy def_load_image_node()
    {
        using dyn_node::input_sequence_t;
        using dyn_node::output_sequence_t;
        using dyn_node::util::constant_node_definition;

        std::string def_id = load_image_node_id;
        return std::make_shared<load_image_definition>();
    }

    // Instance

    namespace
    {
        signature_t load_image_sig {
            {},
            {matrix_id}
        };
    }

    load_image_instance::load_image_instance()
    {

    }

    id_type load_image_instance::type_id() const noexcept { return load_image_node_id; }
    std::span<const signature_t> load_image_instance::signatures() const noexcept { return {&load_image_sig, 1}; }
    std::size_t load_image_instance::current_variant() const noexcept { return 0; }
    bool load_image_instance::set_variant(std::size_t) noexcept
    {
        return false;
    }
    bool load_image_instance::compute(data_sequence ds) noexcept
    {
        return ds.size() != 0;
    }
    data_sequence load_image_instance::output() const noexcept { return {img_}; }
    bool load_image_instance::has_image() const
    {
        return ! img_->empty();
    }

    fs::path load_image_instance::path()
    {
        return path_;
    }
    void load_image_instance::set_path(fs::path p)
    {
        path_ = std::move(p);

        if (fs::is_regular_file(path_))
        {
            auto img = cv::imread(path_);
            img.convertTo(img, CV_64F, 1/255.0);
            *img_ = std::move(img);
        }
        else
        {
            img_->release();
        }

    }


    // Definition

    id_type load_image_definition::id() const noexcept { return load_image_node_id; }
    text_type load_image_definition::name() const noexcept { return load_image_node_name; }
    text_type load_image_definition::description() const noexcept { return load_image_node_description; }

    node_instance_proxy load_image_definition::create_node() noexcept
    {
        return std::make_shared<load_image_instance>();
    }

    node_instance_proxy load_image_definition::load(archive::input_archive_t& ar)
    {
        std::string str;
        ar(str);
        auto nt = std::make_shared<load_image_instance>();
        nt->set_path(str);
        return std::move(nt);
    }

    void load_image_definition::save(archive::output_archive_t& ar, node_instance_proxy& p)
    {
        assert(p->type_id() == load_image_node_id);
        auto &&nt = proxy_cast<load_image_instance&>(*p);

        ar(nt.path().string());

    }
} // namespace blueprint::builtin
