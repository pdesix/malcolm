#pragma once
#include "Types.h"
#include "Board.h"

namespace malcolm
{
	/// Abstract class defining interface for various strategies of solving sudoku board
	/// @tparam T type of data stored in a board
	template<typename T>
	class ISolvingStrategy_t
	{
	protected:
		/// Represents type of data stored in a board
		using value_t = T;

	public:
		/// For given state of game search and returns next move compatible with a strategy
		/// @param possibilities object representing possibilities of filling empty fields that we have at the moment
		/// @param board current state of game
		/// @returns next move obtained by the strategy
		virtual field_t<value_t> retrieveNextMove(const PossibilitiesBoard_t<value_t>& possibilities, const Board_t<value_t>& board) const = 0;
	};
}