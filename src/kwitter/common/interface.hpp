#pragma once

#include "types.hpp"

namespace kwitter {
/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ Interfaces ░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class SecureClient {
public:
virtual ~SecureClient() {}
virtual bool    HasAuth() = 0;
virtual Account GetAccount() = 0;
};

class TwitterStatusClient {
public:
using UserID  = std::string;
using TweetID = std::string;

virtual ~TwitterStatusClient() {}
virtual Tweet              FetchTweet(TweetID id) = 0;
virtual std::vector<Tweet> FetchUserTweets(UserID id) = 0;
virtual bool                PostTweet(Tweet tweet) = 0;
};

class TwitterMediaClient {
public:
virtual ~TwitterMediaClient() {}
virtual Media PostMedia(File file) = 0;
virtual bool  PostTweet(Tweet tweet, std::vector<File> media) = 0;
};

// class ConversationTracker {
// public:
// virtual ~ConversationTracker() {}
// // virtual std::vector<Conversation> FindReplies()  = 0;
// // virtual std::vector<Tweet> FindComments() = 0;
// };

} // namespace kwitter
