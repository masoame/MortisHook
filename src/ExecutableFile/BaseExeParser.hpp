#include <MoUtils/utils.hpp>

namespace Mortis::Parser {
class IBaseExe {
  std::vector<char> _cache;

public:
  explicit IBaseExe(std::span<char> staticExeCache);
};
} // namespace Mortis::Parser