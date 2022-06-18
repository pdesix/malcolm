#pragma once
#include "Types.h"
#include "ISolvingStrategy.h"
#include "Board.h"

#include "VectorUtils.h"
#include "Vector2DUtils.h"

namespace malcolm
{
	/// @brief Strategy implementation - bruteforce
	/// @tparam T type of data stored in board
	/// @see ISolvingStrategy_t for interface description
	/// @li Strategy steps: look at every 3x3 subboard and find values that aren't filled into the 3x3 subboard yet.
	/// For every such value we retrieve number of fields where it's possible to insert the value.
	/// If number of fields is exactly 1 then we are 100% sure that this is the right place for the value (this is 
	/// so called TheOnlyPossibilityStrategy_t). In BruteforceStrategy_t we retrieve more than 1 possible field,
	/// and we do as follows: for every possibility we create copy of game and input value into field.
	/// Then we try to process copy-board as it was the regular one, which can lead us to contradition or to
	/// the solution
	template<typename T>
	class BruteforceStrategy_t
	{
		using value_t = T;
		using field_t = field_t<value_t>;
		using field_container_t = value_container_t<field_t>;
		using value_container_t = value_container_t<value_t>;

	public:
		/// @brief gets state of game and tries to find optimal bruteforce movements
		/// @param possibilities
		/// @param board 
		/// @returns list of all moves that have some sense for bruteforce processing
		inline virtual field_container_t retrieveNextMoves(const PossibilitiesBoard_t<value_t>& possibilities, const Board_t<value_t>& board) const // override
		{
			static constexpr size_t max_threeshold_count{ 2 };
			static const value_container_t target{ 1,2,3,4,5,6,7,8,9 };

			field_container_t result;

			/// @brief Counts how many possibilities do we have to put 'goal' by searching whole possibilities board for fields with 'goal' value present
			/// @remark It's used both for 3x3 and 9x9 boards
			/// @param goal value for which we search for
			/// @param possibilities matrix of input possibilities
			/// @returns number of ways to insert goal into the board
			auto possibilities_for_goal = 
				[&](const value_t& goal, const PossibilitiesBoard_t<value_t>& possibilities) 
					-> size_t
				{ 
					return possibilities.count(
						[&](value_container_t container)
							-> bool
						{
							return container.contains(goal);
						}
					);
				};
			
			for (auto& pos : Board_t<value_t>::centersFor9x9Board())
			{
				for (auto& val : target)
				{
					value_container_t neighborhood{ board.getNeighborhood(pos) };
					PossibilitiesBoard_t<value_t> neighborhood_possibilities{ possibilities.copyNeighborhood(pos) };
					if (neighborhood.contains(val))
						continue;

					size_t possibilities_count{ possibilities_for_goal(val, neighborhood_possibilities) };
					if (possibilities_count <= max_threeshold_count && possibilities_count > 1)
					{
						//std::vector<Board_t<value_t>> scenario_collection;
						for (auto& possibility : possibilities.findAll([&](value_container_t value_container)
							{
								return value_container.contains(val);
							}
						))
						{
							if (utils::vector2d::getLocalCenter(possibility).first == pos.first && utils::vector2d::getLocalCenter(possibility).second == pos.second)
							{
								//auto& scenario{ scenario_collection.emplace_back(Board_t<value_t>(board)) };
								//scenario[possibility] = val;
								result.insert({ possibility,val });
							}
						}

					}
				}
			}

			return result;
		}
	};
}