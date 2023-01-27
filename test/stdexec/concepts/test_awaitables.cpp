/*
 * Copyright (c) 2022 Shreyas Atre
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

#include <stdexec/coroutine.hpp>
#include <tuple>
#include <variant>

#include <test_common/type_helpers.hpp>

#if !_STD_NO_COROUTINES_

namespace ex = stdexec;

template <class Sender>
  concept sender_with_attrs =
    ex::sender<Sender> &&
    requires (const Sender& s) {
      ex::get_env(s);
    };

template <typename Awaiter>
struct promise {
  __coro::coroutine_handle<promise> get_return_object() {
    return {__coro::coroutine_handle<promise>::from_promise(*this)};
  }
  __coro::suspend_always initial_suspend() noexcept { return {}; }
  __coro::suspend_always final_suspend() noexcept { return {}; }
  void return_void() {}
  void unhandled_exception() {}

  template <typename... T>
  auto await_transform(T&&...) noexcept {
    return Awaiter{};
  }
};

struct awaiter {
  bool await_ready() { return true; }
  bool await_suspend(__coro::coroutine_handle<>) { return false; }
  bool await_resume() { return false; }
};

using dependent = ex::dependent_completion_signatures<ex::no_env>;

template <typename Awaiter>
struct awaitable_sender_1 {
  Awaiter operator co_await();
};

struct awaitable_sender_2 {
  using promise_type = promise<__coro::suspend_always>;
private:
  friend dependent operator co_await(awaitable_sender_2);
};

struct awaitable_sender_3 {
  using promise_type = promise<awaiter>;
private:
  friend dependent operator co_await(awaitable_sender_3);
};

struct awaitable_sender_4 {
  using promise_type = promise<__coro::suspend_always>;
private:
  template <class Promise>
  friend awaiter tag_invoke(ex::as_awaitable_t, awaitable_sender_4, Promise&) {
    return {};
  }
  friend dependent tag_invoke(ex::as_awaitable_t, awaitable_sender_4, ex::no_env_promise&) {
    return {};
  }
};

struct awaitable_sender_5 {
private:
  template <class Promise>
  friend awaiter tag_invoke(ex::as_awaitable_t, awaitable_sender_5, Promise&) {
    return {};
  }
};

template <typename Signatures, typename Awaiter>
void test_awaitable_sender1(Signatures*, Awaiter&&) {
  static_assert(ex::sender<awaitable_sender_1<Awaiter>>);
  static_assert(sender_with_attrs<awaitable_sender_1<Awaiter>>);
  static_assert(ex::__awaitable<awaitable_sender_1<Awaiter>>);

  static_assert(
    !ex::__get_completion_signatures::__with_member_alias<awaitable_sender_1<Awaiter>>);
  static_assert(
    std::is_same_v<ex::completion_signatures_of_t<awaitable_sender_1<Awaiter>>, Signatures>);
}

void test_awaitable_sender2() {
  static_assert(ex::sender<awaitable_sender_2>);
  static_assert(sender_with_attrs<awaitable_sender_2>);
  static_assert(!ex::sender<awaitable_sender_2, ex::__empty_env>);

  static_assert(ex::__awaitable<awaitable_sender_2>);
  static_assert(ex::__awaitable<awaitable_sender_2, promise<__coro::suspend_always>>);

  static_assert(
    !ex::__get_completion_signatures::__with_member_alias<awaitable_sender_2>);

  static_assert(std::is_same_v<
      ex::completion_signatures_of_t<awaitable_sender_2>,
      dependent>);
}

void test_awaitable_sender3() {
  static_assert(ex::sender<awaitable_sender_3>);
  static_assert(sender_with_attrs<awaitable_sender_3>);
  static_assert(!ex::sender<awaitable_sender_3, ex::__empty_env>);

  static_assert(ex::__awaiter<awaiter>);
  static_assert(ex::__awaitable<awaitable_sender_3>);
  static_assert(ex::__awaitable<awaitable_sender_3, promise<awaiter>>);

  static_assert(
    !ex::__get_completion_signatures::__with_member_alias<awaitable_sender_3>);

  static_assert(std::is_same_v<
      ex::completion_signatures_of_t<awaitable_sender_3>,
      dependent>);
}

template <class Signatures>
void test_awaitable_sender4(Signatures*) {
  static_assert(ex::sender<awaitable_sender_4>);
  static_assert(sender_with_attrs<awaitable_sender_4>);
  static_assert(ex::sender<awaitable_sender_4, ex::__empty_env>);

  static_assert(ex::__awaiter<awaiter>);
  static_assert(!ex::__awaitable<awaitable_sender_4>);
  static_assert(ex::__awaitable<awaitable_sender_4, promise<awaiter>>);
  static_assert(ex::__awaitable<awaitable_sender_4, ex::no_env_promise>);
  static_assert(ex::__awaitable<awaitable_sender_4, ex::__env_promise<ex::__empty_env>>);

  static_assert(
    !ex::__get_completion_signatures::__with_member_alias<awaitable_sender_4>);

  static_assert(std::is_same_v<
      ex::completion_signatures_of_t<awaitable_sender_4>,
      dependent>);
  static_assert(std::is_same_v<
      ex::completion_signatures_of_t<awaitable_sender_4, ex::__empty_env>,
      Signatures>);
}

struct connect_awaitable_promise
  : ex::with_awaitable_senders<connect_awaitable_promise>
{};

template <class Signatures>
void test_awaitable_sender5(Signatures*) {
  static_assert(ex::sender<awaitable_sender_5>);
  static_assert(sender_with_attrs<awaitable_sender_5>);
  static_assert(ex::sender<awaitable_sender_5, ex::__empty_env>);

  static_assert(ex::__awaiter<awaiter>);
  static_assert(!ex::__awaitable<awaitable_sender_5>);
  static_assert(ex::__awaitable<awaitable_sender_5, promise<awaiter>>);
  static_assert(ex::__awaitable<awaitable_sender_5, ex::no_env_promise>);
  static_assert(ex::__awaitable<awaitable_sender_5, ex::__env_promise<ex::__empty_env>>);

  static_assert(
    !ex::__get_completion_signatures::__with_member_alias<awaitable_sender_5>);

  static_assert(std::is_same_v<
      ex::completion_signatures_of_t<awaitable_sender_5>,
      Signatures>);
  static_assert(std::is_same_v<
      ex::completion_signatures_of_t<awaitable_sender_5, ex::__empty_env>,
      Signatures>);
}

template <typename Error, typename... Values>
auto signature_error_values(Error, Values...)
    -> ex::completion_signatures<ex::set_value_t(Values...), ex::set_error_t(Error)>* {
  return {};
}

TEST_CASE("get completion_signatures for awaitables", "[sndtraits][awaitables]") {
  ::test_awaitable_sender1(
    signature_error_values(std::exception_ptr()), __coro::suspend_always{});
  ::test_awaitable_sender1(
    signature_error_values(
      std::exception_ptr(),
      ex::__await_result_t<awaitable_sender_1<awaiter>>()),
    awaiter{});

  ::test_awaitable_sender2();

  ::test_awaitable_sender3();

  ::test_awaitable_sender4(
    signature_error_values(
      std::exception_ptr(),
      ex::__await_result_t<awaitable_sender_4, promise<awaiter>>()));

  ::test_awaitable_sender5(
    signature_error_values(
      std::exception_ptr(),
      ex::__await_result_t<awaitable_sender_5, connect_awaitable_promise>()));
}

struct awaitable_attrs {};
template <typename Awaiter>
struct awaitable_with_get_attrs {
  Awaiter operator co_await();
  friend awaitable_attrs tag_invoke(ex::get_env_t, const awaitable_with_get_attrs&) noexcept {
    return {};
  }
};

TEST_CASE("get_attrs for awaitables", "[sndtraits][awaitables]") {
#if 0
  // NOT TO SPEC
  // When the __awaitable constrained get_attrs overload is enabled, enable
  // these checks inside this path of the 'if' directive. get_attrs returns
  // __empty_env for awaitables by default.

  check_attrs_type<ex::__empty_env>(awaitable_sender_1<awaiter>{});
  check_attrs_type<ex::__empty_env>(awaitable_sender_3{});
#else
  // And delete these two lines
  check_attrs_type<const awaitable_sender_1<awaiter>&>(awaitable_sender_1<awaiter>{});
  check_attrs_type<const awaitable_sender_3&>(awaitable_sender_3{});
#endif
  check_attrs_type<awaitable_attrs>(awaitable_with_get_attrs<awaiter>{});
}

#endif // !_STD_NO_COROUTINES_
