
cmake_minimum_required(VERSION 3.22.1)

project(ASCII LANGUAGES CXX)

add_executable(ASCII ascii.cpp)


function(print var)
	message("${var} = ${${var}}")
endfunction()

print(PROJECT_NAME)
