#=============================================================================
# Copyright 2023 NVIDIA Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=============================================================================

set(exec_test_sources
    ../test_main.cpp
    async_scope/test_dtor.cpp
    async_scope/test_spawn.cpp
    async_scope/test_spawn_future.cpp
    async_scope/test_start_now.cpp
    async_scope/test_empty.cpp
    async_scope/test_stop.cpp
    )

add_executable(test.exec ${exec_test_sources})
target_link_libraries(test.exec
    PUBLIC
    STDEXEC::stdexec
    $<TARGET_NAME_IF_EXISTS:STDEXEC::tbbexec>
    stdexec_executable_flags
    Catch2::Catch2
    PRIVATE
    common_test_settings)

add_executable(test.system_context_replaceability ../test_main.cpp test_system_context_replaceability.cpp)
target_link_libraries(test.system_context_replaceability
    PUBLIC
    STDEXEC::stdexec
    STDEXEC::system_context
    stdexec_executable_flags
    Catch2::Catch2
    PRIVATE
    common_test_settings)

# Discover the Catch2 test built by the application
catch_discover_tests(test.exec)
if(NOT STDEXEC_ENABLE_CUDA)
    catch_discover_tests(test.system_context_replaceability)
endif()

# Test which parses a source file for an expected error message
icm_add_build_failure_test(
    NAME test_repeat_effect_until_fail
    TARGET test_repeat_effect_until_fail
    SOURCES PARSE test_repeat_effect_until_fail.cpp
    LIBRARIES stdexec
    FOLDER test
)

icm_add_build_failure_test(
    NAME test_repeat_n_fail
    TARGET test_repeat_n_fail
    SOURCES PARSE test_repeat_n_fail.cpp
    LIBRARIES stdexec
    FOLDER test
)
