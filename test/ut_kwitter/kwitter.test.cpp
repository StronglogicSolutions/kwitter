#include "kwitter.test.hpp"


TEST(KWitterTests, DISABLED_Instantiate)
{
  ASSERT_NO_THROW(kwitter::Client{});
}

TEST(KWitterTests, FetchTweet)
{
  kwitter::Bot bot{};
  const std::string TWEET_ID{"1379540071834079237"};

  auto tweet = bot.FetchTweet(TWEET_ID);

  std::cout << tweet.text << std::endl;

  EXPECT_FALSE(tweet.id.empty());
}
