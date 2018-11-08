option(DEBUG "dump debug information" OFF)
if(DEBUG)
    add_definitions(-DJARNGREIPR_DEBUG)
endif()
