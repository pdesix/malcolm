#pragma once
#include <vector>
#include <utility>

namespace utils
{
	/// Namespace providing helper functions for dealing with 2d boards.
	namespace vector2d
	{
		/// Gets an pair of coordinates and returns so called "local center".
		/// Local center is nothing else than center of subboard containing point (x,y) after we divide 9x9 board into 9 equal parts.
		/// @param x first coordinate
		/// @param y second coordinate
		/// @returns center of 3x3 subboard containing point (x,y)
		inline std::pair<std::size_t, std::size_t> getLocalCenter(std::size_t x, std::size_t y)
		{
			if (x % 3 == 0u) x++;
			else if (x % 3 == 2u) x--;
			if (y % 3 == 0u) y++;
			else if (y % 3 == 2u) y--;
			return { x,y };
		}

		/// Gets an pair of coordinates and returns so called "local center".
		/// Local center is nothing else than center of subboard containing point (x,y) after we divide 9x9 board into 9 equal parts.
		/// It's a wrapper for @see std::pair<std::size_t, std::size_t> getLocalCenter(std::size_t, std::size_t)
		inline std::pair<std::size_t, std::size_t> getLocalCenter(std::pair<std::size_t, std::size_t> coords)
		{
			return getLocalCenter(coords.first, coords.second);
		}
	}
}