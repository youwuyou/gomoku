project(Lama-unit-tests)

set(TEST_SOURCE_FILES
        card.cpp
        hand.cpp)

add_executable(Lama-tests ${TEST_SOURCE_FILES})

target_compile_definitions(Lama-tests PRIVATE LAMA_SERVER=1 RAPIDJSON_HAS_STDSTRING=1)

target_link_libraries(Lama-tests gtest gtest_main Lama-lib)