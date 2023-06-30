#pragma once

#include <type_traits>
#include <utility>
#include <variant>

#include "common/message.h"

/* list_index */

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
  static const size_t value = 1 + list_index_v<T, Us...>;
};

template <typename T, typename... Us>
struct list_index<T, T, Us...> {
  static const size_t value = 0;
};

/* variant_index */

template <typename T, typename VariantT>
struct variant_index;

template <typename T, typename VariantT>
constexpr inline size_t variant_index_v = variant_index<T, VariantT>::value;

template <typename T, typename... U>
struct variant_index<T, std::variant<U...>> {
  static const size_t value = list_index_v<T, U...>;
};

/* body_index */

template <typename BodyT>
struct body_index {
  static const size_t value = variant_index_v<BodyT, Body>;
};

template <typename BodyT>
constexpr inline size_t body_index_v = body_index<BodyT>::value;
