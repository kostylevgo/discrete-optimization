#pragma once

#include "tsp/problem.hpp"
#include "tsp/solutions/greedy.hpp"
#include "tsp/solutions/christofides.hpp"
#include "tsp/solutions/genetics.hpp"

#include <util/debug/debug.hpp>

namespace tsp {

Solution solve_fast(Problem p) {
    return greedy_fixed_starting(p);
}

Solution solve(Problem p) {
    Solution answer = christofides_serduykov(p);

    Stopwatch watch("", 50ms);

    try {
        kernighan_lin(answer, watch);
    } catch (const Stopwatch::TimeoutException&) {};
    
    return answer;
}

} // namespace tsp
