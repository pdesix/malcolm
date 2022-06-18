#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>

#include "Board.h"
#include "Vector2DUtils.h"
#include "TracingService.h"

#include "SmartCleaner.h"
#include "ISolvingStrategy.h"
#include "FillSubboardsStrategy.h"
#include "TheOnlyPossibilityStrategy.h"
#include "BruteforceStrategy.h"

namespace malcolm
{
	/// @brief	Provides methods to solve sudoku board with different strategies.
	///	@tparam	T			type of data stored in board
	template<typename T>
	class BoardSolver_t
	{
	protected:
		/// Represents type of data stored in board
		using value_t = T;

		/// Represents set of fields (e.g. list of possible moves)
		using field_container_t = value_container_t<field_t<value_t>>;

		/// Pointer to `ISolvingStrategy_t` implementation.
		using SolvingStrategyPtr_t = std::unique_ptr<ISolvingStrategy_t<value_t>>;

		/// Set of pointers to `ISolvingStrategy_t` implementations
		using SolvingStrategyCollection_t = std::vector<SolvingStrategyPtr_t>;

		/// Alias for `Board_t` with specified data type for code simplification.
		using Board_t = Board_t<value_t>;

	private:
		/// @brief Logging service instance for this class
		FileTracingService_t m_trace;

		/// @brief All possible insertions we can do at the moment
		PossibilitiesBoard_t<value_t> m_current_possibilities;

		/// @brief Set of known strategies managed in constructor of `BoardSolver_t<T>`
		SolvingStrategyCollection_t m_strategies;

		/// @brief Constant reference to externally given board, set by constructor and not changed during lifetime of `BoardSolver_t<T>`
		const Board_t& m_board;

		/// @brief object providing such functionalities as revalidanting boards of possible insertions
		SmartCleaner_t<value_t> m_cleaner;

		/// @param _allow_recursive_usage special parameter which should be set to false if and only if class BoardSolver_t<>
		/// refers to this method recursively. For most cases it shouldn't be even considered!
		/// @returns object of type `Board_t` which is solved version of `m_board` field
		inline Board_t solve(bool _allow_recursive_usage = true)
		{
			Board_t copy{ m_board };

			if (_allow_recursive_usage) smartcleanPossibilitiesBoard(copy);
			if (_allow_recursive_usage) bruteforceStrategyRunner(copy);
			field_container_t queue{ getFieldsToBeFilled(copy) }; // Queue of fields to be filled
			while (queue.size() > 0)
			{
				while (queue.size() > 0)
				{
					while (queue.size() > 0) // Process next fields until queue is empty
					{
						auto [pos, value] = *queue.begin();
						copy[pos] = value;
						m_trace << "[0x10] Inserting value " << static_cast<std::int32_t>(value) << " at position <" << static_cast<std::int32_t>(pos.first) << ", " << static_cast<std::int32_t>(pos.second) << ">\n";
						postprocessInsertion({ pos, value });
						queue.erase(queue.begin());

						// Try to obtain further moves
						queue = getFieldsToBeFilled(copy);
					}
					if (_allow_recursive_usage) smartcleanPossibilitiesBoard(copy);
					queue = getFieldsToBeFilled(copy);
				}
				if (_allow_recursive_usage) bruteforceStrategyRunner(copy);
				smartcleanPossibilitiesBoard(copy);
				queue = getFieldsToBeFilled(copy);
			}
			return copy;
		}

