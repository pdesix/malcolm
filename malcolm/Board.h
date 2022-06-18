#pragma once
#include <map>
#include <set>
#include "Types.h"

namespace malcolm
{
	/// Represents 2D matrix of object of type T.
	/// @tparam T type of data to be stored in matrix
	template<typename T>
	class Board_t
	{
	protected:
		/// Alias for type of data stored in matrix
		using value_t = T;

		/// Alias for set of values stored in matrix
		using value_container_t = value_container_t<value_t>;

	private:
		/// Object of type std::vector<std::vector<value_t>> containing 2D table of value_t-elements
		board_t<value_t> m_board;
	public:
		/// @returns modal value, which means the most occuring value, together with number of occurencies in board
		inline std::pair<value_t, size_t> getModalValueAndCount() const
		{
			std::map<value_t, size_t> occurencies;
			for (const auto& column : m_board)
				for (const auto& value : column)
				{
					if (occurencies.contains(value)) occurencies[value]++;
					else occurencies[value] = 1;
				}
			value_t modal_value;
			size_t max_occurencies_count{ 0u };
			for (const auto& [value, count] : occurencies)
			{
				if (count > max_occurencies_count && count < m_board.size())
				{
					modal_value = value;
					max_occurencies_count = count;
				}
			}
			return { modal_value, max_occurencies_count };
		}

		/// @returns size of board as pair in form (x-size, y-size)
		inline std::pair<size_t, size_t> getSize() const { return { m_board.size(), m_board.size() == 0 ? 0 : m_board[0].size() }; };

		/// @returns constant reference to raw 2d-vector of value_t-objects
		inline const board_t<value_t>& getRawBoardConstReference() const { return m_board; };

		/// @returns non-const reference to raw 2d-vector of value_t-objects
		inline board_t<value_t>& getRawBoardRef() { return m_board; };

		/// Method to append column at the end of board
		/// @param column vector of values to append
		inline void appendColumn(const std::vector<value_t>& column) { m_board.push_back(column); };

		/// @returns vector of points that are centers of 3x3 boards obtained by dividing 9x9 board into 9 equal parts
		inline static std::vector<size_2d_t> centersFor9x9Board() { return std::vector<size_2d_t>{ {1,1},{4,1},{7,1},{1,4},{4,4},{7,4},{1,7}, {4,7},{7,7} }; }

		/// @returns reference to x-th column
		inline column_t<value_t>& operator[](size_t x) { return m_board[x]; }

		/// @returns reference to object stored at given position
		inline value_t& operator[](const pos_t& coords) { return m_board[coords.first][coords.second]; };

		/// @returns copy of object stored at given position
		inline value_t operator[](const pos_t& coords) const { return m_board[coords.first][coords.second]; };

		/// Searches through whole board to find object for which predicate is satisfied
		/// @param		predicate		function object used to filter interesting value_t objects from the board
		/// @returns							position of first value_t met for which predicate is true, {-1,-1} if there is no such values in board
		pos_t find(std::function<bool(value_t)> predicate) const
		{
			for (auto i{ 0u }; i < m_board.size(); i++)
				for (auto j{ 0u }; j < m_board[i].size(); j++)
					if (predicate(m_board[i][j]))
						return { i, j };
			return { -1,-1 };
		}

		/// Searches through whole board to find all objects for which predicate is satisfied
		/// @param		predicate		function object used to filter interesting value_t objects from the board
		/// @returns					set of pos_t of fields for which predicate is true, {-1,-1} if there is no such values in board
		std::set<pos_t> findAll(std::function<bool(value_t)> predicate) const
		{
			std::set<pos_t> result;
			for (auto i{ 0u }; i < m_board.size(); i++)
				for (auto j{ 0u }; j < m_board[i].size(); j++)
					if (predicate(m_board[i][j]))
						result.insert({ i, j });
			return result;
		}

		/// Searches through whole board to count all objects satysfying predicate
		/// @param		predicate		function object used to filter interesting value_t objects from the board
		/// @returns							number of elements in board for which predicate returns true
		size_t count(std::function<bool(value_t)> predicate) const
		{
			size_t result{ 0u };
			for (const auto& x : m_board)
				for (const auto& y : x)
					result += predicate(y);
			return result;
		}

		/// An overloaded version of find method, used to check if specific element is in board
		inline pos_t find(const value_t& value) const { return find([&](value_t x) -> bool { return x == value; }); }

		/// An overloaded version of count method, used to count all occurencies of specific element
		inline size_t count(const value_t& value) const { return count([&](value_t x) -> bool { return x == value; }); }

		/// Initializes object with values given by std::vector<std::vector<value_t>> object
		/// @param		board		initial state of board
		Board_t(const board_t<value_t>& board)
			: m_board{ board }
		{
		}

		/// Initializes Board_t object to be board of size x times y with default value static_cast<value_t>(0u)
		/// @param	x		width of board
		/// @param	y		height of board
		Board_t(size_t x, size_t y)
			: m_board{ x, static_cast<column_t<value_t>>(std::vector<value_t>(y,static_cast<value_t>(0u))) }
		{
		}

		/// Default constructor
		Board_t() {}

