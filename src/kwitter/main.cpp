#include "config.hpp"

/**
 * kwitter Main
 *
 * kwitter Bot class will do one of the following:

 * 1. Post message to Twitter
 *
 * @param   [in]  {int}    argc
 * @param   [in]  {char**} argv
 * @returns [out] {int}
 */
int main(int argc, char** argv)
{
  if (argc < 2)
    throw std::invalid_argument{"kwitter called without arguments"};

  kwitter::Bot           twitter_bot{};
  std::string            std_out{};
  kwitter::ExecuteConfig config = kwitter::ParseRuntimeArguments(argc, argv);

  for (int i = 1; i < argc; i++)
  {
    const std::string arg = kwitter::SanitizeInput(argv[i]);

    if (arg == "search" && !(config.description.empty()))
      std_out += twitter_bot.FetchTweetsByTopicJSON(config.description, config.prefer_media, config.max_results);
    if (arg == "usersearch" && !(config.username.empty()))
      std_out += twitter_bot.FetchTweetsByUserJSON(config.username, config.max_results, config.get_threads);
    if (arg == "usertopsearch" && !(config.username.empty()))
      std_out += twitter_bot.FetchTopTweetByDateJSON(config.username, config.max_results, config.prefer_media);
  }

  if (std_out.empty()) throw std::runtime_error{"Application did not yield output"};

  kwitter::log(std_out);

  return 0;
}
