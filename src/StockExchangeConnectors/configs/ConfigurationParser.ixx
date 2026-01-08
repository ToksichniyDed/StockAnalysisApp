//
// Created by DED on 22.12.2025.
//
module;

#include <toml++/toml.h>
#include <expected>
#include <filesystem>
#include <concepts>

export module ConfigurationParser;

namespace {
	template <typename T>
	concept configValue = std::is_arithmetic_v<T> || std::same_as<T, std::string> || std::same_as<T, bool>;

	enum class ConfigError {
		KeyNotFound,
		InvalidType,
		FileNotFound,
		ParseError
	};
}

export class ConfigurationParser {
public:
	ConfigurationParser() = default;

	explicit ConfigurationParser(const std::filesystem::path& configPath) {
		if (!std::filesystem::exists(configPath)) {
			_lastError = ConfigError::FileNotFound;
			return;
		}

		try {
			_config = toml::parse_file(configPath.string());
		} catch (const toml::parse_error&) {
			_lastError = ConfigError::ParseError;
		}
	}

	template <configValue T>
	[[nodiscard]] std::expected<T, ConfigError> getValue(const std::string& key) const {
		if (!_config) {
			return std::unexpected(_lastError.value_or(ConfigError::FileNotFound));
		}

		auto node = _config->at_path(key);
		if (!node) {
			return std::unexpected(ConfigError::KeyNotFound);
		}

		auto val = node.value<T>();
		if (!val) {
			return std::unexpected(ConfigError::InvalidType);
		}

		return *val;
	}

	template <configValue T>
	[[nodiscard]] T getValue(const std::string& key, const T& defaultValue) const {
		auto result = getValue<T>(key);
		return result.value_or(defaultValue);
	}

private:
	std::optional<toml::table> _config;
	std::optional<ConfigError> _lastError;
};
