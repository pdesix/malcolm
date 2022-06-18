#pragma once
#include "Board.h"
#include "VectorUtils.h"

namespace malcolm
{
	/// Singleton class which can be used to check if board is filled in valid way, i.e. no duplicates in any row/column/3x3 subboard.
	/// @tparam T type of data contained in board (in classic sudoku it is number from range 1-9 or empty field)
	template<typename T>
	class BoardValidator_t
	{
	private:
		/// Default constructor. It's private as BoardValidator_t<T> is a singleton
		BoardValidator_t() { }
		/// Deleted copy constructor - singleton pattern
		BoardValidator_t(const BoardValidator_t&) = delete;
		/// Deleted move constructor - singleton pattern
		BoardValidator_t(BoardValidator_t&&) = delete;
	protected:
		/// Represents type of data stored in fields of the board. Given by template parameter
		using value_t = T;
	public:
		/// Singleton implementation - getter for an instance of BoardValidator_t<T>
		static BoardValidator_t& getInstance()
		{
			static BoardValidator_t validator;
			return validator;
		}

		/// Main method used to check if board is filled correctly.
		/// @param		board							reference to Board_t<T> object
		/// @param		additional_checks	function object which is called after internal hardcoded checks. Can be used to verify optional conditions
		/// @returns										true if there is no duplicates in any row, column and 3x3 subboard and additional_checks are satisfyed by current state of the board
		bool validateBoard(const Board_t<value_t>& board, std::function<bool(const Board_t<value_t>&)> additional_checks = [](const Board_t<value_t>& board) { return true; }) const
		{
			using namespace utils::vector;
			using row_t = row_t<value_t>;
			using column_t = column_t<value_t>;

			auto vector_inserter = [](std::vector<value_t>& v, const value_t& val) -> void { v.push_back(val); };
			auto rawBoard{ board.getRawBoardConstReference() };
			auto x{ board.getSize().first }, y{ board.getSize().second };
			bool is_correct{ true };
			for (auto i{ 0u }; i < x; i++) // search for duplicates row-by-row
			{
				auto row = board.getRow<row_t>(i, vector_inserter);
				if (hasDuplicates<value_t>(row))
					return false;
			}
			for (auto i{ 0u }; i < y; i++) // search for duplicates column-by-column
			{
				auto column = board.getColumn<column_t>(i, vector_inserter);
				if (hasDuplicates<value_t>(column))
					return false;
			}
			for (const auto& pos : Board_t<value_t>::centersFor9x9Board()) // retrieve all 3x3 subboards and verify if there is no duplicates in any of subboards
			{
				auto neighborhood = board.copyNeighborhood(pos).toContainer<column_t>([](auto& v, const field_t<value_t>& field) { v.push_back(field.second); });
				if (hasDuplicates<value_t>(neighborhood))
					return false;
			}
			return is_correct && additional_checks(board);
		}
	};
}