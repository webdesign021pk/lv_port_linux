#!/bin/bash

set -e  # Exit on error

echo "🔄 Building all app .so files in src/apps/*/ ..."

for app_src in src/apps/*/*.c; do
    [ -f "$app_src" ] || continue  # skip if not a file

    app_dir=$(dirname "$app_src")
    app_name=$(basename "$app_dir")
    so_file="$app_dir/lib${app_name}.so"

    echo "⚙️  Compiling $app_name → $so_file"

    gcc -fPIC -shared \
        -o "$so_file" \
        "$app_src" \
        -I./lvgl -I./lvgl/src -I./config -I./src
done

echo "✅ Done. All apps compiled."
