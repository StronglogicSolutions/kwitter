#include "kwitter.test.hpp"


TEST(KWitterTests, DISABLED_Instantiate)
{
  ASSERT_NO_THROW(kwitter::Client{});
}

/**
 * FetchTweet
 */
TEST(KWitterTests, DISABLED_FetchTweet)
{
  kwitter::Bot bot{};
  const std::string TWEET_ID{"1379540071834079237"};

  auto tweet = bot.FetchTweet(TWEET_ID);

  std::cout << tweet.text << std::endl;

  EXPECT_FALSE(tweet.id.empty());
}

/**
 * FetchTimeLine
 */
TEST(KTwitterTests, DISABLED_FetchTimeline)
{
  using Tweet = kwitter::Tweet;
  kwitter::Bot bot{};
  const std::string TEST_USER_ID{"1379410084187504641"};

  std::vector<Tweet> tweets = bot.FetchUserTweets(TEST_USER_ID, 20);

  EXPECT_FALSE(tweets.empty());
}

/**
 * FetchTimeLine V1
 */
TEST(KTwitterTests, DISABLED_FetchTimelineV1)
{
  using Tweet = kwitter::Tweet;
  kwitter::Bot bot{};
  const std::string TEST_USER_NAME{"RBuckshi"};

  std::vector<Tweet> tweets = bot.FetchUserTweetsV1(TEST_USER_NAME, 20);

  EXPECT_FALSE(tweets.empty());
}

TEST(KTwitterTests, DISABLED_PostTweet)
{
  using Tweet = kwitter::Tweet;
  kwitter::Bot bot{};
  Tweet        tweet{};

  tweet.text  = "Test!";
  bool result = bot.PostTweet(tweet);

  EXPECT_TRUE(result);
}

TEST(KTwitterTests, FetchPopularTweetsBySubject)
{
  using Tweet  = kwitter::Tweet;
  using Tweets = std::vector<Tweet>;

  kwitter::Client client{};
  Tweet           trending_tweet;
  Tweets          tweets = client.FetchTweets("Korean");
  int32_t         score{};

  for (const Tweet& tweet : tweets)
  {
    kwitter::log(tweet);
    int32_t popularity_score = (tweet.favourite_count > tweet.retweet_count) ?
                                 tweet.favourite_count : tweet.retweet_count;
    if (popularity_score > score)
    {
      trending_tweet = tweet;
      score = popularity_score;
    }
  }

  kwitter::log("Most popular tweet was:");
  kwitter::log(trending_tweet);
  EXPECT_FALSE(tweets.empty());
}