//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

#include <proxy/proxy.h>
#include <memory>

export module blueprint.builtin_node:image;
import blueprint.dyn_node;

namespace blueprint::builtin
{
    export constexpr char image_id[] = "core.image";
    export using builtin_image_t = cv::Mat;

    export struct image_hint {};
}

namespace blueprint::dyn_node
{
    template <>
    struct lookup_hint<builtin::builtin_image_t>
    {
        using type = builtin::image_hint;
    };
}

namespace blueprint::builtin
{
    using dyn_node::text_type;
    using dyn_node::id_type;

    export struct image_definition
    {
        /* NOLINT */ [[nodiscard]] text_type name() const
        {
            return "Image";
        }
        /* NOLINT */ [[nodiscard]] text_type description() const
        {
            return "The image";
        }
        /* NOLINT */ [[nodiscard]] id_type id() const
        {
            return image_id;
        }


        dyn_node::data_proxy load(archive::input_archive_t &ar)
        {
            std::vector<uchar> buffer;
            cv::Mat mat = cv::imdecode(buffer, cv::IMREAD_UNCHANGED);
            return std::make_shared<cv::Mat>(std::move(mat));
        }

        void save(archive::output_archive_t &ar, dyn_node::data_proxy &p)
        {
            auto &&mat = proxy_cast<cv::Mat&>(*p);

            std::vector<uchar> buffer;
            cv::imencode(".png", mat, buffer);
            ar(buffer);
        }

    };

    export id_type type_id(image_hint, builtin_image_t) noexcept
    {
        return image_id;
    }
    export dyn_node::data_proxy clone(image_hint, builtin_image_t d) noexcept
    {
        return std::make_shared<builtin_image_t>(d.clone());
    }

    static_assert(pro::proxiable<std::shared_ptr<builtin_image_t>, dyn_node::data_interface_facade>);
    static_assert(pro::proxiable<std::shared_ptr<image_definition>, dyn_node::type_definition_facade>);
}
