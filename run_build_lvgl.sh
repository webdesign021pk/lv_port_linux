#!/bin/bash
clear
echo "Building LVGL Simulator..."
echo "----------------------------------------"
cmake -B build -S .
make -C build -j
echo "----------------------------------------"
echo "Running LVGL Simulator..."
echo "----------------------------------------"
./build/bin/lvglsim -b sdl