		/// @brief Manages `BruteforceStrategy_t` solving strategy
		/// @li Special handling for `BruteforceStrategy_t` is needed because of potential high resource load during bruteforce solving
		/// @see `BruteforceStrategy_t` for implementation details
		/// @see `ISolvingStrategy_t` for strategy interface description
		/// @param board Current state of game.
		void bruteforceStrategyRunner(Board_t& board)
		{
			m_trace << "[0x5A] bruteforceStrategyRunner_in\n";
			using scenario_t = Board_t;
			using scenario_case_t = std::pair<scenario_t, field_t<value_t>>;
			using scenario_collection_t = std::vector<scenario_case_t>;
			scenario_collection_t scenario_collection;

			BruteforceStrategy_t<value_t> bs;
			auto bruteforce_movements{ bs.retrieveNextMoves(m_current_possibilities, board) };
			if (bruteforce_movements.size() == 0u)
			{
				m_trace << "[0x5A] nothing to do\n";
				return;
			}
			for (auto& [position, value] : bruteforce_movements)
			{
				scenario_t scenario{ board };
				scenario[position] = value;
				scenario_collection.push_back({ scenario, {position, value} });
			}

			for (const auto& [scenario, field] : scenario_collection)
			{
				BoardSolver_t<value_t> scenario_solver(scenario);
				scenario_solver.postprocessInsertion(field);
				Board_t scenario_solved{ scenario_solver.solve(false) };

				auto& validator{ BoardValidator_t<value_t>::getInstance() };

				if (scenario_solved.count(0u) > 0u && !validator.validateBoard(scenario_solved))
				{
					m_trace << "[0x52] Contradition obtained by inserting value " << static_cast<std::int32_t>(field.second) << " at position <" << static_cast<std::int32_t>(field.first.first) << ", " << static_cast<std::int32_t>(field.first.second) << ">\n";
					m_current_possibilities[field.first].erase(field.second);
				}
				else if (scenario_solved.count(0u) == 0u && validator.validateBoard(scenario_solved))
				{
					m_trace << "[0x53] Solution obtained by inserting value " << static_cast<std::int32_t>(field.second) << " at position <" << static_cast<std::int32_t>(field.first.first) << ", " << static_cast<std::int32_t>(field.first.second) << ">\n";
					board[field.first] = field.second;
					return;
				}
				else
				{
					m_trace << "[0x54] Nothing obtained by inserting value " << static_cast<std::int32_t>(field.second) << " at position <" << static_cast<std::int32_t>(field.first.first) << ", " << static_cast<std::int32_t>(field.first.second) << ">\n";
					m_trace << "[*DEV] Consider building assumption-stack and providing bruteforce recursive solutions with stack unwinding and contradition detector with automatic corrections\n";
					//scenario_solver.bruteforceStrategyRunner(scenario_solved);
 				}
			}
			m_trace << "[0x5B] bruteforceStrategyRunner_out\n";
		}

		// In general it should erase some elements from the possibilities board.
		// After it works it should be moved into separate interface similar to ISolvingStrategy_t.
		void smartcleanPossibilitiesBoard(Board_t& board)
		{
			if (board.count(static_cast<value_t>(0u)) == 0u)
			{
				throw std::exception();
			}
			for(auto x : {1,2,3,4,5,6,7,8,9})
				m_cleaner.smartclean(board, m_current_possibilities, x);
		}

		/// @returns 2D board of all possible insertions which can be done at the moment
		PossibilitiesBoard_t<value_t>& retrievePossibilitiesBoard()
		{
			auto [x_size, y_size] = m_board.getSize();
			for (auto i{ 0u }; i < x_size; i++)
				for (auto j{ 0u }; j < y_size; j++)
					m_current_possibilities[{i, j}] = getPossibleValues({ i,j });
			return m_current_possibilities;
		}

		/// Updates possibilities board after successful insertion of new value into the board
		/// @param		field			pair<position, value> of last move in the game
		void postprocessInsertion(const field_t<value_t>& field)
		{
			auto eraseFrompossibilities_board = [&](pos_t coords, const T& value) { m_current_possibilities[coords].erase(value); };
			auto [pos, value] = field;
			for (auto i{ 0u }; i < 9u; i++)
			{
				eraseFrompossibilities_board({ pos.first, i }, value);
				eraseFrompossibilities_board({ i, pos.second }, value);
			}
			auto local_center_position{ utils::vector2d::getLocalCenter(pos) };
			for (auto i{ -1 }; i < 2; i++)
				for (auto j{ -1 }; j < 2; j++)
				{
					eraseFrompossibilities_board({ local_center_position.first + j, local_center_position.second + i }, value);
				}
			m_current_possibilities[pos] = value_container_t<T>();
		}

