#include "BaseExeParser.hpp"

Mortis::Parser::IBaseExe::IBaseExe(std::span<char> staticExeCache) :
	_cache(staticExeCache.cbegin(), staticExeCache.cend())
{

}
