#pragma once

#include "types.hpp"

namespace kwitter {
/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ HelperFns ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

inline std::string GetConfigPath() {
  return get_executable_cwd() + "../" + constants::DEFAULT_CONFIG_PATH;
}

inline INIReader GetConfigReader() {
  return INIReader{GetConfigPath()};
}

inline Media ParseMediaFromJSON(nlohmann::json data) {
  using namespace kjson;

  Media media{};

  return media;
}

inline std::vector<Media> ParseMediaFromJSONArr(nlohmann::json data) {
  std::vector<Media> media_v{};

  if (!data.is_null() && data.is_array()) {
    for (const auto& item : data) {
      media_v.emplace_back(ParseMediaFromJSON(item));
    }
  }

  return media_v;
}

inline Tweet ParseTweetFromJSON(nlohmann::json data)
{
  Tweet       tweet{};
  std::string s = data.dump();

  if (!data.is_null() && data.is_object() && data.contains("data"))
  {
    const nlohmann::json payload = data["data"];
    tweet.id              = kjson::GetJSONStringValue(payload, "id");
    tweet.text            = kjson::GetJSONStringValue(payload, "text");
    tweet.author_id       = kjson::GetJSONStringValue(payload, "author_id");
    tweet.conversation_id = kjson::GetJSONStringValue(payload, "conversation_id");
    tweet.created_at      = kjson::GetJSONStringValue(payload, "created_at");
  }

  return tweet;
}

/**
 * @brief
 *
 * @param data
 * @return Tweet
 */
inline Tweet JSONToTweet(nlohmann::json data) {
  using namespace kjson;
  Tweet tweet{};
  return tweet;
}

inline std::vector<Tweet> JSONToTweets(nlohmann::json data) {
  std::vector<Tweet> tweets{};
  if (!data.is_null() && data.is_array())
    for (const auto& status_data : data)
    {
      Tweet status = JSONToTweet(status_data);
      if (status.is_valid())
        tweets.emplace_back(status);
    }

  return tweets;
}


inline std::vector<Tweet> JSONContextToTweets(nlohmann::json data) {
  std::vector<Tweet> tweets{};
  std::string s = data.dump();

  try {
    for (const auto& context : data) {
      auto new_tweets = JSONToTweets(context);
      tweets.insert(
        tweets.end(),
        std::make_move_iterator(new_tweets.begin()),
        std::make_move_iterator(new_tweets.end())
      );
    }
  }
  catch (const std::exception& e) {
    std::string error = e.what();
    log(error);
  }

  return tweets;
}

// inline std::vector<Conversation> JSONToConversation(nlohmann::json data) {
//   using namespace kjson;

//   std::vector<Conversation> conversations{};

//   if (!data.is_null() && data.is_array()) {
//     for (const auto& item : data) {
//       if (!item.is_null() && item.is_object()) {
//         std::string s = item.dump();
//         conversations.emplace_back(
//           Conversation{
//             .id       = GetJSONStringValue(item, "id"),
//             .unread   = GetJSONBoolValue(item, "unread"),
//             .tweet   = JSONToTweet(item["last_status"]),
//             .accounts = ParseAccountsFromJSON(item["accounts"])
//           }
//         );
//       }
//     }
//   }
//   return conversations;
// }


inline std::string PlatformFromURL(const std::string& url) {
  const std::string::size_type begin_length{8};
  auto begin = url.find_first_of("https://");
  std::string return_s{};
  if (begin != std::string::npos) {
  auto end = url.find_last_of('@');
    if (end != std::string::npos) {
      return_s = url.substr(begin + begin_length, (end - (begin + begin_length - 1)));
    }
  }
  return return_s;
}


} // namespace kwitter
