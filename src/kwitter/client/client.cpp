#include "client.hpp"

namespace kwitter {

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ Helper FNs ░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

/**
 * @brief Chunk Message
 *
 * @param   [in]  {std::string} message
 * @returns [out] {std::vector<std::string>}
 */
static std::vector<std::string> const ChunkMessage(const std::string& message) {
  using namespace constants;

  const uint32_t MAX_CHUNK_SIZE = TWITTER_CHAR_LIMIT - 6;

  std::vector<std::string>     chunks{};
  const std::string::size_type message_size = message.size();
  const std::string::size_type num_of_chunks{message.size() / MAX_CHUNK_SIZE + 1};
  uint32_t                     chunk_index{1};
  std::string::size_type       bytes_chunked{0};

  if (num_of_chunks > 1) {
    chunks.reserve(num_of_chunks);

    while (bytes_chunked < message_size) {
      const std::string::size_type size_to_chunk =
        (bytes_chunked + MAX_CHUNK_SIZE > message_size) ?
          (message_size - bytes_chunked) :
          MAX_CHUNK_SIZE;

      std::string oversized_chunk = message.substr(bytes_chunked, size_to_chunk);

      const std::string::size_type ws_idx = oversized_chunk.find_last_of(" ") + 1;
      const std::string::size_type pd_idx = oversized_chunk.find_last_of(".") + 1;
      const std::string::size_type index  =
        (size_to_chunk > MAX_CHUNK_SIZE) ?
          (ws_idx > pd_idx) ?
            ws_idx : pd_idx
            :
            size_to_chunk;

      chunks.emplace_back(
        index == 0 ?
        oversized_chunk :
        std::string{
          oversized_chunk.substr(0, index) + '\n' +
          std::to_string(chunk_index++)    + '/'  + std::to_string(num_of_chunks) // i/n
        }
      );

      bytes_chunked += index;
    }
  } else {
    chunks.emplace_back(message);
  }

  return chunks;
}


/**
 * @brief Construct a new Client:: Client object
 *
 * @param username
 */
Client::Client(const std::string& username)
: m_authenticator(Authenticator{username})
{}


bool Client::HasAuth() {
  return m_authenticator.IsAuthenticated();
}

/**
 * @brief
 *
 * @param id
 * @return Tweet
 */
Tweet Client::FetchTweet(uint64_t id) {
  using json = nlohmann::json;
  using namespace constants;

  const std::string TWEETS_URL = BASE_URL + PATH.at(TWEETS_INDEX) + "/" + std::to_string(id);

  cpr::Response r = cpr::Get(
    cpr::Url{TWEETS_URL}
  );

  if (r.status_code >= 400) {
    // Error handling
    std::string error_message{
      "Request failed with message: " + r.error.message
    };
  }

  if (!r.text.empty()) {
    return JSONToTweet(json::parse(r.text, nullptr, constants::JSON_PARSE_NO_THROW));
  }

  return Tweet{};
}

/**
 * @brief
 *
 * @param id
 * @return std::vector<Tweet>
 */
std::vector<Tweet> Client::FetchUserTweets(UserID id) {
  // using json = nlohmann::json;
  // using namespace constants;
  // // api/v1/accounts/:id/tweets
  // const std::string URL = m_authenticator.GetBaseURL() + PATH.at(ACCOUNTS_INDEX) + '/' + id + "/tweets";

  // cpr::Response r = cpr::Get(
  //   cpr::Url{URL}
  // );

  // if (!r.text.empty()) {
  //   return JSONToTweets(json::parse(r.text, nullptr, constants::JSON_PARSE_NO_THROW));
  // }

  return std::vector<Tweet>{};
}

/**
 * @brief FetchChildStatuses
 *
 * @param   [in]  {TweetID}              id
 * @returns [out] {std::vector<Tweet>}
 */
std::vector<Tweet> Client::FetchChildTweets(TweetID id) {
  // using json = nlohmann::json;
  // using namespace constants;

  // const std::string URL = STATUS_CONTEXT_URL(m_authenticator.GetBaseURL(), id); // api/v1/accounts/:id/tweets

  // RequestResponse response{cpr::Get(
  //   cpr::Url{URL}
  // )};

  // if (response.error)
  //   throw request_error{response.GetError()};

  // return JSONContextToTweets(response.json());

  return std::vector<Tweet>{};
}

/**
 * @brief PostMedia
 *
 * @param   [in]  {File}  file
 * @returns [out] {Media}
 */
Media Client::PostMedia(File file) {
  using namespace constants;

  // const std::string URL{m_authenticator.GetBaseURL() + PATH.at(MEDIA_INDEX)};

  // cpr::Response r = cpr::Post(
  //   cpr::Url{URL},
  //   cpr::Header{
  //     {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
  //   },
  //   file.multiformdata(),
  //   cpr::VerifySsl{m_authenticator.verify_ssl()}
  // );

  // if (r.status_code < 400) {
  //   return ParseMediaFromJSON(json::parse(r.text, nullptr, constants::JSON_PARSE_NO_THROW));
  // }

  return Media{};
}

/**
 * PostTweet
 *
 * TODO: considering returning Tweet object (updated)
 *
 * @param   [in]  {Tweet} tweet
 * @returns [out] {bool}
 */
bool Client::PostTweet(Tweet tweet) {
  using json = nlohmann::json;
  using namespace constants;

  // const std::string              URL = m_authenticator.GetBaseURL() + PATH.at(TWEETS_INDEX);
  // const std::vector<std::string> messages = ChunkMessage(tweet.text);
  // std::string                    reply_to_id = tweet.replying_to_id;

  // for (const auto& message : messages) {
  //   Tweet outgoing_status = Tweet::create_instance_with_message(tweet, message, reply_to_id);

  //   RequestResponse response{cpr::Post(
  //     cpr::Url{URL},
  //     cpr::Header{
  //       {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
  //     },
  //     cpr::Body{outgoing_status.postdata()},
  //     cpr::VerifySsl{m_authenticator.verify_ssl()}
  //   )};

  //   if (response.error)
  //     throw request_error(response.GetError());

  //   Tweet returned_status = JSONToTweet(response.json());

  //   if (returned_status.content.empty())
  //     return false;

  //   m_tweets.emplace_back(std::move(returned_status));

  //   reply_to_id = std::to_string(returned_status.id);
  // }

  return true;
}

/**
 * @brief
 *
 * @param tweet
 * @param files
 * @return true
 * @return false
 */
bool Client::PostTweet(Tweet tweet, std::vector<File> files) {
  // for (auto&& file : files) {
  //   const bool download_file = (file.path.empty() && (!file.url.empty()));
  //   file.path        = download_file ? FetchTemporaryFile(file.url, m_authenticator.verify_ssl()) :
  //                                      file.path;
  //   Media      media = PostMedia(file);
  //   tweet.media.emplace_back(std::move(media));
  //   if (download_file) EraseFile(file.path);
  // }
  return PostTweet(tweet);
}

/**
 * @brief
 *
 * @param tweet
 * @param files
 * @return true
 * @return false
 */
bool Client::PostTweet(Tweet tweet, std::vector<std::string> files) {
  // for (const auto& file : files) {
  //   auto path = FetchTemporaryFile(file, m_authenticator.verify_ssl());
  //     tweet.media.emplace_back(std::move(
  //       PostMedia(path)
  //     ));
  //     EraseFile(path);
  // }
  return PostTweet(tweet);
}
/**
 * @brief
 *
 * @return Account
 */
Account Client::GetAccount() {
  return m_authenticator.GetAccount();
}

/**
 * @brief
 *
 * @return std::string
 */
std::string Client::GetUsername() {
  return m_authenticator.GetUsername();
}

bool Client::SetUser(const std::string& username)
{
  return m_authenticator.SetUser(username);
}

bool Client::HasPostedTweets() const
{
  return (!m_tweets.empty());
}

Tweet Client::GetPostedTweet()
{
  Tweet tweet;

  if (HasPostedTweets())
  {
    tweet = m_tweets.front();
    m_tweets.pop_front();
  }

  return tweet;
}

} // namespace kwitter
