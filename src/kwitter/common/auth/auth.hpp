#pragma once

#include <INIReader.h>
#include "kwitter/common/request.hpp"
#include "kwitter/common/twitter_util.hpp"

namespace kwitter {
inline const std::string get_dir() {
  char* path = realpath("/proc/self/exe", NULL);
  char* name = basename(path);
  return std::string {path, path + strlen(path) - strlen(name)};
}

inline bool ValidateCredentialsJSON(nlohmann::json json_file) {
  return(
    !json_file.is_null()                &&
    json_file.is_object()               &&
    json_file.contains("id")            &&
    json_file.contains("name")          &&
    json_file.contains("website")       &&
    json_file.contains("redirect_uri")  &&
    json_file.contains("scope")         &&
    json_file.contains("client_id")     &&
    json_file.contains("client_secret") &&
    json_file.contains("vapid_key")     &&
    json_file.contains("code")
  );
}

inline bool JSONHasUser(nlohmann::json data, std::string username) {
  auto is_null = data.is_null();
  auto is_obj  = data.is_object();
  auto hasname = data.contains(username);

  return (!data.is_null() && data.is_object() && data.contains(username));
}

inline Credentials ParseCredentialsFromJSON(nlohmann::json json_file, std::string username) {
  using json = nlohmann::json;
  using namespace kjson;

  Credentials creds{};

  if (json_file.contains("users")) {
    json users_json = json_file["users"];

    if (
      !users_json.is_null()            &&
      JSONHasUser(users_json, username) &&
      ValidateCredentialsJSON(users_json[username])) {

      json user_json = users_json[username];

      creds.id =            GetJSONStringValue(user_json, "id");
      creds.name =          GetJSONStringValue(user_json, "name");
      creds.website =       GetJSONStringValue(user_json, "website");
      creds.redirect_uri =  GetJSONStringValue(user_json, "redirect_uri");
      creds.scope        =  GetJSONStringValue(user_json, "scope");
      creds.client_id =     GetJSONStringValue(user_json, "client_id");
      creds.client_secret = GetJSONStringValue(user_json, "client_secret");
      creds.vapid_key =     GetJSONStringValue(user_json, "vapid_key");
      creds.code =          GetJSONStringValue(user_json, "code");
    }
  }

  return creds;
}

inline bool ValidateAuthJSON(nlohmann::json json_file) {
  return(
    !json_file.is_null()               &&
    json_file.is_object()              &&
    json_file.contains("apiKey") &&
    json_file.contains("apiKeySecret")   &&
    json_file.contains("bearer")
  );
}

inline std::string AuthToJSON(Auth auth) {
  nlohmann::json auth_json{};
  auth_json["apiKey"] = auth.api_key;
  auth_json["apiKeySecret"]   = auth.api_key_secret;
  auth_json["bearer"]        = auth.token;

  return auth_json.dump();
}

inline Auth ParseAuthFromJSON(nlohmann::json json_file) {
  using json = nlohmann::json;
  using namespace kjson;

  Auth auth{};

  if (ValidateAuthJSON(json_file)) {
    auth.api_key        = GetJSONStringValue(json_file, "apiKey");
    auth.api_key_secret = GetJSONStringValue(json_file, "apiKeySecret");
    auth.token          = GetJSONStringValue(json_file, "bearer");
  }

  return auth;
}




class Authenticator {

public:

Authenticator(std::string username = "")
: m_authenticated(false),
  m_verify_ssl{true}
{
  auto config = GetConfigReader();

  if (username.empty()) {
    if (config.ParseError() < 0) {
      log("Error loading config");
      throw std::invalid_argument{"No configuration path"};
    }

    username = config.GetString(constants::kwitter_SECTION, constants::USER_CONFIG_KEY, "");
    if (username.empty()) {
      throw std::invalid_argument{"No username in config. Please provide a username"};
    }
  }

  auto verify_ssl = config.GetString(constants::kwitter_SECTION, constants::VERIFY_SSL_KEY, "true");
  if (!verify_ssl.empty()) {
    m_verify_ssl = (verify_ssl == "true");
  }

  auto tokens_path = config.GetString(constants::kwitter_SECTION, constants::TOKENS_PATH_KEY, "");
  if (!tokens_path.empty())
  {
    m_tokens_path = tokens_path;
    m_token_json = LoadJSONFile(tokens_path);
  }

  if (m_token_json.is_null())
    throw std::invalid_argument{"Tokens not found"};

  if (!SetUser(username))
    throw std::invalid_argument{"Error setting user tokens"};
}


bool IsAuthenticated() {
  return m_authenticated;
}

bool HasValidToken() {
  return m_auth.is_valid();
}

void ClearToken() {
  m_auth = Auth{};
}

std::string GetBearerAuth() {
  if (m_auth.token.empty())
    return "";
  return std::string{"Bearer " + m_auth.token};
}

Credentials get_credentials() {
  return m_credentials;
}

Account GetAccount() {
  return m_account;
}

bool SetUser(const std::string& username)
{
  if (m_token_json.contains("users")           &&
      !m_token_json["users"].is_null()         &&
      m_token_json["users"].contains(username) &&
      !m_token_json["users"][username].is_null())
  {
    auto auth = ParseAuthFromJSON(m_token_json["users"][username]);

    if (auth.is_valid()) {
      m_auth          = auth;
      m_username      = username;
      m_authenticated = true;
      return true;
    }
  }

  return false;
}

std::string GetUsername() {
  return m_username;
}

bool verify_ssl() {
  return m_verify_ssl;
}

private:
using json = nlohmann::json;

Credentials  m_credentials;
Auth         m_auth;
Account      m_account;
std::string  m_username;
bool         m_authenticated;
json         m_token_json;
std::string  m_tokens_path;
bool         m_verify_ssl;
};

} // namespace kwitter
