#pragma once

// Debug
#pragma region
#if defined(_DEBUG)
static constexpr bool IS_DEBUG_BUILD = true;
#else
static constexpr bool IS_DEBUG_BUILD = false;
#endif
#pragma endregion

