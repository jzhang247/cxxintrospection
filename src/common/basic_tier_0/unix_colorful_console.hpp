#pragma once
#include <fmt/format.h>

namespace rtd::unix_colorful_console {



inline char COLOR_CODE_ESCAPE_FRONT[] = "\033[";
inline char COLOR_CODE_ESCAPE_BACK[] = "m";

inline auto by_color_code(auto cc0) {
	return fmt::format("{}{}{}", COLOR_CODE_ESCAPE_FRONT, cc0, COLOR_CODE_ESCAPE_BACK);
}
inline auto by_color_code(auto cc0, auto cc1) {
	return fmt::format("{}{};{}{}", COLOR_CODE_ESCAPE_FRONT, cc0, cc1, COLOR_CODE_ESCAPE_BACK);
}
inline auto by_color_code(auto cc0, auto cc1, auto cc2) {
	return fmt::format("{}{};{};{}{}", COLOR_CODE_ESCAPE_FRONT, cc0, cc1, cc2, COLOR_CODE_ESCAPE_BACK);
}
inline auto by_color_code(auto cc0, auto cc1, auto cc2, auto cc3) {
	return fmt::format("{}{};{};{};{}{}", COLOR_CODE_ESCAPE_FRONT, cc0, cc1, cc2, cc3, COLOR_CODE_ESCAPE_BACK);
}
inline auto by_color_code(auto cc0, auto cc1, auto cc2, auto cc3, auto cc4) {
	return fmt::format("{}{};{};{};{};{}{}", COLOR_CODE_ESCAPE_FRONT, cc0, cc1, cc2, cc3, cc4, COLOR_CODE_ESCAPE_BACK);
}

inline auto reset() {
	return by_color_code(0);
}
inline auto by_rgb(int r, int g, int b) {
	return by_color_code("38", "2", r, g, b);
}
inline auto by_rgb_f(float r, float g, float b) {
	auto clamp = [](float value) {
		value = std::max(0.0f, std::min(1.0f, value)); // Clamp the value between 0 and 1
		return static_cast<int>(value * 255.0f + 0.5f); // Scale the value to the range 0 to 255 and round
	};
	return by_rgb(clamp(r), clamp(g), clamp(b));
}

struct RGBcolor {
	char8_t r, g, b;
};

inline auto hsbToRgb(double h, double s, double b) -> RGBcolor {
	// Normalize the hue value to be between 0 and 360 degrees
	h = fmod(h, 360.0);
	if (h < 0) {
		h += 360.0;
	}

	// Calculate the RGB values based on the HSB values
	if (s == 0.0) {
		// Achromatic (gray)
		return { static_cast<uint8_t>(b * 255.0), static_cast<uint8_t>(b * 255.0), static_cast<uint8_t>(b * 255.0) };
	} else {
		double c = b * s;
		double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
		double m = b - c;

		switch (static_cast<int>(h / 60.0)) {
		case 0: return { static_cast<uint8_t>((c + m) * 255.0), static_cast<uint8_t>((x + m) * 255.0), static_cast<uint8_t>((m + 0) * 255.0) };
		case 1: return { static_cast<uint8_t>((x + m) * 255.0), static_cast<uint8_t>((c + m) * 255.0), static_cast<uint8_t>((m + 0) * 255.0) };
		case 2: return { static_cast<uint8_t>((m + 0) * 255.0), static_cast<uint8_t>((c + m) * 255.0), static_cast<uint8_t>((x + m) * 255.0) };
		case 3: return { static_cast<uint8_t>((m + 0) * 255.0), static_cast<uint8_t>((x + m) * 255.0), static_cast<uint8_t>((c + m) * 255.0) };
		case 4: return { static_cast<uint8_t>((x + m) * 255.0), static_cast<uint8_t>((m + 0) * 255.0), static_cast<uint8_t>((c + m) * 255.0) };
		case 5: return { static_cast<uint8_t>((c + m) * 255.0), static_cast<uint8_t>((m + 0) * 255.0), static_cast<uint8_t>((x + m) * 255.0) };
		}
	}
	return {};
}

inline auto by_hsb_f(double h, double s, double b) {
	auto rgb = hsbToRgb(h, s, b);
	return by_rgb(rgb.r, rgb.g, rgb.b);
}








}






