//  Copyright (c) 2025 sfinea687
//  Licensed under the MIT License. See LICENSE in the project root for details.

//
// Created by ll06 on 2025/6/1.
//

module;

#include <any>

export module blueprint.dyn_node:any_context;

namespace blueprint::dyn_node
{
    /**
     * Class ayn_context provide a setter and a getter for an any type context.
     */
    class any_context
    {
    public:

        template <typename T>
        void set_context(T &&t)
        {
            ctx_ = std::forward<T>(t);
        }

        template <typename T>
        T get_context(this auto &&self)
        {
            return std::any_cast<T>(self.ctx_);
        }

        bool is_set() const
        {
            return ctx_.has_value();
        }

    private:
       std::any ctx_;
    };
}

