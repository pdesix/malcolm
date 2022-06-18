#pragma once
#include <unordered_set>
#include "Types.h"
#include "Board.h"
#include "VectorUtils.h"
#include "Vector2DUtils.h"
#include "FileTracingService.h"

namespace malcolm
{
	/// @brief provides functionalities to support board solving
	/// @li We will work with 3x3 subboards centered at one of `Board_t<size_t>::centersFor9x9Board()`, which will
	/// be called just subboards for simplicity.
	/// @tparam T type of data stored in board
	template<typename T>
	class SmartCleaner_t
	{
		/// @brief For every subboard we can try to predict some kind of rule. 
		/// @li Let's say this is current board:
		/// 
		///       |     7 | 2 5  
		///   1   | 3   2 | 6    
		/// 4 2   |     8 | 9 1  
		///  - - - - - - - - - - 
		///   7   |   2   |   4 5
		///   4   |     1 |   2 6
		/// 2     | 4     |   9 1
		///  - - - - - - - - - - 
		///     2 |   8   |      
		///     4 |     9 |      
		/// 5     |       |   6 2
		/// 
		/// It's very hard to find any field with two default insertion strategies.
		/// The idea is to simplify possibilities board instead of inserting numbers directly into the board.
		/// If we ask about rows and columns where we can't put number 4 because of other fours, we'll  get answer like that:
		///     A B C | D E F | G H I
		///      - - - - - - - - - - 
		///1.         |   X 7 | 2 5 T
		///2.     1   | 3 X 2 | 6   T
		///3.   4 2   |     8 | 9 1  
		///      - - - - - - - - - - 
		///4.     7   |   2   |   4 5
		///5.     4   |     1 |   2 6
		///6.   2     | 4     |   9 1
		///      - - - - - - - - - - 
		///7.       2 |   8 Y | Z    
		///8.       4 |     9 |      
		///9.   5     |     Y | Z 6 2		
		/// 
		/// 
		///     A B C | D E F | G H I
		///      - - - - - - - - - - 
		///1.         |     7 | 2 5  
		///2.     1   | 3   2 | 6    
		///3.   4 2   |     8 | 9 1  
		///      - - - - - - - - - - 
		///4.     7   |   2   |   4 5
		///5.     4   |     1 |   2 6
		///6.   2     | 4     |   9 1
		///      - - - - - - - - - - 
		///7.       2 | X 8 X |      
		///8.       4 |   X 9 |      
		///9.   5     | X X X |   6 2
		/// 
		/// After movements:
		///     A B C | D E F | G H I
		///      - - - - - - - - - - 
		///1.         |   X 7 | 2 5 T
		///2.     1   | 3 X 2 | 6   T
		///3.   4 2   |     8 | 9 1  
		///      - - - - - - - - - - 
		///4.     7   |   2   |   4 5
		///5.     4   |     1 |   2 6
		///6.   2     | 4     |   9 1
		///      - - - - - - - - - - 
		///7.       2 |   8 Y | Z   9
		///8.       4 | 2   9 |      
		///9.   5     |     Y | Z 6 2		
		/// 
		/// 
		/// Let's look at fields marked with X, Y, Z and T and corresponding subboards:
		/// X: 4 can't be in third row (A3) nor first column (D6)
		/// Y: 4 can't be in second row (C8) nor first column (D6)
		/// Z: 4 can't be in second row (C8) nor second column (H4)
		/// T: 4 can't be in third row nor (A3) second column (H4)
		/// This is all we get by analyzing fours that currently are on the board. But there are also other rules.
		/// We can check if any row or column in subboard is fulfilled (like F1-F3). This allows us to deduce that E9 is
		/// not valid place for number 4. Why? Because every column has to contain each number exactly once.
		/// If F7 and F9 are not equal to 4, then F1-F9 doesn't contain any four which leads to contradition..
		/// This can be used to clear some values from possibility board, which should lead to processing even more complexified problems.

