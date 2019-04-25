set(THIRD_PARTY_MODULE_PATH "${PROJECT_SOURCE_DIR}/third_party")

# Catch2

set(CATCH_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/Catch2")
set(CATCH_INCLUDE_PATH "${CATCH_MODULE_PATH}/single_include" PARENT_SCOPE)

include ("${CATCH_MODULE_PATH}/contrib/ParseAndAddCatchTests.cmake")
