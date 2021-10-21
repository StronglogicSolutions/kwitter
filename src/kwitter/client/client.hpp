#pragma once

#include <deque>

#include "kwitter/common/auth/auth.hpp"
#include "kwitter/common/interface.hpp"

namespace kwitter {
/**
  @class
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░░ kwitter ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class Client : public SecureClient,
               public TwitterStatusClient,
               public TwitterMediaClient
{
public:
Client(const std::string& username = "");

virtual ~Client() override {}

virtual bool                HasAuth() override;
virtual Tweet               FetchTweet(TweetID id) override;
        std::vector<Tweet>  FetchTweets(const std::string& subject);
virtual std::vector<Tweet>  FetchUserTweets(UserID id, uint8_t max = 10) override;
        std::vector<Tweet>  FetchUserTweetsV1(UserID id, uint8_t max = 10);
        std::vector<Tweet>  FetchChildTweets(TweetID id);
virtual bool                PostTweet(Tweet tweet) override;
virtual bool                PostTweet(Tweet tweet, std::vector<File> media) override;
        bool                PostTweet(Tweet tweet, std::vector<std::string> media);
virtual Media               PostMedia(File file) override;
virtual Account             GetAccount() override;
        std::string         GetUsername();
        bool                SetUser(const std::string& username);

        bool                HasPostedTweets() const;
        Tweet               GetPostedTweet();

std::vector<Conversation>   FetchConversations();

private:
using json = nlohmann::json;
Authenticator     m_authenticator;
std::deque<Tweet> m_tweets;
};

} // namespace kwitter
