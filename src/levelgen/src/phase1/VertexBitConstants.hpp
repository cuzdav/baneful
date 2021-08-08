#pragma once

#include <cstdint>

namespace vertex {

// bit shifting for internal fields
static constexpr std::uint32_t StartBitShift      = 29;
static constexpr std::uint32_t ColorShift         = 24;
static constexpr std::uint32_t Block1Shift        = 20;
static constexpr std::uint32_t Block2Shift        = 16;
static constexpr std::uint32_t Block3Shift        = 12;
static constexpr std::uint32_t Block4Shift        = 8;
static constexpr std::uint32_t Block5Shift        = 4;
static constexpr std::uint32_t Block6Shift        = 0;
static constexpr std::uint32_t MaxBlocksPerVertex = 6;
static constexpr std::uint32_t BitsPerBlock       = 4;
static constexpr std::uint32_t BlockMask          = (1 << BitsPerBlock) - 1;
static constexpr std::uint32_t BitsForColor       = 5;
static constexpr std::uint32_t ColorMask          = (1 << BitsForColor) - 1;
static constexpr std::uint32_t AllBlocksMask =
    (1 << MaxBlocksPerVertex * BitsPerBlock) - 1;

} // namespace vertex
