# Compiler flags
set(FLAGS "-std=c++11 -Ofast -march=native -mtune=native")

# Append flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
