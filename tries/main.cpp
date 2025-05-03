//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

#include "test_include.h"
#include <iostream>

import module_unit;

int main()
{
    info v{};
    std::cout << another_fun<int>(v) << std::endl;
    std::cout << value<int>(v) << std::endl;
    std::cout << v.value<int>() << std::endl;
    return 0;
}