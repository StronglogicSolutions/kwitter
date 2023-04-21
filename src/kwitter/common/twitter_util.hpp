#pragma once

#include "types.hpp"

namespace kwitter {
/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ HelperFns ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

static auto GetConfigReader = [] { return INIReader{get_executable_cwd() + "../" + constants::DEFAULT_CONFIG_PATH}; };

static std::string BuildCaption(const std::string& username, const std::string& id, const nlohmann::json& data)
{
  static const char* SITE_URL{"https://twitter.com/"};
  return kjson::GetJSONStringValue(data, "text") + '\n' + SITE_URL + username + "/status/" + id;
};

static std::vector<Tweet> ParseTweetsFromJSON(const nlohmann::json& data, const std::string& username = "")
{
  std::map<std::string, std::string> media_map;
  std::map<std::string, Tweet>       tweets;
  Tweets                             ret;

  try
  {
    if (!data.is_null() && data.contains("data"))
    for (const auto& item : data["data"])
    {
      const auto id             = kjson::GetJSONStringValue   (item,         "id");
      tweets[id]   = Tweet{};
      Tweet& tweet = tweets[id];
      tweet.id                  = id;
      tweet.author_id           = kjson::GetJSONStringValue   (item,         "author_id");
      tweet.username            = username;
      tweet.text                = BuildCaption(tweet.username, tweet.id, item);
      tweet.created_at          = to_unixtime(kjson::GetJSONStringValue   (item,         "created_at"));
      tweet.in_reply_to_user_id = kjson::GetJSONStringValue   (item,         "in_reply_to_user_id");
      tweet.conversation_id     = kjson::GetJSONStringValue   (item,         "conversation_id");

      if (item.contains("public_metrics"))
      {
        tweet.retweet_count       = kjson::GetJSONValue<int32_t>(item["public_metrics"], "retweet_count");
        tweet.favourite_count     = kjson::GetJSONValue<int32_t>(item["public_metrics"], "like_count");
      }

      if (item.contains("attachments") && item["attachments"].contains("media_keys"))
        for (const auto& key : item["attachments"]["media_keys"])
          media_map[key] = id;

      if (item.contains("entities"))
      {
        for (const auto& entity : item["entities"].items())
        {
          if (entity.key() == "mentions")
            for (const auto& mention : entity.value())
              tweet.mentions.emplace_back(kjson::GetJSONStringValue(mention, "username"));
          else
          if (entity.key() == "hashtags")
            for (const auto& hashtag : entity.value())
              tweet.hashtags.emplace_back(kjson::GetJSONStringValue(hashtag, "tag"));
          else
          if (entity.key() == "media")
            for (const auto& media : entity.value())
              tweet.urls.emplace_back(kjson::GetJSONStringValue(media, "media_url_https"));
        }
      }
      if (item.contains("includes"))
      {
        for (const auto& entity : item["includes"].items())
        {
          if (entity.key() == "media")
            for (const auto& media : entity.value())
              tweet.urls.emplace_back(media["url"].get<std::string>());
        }
      }
    }
    if (data.contains("includes"))
    {

      if (data["includes"].contains("media"))
      {
        for (const auto& media : data["includes"]["media"])
          tweets[media_map.at(media["media_key"])].urls.push_back(media["url"].get<std::string>());
      }
    }
  }
  catch (const std::exception& e)
  {
    kwitter::log("Exception was caught: ", e.what());
  }

  for (auto&& [_, tweet] : tweets)
    ret.push_back(std::move(tweet));

  return ret;
}

static std::vector<Tweet> ParseV1TweetsFromJSON(const nlohmann::json& data)
{
  std::vector<Tweet> tweets{};

  if (!data.is_null() && data.is_array())
  for (const auto& item : data)
  {
    Tweet tweet;
    tweet.id                  = kjson::GetJSONStringValue   (item,         "id_str");
    tweet.author_id           = kjson::GetJSONStringValue   (item["user"], "id_str");
    tweet.username            = kjson::GetJSONStringValue   (item["user"], "screen_name");
    tweet.text                = BuildCaption(tweet.username, tweet.id, item);
    tweet.followers_count     = kjson::GetJSONValue<int32_t>(item["user"], "followers_count");
    tweet.friends_count       = kjson::GetJSONValue<int32_t>(item["user"], "friends_count");
    tweet.conversation_id     = kjson::GetJSONStringValue   (item,         "conversation_id");
    tweet.profile_img_url     = kjson::GetJSONStringValue   (item["user"], "profile_image_url_https");
    tweet.created_at          = to_unixtimev1(kjson::GetJSONStringValue   (item,         "created_at"));
    tweet.retweet_count       = kjson::GetJSONValue<int32_t>(item,         "retweet_count");
    tweet.favourite_count     = kjson::GetJSONValue<int32_t>(item,         "favorite_count");
    tweet.in_reply_to_user_id = kjson::GetJSONStringValue   (item,         "in_reply_to_user_id");

    if (item.contains("entities"))
    {
      for (const auto& entity : item["entities"].items())
      {
        if (entity.key() == "user_mentions")
          for (const auto& mention : entity.value())
            tweet.mentions.emplace_back(kjson::GetJSONStringValue(mention, "screen_name"));
        else
        if (entity.key() == "hashtags")
          for (const auto& hashtag : entity.value())
            tweet.hashtags.emplace_back(kjson::GetJSONStringValue(hashtag, "text"));
        else
        if (entity.key() == "media")
          for (const auto& media : entity.value())
            tweet.urls.emplace_back(kjson::GetJSONStringValue(media, "media_url_https"));

      }
    }
    tweets.emplace_back(tweet);
  }

  return tweets;
}

static bool HasQuotedTweet(const nlohmann::json& data)
{
  return (data.contains("retweeted_status")                  &&
         !data["retweeted_status"].is_null()                 &&
          data["retweeted_status"].contains("quoted_status") &&
         !data["retweeted_status"]["quoted_status"].is_null());
}

static Tweet ParseV1StatusFromJSON(const nlohmann::json& data)
{
  Tweet tweet{};

  if (!data.is_null() && data.is_object())
  {

    tweet.id              = kjson::GetJSONStringValue   (data,         "id_str");
    tweet.author_id       = kjson::GetJSONStringValue   (data["user"], "id_str");
    tweet.username        = kjson::GetJSONStringValue   (data["user"], "screen_name");
    tweet.text            = BuildCaption(tweet.username, tweet.id, data);
    tweet.followers_count = kjson::GetJSONValue<int32_t>(data["user"], "followers_count");
    tweet.friends_count   = kjson::GetJSONValue<int32_t>(data["user"], "friends_count");
    tweet.profile_img_url = kjson::GetJSONStringValue   (data["user"], "profile_image_url_https");
    tweet.created_at      = to_unixtimev1(kjson::GetJSONStringValue   (data,         "created_at"));
    tweet.retweet_count   = kjson::GetJSONValue<int32_t>(data,         "retweet_count");
    tweet.favourite_count = kjson::GetJSONValue<int32_t>(data,         "favourites_count");

    if (data.contains("entities"))
    {
      for (const auto& entity : data["entities"].items())
      {
        if (entity.key() == "user_mentions")
        {
          for (const auto& mention : entity.value())
            tweet.mentions.emplace_back(kjson::GetJSONStringValue(mention, "screen_name"));
        }
        else
        if (entity.key() == "hashtags")
        {
          for (const auto& hashtag : entity.value())
            tweet.hashtags.emplace_back(kjson::GetJSONStringValue(hashtag, "text"));
        }
        else
        if (entity.key() == "media")
        {
          for (const auto& media : entity.value())
            tweet.urls.emplace_back(kjson::GetJSONStringValue(media, "media_url_https"));
        }
      }
    }
    if (data.contains("extended_entities"))
    {
      for (const auto& entity : data["entities"].items())
      {
        if (entity.key() == "user_mentions")
        {
          for (const auto& mention : entity.value())
            tweet.mentions.emplace_back(kjson::GetJSONStringValue(mention, "screen_name"));
        }
        else
        if (entity.key() == "hashtags")
        {
          for (const auto& hashtag : entity.value())
            tweet.hashtags.emplace_back(kjson::GetJSONStringValue(hashtag, "text"));
        }
        else
        if (entity.key() == "media")
        {
          for (const auto& media : entity.value())
            tweet.urls.emplace_back(kjson::GetJSONStringValue(media, "media_url_https"));
        }
      }
    }
  }
  return tweet;
}

static std::vector<Tweet> ParseV1StatusesFromJSON(const nlohmann::json& json_data)
{
  std::vector<Tweet> tweets{};

  if (json_data.contains("statuses"))
  {
    const nlohmann::json& data = json_data["statuses"];

    if (!data.is_null() && data.is_array())
    {
      for (const auto& item : data)
      {
        Tweet tweet = ParseV1StatusFromJSON(item);

        if (!tweet.has_media() && HasQuotedTweet(item))
        {
          const Tweet quoted_tweet = ParseV1StatusFromJSON(item["retweeted_status"]);
          tweet.urls = quoted_tweet.urls;
        }
        tweets.emplace_back(tweet);
      }
    }
  }

  return tweets;
}

static std::string ParseUserIDFromJSON(const nlohmann::json& data)
{
  std::string id;
  if (!data.is_null() && data.is_array() && data.size())
    id = data.front()["id_str"];
  return id;
}
/**
 * @brief
 *
 * @param data
 * @return Tweet
 */
static Tweet JSONToTweet(nlohmann::json data) {
  using namespace kjson;
  Tweet tweet{};
  return tweet;
}

static std::vector<Tweet> JSONToTweets(nlohmann::json data) {
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

static std::vector<Tweet> JSONContextToTweets(nlohmann::json data) {
  std::vector<Tweet> tweets{};

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
  catch (const std::exception& e)
  {
    std::string error = e.what();
    log(error);
  }

  return tweets;
}

static std::string PlatformFromURL(const std::string& url) {
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
