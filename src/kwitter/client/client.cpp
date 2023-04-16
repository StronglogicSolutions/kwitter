#include "client.hpp"

namespace kwitter {

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ Helper FNs ░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

const std::string GetDefaultFields()
{
  using namespace constants;
  return
  PARAM_VALUES.at(PARAM_VALUE_ATTACHMENTS_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_AUTHOR_ID_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_CONVERSATION_ID_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_CREATED_AT_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_ENTITIES_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_GEO_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_ID_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_IN_REPLY_TO_USER_ID_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_LANG_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_POSSIBLY_SENSITIVE_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_REFERENCED_TWEETS_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_SOURCE_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_TEXT_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_WITHHELD_INDEX);
}

const std::string GetMediaFields()
{
  using namespace constants;
  return
  PARAM_VALUES.at(PARAM_VALUE_PREVIMG_URL_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_TYPE_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_MEDIAKEY_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_URL_INDEX);
}

const std::string GetUserFields()
{
  using namespace constants;
  return
  PARAM_VALUES.at(PARAM_VALUE_PROFILEURL_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_NAME_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_USERNAME_INDEX);
}

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
// Tweet Client::FetchTweet(TweetID id) {
//   using json = nlohmann::json;
//   using namespace constants;

//   const std::string TWEETS_URL = BASE_URL + PATH.at(TWEETS_INDEX) + "/" + id;

//   cpr::Response r = cpr::Get(
//     cpr::Url{TWEETS_URL}
//   );

//   if (r.status_code >= 400) {
//     // Error handling
//     std::string error_message{
//       "Request failed with message: " + r.error.message
//     };
//   }

//   if (!r.text.empty()) {
//     return JSONToTweet(json::parse(r.text, nullptr, constants::JSON_PARSE_NO_THROW));
//   }

//   return Tweet{};
// }

/**
 * @brief
 *
 * @param id
 * @return std::vector<Tweet>
 */
std::vector<Tweet> Client::FetchUserTweets(UserID name, uint8_t max)
{
  using json = nlohmann::json;
  using namespace constants;

  const auto        id  = m_authenticator.GetIDforUser(name);
  const std::string URL = BASE_URL + PATH.at(USER_INDEX) + '/' + id + "/tweets";

  RequestResponse response{cpr::Get(
    cpr::Url{URL},
    cpr::Header{
      {HEADER_NAMES.at(HEADER_ACCEPT_INDEX), HEADER_VALUES.at(ACCEPT_JSON_INDEX)},
      {HEADER_NAMES.at(HEADER_AUTH_INDEX),   m_authenticator.GetBearerAuth()}
    },
    cpr::Parameters{
      {PARAM_NAMES.at(PARAM_NAME_TWEET_FIELDS_INDEX), GetDefaultFields()},
      {PARAM_NAMES.at(PARAM_NAME_USER_FIELDS_INDEX),  GetUserFields()   },
      {PARAM_NAMES.at(PARAM_NAME_MEDIA_FIELDS_INDEX), GetMediaFields()  },
      {"expansions", "attachments.media_keys"},
      {"max_results", std::to_string(max)}
    }
  )};

  if (response.error)
    log(response.GetError());

  return ParseTweetsFromJSON(response.json());
}

std::vector<Tweet> Client::FetchUserTweetsV1(UserID username, uint8_t max)
{
  using json = nlohmann::json;
  using namespace constants;

  const std::string URL = BASE_URL + PATH_V1.at(USER_INDEX);

  RequestResponse response{cpr::Get(
    cpr::Url{URL},
    cpr::Header{
      {HEADER_NAMES.at(HEADER_ACCEPT_INDEX), HEADER_VALUES.at(ACCEPT_JSON_INDEX)},
      {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
    },
    cpr::Parameters{
      {PARAM_NAMES.at(PARAM_NAME_SCREEN_NAME_INDEX), username},
      {"tweet_mode", "extended"},
      {"count",                                      std::to_string(max)}
    }
  )};

  if (response.error)
    log(response.GetError());

  return ParseV1TweetsFromJSON(response.json());
}

Tweets Client::FetchThread(const std::string& id, const UserID& user_id)
{
  using json = nlohmann::json;
  using namespace constants;

  const std::string URL = BASE_URL + PATH.at(TWEETS_INDEX) + '/' + "/all";

  RequestResponse response{cpr::Get(
    cpr::Url{URL},
    cpr::Header{
      {HEADER_NAMES.at(HEADER_ACCEPT_INDEX), HEADER_VALUES.at(ACCEPT_JSON_INDEX)},
      {HEADER_NAMES.at(HEADER_AUTH_INDEX),   m_authenticator.GetBearerAuth()}
    },
    cpr::Parameters{
      {PARAM_NAMES.at(PARAM_NAME_TWEET_FIELDS_INDEX), GetDefaultFields()},
      {PARAM_NAMES.at(PARAM_NAME_USER_FIELDS_INDEX),  GetUserFields()   },
      {PARAM_NAMES.at(PARAM_NAME_MEDIA_FIELDS_INDEX), GetMediaFields()  },
      {"expansions", "attachments.media_keys"},
      {"conversation_id", id},
      {"from", user_id}
    }
  )};

  if (response.error)
    log(response.GetError());

  return ParseTweetsFromJSON(response.json());
}
/**
 * @brief FetchChildStatuses
 *
 * @param   [in]  {TweetID}              id
 * @returns [out] {std::vector<Tweet>}
 */
std::vector<Tweet> Client::FetchChildTweets(TweetID id)
{
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

  const std::string URL = BASE_URL + PATH_V1.at(TWEETS_INDEX);
  const std::vector<std::string> messages = ChunkMessage(tweet.text);
        std::string              reply_id = tweet.in_reply_to_status_id;

  for (const auto& message : messages) {
    Tweet outgoing_tweet = Tweet::create_instance_with_message(tweet, message);

    RequestResponse response{cpr::Post(
      cpr::Url{URL},
      cpr::Header{
        {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
      },
      cpr::Parameters{
        {PARAM_NAMES.at(PARAM_NAME_STATUS_INDEX), outgoing_tweet.text}
        // {PARAM_NAMES.at(PARAM_NAME_REPLY_ID_INDEX), outgoing_tweet.in_reply_to_status_id}
      },
      cpr::VerifySsl{m_authenticator.verify_ssl()}
    )};

    if (response.error)
      throw request_error(response.GetError());

    Tweet returned_status = JSONToTweet(response.json());

    if (returned_status.text.empty())
      return false;

    m_tweets.emplace_back(std::move(returned_status));

    reply_id = returned_status.id;
  }

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


Tweet Client::FetchTweet(TweetID id)
{
  using namespace constants;

  const std::string URL = BASE_URL + PATH.at(TWEETS_INDEX) + '/' + id;

  RequestResponse response{
    cpr::Get(
      cpr::Url{URL},
      cpr::Header{
        {HEADER_NAMES.at(HEADER_ACCEPT_INDEX), HEADER_VALUES.at(ACCEPT_JSON_INDEX)},
        {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
      },
      cpr::Parameters{
        {PARAM_NAMES.at(PARAM_NAME_TWEET_FIELDS_INDEX), GetDefaultFields()}
      }
    )
  };

  if (response.error)
    log(response.GetError());
  else
    return ParseTweetFromJSON(response.json());
  return Tweet{};
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

std::vector<Tweet>  Client::FetchTweets(const std::string& subject, uint8_t max, bool media_only)
{
  using namespace constants;
  static const std::string MEDIA_FILTER{" filter:links"};
  static const std::string NO_RETWEETS {" -filter:retweet"};
  static const std::string URL    = BASE_URL + PATH_V1.at(TWEETS_V1_INDEX);
         const std::string FILTER = (media_only) ? MEDIA_FILTER + NO_RETWEETS : NO_RETWEETS;
         const std::string query  = subject + FILTER;

  RequestResponse response{
    cpr::Get(
      cpr::Url{URL},
      cpr::Header{
        {HEADER_NAMES.at(HEADER_ACCEPT_INDEX), HEADER_VALUES.at(ACCEPT_JSON_INDEX)},
        {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
      },
      cpr::Parameters{
        {PARAM_NAMES.at(PARAM_NAME_TWEET_FIELDS_INDEX), GetDefaultFields()},
        {PARAM_NAMES.at(PARAM_NAME_QUERY_INDEX), query},
        {"tweet_mode", "extended"}
        // {PARAM_NAMES.at(PARAM_NAME_COUNT_INDEX), std::to_string(max)}
      },
      cpr::VerifySsl{false}
    )
  };

  if (response.error)
    log(response.GetError());

  return ParseV1StatusesFromJSON(response.json());
}

std::vector<Tweet>  Client::FetchTweetsByUser(const std::string& username, uint8_t max)
{
    using namespace constants;
  static const std::string URL = BASE_URL + PATH_V1.at(TWEETS_V1_INDEX);
         const std::string query{"from@" + username};

  RequestResponse response{
    cpr::Get(
      cpr::Url{URL},
      cpr::Header{
        {HEADER_NAMES.at(HEADER_ACCEPT_INDEX), HEADER_VALUES.at(ACCEPT_JSON_INDEX)},
        {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
      },
      cpr::Parameters{
        {PARAM_NAMES.at(PARAM_NAME_TWEET_FIELDS_INDEX), GetDefaultFields()},
        {PARAM_NAMES.at(PARAM_NAME_QUERY_INDEX),        query},
        {PARAM_NAMES.at(PARAM_NAME_RESULT_TYPE_INDEX),  PARAM_VALUES.at(PARAM_VALUE_RECENT_INDEX)}
      },
      cpr::VerifySsl{false}
    )
  };

  if (response.error)
    log(response.GetError());

  return ParseV1StatusesFromJSON(response.json());
}

std::string Client::FetchUserID(const std::string& name)
{
  using namespace constants;
  static const std::string URL = BASE_URL + PATH_V1.at(USER_V1_INDEX);

  RequestResponse response{
    cpr::Get(
      cpr::Url{URL},
      cpr::Header{
        {HEADER_NAMES.at(HEADER_ACCEPT_INDEX), HEADER_VALUES.at(ACCEPT_JSON_INDEX)},
        {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
      },
      cpr::Parameters{
        {PARAM_NAMES.at(PARAM_NAME_TWEET_FIELDS_INDEX), GetDefaultFields()},
        {PARAM_NAMES.at(PARAM_NAME_SCREEN_NAME_INDEX),        name}
      },
      cpr::VerifySsl{false}
    )
  };

  if (response.error)
    log(response.GetError());

  return ParseUserIDFromJSON(response.json());
}
} // namespace kwitter
