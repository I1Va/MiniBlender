#! /bin/bash

PROJECT_NAME=OpticApplication2
CMAKE_CONFIGURATION_FLAGS="-B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -DSANITIZE=OFF"

DR4_PLUGIN_NAME=libIAGraphicsPlugin.so
rm ./external/plugins/dr4/"$DR4_PLUGIN_NAME"
cd ./plugins/IAGraphicsPlugin
cmake $CMAKE_CONFIGURATION_FLAGS
cmake --build build
cd -
cp ./plugins/IAGraphicsPlugin/build/"$DR4_PLUGIN_NAME" ./external/plugins/dr4

PP_PLUGIN_NAME=libIADorisovkaPlugin.so
rm ./external/plugins/pp/"$PP_PLUGIN_NAME"
cd ./plugins/IADorisovkaPlugin
cmake $CMAKE_CONFIGURATION_FLAGS
cmake --build build
cd -
cp ./plugins/IADorisovkaPlugin/build/"$PP_PLUGIN_NAME" ./external/plugins/pp

cmake $CMAKE_CONFIGURATION_FLAGS
cmake --build build

./build/$PROJECT_NAME ./external/plugins/dr4/$DR4_PLUGIN_NAME

