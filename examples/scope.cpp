/*
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

// Pull in the reference implementation of P2300:
#include <stdexec/execution.hpp>
#include <exec/async_scope.hpp>

#include "exec/env.hpp"
#include "exec/static_thread_pool.hpp"

#include <cstdio>

///////////////////////////////////////////////////////////////////////////////
// Example code:
using namespace stdexec;
using stdexec::sync_wait;


int main() {
  exec::static_thread_pool ctx{1};
  exec::async_scope scope;

  struct Lambda {
    void operator()() const noexcept {

    }
  };

  scheduler auto sch = ctx.get_scheduler();                               // 1
  sender auto printEmpty = then(on(sch, scope.on_empty()),
    Lambda{});

  printf("\n"
    "spawn void\n"
    "==========\n");

  //sync_wait(printEmpty);
  //sender auto allDone = when_all(printEmpty);
  //decltype(allDone)::okok;
  sender auto allDone = stdexec::__when_all::when_all_t::__sender_t<decltype(printEmpty)&>{printEmpty};
  //sync_wait(std::move(allDone));
}
