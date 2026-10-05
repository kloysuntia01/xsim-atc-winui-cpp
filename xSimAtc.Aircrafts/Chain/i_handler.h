#pragma once

#include <memory>

namespace xsim::aircrafts::chain
{
    template<typename T>
    class IHandler
    {
    public:
        virtual ~IHandler() = default;

        virtual void set_next(
            std::shared_ptr<IHandler<T>> next) = 0;

        virtual void handle(T& request) = 0;
    };
}
