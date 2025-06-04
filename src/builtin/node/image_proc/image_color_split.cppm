//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/3.
//
module;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <utility>

export module blueprint.builtin_node:image_color_split;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;

namespace blueprint::builtin
{
    export std::optional<std::tuple<cv::Mat, cv::Mat, cv::Mat, cv::Mat>>
    image_color_split(cv::Mat img)
    {
        if (img.channels() != 4)
        {
            return std::nullopt;
        }

        cv::Mat zeros = cv::Mat::zeros(img.rows, img.cols, img.depth());
        cv::Mat ones = cv::Mat::ones(img.rows, img.cols, img.depth());

        std::vector<cv::Mat> rts;
        cv::split(img, rts);

        cv::Mat r, g, b, a;
        cv::merge(std::vector{rts[0], zeros, zeros, ones}, r);
        cv::merge(std::vector{zeros, rts[1], zeros, ones}, g);
        cv::merge(std::vector{zeros, zeros, rts[2], ones}, b);
        cv::merge(std::vector{zeros, zeros, zeros, rts[3]}, a);
        return std::make_tuple(r, g, b, a);
    }

    export constexpr char image_color_split_id[] = "core.image-process.color-split";
    export constexpr char image_color_split_name[] = "Color split";
    export constexpr char image_color_split_description[] = "Split color channel into four color channel.";

    export dyn_node::node_definition_proxy def_image_color_split_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_color_split,
            image_color_split_id, image_color_split_name, image_color_split_description
        );
    }

}
