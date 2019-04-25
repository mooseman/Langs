# get rid of file glob. maybe change for target_sources() command
file(GLOB TEST_SOURCE_FILES "${PROJECT_SOURCE_DIR}/tests/*.cxx")

add_executable(Tests ${TEST_SOURCE_FILES})

set_target_properties(Tests
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

target_include_directories(Tests
  PRIVATE
    ${CATCH_INCLUDE_PATH}
    ${SRC_DIR}
)

target_sources(Tests
  PRIVATE
    ${SRC_DIR}/token.cxx
    ${SRC_DIR}/lexer.cxx
)

enable_testing()
ParseAndAddCatchTests(Tests)
