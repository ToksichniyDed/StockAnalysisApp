//
// Created by DED on 22.12.2025.
//

export module ConfigurationParser;

#include <boost/property_tree/ptree.hpp>;
#include <boost/property_tree/ini_parser.hpp>;
#include <filesystem>
#include <concepts>

namespace {
	template <typename T>
	concept configValue = std::is_arithmetic_v<T> || std::same_as<T, std::string> || std::same_as<T, bool>;
}

export class ConfigurationParser {
public:
	explicit ConfigurationParser(const std::filesystem::path& configPath) {
		if (std::filesystem::exists(configPath)) {
			boost::property_tree::read_ini(configPath.string(), _ptree);
		} else {

		}
	}

	template <configValue T>
	[[nodiscard]] std::optional<T> getValue(const std::string& key) const {
		return _ptree.get<T>(key);
	}

	template <configValue T>
	[[nodiscard]] T getValue(const std::string& key, const T& defaultValue) const {
		return _ptree.get<T>(key, defaultValue);
	}

private:
	boost::property_tree::ptree _ptree;
};
