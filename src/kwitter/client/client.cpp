#include "client.hpp"

namespace kwitter {

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ Helper FNs ░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

static std::string GetDefaultFields()
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
  PARAM_VALUES.at(PARAM_VALUE_PUBLIC_METRICS_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_REFERENCED_TWEETS_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_SOURCE_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_TEXT_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_WITHHELD_INDEX);
}
//---------------------------------------------------
static std::string GetMediaFields()
{
  using namespace constants;
  return
  PARAM_VALUES.at(PARAM_VALUE_PREVIMG_URL_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_TYPE_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_MEDIAKEY_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_URL_INDEX);
}
//---------------------------------------------------
static std::string GetUserFields()
{
  using namespace constants;
  return
  PARAM_VALUES.at(PARAM_VALUE_PROFILEURL_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_NAME_INDEX) + ',' +
  PARAM_VALUES.at(PARAM_VALUE_USERNAME_INDEX);
}
//---------------------------------------------------
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
//---------------------------------------------------
//------------------CLIENT---------------------------
//---------------------------------------------------
Client::Client(const std::string& username)
: m_authenticator(Authenticator{username})
{}
//---------------------------------------------------
bool Client::HasAuth() {
  return m_authenticator.IsAuthenticated();
}
//--------------------------------------------------------------------/
std::vector<Tweet> Client::FetchUserTweets(UserID name, uint8_t max)
{
  using json = nlohmann::json;
  using namespace constants;

  const auto        id  = m_authenticator.GetIDforUser(name);
  const std::string URL = BASE_URL + PATH.at(USER_INDEX) + '/' + id + "/tweets";

  if (id.empty())
    return {};

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

  return ParseTweetsFromJSON(response.json(), name);
}
//---------------------------------------------------
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
//---------------------------------------------------
Tweets Client::FetchThread(const std::string& id, const UserID& username)
{
  using json = nlohmann::json;
  using namespace constants;
  const auto        user_id  = m_authenticator.GetIDforUser(username);
  const std::string URL      = BASE_URL + PATH.at(TWEETS_INDEX) + "/search/recent";
  const std::string query    = "conversation_id:" + id + " from:" + user_id + " to:" + user_id;
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
      {PARAM_NAMES.at(PARAM_NAME_QUERY_V2_INDEX),     query             },
      {"expansions", "attachments.media_keys,referenced_tweets.id"      },
    })};

  if (response.error)
    log(response.GetError());

  return ParseTweetsFromJSON(response.json());
}
//---------------------------------------------------
bool Client::PostTweet(Tweet tweet)
{
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
//---------------------------------------------------
// Tweet Client::FetchTweet(TweetID id)
// {
//   using namespace constants;

//   const std::string URL = BASE_URL + PATH.at(TWEETS_INDEX) + '/' + id;

//   RequestResponse response{
//     cpr::Get(
//       cpr::Url{URL},
//       cpr::Header{
//         {HEADER_NAMES.at(HEADER_ACCEPT_INDEX), HEADER_VALUES.at(ACCEPT_JSON_INDEX)},
//         {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
//       },
//       cpr::Parameters{
//         {PARAM_NAMES.at(PARAM_NAME_TWEET_FIELDS_INDEX), GetDefaultFields()}
//       })};

//   if (response.error)
//     log(response.GetError());
//   else
//     return ParseTweetFromJSON(response.json());
//   return Tweet{};
// }
//---------------------------------------------------
Account Client::GetAccount() {
  return m_authenticator.GetAccount();
}
//---------------------------------------------------
std::string Client::GetUsername() {
  return m_authenticator.GetUsername();
}
//---------------------------------------------------
bool Client::SetUser(const std::string& username)
{
  return m_authenticator.SetUser(username);
}
//---------------------------------------------------
bool Client::HasPostedTweets() const
{
  return (!m_tweets.empty());
}
//---------------------------------------------------
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
//---------------------------------------------------
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
      },
      cpr::VerifySsl{false}
    )
  };

  if (response.error)
    log(response.GetError());

  return ParseV1StatusesFromJSON(response.json());
}
//---------------------------------------------------
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
//---------------------------------------------------
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
      cpr::VerifySsl{false})};

  if (response.error)
    log(response.GetError());

  return ParseUserIDFromJSON(response.json());
}
} // namespace kwitter
