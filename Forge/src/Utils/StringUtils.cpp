#include "ForgePch.h"
#include "StringUtils.h"

namespace Forge
{

	std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter)
	{
		std::vector<std::string> result;
		size_t begin = 0;
		size_t end = str.find(delimiter, begin);
		while (end != std::string::npos)
		{
			result.push_back(str.substr(begin, end - begin));
			begin = end + delimiter.size();
			end = str.find(delimiter, begin);
		}
		result.push_back(str.substr(begin, end - begin));
		return result;
	}

}
