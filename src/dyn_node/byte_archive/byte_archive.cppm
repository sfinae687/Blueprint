//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/8.
//

module;

#include <functional>
#include <utility>
#include <vector>
#include <ranges>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <cstring>
#include <array>

export module blueprint.dyn_node:byte_archive;

namespace blueprint::dyn_node::byte_archive
{
    namespace ranges = std::ranges;
    namespace views = std::views;

    export class binary_archive
    {
    public:
        binary_archive() = default;

        template <typename T>
            requires std::is_trivially_constructible_v<T>
        void save_byte(const T &d)
        {
            constexpr auto t_sz = sizeof(T);
            auto t_data = std::bit_cast<std::array<std::byte, t_sz>>(d);
            insert_byte(t_sz, t_data.data());
        }

        template <typename T>
            requires std::is_trivially_constructible_v<T>
        T load_byte()
        {
            auto t_sz = sizeof(T);
            assert(cursor_ + t_sz <= bytes_.size());
            T t;
            std::memcpy(&t, bytes_.data()+cursor_, t_sz);
            cursor_ += t_sz;
            return t;
        }

        void save_string(std::string_view str)
        {
            auto sz = str.size();
            save_byte(sz);
            auto byte_str = std::as_bytes(std::span{str});
            insert_byte(byte_str.size(), byte_str.data());
        }
        std::string load_string()
        {
            const auto sz = load_byte<std::size_t>();
            std::string rt;
            for (std::size_t i=0; i<sz; ++i)
            {
                rt.push_back(load_byte<std::string::value_type>());
            }
            return rt;
        }

        void seek(std::ptrdiff_t d)
        {
            assert(d >= 0 || std::abs(d) <= cursor_);
            auto new_pos = cursor_ + d;
            assert(new_pos <= bytes_.size());
            cursor_ = new_pos;
        }
        void to_begin()
        {
            cursor_ = 0;
        }

        [[nodiscard]] std::size_t size() const
        {
            return bytes_.size();
        }

        friend binary_archive concat (const binary_archive &lhs, const binary_archive &rhs)
        {
            binary_archive rt(lhs);
            ranges::copy(rhs.bytes_, std::back_inserter(rt.bytes_));
            return rt;
        }
        friend binary_archive operator| (const binary_archive &lhs, const binary_archive &rhs)
        {
            return concat(lhs, rhs);
        }
        binary_archive& operator|= (const binary_archive &rhs)
        {
            insert_byte(rhs.bytes_.size(), rhs.bytes_.data());
            return *this;
        }

        template <typename T>
            requires std::is_trivially_constructible_v<T>
        friend void save_binary(binary_archive &ar, const T &d)
        {
            ar.save_byte<T>(d);
        }

        template <typename T>
            requires std::is_trivially_constructible_v<T>
        friend T load_binary(binary_archive &ar)
        {
            return ar.load_byte<T>();
        }

    private:

        void insert_byte(std::size_t sz, const std::byte *in_byte)
        {
            auto remain_byte = bytes_.size() - cursor_;

            auto overlap_size = std::min(remain_byte, sz);
            ranges::copy(in_byte, in_byte+overlap_size, bytes_.begin() + cursor_);
            cursor_ += overlap_size;

            if (remain_byte < sz)
            {
                in_byte += overlap_size;
                ranges::copy(in_byte, in_byte+(sz-remain_byte), std::back_inserter(bytes_));
                cursor_ += sz - remain_byte;
            }
        }

        std::pmr::vector<std::byte> bytes_{};
        std::size_t cursor_ = 0;
    };


}
