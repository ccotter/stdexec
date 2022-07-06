#include <execution.hpp>
#include <unifex/just.hpp>

namespace ex = std::execution;

int main() {

  {
    auto [val] = std::this_thread::sync_wait(ex::just(10)).value();
    assert(val == 10);
  }

  {
    auto [val] = std::this_thread::sync_wait(unifex::just(10)).value();
    assert(val == 10);
  }

  return 0;
}
