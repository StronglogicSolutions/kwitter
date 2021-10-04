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
