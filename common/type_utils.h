#pragma once

#include <type_traits>
#include <utility>
#include <variant>

#include "common/message.h"

template <typename T, typename... U>
struct list_index;

template <typename T, typename... U>
constexpr inline size_t list_index_v = list_index<T, U...>::value;

template <typename T, typename U>
struct list_index<T, U> {
  static_assert(std::is_same_v<T, U>, "unknown type");
  static const size_t value = 0;
};

template <typename T, typename U, typename... Us>
struct list_index<T, U, Us...> {
  static const size_t value =
      std::is_same_v<T, U> ? 0 : (1 + list_index_v<T, Us...>);
};

template <typename T, typename... U>
constexpr size_t VariantIndex(std::variant<U...>* = nullptr) {
  return list_index_v<T, U...>;
}

template <typename BodyT>
constexpr size_t BodyIndex() {
  return VariantIndex<BodyT, Body>();
}
