#pragma once
#include <fstream>
#include <string>
#include "TracingService.h"

/// @brief Implementation of TracingService_t based on std::ofstream 
class FileTracingService_t : public TracingService_t
{
private:
	/// @brief Used to distinct instances of FileTracingService_t class and for file numeration
	static std::size_t _instance_counter;
protected:
	/// @brief handler to file where all traces will be saved
	std::fstream m_file;

	/// @brief path to file where all traces will be saved
	std::string m_filepath;

	virtual void prepareFirstUsage() override
	{
		m_used = true;
		m_file.open("trc\\fts_malcolm_" + m_filepath, std::ios::out);
	}

public:
	/// @brief constructor for tracing service operating on files
	/// @param filepath path to file where traces should be saved
	FileTracingService_t(const std::string& filepath)
		: TracingService_t{m_file}, m_filepath{ filepath + "_" + std::to_string(_instance_counter++) + ".log" }
	{
	}

	/// @brief default constructor - closing file
	~FileTracingService_t()
	{
		if(m_used)
			m_file.close();
	}
};

std::size_t FileTracingService_t::_instance_counter{ 0u };