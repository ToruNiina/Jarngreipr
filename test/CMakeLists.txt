################
# Binary setup #
################

# Define test executable
add_executable(hello_test
    main.cpp
)

# Add test
add_test("Hello" hello_test)
