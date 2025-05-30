//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-5-4.
//

#include "test_include.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include <iostream>
#include <ranges>

import module_unit;

using namespace ::boost;
using namespace ::boost::multi_index;
namespace ranges = std::ranges;

struct employee
{
    int         id;
    std::string name;

    employee(int id,const std::string& name):id(id),name(name){}

    bool operator<(const employee& e)const{return id<e.id;}
};

// define a multiply indexed set with indices by id and name
typedef multi_index_container<
  employee,
  indexed_by<
    // sort by employee::operator<
    ordered_unique<identity<employee> >,

    // sort by less<string> on name
    hashed_non_unique<member<employee,std::string,&employee::name> >
  >
> employee_set;

int main()
{
    info v{};
    std::cout << another_fun<int>(v) << std::endl;
    std::cout << value<int>(v) << std::endl;
    std::cout << v.value<int>() << std::endl;

    employee_set st{};
    auto &&index = st.get<1>();
    auto [l_iter, u_iter] = index.equal_range("Hello");
    ranges::subrange rg(l_iter, u_iter);

    return 0;
}