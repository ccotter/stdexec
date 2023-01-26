/*
 * Copyright (c) 2022 Lucian Radu Teodorescu
 * Copyright (c) 2021-2022 NVIDIA Corporation
 *
 * Licensed under the Apache License Version 2.0 with LLVM Exceptions
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *   https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <catch2/catch.hpp>
#include <stdexec/execution.hpp>
#include <test_common/schedulers.hpp>
#include <test_common/receivers.hpp>
#include <test_common/type_helpers.hpp>
#include <exec/env.hpp>
#include <exec/static_thread_pool.hpp>

namespace ex = stdexec;

using namespace std::chrono_literals;

struct move_only_type {
  move_only_type(int v) : val(v) {}
  move_only_type(move_only_type&&) = default;
  int val;
};
struct copy_and_movable_type {
  copy_and_movable_type(int v) : val(v) {}
  int val;
};

struct Lambda {
  int operator()(auto&&...) { return 0; }
};

TEST_CASE("split move only into then", "[adaptors][split]") {
  using Type = move_only_type;
  //using Type = copy_and_movable_type;
  SECTION("split sender is rvalue") {
    int counter{};
    auto snd = ex::split(ex::just(Type{0})) | ex::then(Lambda{});
  }
  SECTION("split sender is lvalue") {
    int counter{};
    auto multishot = ex::split(ex::just(Type{0}));
    //static_assert(ex::sender<decltype(multishot)&>);

    using _Sender = typename stdexec::__just::__sender<move_only_type>::__t &;
    using _Env = stdexec::__env::__env<stdexec::__env::__empty_env>;
    static_assert(ex::sender<_Sender, _Env>);
    //tag_invoke(ex::get_completion_signatures_t{}, multishot, 0);

    //auto snd = multishot | ex::then(Lambda{});
  }
}
