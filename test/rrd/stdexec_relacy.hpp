#pragma once

// Relacy does not provide rl::atomic_ref or std::atomic_ref. We don't actually need it
// for the tests we have written for stdexec.
namespace std {
template <class T> class atomic_ref;
}
