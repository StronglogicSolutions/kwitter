#pragma once

#include "kwitter/common/auth/auth.hpp"

namespace kwitter {
using TweetID = std::string;
using UserID  = std::string;

class Client
{
public:
  Client(const std::string& username = "");

  bool                      HasAuth           ();
  // Tweet                     FetchTweet        (TweetID id) ;
  std::vector<Tweet>        FetchTweets       (const std::string& subject, uint8_t max = 50, bool media_only=false);
  std::vector<Tweet>        FetchTweetsByUser (const std::string& username, uint8_t max = 10);
  std::vector<Tweet>        FetchUserTweets   (UserID id, uint8_t max = 10);
  std::vector<Tweet>        FetchUserTweetsV1 (UserID id, uint8_t max = 10);
  std::vector<Tweet>        FetchChildTweets  (TweetID id);
  std::string               FetchUserID       (const std::string& name);
  Tweets                    FetchThread       (const std::string& id, const UserID& user_id);
  bool                      PostTweet         (Tweet tweet);
  Media                     PostMedia         (File file);
  Account                   GetAccount        ();
  std::string               GetUsername       ();
  bool                      SetUser           (const std::string& username);
  bool                      HasPostedTweets   () const;
  Tweet                     GetPostedTweet    ();
std::vector<Conversation> FetchConversations();

private:
using json = nlohmann::json;

  Authenticator     m_authenticator;
  std::deque<Tweet> m_tweets;
};

} // namespace kwitter
