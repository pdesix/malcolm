#pragma once
#include <iostream>
#include <fstream>

/// @brief Class to manage traces
class TracingService_t
{
protected:
	/// @brief Output stream object where all data will be saved
	std::ostream& m_stream;

	/// @brief Flag used to mark if we put anything into traces
	bool m_used{ false };

	/// @brief Derived classes should provide various types of trace management (synchronization with database, logging to files, etc) and this special method is used to prepare first usage of service
	/// @li It's not enough to provide constructors, e.g. if we work with files we probably won't let empty files to be created. We create files by this function instead of constructor, so we don't
	///	left any empty file at the end of the program.
	virtual void prepareFirstUsage() = 0;
public:
	/// @param stream output stream where all traces should be displayed
	TracingService_t(std::ostream& stream)
		: m_stream{stream}
	{
	}

	/// @brief overloaded output operator redirects objects to private output stream member field
	/// @tparam T type with overloaded operator<< (i.e. std::ostream& operator<<(std::ostream&, const T&) must exist)
	/// @param object information to be saved in traces
	/// @returns reference to *this object for chaining << operator
	template<typename T>
	TracingService_t& operator<<(const T& object)
	{
		if (!m_used)
			prepareFirstUsage();
		m_stream << object;
		return *this;
	}
};

