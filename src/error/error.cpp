#include "error.hpp"

std::fstream Error::error_log("error_log.txt", std::ios_base::out | std::ios_base::app);

Error::Error(const char* message) : Error(std::string(message))
{
}

Error::Error(const std::string& message)
	: m_msg(message)
{
	if (!error_log.is_open())
		throw exception("In Error::Error: failed opening error log file.");

	error_log << __DATE__ << " " << __TIME__ << std::endl
		<< "\tFILE\t" << __FILE__ << std::endl
		<< "\tLINE\t" << __LINE__ << std::endl
		<< "\tWHAT\t" << m_msg << std::endl;
}

const char* Error::what() const noexcept
{
	return m_msg.c_str();
}
