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

  template <class T> struct Lambda1 {
    T operator()(auto&&...) {
      return 0;
    }
  };
  struct Lambda2 {
    int operator()(auto&&...) {
      return 0;
    }
  };
TEST_CASE("foo", "[adaptors][split]") {
  using TestType = move_only_type;
  //using TestType = copy_and_movable_type;
  int called = 0;
  auto multishot = 
      ex::just(TestType(10)) |
      //ex::then([&](TestType obj) { ++called; return TestType(obj.val+1); }) |
      ex::then(Lambda1<TestType>{}) |
      ex::split();
  auto multishot2 = ex::just(4);
  decltype(multishot)::okok;
  auto wa =
      ex::then(multishot, Lambda2{});
  decltype(wa)::okok;
  //using X = stdexec::__split::__sender<stdexec::__then::__sender<stdexec::__just::__sender<move_only_type>, Lambda1<move_only_type>>, stdexec::__env::__empty_env>::__t ;
  //static_assert(ex::sender<X>);
}