		/// This template provides possibility to get specific rows as objects of different types.
		/// In some cases it is useful to use std::vector as a container, but in other ones it might be more efficient to use std::set instead.
		/// For example if we want to verify if board is valid, then we'll prefer rather std::vector over std::set, because set isn't appropriate if we deal with potential duplicates.
		/// On the other hand std::set is faster when it comes about searching and it's better retrieve rows only as sets while we are solving board.
		/// @tparam	container_t type of container to store values from specified row
		/// @param	row_number	number of row to retrieve
		/// @param	inserter		function object which takes container_t& and const value_t& and inserts the value into the container
		template<typename container_t>
		inline container_t getRow(size_t row_number, std::function<void(container_t&, const value_t&)> inserter = [](container_t& container, value_t& value) { container.insert(value); }) const
		{
			container_t result;
			for (auto i{ 0u }; i < m_board.size(); i++)
				inserter(result, m_board[i][row_number]);
			return result;
		}

		/// @tparam	container_t		type of container to store values from specified column
		/// @param	column_number	number of column to retrieve
		/// @param	inserter		function object which takes container_t& and const value_t& and inserts the value into the container
		/// @see					getRow()
		template<typename container_t>
		inline container_t getColumn(size_t column_number, std::function<void(container_t&, const value_t&)> inserter) const
		{
			container_t result;
			for (auto i{ 0u }; i < m_board[0].size(); i++)
				inserter(result, m_board[column_number][i]);
			return result;
		}

		/// Helper function used to solve or validate board
		/// @param	coords	pair of coordinates of field
		/// @returns		set of values placed in the 3x3 square containing specified coords
		inline value_container_t getNeighborhood(pos_t coords) const
		{
			std::int8_t x = coords.first, y = coords.second;
			if (x % 3 == 0u) x++;
			else if (x % 3 == 2u) x--;
			if (y % 3 == 0u) y++;
			else if (y % 3 == 2u) y--;

			value_container_t result;
			result.insert(m_board[x][y]);
			if (x > 0u) result.insert(m_board[x - 1][y]);
			if (x < m_board.size() - 1u) result.insert(m_board[x + 1][y]);
			if (y > 0u) result.insert(m_board[x][y - 1u]);
			if (y < m_board[x].size() - 1u) result.insert(m_board[x][y + 1]);
			if (x > 0u && y > 0u) result.insert(m_board[x - 1u][y - 1u]);
			if (x < m_board.size() - 1u && y > 0u) result.insert(m_board[x + 1][y - 1]);
			if (x > 0u && y < m_board[x].size() - 1) result.insert(m_board[x - 1][y + 1]);
			if (x < m_board.size() - 1u && y < m_board[x].size() - 1) result.insert(m_board[x + 1][y + 1]);
			return result;
		}

		/// Helper function used to solve
		/// @param		coords	pair of coordinates of field
		/// @returns					Board_t of values placed in the 3x3 square containing specified coords keeping relative layout of fields
		inline Board_t<value_t> copyNeighborhood(pos_t coords) const
		{
			auto [x, y] = pos_t{ coords.first, coords.second };
			if (x % 3 == 0u) x++;
			else if (x % 3 == 2u) x--;
			if (y % 3 == 0u) y++;
			else if (y % 3 == 2u) y--;

			Board_t<T> result;
			for (auto i{ -1 }; i < 2; i++)
				result.appendColumn({ m_board[x + i][y - 1],m_board[x + i][y], m_board[x + i][y + 1] });
			return result;
		}

		/// @returns transposed Board_t object
		inline Board_t<value_t> transposed() const
		{
			Board_t<T> result;
			for (auto i{ 0u }; i < m_board.size(); i++)
				result.appendColumn(m_board[i]);
			return result;
		}

		/// @tparam		container_t		type of container used to store values from the board
		/// @param		inserter			function object defining how to insert field into container, including initial preprocessing of fields
		/// @returns								container filled with values from the board
		template<typename container_t>
		inline container_t toContainer(std::function<void(container_t&, const field_t<value_t>&)> inserter)
		{
			container_t result;
			for (auto i{ 0u }; i < getSize().first; i++)
				for (auto j{ 0u }; j < getSize().second; j++)
					inserter(result, { {i,j},m_board[i][j] });
			return result;
		}
	};

	/// @brief Represents 2D matrix of all possible values, i.e. PossibilitiesBoard_t::operator[](x)::operator[](y) returns all possible values
	/// which can be written into field at position (x,y). For details see Board_t<T>::operator[].
	/// @tparam T values stored at board
	template<typename T>
	using PossibilitiesBoard_t = Board_t<value_container_t<T>>;
}

/// Overloaded output operator of Board_t<T> class
/// @tparam		T					type of data stored in board
/// @param		stream		an output stream to be used
/// @param		m_board		constant reference to an Board_t object
/// @returns						reference to the stream
template<typename T>
inline std::ostream& operator<<(std::ostream& stream, const malcolm::Board_t<T>& m_board)
{
	for (auto i{ 0u }; i < m_board.getSize().first; i++)
	{
		for (auto j{ 0u }; j < m_board.getSize().second; j++)
		{
			auto val{ m_board.getRawBoardConstReference()[j][i] };
			if (val != 0u)
				stream << val << " ";
			else
				stream << "  ";
		}
		stream << "\n";
	}
	return stream;
}