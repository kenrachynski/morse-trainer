# Runs at build time to generate version_generated.h with current git SHA and date.
# Called via add_custom_target in src/CMakeLists.txt.

execute_process(
    COMMAND git -C "${SRC_DIR}" rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_SHA
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT GIT_SHA)
    set(GIT_SHA "unknown")
endif()

string(TIMESTAMP BUILD_DATE "%Y-%m-%d" UTC)

file(WRITE "${OUTPUT}"
    "#pragma once\n"
    "#define APP_GIT_SHA \"${GIT_SHA}\"\n"
    "#define APP_BUILD_DATE \"${BUILD_DATE}\"\n"
)
