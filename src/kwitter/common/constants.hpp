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
const uint8_t  HEADER_ACCEPT_INDEX   = 0x01;

const std::vector<std::string> HEADER_NAMES{
"Authorization",
"Accept"
};

const uint8_t  ACCEPT_JSON_INDEX   = 0x00;

const std::vector<std::string> HEADER_VALUES{
"application/json"
};

const uint8_t PARAM_NAME_TWEET_FIELDS_INDEX = 0x00;
const std::vector<std::string> PARAM_NAMES{
  "tweet.fields"
};

const uint8_t PARAM_VALUE_ATTACHMENTS_INDEX = 0x00;
const uint8_t PARAM_VALUE_AUTHOR_ID_INDEX = 0x01;
const uint8_t PARAM_VALUE_CREATED_AT_INDEX = 0x02;
const uint8_t PARAM_VALUE_ENTITIES_INDEX = 0x03;
const uint8_t PARAM_VALUE_GEO_INDEX = 0x04;
const uint8_t PARAM_VALUE_ID_INDEX = 0x05;
const uint8_t PARAM_VALUE_IN_REPLY_TO_USER_ID_INDEX = 0x06;
const uint8_t PARAM_VALUE_LANG_INDEX = 0x07;
const uint8_t PARAM_VALUE_POSSIBLY_SENSITIVE_INDEX = 0x08;
const uint8_t PARAM_VALUE_REFERENCED_TWEETS_INDEX = 0x09;
const uint8_t PARAM_VALUE_SOURCE_INDEX = 0x0A;
const uint8_t PARAM_VALUE_TEXT_INDEX = 0x0B;
const uint8_t PARAM_VALUE_WITHHELD_INDEX = 0x0C;

const std::vector<std::string> PARAM_VALUES{
  "attachments",
  "author_id",
  "created_at",
  "entities",
  "geo",
  "id",
  "in_reply_to_user_id",
  "lang",
  "possibly_sensitive",
  "referenced_tweets",
  "source",
  "text",
  "withheld"
};
// namespace  {
const std::string BASE_URL{"https://api.twitter.com"};
const uint8_t     TWEETS_INDEX = 0x00;
const std::vector<std::string> PATH{
  "/2/tweets"
};

} // namespace constants
} // namespace kwitter
