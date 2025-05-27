//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-27.
//

module;

#include <imgui.h>

#include <concepts>

export module blueprint:style;
import blueprint.dyn_node;
import blueprint.flow;
import blueprint.constraint;

namespace blueprint
{
    export ImColor node_state_color(constraint::node_state ns)
    {
        using enum constraint::node_state;
        switch (ns)
        {
        case NOT_PREPARED:
            return {0x5e, 0x64, 0x72};
        case DIRTY:
            return {0x57, 0x75, 0x90};
        case COMPUTING:
            return {0xf4, 0x8c, 0x06};
        case CLEAN:
            return {0xa8, 0xe6, 0xcf};
        case ERROR:
            return {0xf7, 0x38, 0x59};
        default:
            return {0xf9, 0xf7, 0xf7};
        }
    }

    template <std::integral I>
    constexpr I int_pow(I x, I k, I m)
    {
        I ans{1};
        while (k > 0)
        {
            if (k & 1)
            {
                ans = ans * x % m;
            }
            x = x * x % m;
            k >>= 1;
        }
        return ans;
    }

    template <std::integral I>
    constexpr I int_inv(I x, I m)
    {
        return int_pow(x, m-2, m);
    }


    export ImColor mix_color(const ImColor &lhs, const ImColor &rhs)
    {
        auto [lr, lg, lb, la] = lhs.Value;
        auto [rr, rg, rb, ra] = rhs.Value;
        auto sum_a = la + ra;
        auto lw = la / sum_a;
        auto rw = ra / sum_a;
        auto mix = [&](auto l, auto r)
        {
            return l * lw + r * rw;
        };

        return {
            mix(lr, rr),
            mix(lg, rg),
            mix(lb, rb),
            1.,
        };

    }
}