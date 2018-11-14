add_executable(jarngreipr jarngreipr.cpp)
set_target_properties(jarngreipr
    PROPERTIES
    COMPILE_FLAGS "-O2 -Wall -Wextra -Wpedantic"
    RUNTIME_OUTPUT_DIRECTORY "${JARNGREIPR_BIN_DIR}"
)
