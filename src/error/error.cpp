#include "error.hpp"

std::fstream Error::error_log("error_log.txt", std::ios_base::out | std::ios_base::app);

Error::Error(const char* message) : Error(std::string(message))
{
}

Error::Error(const std::string& message) 
	: m_msg(message)
{
	error_log << __DATE__ << " " << __TIME__ << std::endl
		<< "\tFILE\t" << __FILE__ << std::endl
		<< "\tLINE\t" << __LINE__ << std::endl
		<< "\tWHAT\t" << m_msg << std::endl;
}

inline const char* Error::what() const
{
	return m_msg.c_str();
}
