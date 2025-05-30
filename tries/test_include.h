//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

#ifndef TEST_INCLUDE_H
#define TEST_INCLUDE_H


template <typename T>
struct base_class
{
    template <typename E>
    E value() const
    {
        return {};
    }

    template <typename E>
    friend E value(base_class& )
    {
        return {};
    }
};

template <typename T, typename E>
T another_fun(base_class<E>& e)
{
    return T{};
}
#endif //TEST_INCLUDE_H