		/// Finds all values that can be written in field at given coords
		/// @param		coords		position to be checked
		/// @returns						set of all values which can be stored at given position
		value_container_t<value_t> getPossibleValues(pos_t coords) const
		{
			using value_container_t = value_container_t<T>;

			if (m_board[coords] != 0u)
				return value_container_t();

			value_container_t found_values;
			value_container_t column = m_board.getColumn<value_container_t>(coords.first, [](auto& v, const auto& val) { v.insert(val); });
			value_container_t row = m_board.getRow<value_container_t>(coords.second, [](auto& v, const auto& val) { v.insert(val); });
			value_container_t neighborhood = m_board.getNeighborhood(coords);
			found_values.insert(neighborhood.begin(), neighborhood.end());
			found_values.insert(column.begin(), column.end());
			found_values.insert(row.begin(), row.end());

			value_container_t result;
			value_container_t target{ 1,2,3,4,5,6,7,8,9 };
			std::set_difference(target.begin(), target.end(), found_values.begin(), found_values.end(), std::inserter(result, result.begin()));
			return result;
		}

		/// Gets state of game and queries every filling strategy about next field
		/// @param		board		current state of game
		/// @returns				set of fields (positions and values) that are 100% correct and can be filled
		field_container_t getFieldsToBeFilled(const Board_t& board) const
		{
			field_container_t result;
			for (const auto& strategy : m_strategies)
			{
				auto field{ strategy->retrieveNextMove(m_current_possibilities, board) };
				if (field.second != static_cast<T>(0u))
					result.insert(field);
			}
			return result;
		}
	public:
		/// Creates solver object for given Board_t. Prepares possibilities board and strategies vector
		/// @param	current_board		board which will be solved using BoardSolver_t
		BoardSolver_t(const Board_t& current_board)
			: m_board{ current_board }, m_current_possibilities{}, m_trace{ "board_solver" }
		{
			auto size_of_board{ m_board.getSize() };
			auto append_possibilities_to_last_column = [&](const value_container_t<value_t>& v) {
				m_current_possibilities[m_current_possibilities.getSize().first - 1].push_back(v);
			};
			for (auto i{ 0u }; i < size_of_board.first; i++)
			{
				m_current_possibilities.getRawBoardRef().push_back({});
				for (auto j{ 0u }; j < size_of_board.second; j++)
				{
					if (m_board[{ i, j }] == 0u)
						append_possibilities_to_last_column({ 1,2,3,4,5,6,7,8,9 });
					else
						append_possibilities_to_last_column({});
				}
			}
			retrievePossibilitiesBoard();
			m_strategies.push_back(std::make_unique<TheOnlyPossibilityStrategy_t<value_t>>());
			m_strategies.push_back(std::make_unique<FillSubboardsStrategy_t<value_t>>());

			m_trace << "[9xXX] New BoardSolver_t<> instance for the following problem...\n";
			for (auto i{ 0u }; i < 9u; i++)
			{
				m_trace << "[9xFF] ";
				for (auto j{ 0u }; j < 9u; j++)
				{
					if (m_board[{i, j}] == static_cast<value_t>(0u))
						m_trace << "   ";
					else
						m_trace << " " << m_board[{i, j}] << " ";
				}
				m_trace << "\n";
			}
		}

		~BoardSolver_t()
		{
		}

		/// Public method for providing basic functionality of class - solving sudoku board. This method solves
		/// board given as a parameter of BoardSolver_t<T> constructor, referenced by m_board variable
		/// @returns object representing filled board
		inline Board_t getSolved() { return solve(true); }
	};
}

