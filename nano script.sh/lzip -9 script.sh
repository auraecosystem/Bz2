#!/bin/bash
# Find and compress all files in the current folder, skipping hidden git files
find . -type f ! -path '*/.*' | while read -r file; do
    lzip -9 -f -m "$file"
done
