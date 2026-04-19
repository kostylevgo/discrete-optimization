#pragma once

#include <bits/stdc++.h>

using namespace std;

class Stopwatch {
  public:
    using Duration = chrono::milliseconds;

  private:
    using Clock = chrono::high_resolution_clock;

    static auto now() {
        return chrono::duration_cast<Duration>(Clock().now().time_since_epoch());
    }

  public:
    struct TimeoutException : exception {
        const char* what() const noexcept override {
            return "Timeout";
        }
    };

    explicit Stopwatch(string name, Duration budget):
        name(std::move(name)), deadline(now() + budget) {
    }

    void operator()() {
        ++ticks;
        if (now() > deadline) {
            throw TimeoutException{};
        }
    }

    void tick() {
        (*this)();
    }

    ~Stopwatch() {
        if (!name.empty()) {
            cerr << "ticks at " << name << ": " << ticks << endl;
        }
    }

  private:
    string name;
    Duration deadline;
    int ticks = 0;
};
