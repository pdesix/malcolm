#pragma once
#include "Types.h"
#include "ISolvingStrategy.h"
#include "Board.h"

namespace malcolm
{
	/// Strategy implementation.
	/// Strategy steps: for each 3x3 subboard and for every value from range 1 to 9 check if the value is already in the subboard.
	/// If value isn't already in subboard then check how many fields in the subboard can be filled with the value without immediate error.
	/// If number of such fields is equal to 1, then we have next move found.
	/// @see ISolvingStrategy_t
	template<typename T>
	class FillSubboardsStrategy_t : public ISolvingStrategy_t<T>
	{
		using value_t = T;
		using field_t = field_t<value_t>;
		using value_container_t = value_container_t<value_t>;

	public:
		inline virtual field_t retrieveNextMove(const PossibilitiesBoard_t<value_t>& possibilities, const Board_t<value_t>& board) const override
		{
			value_container_t target{ 1,2,3,4,5,6,7,8,9 };
			for (const auto& pos : Board_t<size_t>::centersFor9x9Board())
			{
				auto neighborhood{ possibilities.copyNeighborhood(pos) };
				auto local_neighborhood{ board.getNeighborhood(pos) };
				for (const auto& goal : target)
				{
					if (local_neighborhood.contains(goal))
						continue;
					auto has_goal = [goal](const value_container_t& container) { return container.contains(goal); };
					auto count = neighborhood.count(has_goal);
					if (count == 1u)
					{
						auto relative_position{ neighborhood.find(has_goal) };
						pos_t valid_position{ relative_position.first + pos.first - 1, relative_position.second + pos.second - 1 };
						return { valid_position, goal };
					}
				}
			}
			return { {0,0},0 };
		}
	};
}