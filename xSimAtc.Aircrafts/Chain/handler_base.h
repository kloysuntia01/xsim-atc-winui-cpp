#pragma once

#include "i_handler.h"

#include <memory>
#include <utility>

namespace xsim::aircrafts::chain
{
    template<typename T>
    class HandlerBase : public IHandler<T>
    {
    public:
        void set_next(
            std::shared_ptr<IHandler<T>> next) override
        {
            next_ = std::move(next);
        }

    protected:
        void next(T& request)
        {
            if (next_)
            {
                next_->handle(request);
            }
        }

    private:
        std::shared_ptr<IHandler<T>> next_;
    };
}
