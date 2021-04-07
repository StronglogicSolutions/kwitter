#pragma once

#include <string>

namespace kwitter {
namespace constants {
const bool        JSON_PARSE_NO_THROW{false};
const std::string DEFAULT_CONFIG_PATH{"config/config.ini"};
const std::string CONFIG_JSON_PATH   {"config/credentials.json"};
const std::string TOKEN_JSON_PATH    {"config/token.json"};
const std::string kwitter_SECTION    {"kwitter"};
const std::string VERIFY_SSL_KEY     {"verify_ssl"};
const std::string USER_CONFIG_KEY    {"user"};
const std::string CREDS_PATH_KEY     {"credentials_path"};
const std::string TOKENS_PATH_KEY    {"token_path"};

const uint32_t TWITTER_CHAR_LIMIT = 0xDC;
const uint8_t  HEADER_AUTH_INDEX   = 0x00;

const std::vector<std::string> HEADER_NAMES{
"Authorization"
};

// namespace  {
const std::string BASE_URL{"https://api.twitter.com"};
const uint8_t     TWEETS_INDEX = 0x00;
const std::vector<std::string> PATH{
  "/2/tweets"
};

} // namespace constants
} // namespace kwitter
