#pragma once
#include <vector>

namespace utils
{
	/// @brief Provides basic functionalities associated with vectors
	namespace vector
	{
		/// @brief Searchs vector for duplicates.
		/// @param T type of values stored in vector
		/// @param v vector to be checked for duplicates
		/// @param ignored_value is a set of values that are no taken into account during duplicate check (for example empty fields can be denoted by 0 and it's possible we don't care about duplicates of 0)
		/// @returns true if v has any occuring value not specified in ignored_values, false otherwise
		template<typename T>
		bool hasDuplicates(const std::vector<T>& v, const std::set<T>& ignored_values = {static_cast<T>(0u)})
		{
			for (auto i{ 0u }; i < v.size(); i++)
				for (auto j{ i + 1 }; j < v.size(); j++)
					if (v[i] == v[j] && !ignored_values.contains(v[i]))
						return true;
			return false;
		}

		/// @brief erases all occurencies of given element in specified vector
		/// @tparam T type of values stored in vector
		/// @param v vector to process
		/// @param el element to be deleted
		/// @returns number of erased elements, i.e. difference between v.size() before and after function call
		template<typename T>
		std::size_t eraseOccurencies(std::vector<T>& v, const T& el)
		{
			size_t count{ 0u };
			for (auto i{ 0u }; i < v.size(); i++)
			{
				if (v[i] == el)
				{
					v.erase(v.begin() + i, v.begin() + i + 1);
					i--;
					count++;
				}
			}
			return count;
		}

		/// @brief Check if vector contains specified value
		/// @tparam T type of data stored in the vector
		/// @param v vector to search
		/// @param val value to search for
		/// @returns true if val is in v, false otherwise
		template<typename T>
		bool contains(const std::vector<T>& v, const T& val)
		{
			for (const auto& x : v)
				if (x == val)
					return true;
			return false;
		}

		/// @brief checks if vector (for example column or row) is full - has no empty places (denoted by static_cast<T>(0u))
		/// @tparam T type of data stored in the vector 
		/// @param v vector to checks
		/// @returns true if v has no more empty fields, false otherwise
		template<typename T>
		bool isFull(const std::vector<T>& v)
		{
			return !contains(v, static_cast<T>(0u));
		}
	}
}