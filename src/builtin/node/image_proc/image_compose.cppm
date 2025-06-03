//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/3.
//

module;

#include <opencv2/core.hpp>

#include <utility>

export module blueprint.builtin_node:image_compose;
import blueprint.dyn_node;
import blueprint.stk_node;
import :builtin_mapper;


namespace blueprint::builtin
{

    export std::optional<cv::Mat> image_compose(cv::Mat r, cv::Mat g, cv::Mat b, cv::Mat a)
    {
        auto rs = r.channels();
        auto gs = r.channels();
        auto bs = r.channels();
        auto as = r.channels();

        if (rs != gs || rs != bs || rs != as)
        {
            return std::nullopt;
        }

        if (rs == 4)
        {
            return r + g + b + a;
        }
        if (rs == 1)
        {
            cv::Mat rt;
            cv::merge(std::vector{r, g, b, a}, rt);
            return rt;
        }
        return std::nullopt;
    }

    export constexpr char image_compose_id[] = "core.image-process.compose";
    export constexpr char image_compose_name[] = "Compose";
    export constexpr char image_compose_description[] = "Compose four image into one.";

    export dyn_node::node_definition_proxy def_image_compose_node()
    {
        return stk_node::make_deduced_func_node<builtin_mapper>(&image_compose,
            image_compose_id, image_compose_name, image_compose_description
        );
    }

}
