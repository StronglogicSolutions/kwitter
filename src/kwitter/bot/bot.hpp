#pragma once

#include "kwitter/client/client.hpp"
#include <nlp.hpp>

namespace kwitter {
static std::vector<File> GetDefaultFilesArg()
{
  return std::vector<File>{};
}
//------------------------------------------------------------------
static void SortPopularity(Tweets& tweets)
{
  std::sort(tweets.begin(), tweets.end(), [](const Tweet& a, const Tweet& b)
  {
    const auto a_num = (a.favourite_count > a.retweet_count) ? a.favourite_count : a.retweet_count;
    const auto b_num = (b.favourite_count > b.retweet_count) ? b.favourite_count : b.retweet_count;
    return a_num > b_num;
  });
};
//------------------------------------------------------------------
static void FilterMedia(Tweets& tweets)
{
  tweets.erase(
    std::remove_if(tweets.begin(), tweets.end(), [](const Tweet& tweet) { return !(tweet.has_media()); }),
    tweets.end());
}
//------------------------------------------------------------------
static void FilterIfMedia(Tweets& tweets)
{
  for (const auto& tweet : tweets)
    if (tweet.has_media()) return FilterMedia(tweets);
}
//------------------------------------------------------------------
static void FilterTodayOnly(Tweets& tweets)
{
  tweets.erase(
    std::remove_if(tweets.begin(), tweets.end(), [](const Tweet& t)  { return (is_today(to_time_t(t.created_at))); }),
    tweets.end());
}

//------------------------------------------------------------------
//--------------------------BOT-------------------------------------
//------------------------------------------------------------------
class Bot
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
//------------------------------------------------------------------
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
//------------------------------------------------------------------
std::vector<Tweet> FetchUserTweets(const std::string& user, uint8_t max = 10, bool get_threads = false)
{
  auto is_thread = [](const auto& t) { return !t.conversation_id.empty() && (t.author_id == t.in_reply_to_user_id); };
  auto tweets = m_client.FetchUserTweets(user);
  if (get_threads)
  {
    Tweets threads;
    for (const auto& tweet : tweets)
      if (is_thread(tweet))
      {
        const auto thread = m_client.FetchThread(tweet.conversation_id, user);
        threads.insert(threads.end(), std::make_move_iterator(thread.begin()), std::make_move_iterator(thread.end()));
      }
    tweets.insert(tweets.end(), std::make_move_iterator(threads.begin()), std::make_move_iterator(threads.end()));
  }
  return tweets;
}
//------------------------------------------------------------------
std::vector<Tweet> FetchUserTweetsV1(const std::string& username, uint8_t max = 10)
{
  return m_client.FetchUserTweetsV1(username);
}
//------------------------------------------------------------------
std::string FetchTweetsByUserJSON(const std::string& username, uint8_t max = 10, bool get_threads = false)
{
  return Tweet::TweetsToJSON(FetchUserTweets(username, max, get_threads));
}
//------------------------------------------------------------------
std::string FetchTopTweetByDateJSON(const std::string& username, uint8_t max = 1, bool prefer_media = false)
{
  auto tweets = m_client.FetchUserTweetsV1(username);
  if (prefer_media) FilterIfMedia(tweets);
  SortPopularity(tweets);
  return Tweet::TweetsToJSON(Tweets{tweets.begin(), tweets.begin() + max});
}
//------------------------------------------------------------------
std::string FetchTweetsByTopicJSON(const std::string& topic, bool prefer_media = false, uint8_t max = 50)
{
  auto tweets = m_client.FetchTweets(topic, max, prefer_media);

  if (prefer_media)
    FilterMedia(tweets);
  else
    SortPopularity(tweets);

  const Tweets final_tweets = (tweets.size() > max) ?
    Tweets{tweets.begin(), tweets.begin() + max} : tweets;
  return Tweet::TweetsToJSON(final_tweets);
}
//------------------------------------------------------------------
const bool SetUser(const std::string& username)
{
  return m_client.SetUser(username);
}
//------------------------------------------------------------------
const std::string GetLastError() const
{
  return m_last_error;
}

private:
Client           m_client;
std::string      m_last_error;
};

} // namespace kwitter
