//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 25-4-28.
//
module;
#include <proxy/proxy.h>

export module blueprint.gui:forward_declaration;

namespace blueprint::GUI
{
    export class window;

    // context manage

    PRO_DEF_MEM_DISPATCH(MEM_activate_context, activate_context);
    PRO_DEF_MEM_DISPATCH(MEM_inactivate_context,inactivate_context);

    struct context_manage_facade : pro::facade_builder
        ::add_convention<MEM_activate_context, void(void)>
        ::add_convention<MEM_inactivate_context, void(void)>
        ::support_copy<pro::constraint_level::trivial>
        ::build {};

    using context_manage_proxy = pro::proxy<context_manage_facade>;
}
