//
// Created by DED on 25.01.2026.
//

module;

#include <regex>
#include <expected>

export module UrlParser;

import IHttpClient;

export class UrlParser {
public:
    [[nodiscard]] static std::expected<Http::ParsedUrl, Http::ParseUrlError> parse(const std::string& url) {
        std::regex urlRegex(
            R"(^(https?):\/\/([^:\/\s]+)(?::(\d+))?(\/[^?\s]*)?(?:\?([^\s]*))?$)",
            std::regex::icase
        );

        std::smatch match;
        if (!std::regex_match(url, match, urlRegex)) {
            return std::unexpected("Invalid URL format: " + url);
        }

        Http::ParsedUrl parsed;
        parsed.scheme = match[1].str();
        parsed.host = match[2].str();

        if (match[3].matched) {
            parsed.port = match[3].str();
        }

        parsed.path = match[4].matched ? match[4].str() : "/";
        parsed.query = match[5].str();

        return parsed;
    }
};