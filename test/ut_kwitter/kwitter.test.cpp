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
TEST(KTwitterTests, FetchTimelineV1)
{
  using Tweet = kwitter::Tweet;
  kwitter::Bot bot{};
  const std::string TEST_USER_NAME{"ProfKlausSchwab"};

  std::vector<Tweet> tweets = bot.FetchUserTweetsV1(TEST_USER_NAME, 20);

  EXPECT_FALSE(tweets.empty());
}
