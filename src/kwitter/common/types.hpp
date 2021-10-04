#pragma once

#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <cpr/cpr.h>
#include <kjson.hpp>

#include "util.hpp"

namespace kwitter {
/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ CONSTANTS ░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/
namespace constants {
namespace StatusVisibility {
const std::string PUBLIC  {"public"};
const std::string UNLISTED{"unlisted"};
const std::string PRIVATE {"private"};
const std::string DIRECT  {"direct"};
} // namespace StatusVisibility
} // namespace constants

inline bool visibility_is_valid(const std::string& visibility) {
  return (
    visibility.compare(constants::StatusVisibility::PUBLIC)   == 0 ||
    visibility.compare(constants::StatusVisibility::UNLISTED) == 0 ||
    visibility.compare(constants::StatusVisibility::PRIVATE)  == 0 ||
    visibility.compare(constants::StatusVisibility::DIRECT)   == 0
  );
}

class PostDataInterface {
public:
virtual ~PostDataInterface() {}
virtual std::string postdata() = 0;
};


struct File : public PostDataInterface {
std::string path;
std::string description;
std::string thumbnail;
std::string url;

std::string GetBytes() {
  return ReadFromFile(path);
}

File(std::string file_path) : path(file_path) {}

virtual ~File() override {}
/**
 * @brief
 *
 * TODO: Add thumbnail support
 *
 * @return std::string
 */
virtual std::string postdata() {
  std::string payload = GetBytes();
  return std::string{
    "file="         + payload + "&" +
    "description="  + description
  };
}

cpr::Multipart multiformdata() {
  std::string bytes = GetBytes();
  return cpr::Multipart{
    {"description", description},
    {"file", cpr::File{path}}
  };
}
};


/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░ STRUCTS ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

struct AccountField {
std::string name;
std::string value;
};

struct Account {
std::string               id;
std::string               username;
std::string               acct;
std::string               display_name;
bool                      locked;
bool                      bot;
bool                      discoverable;
std::string               group;
std::string               created_at;
std::string               note;
std::string               url;
std::string               avatar;
std::string               header;
std::string               header_static;
uint32_t                  followers_count;
uint32_t                  following_count;
uint32_t                  tweets_count;
std::string               last_status_at;
std::vector<AccountField> fields;

friend std::ostream &operator<<(std::ostream& o, const Account& a) {
  std::string fields{};
  for (const auto& field : a.fields) fields += "\nName: " + field.name + "\nValue: " + field.value;

  o << "ID:           " << a.id              << "\n" <<
       "Username:     " << a.username        << "\n" <<
       "Account:      " << a.acct            << "\n" <<
       "Display Name: " << a.display_name    << "\n" <<
       "Locked:       " << a.locked          << "\n" <<
       "Bot:          " << a.bot             << "\n" <<
       "Discoverable: " << a.discoverable    << "\n" <<
       "Group:        " << a.group           << "\n" <<
       "Created:      " << a.created_at      << "\n" <<
       "Note:         " << a.note            << "\n" <<
       "URL:          " << a.url             << "\n" <<
       "Avatar:       " << a.avatar          << "\n" <<
       "Header:       " << a.header          << "\n" <<
       "Followers:    " << a.followers_count << "\n" <<
       "Following:    " << a.following_count << "\n" <<
       "Tweets:       " << a.tweets_count  << "\n" <<
       "Last Tweet:   " << a.last_status_at  << "\n" <<

       "FIELDS\n"       << fields;

  return o;
}

bool is_valid()
{
  return !id.empty();
}
};

struct Mention {
std::string acct;
std::string id;
std::string url;
std::string username;

friend std::ostream &operator<<(std::ostream& o, const Mention& m) {
  o << "ID: " << m.id << "\n" << "Acct: " << m.acct << "\n" << "URL: " << m.url << "\nUsername:" << m.username << std::endl;

  return o;
}

};


struct Credentials {
std::string id;
std::string name;
std::string website;
std::string redirect_uri;
std::string scope;
std::string client_id;
std::string client_secret;
std::string vapid_key;
std::string code;

bool is_valid() {
  return
    !id.empty() &&
    !name.empty() &&
    !website.empty() &&
    !redirect_uri.empty() &&
    !scope.empty() &&
    !client_id.empty() &&
    !client_secret.empty() &&
    !vapid_key.empty() &&
    !code.empty();
}
};

struct Auth {
std::string api_key;
std::string api_key_secret;
std::string token;

bool is_valid() {
  return (
    !api_key.empty() &&
    !api_key_secret.empty()   &&
    !token.empty()
  );
}
};

struct Withheld {
bool copyright;
std::vector<std::string> country_codes;
};

struct Coordinates {
using Geocoordinate = std::array<float, 2>;
static const uint8_t LATITUDE_INDEX{0};
static const uint8_t LONGITUDE_INDEX{0};
std::string type;
Geocoordinate coordinate;
std::string place_id;
};

struct RefTweet {
std::string id;
std::string type;
};

struct Media{};

struct NonPublicMetrics {
uint32_t impression_count;
uint32_t url_link_clicks;
uint32_t user_profile_clicks;
};

struct OrganicMetrics {
uint32_t impression_count;
uint32_t like_count;
uint32_t reply_count;
uint32_t retweet_count;
uint32_t url_link_clicks;
uint32_t user_profile_clicks;
};

struct PromotedMetrics {
uint32_t    impression_count;
uint32_t    like_count;
uint32_t    reply_count;
uint32_t    retweet_count;
uint32_t    url_link_clicks;
uint32_t    user_profile_clicks;
};

struct PublicMetrics {
uint32_t retweet_count;
uint32_t reply_count;
uint32_t like_count;
uint32_t quote_count;
};

struct Attachment{};
struct ContextAnnotation{};
struct Entity{};
// "context_annotations": [
//       {
//            "domain": {
//                "id": "45",
//                "name": "Brand Vertical",
//                "description": "Top level entities that describe a Brands industry"
//            }
//        },
//        {
//            "domain": {
//                "id": "46",
//                "name": "Brand Category",
//                "description": "Categories within Brand Verticals that narrow down the scope of Brands"
//            },
//            "entity": {
//                "id": "781974596752842752",
//                "name": "Services"
//            }
//        },
//        {
//            "domain": {
//                "id": "47",
//                "name": "Brand",
//                "description": "Brands and Companies"
//            },
//            "entity": {
//                "id": "10045225402",
//                "name": "Twitter"
//            }
//        }
//    ]


struct Tweet : public PostDataInterface {
using Attachments = std::vector<Attachment>;
Tweet() {}
std::string	id;
std::string text;
std::vector<std::string> mentions;
std::vector<std::string> hashtags;
std::vector<std::string> urls;
// Attachments attachments;

// "attachments": {
//     "poll_ids": [
//         "1199786642468413448"
//     ]
// }

// "attachments": {
//     "media_keys": [
//         "3_1136048009270239232"
//     ]
// }
std::string author_id;
std::string username;
std::string profile_img_url;
// std::vector<ContextAnnotation> context_annotations;
std::string conversation_id;
std::string created_at;
std::vector<Entity> entities;
Coordinates geo;
std::string in_reply_to_user_id;
std::string in_reply_to_status_id;
std::string lang;
NonPublicMetrics non_public_metrics;
OrganicMetrics organic_metrics;
bool possiby_sensitive;
PromotedMetrics promoted_metrics;
PublicMetrics public_metrics;
std::vector<RefTweet> referenced_tweets;
std::string reply_settings;
std::string source;
int32_t followers_count;
int32_t friends_count;
int32_t favourite_count;
int32_t retweet_count;
Withheld withheld;

const std::string likes () const
{
  return std::to_string(favourite_count);
}

const std::string retweets () const
{
  return std::to_string(retweet_count);
}

const std::string followers() const
{
  return std::to_string(followers_count);
}

const std::string friends() const
{
  return std::to_string(friends_count);
}

static Tweet create_instance_with_message(      Tweet        tweet,
                                          const std::string& message)
{
  tweet.text = message;
  return tweet;
}

friend std::ostream &operator<<(std::ostream& o, const Tweet& s) {
  std::string mentions; for (const auto& mention : s.mentions) mentions += mention;
  std::string hashtags; for (const auto& hashtag : s.hashtags) hashtags += hashtag;
  std::string urls;     for (const auto& url : s.urls)         urls     += url;
  o << "ID             :" << s.id              << '\n'
    << "TEXT           :" << s.text            << '\n'
    << "MENTIONS       :" << mentions          << '\n'
    << "HASHTAGS       :" << hashtags          << '\n'
    << "LIKES          :" << s.likes()         << '\n'
    << "RETWEETS       :" << s.retweets()      << '\n'
    << "URLS           :" << urls              << '\n'
    << "AUTHOR_ID      :" << s.author_id       << '\n'
    << "USERNAME       :" << s.username        << '\n'
    << "FOLLOWERS      :" << s.followers()     << '\n'
    << "FRIENDS        :" << s.friends()       << '\n'
    << "PROFILE_IMG_URL:" << s.profile_img_url << '\n';
  return o;
}

virtual std::string postdata() override {
  // std::string media_ids{};
  // std::string delim{""};

  // for (const auto& media_item : media) {
  //   media_ids += delim + media_item.id;
  //   delim = ",";
  // }

  // if (!visibility.empty() && !visibility_is_valid(visibility))
  //   throw std::invalid_argument{"This visibility is not recognized"};

  // std::string RC{
  //   "status="         + content   + "&" +
  //   "media_ids[]="    + media_ids + "&" +
  //   "spoiler_text="   + spoiler   + "&" +
  //   "in_reply_to_id=" + replying_to_id + "&" +
  //   "visibility="     + visibility + "&" +
  //   "sensitive="      + std::to_string(sensitive)
  // };

  // return RC;

  return "";
}

bool is_valid() const {
  return (!text.empty());
}

virtual ~Tweet() override {}
};

struct Conversation {
std::string          id;
bool                 unread;
Tweet                tweet;
std::vector<Account> accounts;

friend std::ostream &operator<<(std::ostream& o, const Conversation& c) {
  o << "ConversationID: " << c.id << std::endl;

  return o;
}
};

} // namespace kwitter
