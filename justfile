# Format all C++ source and header files in the project
format:
    find mugjc -type f \( -name '*.hpp' -o -name '*.cpp' \) -exec clang-format -i {} +

# Check formatting without modifying files (useful for CI/pre-commit)
check-format:
    find mugjc -type f \( -name '*.hpp' -o -name '*.cpp' \) -exec clang-format --dry-run --Werror {} +
