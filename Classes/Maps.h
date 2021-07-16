#pragma once
#include <string>
#include <unordered_map>
#include "GameAttributeWithMaxium.h"

namespace Maps {
	template<class _Ty>
	using StringMap = std::unordered_map<std::string, _Ty>;
	template<class _Ty>
	using StringMultiMap = std::unordered_multimap<std::string, _Ty>;

	using AttrMap = std::unordered_map<std::string, std::unique_ptr<GameAttribute>>;
	using AttrWithMaxiumMap = std::unordered_map<std::string, std::unique_ptr<GameAttributeWithMaxium>>;
}