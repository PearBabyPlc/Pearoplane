#!/usr/bin/env bash
cd "$(dirname "$0")"
clang++ -Wall -std=c++17 physics.cpp && ./a.out
