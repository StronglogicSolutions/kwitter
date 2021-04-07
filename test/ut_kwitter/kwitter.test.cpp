#include "kwitter.test.hpp"


TEST(KWitterTests, DISABLED_Instantiate)
{
  ASSERT_NO_THROW(kwitter::Client{});
}

TEST(KWitterTests, FetchTweet)
{
  kwitter::Bot bot{};
}
