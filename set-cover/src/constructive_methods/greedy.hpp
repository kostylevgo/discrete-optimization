#include <cstdint>

template <class P>
void greedy_process(P& problem, bool faster = false) {
    while (problem.has_actions()) {
        auto action = problem.greedy_action();
        problem.apply(action, faster);
    }
}

template <class P>
P greedy_solution(P problem) {
    greedy_process(problem);
    return problem;
}

template <class P>
int64_t greedy_evaluate(P problem) {
    greedy_process(problem);
    return problem.evaluate();
}
