/*
 * Copyright (c) NVIDIA
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
#pragma once

#include <type_traits>
#include <utility>

namespace std {
  struct __ {};

  struct __ignore {
    __ignore() = default;
    __ignore(auto&&) noexcept {}
  };

  struct __immovable {
    __immovable() = default;
    __immovable(__immovable&&) = delete;
  };

  // For hiding a template type parameter from ADL
  template <class _T>
    struct __x_ {
      struct __t {
        using type = _T;
      };
    };
  template <class _T>
    using __x = typename __x_<_T>::__t;

  template <class _T>
    using __t = typename _T::type;

  template <bool _B>
    using __bool = bool_constant<_B>;

  template <size_t _N>
    using __index = integral_constant<size_t, _N>;

  // Some utilities for manipulating lists of types at compile time
  template <class...>
  struct __types
#if defined(__GNUC__) && !defined(__clang__)
  {}  // BUGBUG: GCC does not like this "incomplete type"
#endif
  ;

  template <class _T>
    using __id = _T;

  template <class _T>
    inline constexpr auto __v = _T::value;

  template <class _T, class _U>
    inline constexpr bool __v<is_same<_T, _U>> = false;

  template <class _T>
    inline constexpr bool __v<is_same<_T, _T>> = true;

  template <class _T, _T _I>
    inline constexpr _T __v<integral_constant<_T, _I>> = _I;

  template <template <class...> class _Fn>
    struct __q {
      template <class... _Args>
        using __f = _Fn<_Args...>;
    };

  template <template <class> class _Fn>
    struct __q1 {
      template <class _Arg>
        using __f = _Fn<_Arg>;
    };

  template <template <class, class> class _Fn>
    struct __q2 {
      template <class _First, class _Second>
        using __f = _Fn<_First, _Second>;
    };

  template <template<class...> class _Fn, class... _Front>
    struct __mbind_front_q {
      template <class... _Args>
        using __f = _Fn<_Front..., _Args...>;
    };

  template <template<class...> class _Fn, class... _Front>
    struct __mbind_front_q1 {
      template <class _A>
        using __f = _Fn<_Front..., _A>;
    };

  template <template<class...> class _Fn, class... _Front>
    struct __mbind_front_q2 {
      template <class _A, class _B>
        using __f = _Fn<_Front..., _A, _B>;
    };

  template <template<class...> class _Fn, class... _Front>
    struct __mbind_front_q3 {
      template <class _A, class _B, class _C>
        using __f = _Fn<_Front..., _A, _B, _C>;
    };

  template <class _Fn, class... _Front>
    using __mbind_front = __mbind_front_q<_Fn::template __f, _Front...>;

  template <class _Fn, class... _Back>
    using __mbind_front1 = __mbind_front_q1<_Fn::template __f, _Back...>;

  template <class _Fn, class... _Back>
    using __mbind_front2 = __mbind_front_q2<_Fn::template __f, _Back...>;

  template <class _Fn, class... _Back>
    using __mbind_front3 = __mbind_front_q3<_Fn::template __f, _Back...>;

  template <template<class...> class _Fn, class... _Back>
    struct __mbind_back_q {
      template <class... _Args>
        using __f = _Fn<_Args..., _Back...>;
    };

  template <template<class...> class _Fn, class... _Back>
    struct __mbind_back_q1 {
      template <class _A>
        using __f = _Fn<_A, _Back...>;
    };

  template <template<class...> class _Fn, class... _Back>
    struct __mbind_back_q2 {
      template <class _A, class _B>
        using __f = _Fn<_A, _B, _Back...>;
    };

  template <template<class...> class _Fn, class... _Back>
    struct __mbind_back_q3 {
      template <class _A, class _B, class _C>
        using __f = _Fn<_A, _B, _C, _Back...>;
    };

  template <class _Fn, class... _Back>
    using __mbind_back = __mbind_back_q<_Fn::template __f, _Back...>;

  template <class _Fn, class... _Back>
    using __mbind_back1 = __mbind_back_q1<_Fn::template __f, _Back...>;

  template <class _Fn, class... _Back>
    using __mbind_back2 = __mbind_back_q2<_Fn::template __f, _Back...>;

  template <class _Fn, class... _Back>
    using __mbind_back3 = __mbind_back_q3<_Fn::template __f, _Back...>;

  template <template <class, class, class> class _Fn>
    struct __q3 {
      template <class _First, class _Second, class _Third>
        using __f = _Fn<_First, _Second, _Third>;
    };

  template <class _Fn, class... _Args>
    using __minvoke = typename _Fn::template __f<_Args...>;

  template <class _Fn, class _First>
    using __minvoke1 = typename _Fn::template __f<_First>;

  template <class _Fn, class _First, class _Second>
    using __minvoke2 = typename _Fn::template __f<_First, _Second>;

  template <class _Fn, class _First, class _Second, class _Third>
    using __minvoke3 = typename _Fn::template __f<_First, _Second, _Third>;

  template <template <class...> class _T, class... _Args>
    concept __valid = requires { typename _T<_Args...>; };

  template <template <class> class _T, class _First>
    concept __valid1 = requires { typename _T<_First>; };

  template <template <class, class> class _T, class _First, class _Second>
    concept __valid2 = requires { typename _T<_First, _Second>; };

  template <template <class, class, class> class _T, class _First, class _Second, class _Third>
    concept __valid3 = requires { typename _T<_First, _Second, _Third>; };

  template <class _Fn, class... _Args>
    concept __minvocable = __valid<_Fn::template __f, _Args...>;

  template <class _Fn, class _First>
    concept __minvocable1 = __valid1<_Fn::template __f, _First>;

  template <class _Fn, class _First, class _Second>
    concept __minvocable2 = __valid2<_Fn::template __f, _First, _Second>;

  template <class _Fn, class _First, class _Second, class _Third>
    concept __minvocable3 = __valid3<_Fn::template __f, _First, _Second, _Third>;

  template <template <class...> class _T>
    struct __defer {
      template <class... _Args>
          requires __valid<_T, _Args...>
        struct __f_ { using type = _T<_Args...>; };
      template <class _A>
          requires requires { typename _T<_A>; }
        struct __f_<_A> { using type = _T<_A>; };
      template <class _A, class _B>
          requires requires { typename _T<_A, _B>; }
        struct __f_<_A, _B> { using type = _T<_A, _B>; };
      template <class _A, class _B, class _C>
          requires requires { typename _T<_A, _B, _C>; }
        struct __f_<_A, _B, _C> { using type = _T<_A, _B, _C>; };
      template <class _A, class _B, class _C, class _D>
          requires requires { typename _T<_A, _B, _C, _D>; }
        struct __f_<_A, _B, _C, _D> { using type = _T<_A, _B, _C, _D>; };
      template <class _A, class _B, class _C, class _D, class _E>
          requires requires { typename _T<_A, _B, _C, _D, _E>; }
        struct __f_<_A, _B, _C, _D, _E> { using type = _T<_A, _B, _C, _D, _E>; };
      template <class... _Args>
        using __f = __t<__f_<_Args...>>;
    };

  template <class _T>
    struct __constant {
      template <class...>
        using __f = _T;
    };

  template <class _Fn, class _Continuation = __q<__types>>
    struct __transform {
      template <class... _Args>
        using __f = __minvoke<_Continuation, __minvoke1<_Fn, _Args>...>;
    };

  template <class _Init, class _Fn>
    struct __right_fold {
      template <class...>
        struct __f_ {};
      template <class _State, class _Head, class... _Tail>
          requires __minvocable2<_Fn, _State, _Head>
        struct __f_<_State, _Head, _Tail...>
          : __f_<__minvoke2<_Fn, _State, _Head>, _Tail...>
        {};
      template <class _State>
        struct __f_<_State> {
          using type = _State;
        };
      template <class... _Args>
        using __f = __t<__f_<_Init, _Args...>>;
    };

  template <class _Continuation = __q<__types>>
    struct __concat {
      template <class...>
        struct __f_ {};
      template <class... _As>
          requires (sizeof...(_As) == 0) &&
            __minvocable<_Continuation, _As...>
        struct __f_<_As...> {
          using type = __minvoke<_Continuation>;
        };
      template <template <class...> class _A, class... _As>
          requires __minvocable<_Continuation, _As...>
        struct __f_<_A<_As...>> {
          using type = __minvoke<_Continuation, _As...>;
        };
      template <template <class...> class _A, class... _As,
                template <class...> class _B, class... _Bs,
                class... _Tail>
        struct __f_<_A<_As...>, _B<_Bs...>, _Tail...>
          : __f_<__types<_As..., _Bs...>, _Tail...> {};
      template <template <class...> class _A, class... _As,
                template <class...> class _B, class... _Bs,
                template <class...> class _C, class... _Cs,
                class... _Tail>
        struct __f_<_A<_As...>, _B<_Bs...>, _C<_Cs...>, _Tail...>
          : __f_<__types<_As..., _Bs..., _Cs...>, _Tail...> {};
      template <template <class...> class _A, class... _As,
                template <class...> class _B, class... _Bs,
                template <class...> class _C, class... _Cs,
                template <class...> class _D, class... _Ds,
                class... _Tail>
        struct __f_<_A<_As...>, _B<_Bs...>, _C<_Cs...>, _D<_Ds...>, _Tail...>
          : __f_<__types<_As..., _Bs..., _Cs..., _Ds...>, _Tail...> {};
      template <class... _Args>
        using __f = __t<__f_<_Args...>>;
    };

  template <bool>
    struct __if_ {
      template <class _True, class>
        using __f = _True;
    };
  template <>
    struct __if_<false> {
      template <class, class _False>
        using __f = _False;
    };
  template <class _Pred, class _True, class _False>
    using __if = __minvoke2<__if_<__v<_Pred>>, _True, _False>;
  template <bool _Pred, class _True, class _False>
    using __if_c = __minvoke2<__if_<_Pred>, _True, _False>;

  template <class _Fn>
    struct __curry {
      template <class... _Ts>
        using __f = __minvoke<_Fn, _Ts...>;
    };

  template <class _Fn>
    struct __uncurry : __concat<_Fn> {};

  template <class _Fn, class _List>
    using __mapply =
      __minvoke<__uncurry<_Fn>, _List>;

  struct __mcount {
    template <class... _Ts>
      using __f = integral_constant<size_t, sizeof...(_Ts)>;
  };

  template <class _Fn>
    struct __mcount_if {
      template <class... _Ts>
        using __f =
          integral_constant<size_t, (bool(__minvoke1<_Fn, _Ts>::value) + ...)>;
    };

  template <class _T>
    struct __contains {
      template <class... _Args>
        using __f = __bool<(__v<is_same<_T, _Args>> ||...)>;
    };

  template <class _Continuation = __q<__types>>
    struct __push_back {
      template <class _List, class _Item>
        using __f =
          __mapply<__mbind_back<_Continuation, _Item>, _List>;
    };

  template <class _Continuation = __q<__types>>
    struct __push_back_unique {
      template <class _List, class _Item>
        using __f =
          __mapply<
            __if<
              __mapply<__contains<_Item>, _List>,
              _Continuation,
              __mbind_back<_Continuation, _Item>>,
            _List>;
    };

  template <class _Continuation = __q<__types>>
    struct __munique {
      template <class... _Ts>
        using __f =
          __mapply<
            _Continuation,
            __minvoke<__right_fold<__types<>, __push_back_unique<>>, _Ts...>>;
    };

  template <class...>
    struct __mcompose {};

  template <class _First>
    struct __mcompose<_First> : _First {};

  template <class _Second, class _First>
    struct __mcompose<_Second, _First> {
      template <class... _Args>
        using __f = __minvoke1<_Second, __minvoke<_First, _Args...>>;
    };

  template <class _Last, class _Penultimate, class... _Rest>
    struct __mcompose<_Last, _Penultimate, _Rest...> {
      template <class... _Args>
        using __f =
          __minvoke1<_Last, __minvoke<__mcompose<_Penultimate, _Rest...>, _Args...>>;
    };

  template <class _Old, class _New, class _Continuation = __q<__types>>
    struct __replace {
      template <class... _Args>
        using __f =
          __minvoke<_Continuation, __if<is_same<_Args, _Old>, _New, _Args>...>;
    };

  template <class _Old, class _Continuation = __q<__types>>
    struct __remove {
      template <class... _Args>
        using __f =
          __minvoke<
            __concat<_Continuation>,
            __if<is_same<_Args, _Old>, __types<>, __types<_Args>>...>;
    };

  template <class _Return>
    struct __qf {
      template <class... _Args>
        using __f = _Return(_Args...);
    };

  template <class _T>
    _T&& __declval() noexcept requires true;

  template <class>
    void __declval() noexcept;

  // For copying cvref from one type to another:
  template <class _Member, class _Self>
    _Member _Self::*__memptr(const _Self&);

  template <typename _Self, typename _Member>
    using __member_t = decltype(
      (__declval<_Self>() .* __memptr<_Member>(__declval<_Self>())));

  template <class... _As>
      requires (sizeof...(_As) != 0)
    struct __front;
  template <class _A, class... _As>
    struct __front<_A, _As...> {
      using type = _A;
    };
  template <class... _As>
      requires (sizeof...(_As) == 1)
    using __single_t = __t<__front<_As...>>;
  template <class _Ty>
    struct __single_or {
      template <class... _As>
          requires (sizeof...(_As) <= 1)
        using __f = __t<__front<_As..., _Ty>>;
    };

  template <class _Fun, class... _As>
    concept __callable =
      requires (_Fun&& __fun, _As&&... __as) {
        ((_Fun&&) __fun)((_As&&) __as...);
      };
  template <class _Fun, class... _As>
    concept __nothrow_callable =
      __callable<_Fun, _As...> &&
      requires (_Fun&& __fun, _As&&... __as) {
        { ((_Fun&&) __fun)((_As&&) __as...) } noexcept;
      };
  template <class _Fun, class... _As>
    using __call_result_t = decltype(__declval<_Fun>()(__declval<_As>()...));

  // For working around clang's lack of support for CWG#2369:
  // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#2369
  struct __qcall_result {
    template <class _Fun, class... _As>
      using __f = __call_result_t<_Fun, _As...>;
  };
  template <bool _Enable, class _Fun, class... _As>
    using __call_result_if_t =
      typename __if<__bool<_Enable>, __qcall_result, __>
        ::template __f<_Fun, _As...>;

  // For emplacing non-movable types into optionals:
  template <class _Fn>
      requires is_nothrow_move_constructible_v<_Fn>
    struct __conv {
      _Fn __fn_;
      using type = __call_result_t<_Fn>;
      operator type() && noexcept(__nothrow_callable<_Fn>) {
        return ((_Fn&&) __fn_)();
      }
      type operator()() && noexcept(__nothrow_callable<_Fn>) {
        return ((_Fn&&) __fn_)();
      }
    };
  template <class _Fn>
    __conv(_Fn) -> __conv<_Fn>;

  template <class _T>
    using __cref_t = const remove_reference_t<_T>&;

  template <class _Fn, class _Continuation = __q<__types>>
    struct __mzip_with2 {
      template <class, class>
        struct __f_;
      template <template <class...> class _C, class... _Cs,
                template <class...> class _D, class... _Ds>
          requires requires {
            typename __minvoke<_Continuation, __minvoke2<_Fn, _Cs, _Ds>...>;
          }
        struct __f_<_C<_Cs...>, _D<_Ds...>> {
          using type = __minvoke<_Continuation, __minvoke2<_Fn, _Cs, _Ds>...>;
        };
      template <class _C, class _D>
        using __f = __t<__f_<_C, _D>>;
    };

  template <size_t... _Indices>
    auto __mconvert_indices(index_sequence<_Indices...>)
      -> __types<__index<_Indices>...>;
  template <size_t _N>
    using __mmake_index_sequence =
      decltype(__mconvert_indices(make_index_sequence<_N>{}));
  template <class... _Ts>
    using __mindex_sequence_for =
      __mmake_index_sequence<sizeof...(_Ts)>;
}
/*
 * Copyright (c) NVIDIA
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
#pragma once

#include <__utility.hpp>
#include <concepts.hpp>

#include <functional>

// A std::declval that doesn't instantiate templates:
#define _DECLVAL(...) \
  ((static_cast<__VA_ARGS__(*)()noexcept>(0))())

namespace std {
  // [func.tag_invoke], tag_invoke
  namespace __tag_invoke {
    void tag_invoke();

    // NOT TO SPEC: Don't require tag_invocable to subsume invocable.
    // std::invoke is more expensive at compile time than necessary,
    // and results in diagnostics that are more verbose than necessary.
    template <class _Tag, class... _Args>
      concept tag_invocable =
        requires (_Tag __tag, _Args&&... __args) {
          tag_invoke((_Tag&&) __tag, (_Args&&) __args...);
        };

    // NOT TO SPEC: nothrow_tag_invocable subsumes tag_invocable
    template<class _Tag, class... _Args>
      concept nothrow_tag_invocable =
        tag_invocable<_Tag, _Args...> &&
        requires (_Tag __tag, _Args&&... __args) {
          { tag_invoke((_Tag&&) __tag, (_Args&&) __args...) } noexcept;
        };

    template<class _Tag, class... _Args>
      using tag_invoke_result_t =
        decltype(tag_invoke(__declval<_Tag>(), __declval<_Args>()...));

    template<class _Tag, class... _Args>
      struct tag_invoke_result {};

    template<class _Tag, class... _Args>
        requires tag_invocable<_Tag, _Args...>
      struct tag_invoke_result<_Tag, _Args...> {
        using type = tag_invoke_result_t<_Tag, _Args...>;
      };

    struct __tag {
      template <class _Tag, class... _Args>
          requires tag_invocable<_Tag, _Args...>
        constexpr auto operator()(_Tag __tag, _Args&&... __args) const
          noexcept(nothrow_tag_invocable<_Tag, _Args...>)
          -> tag_invoke_result_t<_Tag, _Args...> {
          return tag_invoke((_Tag&&) __tag, (_Args&&) __args...);
        }
    };
  } // namespace __tag_invoke

  inline constexpr __tag_invoke::__tag tag_invoke {};

  template<auto& _Tag>
    using tag_t = decay_t<decltype(_Tag)>;

  using __tag_invoke::tag_invocable;
  using __tag_invoke::nothrow_tag_invocable;
  using __tag_invoke::tag_invoke_result_t;
  using __tag_invoke::tag_invoke_result;
}
/*
 * Copyright (c) NVIDIA
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
#pragma once

#include <version>

#if __has_include(<concepts>) && __cpp_lib_concepts	>= 202002
#include <concepts>
#else
#include <type_traits>

namespace std {
  // C++20 concepts
  #if defined(__clang__)
  template<class _A, class _B>
    concept same_as = __is_same(_A, _B) && __is_same(_B, _A);
  #elif defined(__GNUC__)
  template<class _A, class _B>
    concept same_as = __is_same_as(_A, _B) && __is_same_as(_B, _A);
  #else
  template<class _A, class _B>
    inline constexpr bool __same_as_v = false;
  template<class _A>
    inline constexpr bool __same_as_v<_A, _A> = true;

  template<class _A, class _B>
    concept same_as = __same_as_v<_A, _B> && __same_as_v<_B, _A>;
  #endif

  template <class T>
    concept integral = std::is_integral_v<T>;

  template<class _A, class _B>
    concept derived_from =
      is_base_of_v<_B, _A> &&
      is_convertible_v<const volatile _A*, const volatile _B*>;

  template<class _From, class _To>
    concept convertible_to =
      is_convertible_v<_From, _To> &&
      requires(_From (&__fun)()) {
        static_cast<_To>(__fun());
      };

  template<class _T>
    concept equality_comparable =
      requires(const remove_reference_t<_T>& __t) {
        { __t == __t } -> convertible_to<bool>;
        { __t != __t } -> convertible_to<bool>;
      };

  template<class _T>
    concept destructible = is_nothrow_destructible_v<_T>;

  template<class _T, class... _As>
    concept constructible_from =
      destructible<_T> && is_constructible_v<_T, _As...>;

  template<class _T>
    concept move_constructible = constructible_from<_T, _T>;

  template<class _T>
    concept copy_constructible =
      move_constructible<_T> &&
      constructible_from<_T, _T const&>;

  template<class _F, class... _As>
    concept invocable = requires {
      typename invoke_result_t<_F, _As...>;
    };
}
#endif

namespace std {
  template<class _T, class _U>
    concept __decays_to =
      same_as<decay_t<_T>, _U>;

  template <class _C>
    concept __class =
      is_class_v<_C> && __decays_to<_C, _C>;

  template <class _T, class... _As>
    concept __one_of =
      (same_as<_T, _As> ||...);

  template <class _T, class... _Us>
    concept __none_of =
      ((!same_as<_T, _Us>) &&...);

  // Not exactly right, but close.
  template <class _T>
    concept __boolean_testable_ =
      convertible_to<_T, bool>;

  template <class _T>
    concept __movable_value =
      move_constructible<decay_t<_T>> &&
      constructible_from<decay_t<_T>, _T>;

  template <class _Trait>
    concept __is_true = _Trait::value;

  template <class, template <class...> class>
    constexpr bool __is_instance_of_ = false;
  template <class... _As, template <class...> class _T>
    constexpr bool __is_instance_of_<_T<_As...>, _T> = true;

  template <class _Ty, template <class...> class _T>
    concept __is_instance_of =
      __is_instance_of_<_Ty, _T>;

  template <class...>
    concept __typename = true;

} // namespace std
/*
 * Copyright (c) NVIDIA
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
#pragma once

#include <concepts.hpp>

#if __has_include(<coroutine>)
#include <coroutine>
namespace __coro = std;
#else
#include <experimental/coroutine>
namespace __coro = std::experimental;
#endif

namespace std {
  // Defined some concepts and utilities for working with awaitables
  template <class _Promise, class _Awaiter>
  decltype(auto) __await_suspend(_Awaiter& __await) {
    if constexpr (!same_as<_Promise, void>) {
      return __await.await_suspend(__coro::coroutine_handle<_Promise>{});
    }
  }

  template <class _T>
    concept __await_suspend_result =
      __one_of<_T, void, bool> || __is_instance_of<_T, __coro::coroutine_handle>;

  template <class _Awaiter, class _Promise = void>
    concept __awaiter =
      requires (_Awaiter& __await) {
        __await.await_ready() ? 1 : 0;
        {std::__await_suspend<_Promise>(__await)} -> __await_suspend_result;
        __await.await_resume();
      };

  template <class _Awaitable>
  decltype(auto) __get_awaiter(_Awaitable&& __await, void*) {
    if constexpr (requires { ((_Awaitable&&) __await).operator co_await(); }) {
      return ((_Awaitable&&) __await).operator co_await();
    } else if constexpr (requires { operator co_await((_Awaitable&&) __await); }) {
      return operator co_await((_Awaitable&&) __await);
    } else {
      return (_Awaitable&&) __await;
    }
  }

  template <class _Awaitable, class _Promise>
  decltype(auto) __get_awaiter(_Awaitable&& __await, _Promise* __promise)
      requires requires { __promise->await_transform((_Awaitable&&) __await);} {
    if constexpr (requires { __promise->await_transform((_Awaitable&&) __await).operator co_await(); }) {
      return __promise->await_transform((_Awaitable&&) __await).operator co_await();
    } else if constexpr (requires { operator co_await(__promise->await_transform((_Awaitable&&) __await)); }) {
      return operator co_await(__promise->await_transform((_Awaitable&&) __await));
    } else {
      return __promise->await_transform((_Awaitable&&) __await);
    }
  }

  template <class _Awaitable, class _Promise = void>
    concept __awaitable =
      requires (_Awaitable&& __await, _Promise* __promise) {
        {std::__get_awaiter((_Awaitable&&) __await, __promise)} -> __awaiter<_Promise>;
      };

  template <class _T>
    _T& __as_lvalue(_T&&);

  template <class _Awaitable, class _Promise = void>
      requires __awaitable<_Awaitable, _Promise>
    using __await_result_t = decltype(std::__as_lvalue(
        std::__get_awaiter(declval<_Awaitable>(), (_Promise*) nullptr)).await_resume());
}
/*
 * Copyright (c) Facebook
 * Copyright (c) NVIDIA
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
#pragma once

#include <version>
#include <cassert>
#include <cstdint>
#include <utility>
#include <type_traits>
#include <atomic>
#include <thread>

#if __has_include(<stop_token>) && __cpp_lib_jthread >= 201911
#include <stop_token>
#endif

namespace std {
  // [stoptoken.inplace], class in_place_stop_token
  class in_place_stop_token;

  // [stopsource.inplace], class in_place_stop_source
  class in_place_stop_source;

  // [stopcallback.inplace], class template in_place_stop_callback
  template <class _Callback>
    class in_place_stop_callback;

  namespace __detail {
    struct __in_place_stop_callback_base {
      void __execute() noexcept {
        this->__execute_(this);
      }

     protected:
      using __execute_fn_t = void(__in_place_stop_callback_base*) noexcept;
      explicit __in_place_stop_callback_base(const in_place_stop_source* __source, __execute_fn_t* __execute) noexcept
        : __source_(__source), __execute_(__execute) {}

      void __register_callback_() noexcept;

      friend in_place_stop_source;

      const in_place_stop_source* __source_;
      __execute_fn_t* __execute_;
      __in_place_stop_callback_base* __next_ = nullptr;
      __in_place_stop_callback_base** __prev_ptr_ = nullptr;
      bool* __removed_during_callback_ = nullptr;
      atomic<bool> __callback_completed_{false};
    };

    struct __spin_wait {
      __spin_wait() noexcept = default;

      void __wait() noexcept {
        if (__count_++ < __yield_threshold_) {
          // TODO: _mm_pause();
        } else {
          if (__count_ == 0)
            __count_ = __yield_threshold_;
          this_thread::yield();
        }
      }

     private:
      static constexpr uint32_t __yield_threshold_ = 20;
      uint32_t __count_ = 0;
    };

    template<template <class> class>
      struct __check_type_alias_exists;
  }

  // [stoptoken.never], class never_stop_token
  struct never_stop_token {
   private:
    struct __callback_type {
      explicit __callback_type(never_stop_token, auto&&) noexcept
      {}
    };
   public:
    template <class>
      using callback_type = __callback_type;
    static constexpr bool stop_requested() noexcept {
      return false;
    }
    static constexpr bool stop_possible() noexcept {
      return false;
    }
    bool operator==(const never_stop_token&) const noexcept = default;
  };

  template <class _Callback>
    class in_place_stop_callback;

  // [stopsource.inplace], class in_place_stop_source
  class in_place_stop_source {
   public:
    in_place_stop_source() noexcept = default;
    ~in_place_stop_source();
    in_place_stop_source(in_place_stop_source&&) = delete;

    in_place_stop_token get_token() const noexcept;

    bool request_stop() noexcept;
    bool stop_requested() const noexcept {
      return (__state_.load(memory_order_acquire) & __stop_requested_flag_) != 0;
    }

   private:
    friend in_place_stop_token;
    friend __detail::__in_place_stop_callback_base;
    template <class>
      friend class in_place_stop_callback;

    uint8_t __lock_() const noexcept;
    void __unlock_(uint8_t) const noexcept;

    bool __try_lock_unless_stop_requested_(bool) const noexcept;

    bool __try_add_callback_(__detail::__in_place_stop_callback_base*) const noexcept;

    void __remove_callback_(__detail::__in_place_stop_callback_base*) const noexcept;

    static constexpr uint8_t __stop_requested_flag_ = 1;
    static constexpr uint8_t __locked_flag_ = 2;

    atomic<uint8_t> __state_{0};
    __detail::__in_place_stop_callback_base* __callbacks_ = nullptr;
    thread::id __notifying_thread_;
  };

  // [stoptoken.inplace], class in_place_stop_token
  class in_place_stop_token {
   public:
    template <class _Fun>
      using callback_type = in_place_stop_callback<_Fun>;

    in_place_stop_token() noexcept : __source_(nullptr) {}

    in_place_stop_token(const in_place_stop_token& __other) noexcept = default;

    in_place_stop_token(in_place_stop_token&& __other) noexcept
      : __source_(std::exchange(__other.__source_, {})) {}

    in_place_stop_token& operator=(const in_place_stop_token& __other) noexcept = default;

    in_place_stop_token& operator=(in_place_stop_token&& __other) noexcept {
      __source_ = std::exchange(__other.__source_, nullptr);
      return *this;
    }

    bool stop_requested() const noexcept {
      return __source_ != nullptr && __source_->stop_requested();
    }

    bool stop_possible() const noexcept {
      return __source_ != nullptr;
    }

    void swap(in_place_stop_token& __other) noexcept {
      std::swap(__source_, __other.__source_);
    }

    bool operator==(const in_place_stop_token&) const noexcept = default;

   private:
    friend in_place_stop_source;
    template <class>
      friend class in_place_stop_callback;

    explicit in_place_stop_token(const in_place_stop_source* __source) noexcept
      : __source_(__source) {}

    const in_place_stop_source* __source_;
  };

  inline in_place_stop_token in_place_stop_source::get_token() const noexcept {
    return in_place_stop_token{this};
  }

  // [stopcallback.inplace], class template in_place_stop_callback
  template <class _Fun>
    class in_place_stop_callback : __detail::__in_place_stop_callback_base {
     public:
      template <class _Fun2>
        requires constructible_from<_Fun, _Fun2>
      explicit in_place_stop_callback(in_place_stop_token __token, _Fun2&& __fun)
          noexcept(is_nothrow_constructible_v<_Fun, _Fun2>)
        : __detail::__in_place_stop_callback_base(__token.__source_, &in_place_stop_callback::__execute_impl_)
        , __fun_((_Fun2&&) __fun) {
        __register_callback_();
      }

      ~in_place_stop_callback() {
        if (__source_ != nullptr)
          __source_->__remove_callback_(this);
      }

     private:
      static void __execute_impl_(__detail::__in_place_stop_callback_base* cb) noexcept {
        std::move(static_cast<in_place_stop_callback*>(cb)->__fun_)();
      }

      [[no_unique_address]] _Fun __fun_;
    };

  namespace __detail {
    inline void __in_place_stop_callback_base::__register_callback_() noexcept {
      if (__source_ != nullptr) {
        if (!__source_->__try_add_callback_(this)) {
          __source_ = nullptr;
          // Callback not registered because stop_requested() was true.
          // Execute inline here.
          __execute();
        }
      }
    }
  }

  inline in_place_stop_source::~in_place_stop_source() {
    assert((__state_.load(memory_order_relaxed) & __locked_flag_) == 0);
    assert(__callbacks_ == nullptr);
  }

  inline bool in_place_stop_source::request_stop() noexcept {
    if (!__try_lock_unless_stop_requested_(true))
      return true;

    __notifying_thread_ = this_thread::get_id();

    // We are responsible for executing callbacks.
    while (__callbacks_ != nullptr) {
      auto* __callbk = __callbacks_;
      __callbk->__prev_ptr_ = nullptr;
      __callbacks_ = __callbk->__next_;
      if (__callbacks_ != nullptr)
        __callbacks_->__prev_ptr_ = &__callbacks_;

      __state_.store(__stop_requested_flag_, memory_order_release);

      bool removed_during_callback = false;
      __callbk->__removed_during_callback_ = &removed_during_callback;

      __callbk->__execute();

      if (!removed_during_callback) {
        __callbk->__removed_during_callback_ = nullptr;
        __callbk->__callback_completed_.store(true, memory_order_release);
      }

      __lock_();
    }

    __state_.store(__stop_requested_flag_, memory_order_release);
    return false;
  }

  inline uint8_t in_place_stop_source::__lock_() const noexcept {
    __detail::__spin_wait __spin;
    auto __old_state = __state_.load(memory_order_relaxed);
    do {
      while ((__old_state & __locked_flag_) != 0) {
        __spin.__wait();
        __old_state = __state_.load(memory_order_relaxed);
      }
    } while (!const_cast<atomic<uint8_t>&>(__state_).compare_exchange_weak(
        __old_state,
        __old_state | __locked_flag_,
        memory_order_acquire,
        memory_order_relaxed));

    return __old_state;
  }

  inline void in_place_stop_source::__unlock_(uint8_t __old_state) const noexcept {
    (void)const_cast<atomic<uint8_t>&>(__state_).store(__old_state, memory_order_release);
  }

  inline bool in_place_stop_source::__try_lock_unless_stop_requested_(
      bool __set_stop_requested) const noexcept {
    __detail::__spin_wait __spin;
    auto __old_state = __state_.load(memory_order_relaxed);
    do {
      while (true) {
        if ((__old_state & __stop_requested_flag_) != 0) {
          // Stop already requested.
          return false;
        } else if (__old_state == 0) {
          break;
        } else {
          __spin.__wait();
          __old_state = __state_.load(memory_order_relaxed);
        }
      }
    } while (!const_cast<atomic<uint8_t>&>(__state_).compare_exchange_weak(
        __old_state,
        __set_stop_requested ? (__locked_flag_ | __stop_requested_flag_) : __locked_flag_,
        memory_order_acq_rel,
        memory_order_relaxed));

    // Lock acquired successfully
    return true;
  }

  inline bool in_place_stop_source::__try_add_callback_(
      __detail::__in_place_stop_callback_base* __callbk) const noexcept {
    if (!__try_lock_unless_stop_requested_(false)) {
      return false;
    }

    auto& callbacks = *const_cast<__detail::__in_place_stop_callback_base**>(&this->__callbacks_);

    __callbk->__next_ = callbacks;
    __callbk->__prev_ptr_ = &callbacks;
    if (callbacks != nullptr) {
      callbacks->__prev_ptr_ = &__callbk->__next_;
    }
    callbacks = __callbk;

    __unlock_(0);

    return true;
  }

  inline void in_place_stop_source::__remove_callback_(
      __detail::__in_place_stop_callback_base* __callbk) const noexcept {
  auto __old_state = __lock_();

    if (__callbk->__prev_ptr_ != nullptr) {
      // Callback has not been executed yet.
      // Remove from the list.
      *__callbk->__prev_ptr_ = __callbk->__next_;
      if (__callbk->__next_ != nullptr) {
        __callbk->__next_->__prev_ptr_ = __callbk->__prev_ptr_;
      }
      __unlock_(__old_state);
    } else {
      auto notifying_thread = __notifying_thread_;
      __unlock_(__old_state);

      // Callback has either already been executed or is
      // currently executing on another thread.
      if (this_thread::get_id() == notifying_thread) {
        if (__callbk->__removed_during_callback_ != nullptr) {
          *__callbk->__removed_during_callback_ = true;
        }
      } else {
        // Concurrently executing on another thread.
        // Wait until the other thread finishes executing the callback.
        __detail::__spin_wait __spin;
        while (!__callbk->__callback_completed_.load(memory_order_acquire)) {
          __spin.__wait();
        }
      }
    }
  }

  template <class _Token>
    concept stoppable_token =
      copy_constructible<_Token> &&
      move_constructible<_Token> &&
      is_nothrow_copy_constructible_v<_Token> &&
      is_nothrow_move_constructible_v<_Token> &&
      equality_comparable<_Token> &&
      requires (const _Token& __token) {
        { __token.stop_requested() } noexcept -> __boolean_testable_;
        { __token.stop_possible() } noexcept -> __boolean_testable_;
        // workaround ICE in appleclang 13.1
#if !defined(__clang__)
        typename __detail::__check_type_alias_exists<_Token::template callback_type>;
#endif
      };

  template <class _Token, typename _Callback, typename _Initializer = _Callback>
    concept stoppable_token_for =
      stoppable_token<_Token> &&
      __callable<_Callback> &&
      requires {
        typename _Token::template callback_type<_Callback>;
      } &&
      constructible_from<_Callback, _Initializer> &&
      constructible_from<typename _Token::template callback_type<_Callback>, _Token, _Initializer> &&
      constructible_from<typename _Token::template callback_type<_Callback>, _Token&, _Initializer> &&
      constructible_from<typename _Token::template callback_type<_Callback>, const _Token, _Initializer> &&
      constructible_from<typename _Token::template callback_type<_Callback>, const _Token&, _Initializer>;

  template <class _Token>
    concept unstoppable_token =
      stoppable_token<_Token> &&
      requires {
        { _Token::stop_possible() } -> __boolean_testable_;
      } &&
      (!_Token::stop_possible());
} // std
