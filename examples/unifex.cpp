#include <execution.hpp>
#include <unifex/just.hpp>
#include <unifex/just_done.hpp>
#include <unifex/let_value.hpp>
#include <unifex/let_error.hpp>
#include <unifex/let_done.hpp>
#include <unifex/then.hpp>
#include <unifex/done_as_optional.hpp>
#include <unifex/sync_wait.hpp>

#include <exception>
#include <iostream>

#define CHECK(x) \
    do { \
        if (!(x)) { \
            std::cout << "test assertion failure: " << #x << std::endl; \
            std::terminate(); \
        } \
    } while (0)
#define CHECK_THROW(x, etype) \
    do { \
        bool did_throw = false; \
        try { \
            (x); \
        } catch (const etype&) { \
            did_throw = true; \
        } \
        CHECK(did_throw); \
    } while (0)

namespace ex = std::execution;

namespace _from_unifex {

template <typename List>
struct map_set_value;

template <typename... Ts>
struct map_set_value<unifex::type_list<Ts...>>
{
    template <typename U>
    struct map_one;

    template <typename... Us>
    struct map_one<unifex::type_list<Us...>>
    {
        using type = ex::set_value_t(Us...);
    };

    using type = unifex::type_list<typename map_one<Ts>::type...>;
};

template <typename List>
using map_set_value_t = typename map_set_value<List>::type;

template <class Sender>
struct sender_t {
  Sender _sender;

  template <typename Receiver>
  struct UnifexReceiver
  {
      Receiver _receiver;

      template <typename... Values>
      void set_value(Values&&... values) noexcept
      {
          ex::set_value(std::move(_receiver), std::forward<Values>(values)...);
      }

      template <typename Error>
      void set_error(Error&& error) noexcept
      {
          ex::set_error(std::move(_receiver), std::forward<Error>(error));
      }

      void set_done() noexcept
      {
          ex::set_stopped(std::move(_receiver));
      }
  };

  template <typename Sender2, typename Receiver>
  struct State {
      State(Sender2&& sender, Receiver&& receiver) :
          _sender(std::move(sender)),
          _unifex_receiver{std::move(receiver)},
          _state(unifex::connect(std::move(_sender), _unifex_receiver))
      {
      }

      State(const State&) = delete;
      State& operator=(const State&) = delete;

      Sender2 _sender;
      static_assert(unifex::sender<std::decay_t<Sender2>>);
      UnifexReceiver<Receiver> _unifex_receiver;
      static_assert(unifex::receiver<UnifexReceiver<Receiver>>);
      unifex::connect_result_t<Sender2, UnifexReceiver<Receiver>> _state;

      friend auto tag_invoke(ex::start_t, State& self) noexcept {
          self._state.start();
      }
  };

  template <class Self, class Receiver>
      requires (std::same_as<sender_t, std::remove_cv_t<Self>>)
  friend auto tag_invoke(ex::connect_t, Self&& self, Receiver receiver) {
    return State<unifex::member_t<Self, Sender>, Receiver>(((Self&&)self)._sender, std::move(receiver));
  }

  using completion_signatures = 
    typename unifex::concat_type_lists_t<
        map_set_value_t<unifex::sender_value_type_list_t<Sender>>,
        unifex::type_list<ex::set_error_t(std::exception_ptr)>,
        std::conditional_t<Sender::sends_done,
            unifex::type_list<ex::set_stopped_t()>,
            unifex::type_list<>
        >
    >::template apply<ex::completion_signatures>;
};

struct from_unifex_t {
  template <class Sender>
  auto operator()(Sender&& sender) const {
    return sender_t<Sender>{(Sender&&)sender};
  }
};

}

using _from_unifex::from_unifex_t;
inline constexpr from_unifex_t from_unifex{};

int main() {

  {
    auto snd = from_unifex(unifex::just(10));
    static_assert(ex::sender<decltype(snd)>);
    static_assert(std::is_same_v<decltype(snd)::completion_signatures,
        ex::completion_signatures<
          ex::set_value_t(int),
          ex::set_error_t(std::exception_ptr)>
      >);
    auto [val] = std::this_thread::sync_wait(std::move(snd)).value();
    CHECK(val == 10);
  }

  {
    auto snd = from_unifex(unifex::just(10, 20.0));
    static_assert(ex::sender<decltype(snd)>);
    static_assert(std::is_same_v<decltype(snd)::completion_signatures,
        ex::completion_signatures<
          ex::set_value_t(int, double),
          ex::set_error_t(std::exception_ptr)>
      >);
    auto [a, b] = std::this_thread::sync_wait(std::move(snd)).value();
    CHECK(a == 10);
    CHECK(b == 20.);
  }

  {
    auto snd = from_unifex(unifex::just() | unifex::then([] { return 30; }));
    static_assert(ex::sender<decltype(snd)>);
    static_assert(std::is_same_v<decltype(snd)::completion_signatures,
        ex::completion_signatures<
          ex::set_value_t(int),
          ex::set_error_t(std::exception_ptr)>
      >);
    auto [a] = std::this_thread::sync_wait(std::move(snd)).value();
    CHECK(a == 30);
  }

  {
    auto snd = from_unifex(unifex::just() | unifex::then([] { throw std::runtime_error("Oops"); }));
    static_assert(ex::sender<decltype(snd)>);
    static_assert(std::is_same_v<decltype(snd)::completion_signatures,
        ex::completion_signatures<
          ex::set_value_t(),
          ex::set_error_t(std::exception_ptr)>
      >);
    CHECK_THROW(std::this_thread::sync_wait(std::move(snd)).value(), std::runtime_error);
  }

  {
    auto snd = from_unifex(unifex::just(10) | unifex::done_as_optional());
    static_assert(ex::sender<decltype(snd)>);
    static_assert(std::is_same_v<decltype(snd)::completion_signatures,
        ex::completion_signatures<
          ex::set_value_t(std::optional<int>),
          ex::set_error_t(std::exception_ptr)>
      >);
    auto v = std::this_thread::sync_wait(std::move(snd));
    CHECK(v.has_value());
    CHECK(v == std::make_optional(std::make_tuple(10)));
  }

  {
    auto snd = from_unifex(
        unifex::just() |
        unifex::then([]() -> int {
          throw std::exception{};
          return 10;
        }) |
        unifex::let_error([](auto&&) {
          return unifex::just_done();
        })
    );
    static_assert(ex::sender<decltype(snd)>);
    static_assert(std::is_same_v<decltype(snd)::completion_signatures,
        ex::completion_signatures<
          ex::set_value_t(int),
          ex::set_error_t(std::exception_ptr),
          ex::set_stopped_t()>
      >);
    auto v = std::this_thread::sync_wait(std::move(snd));
    CHECK(!v);
  }

  return 0;
}
