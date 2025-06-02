//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/2.
//

module;

#include <opencv2/core.hpp>

#include <boost/hana.hpp>

#include <optional>
#include <utility>

export module blueprint.builtin_node:image_decompose;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;


namespace blueprint::builtin
{

    export std::optional<std::tuple<cv::Mat, cv::Mat, cv::Mat, cv::Mat>> image_decompose(cv::Mat img)
    {
        if (img.channels() != 4)
        {
            return std::nullopt;
        }

        cv::Mat r, g, b, a;
        cv::extractChannel(img, r, 0);
        cv::extractChannel(img, g, 1);
        cv::extractChannel(img, b, 2);
        cv::extractChannel(img, a, 3);
        return std::make_tuple(std::move(r), std::move(g), std::move(b), std::move(a));
    }

    export constexpr char image_decompose_id[] = "core.image-process.decompose";
    export constexpr char image_decompose_name[] = "Decompose";
    export constexpr char image_decompose_description[] = "Decompose a RGBA image into four grayscale image";

    export dyn_node::node_definition_proxy def_image_decompose_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_decompose,
            image_decompose_id, image_decompose_name, image_decompose_description
        );
    }

}
