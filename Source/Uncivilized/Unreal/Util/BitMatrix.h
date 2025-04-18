#pragma once

#include <cstdint>

struct BitMatrix16x16 {
	uint16_t data[16];

	inline bool get(uint8_t x, uint8_t y) const {
		return (data[x] >> y) & 1U;
	}

	inline void set(uint8_t x, uint8_t y, bool value) {
		value ? data[x] |= (1U << y) : data[x] &= ~(1U << y);
	}
};

static_assert(sizeof(BitMatrix16x16) == 32, "Invalid BitMatrix size");