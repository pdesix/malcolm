#pragma once
#include <utility>
#include <set>
#include <vector>
#include <functional>

// This header provides useful type definitions simplifying code and making maintenance easier.
namespace malcolm
{
	/// Represents an index, used to manipulate game board.
	using size_t = std::uint8_t;
	/// Stores information about size of 2D board.
	using size_2d_t = std::pair<size_t, size_t>;
	/// Used to represent pair of coordinates in format (x,y), where x and y are variables of type size_t.
	using pos_t = std::pair<size_t, size_t>;

	/// This templated type alias is used to represent an idea of field in sudoku game. It's basically some value (of type T)
	/// at specified coordinates (which is first element of pair).
	/// @tparam T type of values stored in board
	template<typename T>
	using field_t = std::pair<pos_t, T>;

	/// Type alias used to represent column of values. It could be realised also as vector<field_t<T>>, but in some places
	/// we don't need exact information about absolute position of a field in board - in those places we use column_t.
	/// For example whole class Board_t is wrapper for template class std::vector<column_t<S>>, where S is data type
	/// stored in single game field (in classical sudoku game it's number from 1...9 or empty field), see below - board_t.
	/// @tparam T type of values stored in board
	template<typename T>
	using column_t = std::vector<T>;

	/// Providing an alias for such methods as extraction of a row from 2d board.
	/// @tparam T type of values stored in board
	template<typename T>
	using row_t = column_t<T>;

	/// Alias for 2D matrix of elements of type T. Class Board_t is a wrapper for objects of this type, providing
	/// useful functionalites like row or column extraction, transposition etc.
	/// @tparam T type of values stored in board
	template<typename T>
	using board_t = std::vector<column_t<T>>;

	/// Container for values, where it's not important how values are distributed or ordered.
	/// Main application of this type is to speed up game solving algorithm by replacing computational complexity
	/// of std::vector<> by faster types like std::set or std::unordered_set.
	/// @tparam T type of values stored in board
	template<typename T>
	using value_container_t = std::set<T>;
}