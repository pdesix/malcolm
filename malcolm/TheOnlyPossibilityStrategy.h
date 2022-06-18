#pragma once
#include "Types.h"
#include "ISolvingStrategy.h"
#include "Board.h"

namespace malcolm
{
	/// Strategy implementation.
	/// Strategy steps: look at the current possibilities board and find any field where there is only one possibility for value
	/// @see ISolvingStrategy_t
	template<typename T>
	class TheOnlyPossibilityStrategy_t : public ISolvingStrategy_t<T>
	{
		using value_t = T;
		using field_t = field_t<value_t>;
		using value_container_t = value_container_t<value_t>;

	public:
		inline virtual field_t retrieveNextMove(const PossibilitiesBoard_t<value_t>& possibilities, const Board_t<value_t>& board) const override
		{
			auto has_one_element = [](const value_container_t& val) { return val.size() == 1; };
			if (possibilities.count(has_one_element) > 0)
			{
				pos_t valid_position{ possibilities.find(has_one_element) };
				value_container_t local_neighborhood{ board.getNeighborhood(valid_position) };
				const value_container_t& options = possibilities[valid_position];
				value_t goal{ *(options.begin()) };
				return { valid_position, goal };
			}
			return { {0,0},0 };
		}
	};
}