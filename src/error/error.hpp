#ifndef ERROR_H___
#define ERROR_H___

#include <string>
#include <fstream>

class Error : public std::exception
{
private:
	static std::fstream error_log;
	std::string m_msg;
public:
	explicit Error(const char* message);
	explicit Error(const std::string& message);
	inline const char* what() const noexcept override;
	virtual ~Error() = default;
};

#endif
