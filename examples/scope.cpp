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

struct Lambda {
  void operator()(auto&&...) const noexcept {
  }
};

///////////////////////////////////////////////////////////////////////////////
// Example code:
using namespace stdexec;
using stdexec::sync_wait;

class noop_receiver : receiver_adaptor<noop_receiver> {
  friend receiver_adaptor<noop_receiver>;
  template <class... _As>
    void set_value(_As&&... ) noexcept {
    }
  void set_error(std::exception_ptr) noexcept {
  }
  void set_stopped() noexcept {
  }
  auto get_env() const& {
    return exec::make_env(exec::with(get_stop_token, stdexec::never_stop_token{}));
  }
};

#if 1
int main() {
  exec::static_thread_pool ctx{1};
  exec::async_scope scope;

  scheduler auto sch = ctx.get_scheduler();                               // 1

  sender auto begin = schedule(sch);                                      // 2

  sender auto printVoid = then(begin,
    []()noexcept { printf("void\n"); });                                  // 3

  sender auto printEmpty = then(on(sch, scope.on_empty()),
    []()noexcept{ printf("scope is empty\n"); });                         // 4

  printf("\n"
    "spawn void\n"
    "==========\n");

  scope.spawn(printVoid);                                                 // 5

  sync_wait(printEmpty);

  printf("\n"
    "spawn void and 42\n"
    "=================\n");

  sender auto fortyTwo = then(begin, []()noexcept {return 42;});          // 6

  scope.spawn(printVoid);                                                 // 7

  sender auto fortyTwoFuture = scope.spawn_future(fortyTwo);              // 8

  sender auto printFortyTwo = then(std::move(fortyTwoFuture),
    [](int fortyTwo)noexcept{ printf("%d\n", fortyTwo); });               // 9

  sender auto allDone = then(
    when_all(printEmpty, std::move(printFortyTwo)),
    Lambda{});

  sync_wait(std::move(allDone));

  {
    sender auto nest = scope.nest(begin);
    (void)nest;
  }
  sync_wait(scope.on_empty());

  {
    sender auto nest = scope.nest(begin);
    auto op = connect(std::move(nest), noop_receiver{});
    (void)op;
  }
  sync_wait(scope.on_empty());

  {
    sender auto nest = scope.nest(begin);
    sync_wait(std::move(nest));
  }
  sync_wait(scope.on_empty());
}
#endif
void fn() {
  static_assert(receiver<Lambda>);
  using X = stdexec::__compl_sigs::__env_promise<stdexec::__env::__empty_env>;
  using Y0 = stdexec::_Y<stdexec::__compl_sigs::__env_promise<stdexec::__env::__empty_env> >;
  using Y = stdexec::__when_all::__receiver<1, Y0, std::tuple<std::optional<std::tuple<> >, std::optional<std::tuple<> > >, std::variant<std::__exception_ptr::exception_ptr> >::__t;
  //Y0::okok;
  //static_assert(stdexec::receiver<X>);
}
