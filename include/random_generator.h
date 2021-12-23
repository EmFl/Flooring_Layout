#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <random>

auto get_global_random() -> std::default_random_engine&
{
    static std::default_random_engine u{};
    return u;
}

void randomize()
{
    static std::random_device rd{};
    get_global_random().seed(rd());
}

auto random_pick(int from, int thru) -> int
{
    static std::uniform_int_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d(get_global_random(), parm_t{ from, thru });
}

#endif