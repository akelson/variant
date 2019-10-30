cmake_minimum_required (VERSION 3.0)

add_executable(
  variant
  variant.cpp
  )

target_compile_options(
	variant PRIVATE -Werror -Wall -std=c++03
  )
