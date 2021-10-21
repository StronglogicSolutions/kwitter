#pragma once

#include "kwitter/client/client.hpp"
#include <nlp.hpp>

namespace kwitter {

inline std::vector<File> GetDefaultFilesArg()
{
  return std::vector<File>{};
}

class Bot
// :
// public ConversationTracker
{

public:
Bot(const std::string& username = "")
: m_client(Client{
    username
  })
{
  if (!m_client.HasAuth()) {
    throw std::invalid_argument{"Client was not able to authenticate"};
  }
}


/**
 * @brief
 *
 * @param   [in]  {Tweet}            Tweet
 * @param   [in]  {std::vector<File>} files
 * @returns [out] {bool}
 */
template <typename T = File>
bool PostTweet(Tweet tweet = Tweet{}, std::vector<T> files = GetDefaultFilesArg()) {
  try
  {
    if constexpr(std::is_same_v<T, File> || std::is_same_v<T, std::string>)
      return m_client.PostTweet(tweet, files);
  }
  catch (const std::exception& e)
  {
    const std::string error_message = e.what();
    log(error_message);
    m_last_error = error_message;
  }
  return false;
}

/**
 * @brief ReplyToTweet
 *
 * @param   [in]  {Tweet}      Tweet
 * @param   [in]  {std::string} message
 * @param   [in]  {bool}        remove_id
 * @returns [out] {bool}
 */
bool ReplyToTweet(Tweet tweet, std::string message = "", bool remove_id = true)
{
  Tweet reply;

  reply = Tweet{};
  // reply.content = message;

  // reply.replying_to_id = std::to_string(tweet.id);
  // reply.content        = MakeMention(tweet) + reply.content;
  // reply.visibility     = tweet.visibility;

  return m_client.PostTweet(reply);
}

Tweet FetchTweet(TwitterStatusClient::TweetID id)
{
  return m_client.FetchTweet(id);
}

std::vector<Tweet> FetchUserTweets(const std::string& user_id, uint8_t max = 10)
{
  return m_client.FetchUserTweets(user_id);
}

std::vector<Tweet> FetchUserTweetsV1(const std::string& username, uint8_t max = 10)
{
  return m_client.FetchUserTweetsV1(username);
}

std::string FetchTweetsByTopicJSON(const std::string& topic, bool prefer_media = false, uint8_t max = 50)
{
  using Tweets = std::vector<Tweet>;
  const auto SortPopularity = [](Tweets& tweets) -> void
  {
    std::sort(tweets.begin(), tweets.end(), [](const Tweet& a, const Tweet& b)
    {
      const auto a_num = (a.favourite_count > a.retweet_count) ?
                            a.favourite_count : a.retweet_count;
      const auto b_num = (b.favourite_count > b.retweet_count) ?
                            b.favourite_count : b.retweet_count;
      return a_num > b_num; }
    );
  };

  const auto SortMedia = [](Tweets& tweets) -> void
  {
    std::sort(tweets.begin(), tweets.end(), [](const Tweet& a, const Tweet& b)
    {
      const bool a_has_media = (a.urls.size());
      const bool b_has_media = (b.urls.size());
      return a_has_media > b_has_media;
    });
  };
  auto tweets = m_client.FetchTweets(topic);

  (prefer_media) ? (SortMedia(tweets)) : (SortPopularity(tweets));

  const Tweets final_tweets = (tweets.size() > max) ?
    Tweets{tweets.begin(), tweets.begin() + max} : tweets;
  return Tweet::TweetsToJSON(final_tweets);
}


const bool SetUser(const std::string& username)
{
  return m_client.SetUser(username);
}

const std::string GetLastError() const
{
  return m_last_error;
}

private:
Client           m_client;
std::string      m_last_error;
};

} // namespace kwitter
