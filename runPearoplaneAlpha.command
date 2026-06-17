#!/usr/bin/env bash

# navigate to root folder
cd "$(dirname "$0")"

# clone raylib quickstart git repo
git clone https://github.com/raylib-extras/raylib-quickstart.git pearoplane-alpha

# remove unneeded resources
cd "$(dirname "$0")/pearoplane-alpha/resources"
rm wabbit_alpha.png

# copy Pearbaby source and resources
cd "$(dirname "$0")"
cp -R "$(dirname "$0")/src/" "$(dirname "$0")/pearoplane-alpha/src"
cp -R "$(dirname "$0")/resources/" "$(dirname "$0")/pearoplane-alpha/resources"

# premake
cd "$(dirname "$0")/pearoplane-alpha/build"
./premake5.osx gmake

# make
cd "$(dirname "$0")/pearoplane-alpha"
make

# run binary
cd "$(dirname "$0")/pearoplane-alpha/bin/debug"
./pearoplane-alpha
