#include <execution.hpp>
#include <unifex/just.hpp>

namespace ex = std::execution;

namespace _from_unifex {

template <class Sender>
struct sender_t {
  Sender _sender;

  template <class _Self, class _Receiver>
  friend auto tag_invoke(ex::connect_t, _Self&& self, _Receiver __receiver) {
    return ex::connect(((_Self&&)self)._sender, std::move(__receiver));
  }

  using completion_signatures = 
    ex::completion_signatures<
      ex::set_value_t(int),
      ex::set_error_t(std::exception_ptr)>;
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
    auto snd = from_unifex(ex::just(10));
    static_assert(ex::sender<decltype(snd)>);
    auto [val] = std::this_thread::sync_wait(std::move(snd)).value();
    assert(val == 10);
  }

  //{
  //  auto [val] = std::this_thread::sync_wait(unifex::just(10)).value();
  //  assert(val == 10);
  //}

  return 0;
}
