#!/usr/bin/env bash
cd "$(dirname "$0")"
clang++ -Wall -std=c++14 aero.cpp && ./a.out
