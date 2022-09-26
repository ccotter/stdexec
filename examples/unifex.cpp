#include <execution.hpp>
#include <unifex/just.hpp>
#include <unifex/just_done.hpp>
#include <unifex/then.hpp>

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

  template <typename Receiver>
  struct State {
      State(Sender sender, Receiver&& receiver) :
          _sender(std::move(sender)),
          _unifex_receiver{std::move(receiver)},
          _state(unifex::connect(_sender, _unifex_receiver))
      {
      }

      State(const State&) = delete;
      State& operator=(const State&) = delete;

      Sender _sender;
      UnifexReceiver<Receiver> _unifex_receiver;
      unifex::connect_result_t<Sender, UnifexReceiver<Receiver>> _state;

      friend auto tag_invoke(ex::start_t, State& self) noexcept {
          self._state.start();
      }
  };

  template <class Self, class Receiver>
      requires (std::same_as<sender_t, std::remove_cv_t<Self>>)
  friend auto tag_invoke(ex::connect_t, Self&& self, Receiver receiver) {
    return State<Receiver>(((Self&&)self)._sender, std::move(receiver));
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
  auto operator()(Sender sender) const {
    return sender_t<Sender>{std::move(sender)};
  }
};

}

using _from_unifex::from_unifex_t;
inline constexpr from_unifex_t from_unifex{};

int main() {

  {
    auto snd = from_unifex(unifex::just(10));
    static_assert(ex::sender<decltype(snd)>);
    static_assert(ex::sender_of<decltype(snd), ex::set_value_t(int)>);
    auto [val] = std::this_thread::sync_wait(std::move(snd)).value();
    CHECK(val == 10);
  }

  {
    auto snd = from_unifex(unifex::just(10, 20.0));
    static_assert(ex::sender<decltype(snd)>);
    static_assert(ex::sender_of<decltype(snd), ex::set_value_t(int, double)>);
    auto [a, b] = std::this_thread::sync_wait(std::move(snd)).value();
    CHECK(a == 10);
    CHECK(b == 20.);
  }

  {
    auto snd = from_unifex(unifex::just() | unifex::then([] { return 30; }));
    static_assert(ex::sender<decltype(snd)>);
    static_assert(ex::sender_of<decltype(snd), ex::set_value_t(int)>);
    auto [a] = std::this_thread::sync_wait(std::move(snd)).value();
    CHECK(a == 30);
  }

  {
    auto snd = from_unifex(unifex::just() | unifex::then([] { throw std::runtime_error("Oops"); }));
    static_assert(ex::sender<decltype(snd)>);
    static_assert(ex::sender_of<decltype(snd), ex::set_value_t()>);
    CHECK_THROW(std::this_thread::sync_wait(std::move(snd)).value(), std::runtime_error);
  }

  {
    //auto snd = from_unifex(unifex::just_done());
    auto snd = ex::just_stopped() | ex::stopped_as_optional();
    static_assert(ex::sender<decltype(snd)>);
    //static_assert(ex::sender_of<decltype(snd), ex::set_stopped_t()>);
    auto v = std::this_thread::sync_wait(std::move(snd));
    CHECK(v.has_value());
  }

  return 0;
}
