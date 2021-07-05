#pragma once
#include <type_traits>
#include <cstdint>

// to underlying
template <typename EnumT>
requires std::is_enum_v<EnumT>
constexpr auto operator+(EnumT c) -> std::underlying_type_t<EnumT> {
  return static_cast<std::underlying_type_t<EnumT>>(c);
}
