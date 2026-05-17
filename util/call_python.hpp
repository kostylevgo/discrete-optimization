#pragma once

#include <bits/stdc++.h>

void call_python(char* input, std::string file) {
    freopen(input, "r", stdin);
    std::string python = "./venv/bin/python3";
    char* argv[3];
    argv[0] = python.data();
    argv[1] = file.data();
    argv[2] = nullptr;
    execve(python.data(), argv, nullptr);
}
