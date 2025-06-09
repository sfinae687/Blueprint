//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/9.
//

module;

#include <boost/hof.hpp>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>

#include <functional>
#include <utility>
#include <concepts>
#include <type_traits>

export module blueprint.archive;

namespace blueprint::archive
{
    export using input_archive_t = cereal::BinaryInputArchive;
    export using output_archive_t = cereal::BinaryOutputArchive;

    namespace details /* load */
    {
        template <typename T, typename Ar>
        concept cereal_archivable = requires (T &&t,  Ar &ar)
        {
            ar(t);
        };

        template <typename T>
        concept free_func_loadable = requires (T&& t, input_archive_t ar)
        {
            load(ar, t);
        };

        template <typename T>
        concept mem_func_loadable = requires (T&& t, input_archive_t ar)
        {
            t.load(ar);
        };
    }

    export template <typename T>
    concept loadable = details::cereal_archivable<T, input_archive_t>;

    export auto const load = [] <loadable T> (T &&t, input_archive_t &ar)
    {
        ar(std::forward<T>(t));
    };

    namespace details /* save */
    {
    }

    export template <typename T>
    concept saveable = details::cereal_archivable<T, output_archive_t>;

    export auto const save = [] <saveable T> (T&& t, output_archive_t &ar) {
        ar(t);
    };
}
