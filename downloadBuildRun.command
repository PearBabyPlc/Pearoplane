#!/usr/bin/env bash

# navigate to root folder for raylib build
cd "$(dirname "$0")"

# clone raylib quickstart git repo (pear patch)
git clone https://github.com/PearBabyPlc/raylib-quickstart-pearpatch.git pearoplane-alpha

# delete and recreate resources folder
cd "$(dirname "$0")/pearoplane-alpha"
rm -r resources
mkdir resources

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
