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

const uint32_t TWITTER_CHAR_LIMIT  = 0xDC;
const uint8_t  HEADER_AUTH_INDEX   = 0x00;
const uint8_t  HEADER_ACCEPT_INDEX = 0x01;

const std::vector<std::string> HEADER_NAMES{
"Authorization",
"Accept"
};

const uint8_t  ACCEPT_JSON_INDEX   = 0x00;

const std::vector<std::string> HEADER_VALUES{
"application/json"
};

const uint8_t PARAM_NAME_TWEET_FIELDS_INDEX = 0x00;
const uint8_t PARAM_NAME_USER_FIELDS_INDEX  = 0x01;
const uint8_t PARAM_NAME_MEDIA_FIELDS_INDEX = 0x02;
const uint8_t PARAM_NAME_SCREEN_NAME_INDEX  = 0x03;
const uint8_t PARAM_NAME_STATUS_INDEX       = 0x04;
const uint8_t PARAM_NAME_REPLY_ID_INDEX     = 0x05;
const uint8_t PARAM_NAME_QUERY_INDEX        = 0x06;
const uint8_t PARAM_NAME_COUNT_INDEX        = 0x07;
const uint8_t PARAM_NAME_RESULT_TYPE_INDEX  = 0x08;
const std::vector<std::string> PARAM_NAMES{
  "tweet.fields",
  "user.fields",
  "media.fields",
  "screen_name",
  "status",
  "in_reply_to_status_id",
  "q",
  "count",
  "result_type"
};

const uint8_t PARAM_VALUE_ATTACHMENTS_INDEX         = 0x00;
const uint8_t PARAM_VALUE_AUTHOR_ID_INDEX           = 0x01;
const uint8_t PARAM_VALUE_CREATED_AT_INDEX          = 0x02;
const uint8_t PARAM_VALUE_ENTITIES_INDEX            = 0x03;
const uint8_t PARAM_VALUE_GEO_INDEX                 = 0x04;
const uint8_t PARAM_VALUE_ID_INDEX                  = 0x05;
const uint8_t PARAM_VALUE_IN_REPLY_TO_USER_ID_INDEX = 0x06;
const uint8_t PARAM_VALUE_LANG_INDEX                = 0x07;
const uint8_t PARAM_VALUE_POSSIBLY_SENSITIVE_INDEX  = 0x08;
const uint8_t PARAM_VALUE_REFERENCED_TWEETS_INDEX   = 0x09;
const uint8_t PARAM_VALUE_SOURCE_INDEX              = 0x0A;
const uint8_t PARAM_VALUE_TEXT_INDEX                = 0x0B;
const uint8_t PARAM_VALUE_WITHHELD_INDEX            = 0x0C;
const uint8_t PARAM_VALUE_PROFILEURL_INDEX          = 0x0D;
const uint8_t PARAM_VALUE_NAME_INDEX                = 0x0E;
const uint8_t PARAM_VALUE_USERNAME_INDEX            = 0x0F;
const uint8_t PARAM_VALUE_PREVIMG_URL_INDEX         = 0x10;
const uint8_t PARAM_VALUE_TYPE_INDEX                = 0x11;
const uint8_t PARAM_VALUE_MEDIAKEY_INDEX            = 0x12;
const uint8_t PARAM_VALUE_URL_INDEX                 = 0x13;
const uint8_t PARAM_VALUE_MEDIA_URL_INDEX           = 0x14;
const uint8_t PARAM_VALUE_RECENT_INDEX              = 0x15;

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
  "withheld",
  "profile_image_url",
  "name",
  "username",
  "preview_image_url",
  "type",
  "media_key",
  "url",
  "media_url",
  "favorite_count",
  "retweet_count",
  "followers_count",
  "friends_count",
  "recent"
};
// namespace  {
static const std::string BASE_URL{"https://api.twitter.com"};
static const uint8_t     TWEETS_INDEX    = 0x00;
static const uint8_t     USER_INDEX      = 0x01;
static const uint8_t     TWEETS_V1_INDEX = 0x02;
static const std::vector<std::string> PATH{
  "/2/tweets",
  "/2/users"
};

static const std::vector<std::string> PATH_V1{
  "/1.1/statuses/update.json",
  "/1.1/statuses/user_timeline.json",
  "/1.1/search/tweets.json"
};

} // namespace constants
} // namespace kwitter