	private:
		/// @brief tracing service for smart cleaner
		FileTracingService_t m_trace;

	protected:
		/// @brief type of data stored in board
		using value_t = T;
		/// @brief set of fields (e.g. list of possible moves)
		using field_container_t = value_container_t<field_t<value_t>>;

		/// @brief represents piece of information we have about placement of number
		enum class relation_type_t
		{
			MUST_BE_IN_ROW,
			MUST_BE_IN_COLUMN,
			MUST_NOT_BE_IN_ROW,
			MUST_NOT_BE_IN_COLUMN
		};

		/// @brief determined condition which has to be met in order to solve game.
		/// If we fix any number, then pair {MUST_BE_IN_ROW,7} is equivalent to say that our number is needed to be in third row
		using rule_t = std::tuple<relation_type_t, size_t>;
		/// @brief set of determined conditions
		using rule_container_t = std::set<rule_t>;

	public:
		/// @brief default constructor responsible for tracing service initialization
		SmartCleaner_t()
			: m_trace{ "smart_cleaner" }
		{
		}

		/// @brief analyzes current state of game and processes the possibility board to revalidate possible insertions
		/// @param board current state of game
		/// @param possibilities_board currently possible insertions
		void smartclean(Board_t<value_t>& board, PossibilitiesBoard_t<value_t>& possibilities_board, value_t goal)
		{
			size_t count{ board.count(goal) };
			if (count == 9u || count == 0u)
				return;
			m_trace << "[9x99] opening smart clean for the goal " << goal << " and game...\n";

			m_trace << "[9x99] SmartClean_t<>::smartclean() call for the following problem...\n";
			for (auto i{ 0u }; i < 9u; i++)
			{
				m_trace << "[9xFF] ";
				for (auto j{ 0u }; j < 9u; j++)
				{
					if (board[{i, j}] == 0)
						m_trace << "   ";
					else
						m_trace << " " << board[{i, j}] << " ";
				}
				m_trace << "\n";
			}
			
			m_trace << "[9x99] current goal <" << goal << "> and count <" << static_cast<std::int32_t>(count) << ">\n";
			field_container_t known_fields{ 
				board.toContainer<field_container_t>(
					[](field_container_t& container, const field_t<value_t>& value)
					{ 
						container.insert(value); 
					}
				)
			};
			std::size_t counter{ 0u };
			for (auto& [pos, val] : known_fields)
			{
				if (val != static_cast<value_t>(0u))
					counter ++;
			}
			m_trace << "[9x99] " << counter << " numbers are known for now...\n";

			/// @brief used to retrieve vector<column_t> from Board_t
			/// @param object Board_t from which columns should be extracted
			/// @param column_numbers set of column indices to be extracted
			auto extract_specified_columns = [](const Board_t<value_t>& object, const std::vector<size_t>& column_numbers)
				-> std::vector<column_t<value_t>>
			{
				std::vector<column_t<value_t>> result;
				for (auto number : column_numbers)
					result.push_back(object.getColumn<column_t<value_t>>(number, [](column_t<value_t>& col, const value_t& val) { col.push_back(val); }));
				return result;
			};

			/// @see extract_specified_columns
			auto extract_specified_rows = [&extract_specified_columns](const Board_t<value_t>& object, const std::vector<size_t>& row_numbers)
				-> std::vector<row_t<value_t>>
			{
				return extract_specified_columns(object.transposed(), row_numbers);
			};

			/// @see extract_specified_columns
			auto extract_columns = [&extract_specified_columns](const Board_t<value_t>& object)
				-> std::vector<column_t<value_t>>
			{
				std::vector<size_t> target;
				for (auto i{ 0u }; i < object.getSize().first; i++)
					target.push_back(static_cast<size_t>(i));
				return extract_specified_columns(object, target);
			};

			/// @see extract_specified_columns
			auto extract_rows = [&extract_columns](const Board_t <value_t>& object)
				-> std::vector<row_t<value_t>>
			{
				return extract_columns(object.transposed());
			};

			/// @brief checks row/column if there are free places and there is no goal number filled
			auto can_goal_be_placed_in = [&](const auto& container)
				-> bool
			{
				return !container.contains(goal) && container.contains(static_cast<value_t>(0u));
			};
			
			m_trace << "[9x99] prepared function objects...\n";
			for (auto& x : board.centersFor9x9Board())
			{
				m_trace << "[9x90] received next local_center: <" << static_cast<std::int32_t>(x.first) << ", " << static_cast<std::int32_t>(x.second) << ">\n";
				Board_t<value_t> neighborhood{ board.copyNeighborhood(x) };
				if (neighborhood.find(goal) != pos_t{-1, -1})
					continue;

				m_trace << "[8x90] retrieving rows and columns...\n";
				// determine in which row/column we should put goal in neighborhood
				std::vector<column_t<value_t>> columns{ extract_columns(neighborhood) };
				std::vector<row_t<value_t>> rows{ extract_rows(neighborhood) };
				std::vector<column_t<value_t>> full_columns{ extract_specified_columns(board, {static_cast<size_t>(static_cast<short>(x.first) - 1), static_cast<size_t>(x.first), static_cast<size_t>(static_cast<short>(x.first) + 1)}) };
				std::vector<row_t<value_t>> full_rows{ extract_specified_rows(board, {static_cast<size_t>(static_cast<short>(x.second) - 1), static_cast<size_t>(x.second), static_cast<size_t>(static_cast<short>(x.second) + 1)}) };

				m_trace << "[8x88] determining local rules... \n";
				rule_container_t determined_local_rules_for_columns;
				for (auto i{ 0u }; i < neighborhood.getSize().first; i++)
				{
					column_t<value_t> full_column{ (extract_specified_columns(board, {static_cast<size_t>(-1 + static_cast<short>(i + x.first))}))[0] };
					if (utils::vector::contains(full_column, goal))
					{
						m_trace << "[8x86] goal must not be in column " << i << " at local 3x3 subboard\n";
						determined_local_rules_for_columns.insert({ relation_type_t::MUST_NOT_BE_IN_COLUMN,i + 1 });
					}
					else if (utils::vector::isFull(columns[i]))
					{
						m_trace << "[8x87] goal must not be in column " << i << " at local 3x3 subboard\n";
						determined_local_rules_for_columns.insert({ relation_type_t::MUST_NOT_BE_IN_COLUMN,i + 1 });
					}
				}

				m_trace << "[8x77] determined <" << determined_local_rules_for_columns.size() << "> rules\n";
				short allowed_column{ -5 };
				if (determined_local_rules_for_columns.size() >= 2u)
				{
					std::set<size_t> forbidden_local_columns;
					for (auto& [relation, col_no] : determined_local_rules_for_columns)
						if (relation == relation_type_t::MUST_NOT_BE_IN_COLUMN)
							forbidden_local_columns.insert(col_no);

					short allowed_column_relative{ -5 };
					for (auto val : { 1,2,3 })
						if (!forbidden_local_columns.contains(val))
							allowed_column_relative = val - 2;

					allowed_column = allowed_column_relative + x.first;
				}
				if (allowed_column > -1)
				{
					for (auto i{ 0u }; i < possibilities_board.getSize().second; i++)
					{
						pos_t current_position{ pos_t{allowed_column, i} };
						if (utils::vector2d::getLocalCenter(current_position).first == x.first && utils::vector2d::getLocalCenter(current_position).second == x.second)
							continue;
						if (possibilities_board[current_position].count(goal) > 0u)
						{
							m_trace << "[3x33] erasing occurencies of <" << goal << "> in possibilities_board[" << static_cast<std::int32_t>(current_position.first) << ", " << static_cast<std::int32_t>(current_position.second) << "]\n";
							possibilities_board[current_position].erase(goal);
						}
					}
				}
			}
		}
	};
}