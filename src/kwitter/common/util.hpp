#pragma once

#include <ctime>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

namespace kwitter {
static const std::string get_executable_cwd() {
  char* path = realpath("/proc/self/exe", NULL);
  char* name = basename(path);
  std::string executable_cwd{path, path + strlen(path) - strlen(name)};

  return executable_cwd;
}

/**
 * SaveToFile
 */
static void SaveToFile(std::string data, std::string path) {
  std::ofstream o{path};
  o << data;
}

/**
 * SaveToFile
 */
template <typename T>
static void SaveToFile(T data, std::string path) {
  std::ofstream o{path};
  o << data;
}

/**
 * ReadFromFile
 */
static std::string ReadFromFile(std::string path)
{
  std::ifstream f{path};
  std::stringstream fs{};
  fs << f.rdbuf();
  return fs.str();
}

/**
 * ReadBytesFromFile
 *
 * @param path
 * @return std::vector<uint8_t>
 */
static std::vector<uint8_t> ReadBytesFromFile(std::string path)
{
  std::string file = ReadFromFile(path);

  return std::vector<uint8_t>{file.begin(), file.end()};
}

static nlohmann::json LoadJSONFile(std::string path)
{
  using namespace nlohmann;
  return json::parse(ReadFromFile(path), nullptr, constants::JSON_PARSE_NO_THROW);
}

/**
 * Poor man's log
 *
 * @param
 */
template<typename... Args>
static void log(Args... args)
{
  for (const auto& s : {args...})
    std::cout << s;
  std::cout << std::endl;
}

static std::string SanitizeOutput(const std::string& s)
{
  std::string o{};

  for (const char& c : s) {
    if (c == '\'')
      o += "\'";
    else
    if (c == '(')
      o += "&#x28;";
    else
    if (c == ')')
      o += "&#x29;";
    else
      o += c;
  }

  return o;
}

/**
 * SanitizeJSON
 *
 * Helper function to remove escaped double quotes from a string
 *
 * @param   [in] {std::string}
 * @returns [in] {std::string}
 */
static std::string SanitizeJSON(std::string s)
{
  s.erase(
    std::remove(s.begin(), s.end(),'\"'),
    s.end()
  );
  return s;
}

/**
 * SanitizeInput
 *
 * Helper function to remove quotes from a string
 *
 * @param   [in] {std::string}
 * @returns [in] {std::string}
 */
static std::string SanitizeInput(std::string s)
{
  s.erase(
    std::remove_if(s.begin(), s.end(), [](char c){
      return c == '\'' || c == '\"';
    }),
  s.end());

  return s;
}

/**
 * SanitizeJSON
 *
 * Helper function to remove escaped double quotes from a string
 *
 * @param [in] {std::string&} A reference to a string object
 */
static std::string StripLineBreaks(std::string s)
{
  s.erase(
    std::remove(s.begin(), s.end(),'\n'),
    s.end()
  );

  return s;
}

/**
 * CreateStringWithBreaks
 *
 * @param
 * @param
 * @returns
 */
static std::string CreateStringWithBreaks(const std::string &in, const size_t every_n)
{
  std::string out{};
  out.reserve(in.size() + in.size() / every_n);
  for(std::string::size_type i = 0; i < in.size(); i++) {
    (isascii(static_cast<uint8_t>(in[i]))) ?
    (!(i % every_n) && i) ? out.push_back('\n') : out.push_back(in[i]) :
    (void)(out);
  }
  return out;
}

static uint64_t string_to_uint64(const std::string& s)
{
  uint64_t    value{};
  const char* ptr = s.c_str();
  const char* termination_character = ptr + s.size();

  while (ptr < termination_character) {
    value = (value << 1) + (value << 3) + *(ptr++) - '0';
  }

  return value;
}

static std::string FetchTemporaryFile(const std::string& full_url, const bool verify_ssl = true)
{
  auto ext_end = full_url.find_first_of('?');
  ext_end      = ext_end == std::string::npos ? full_url.size() : ext_end;
  auto url     = full_url.substr(0, ext_end);
  auto ext_beg = url.find_last_of('.');
  auto ext_len = (ext_beg != url.npos) ? (url.size() - ext_beg) : 0;

  auto filename = (ext_len > 0) ?
                    "test_file" + url.substr(ext_beg, ext_len) :
                    "test_file";


  cpr::Response r = cpr::Get(cpr::Url{full_url}, cpr::VerifySsl(verify_ssl));
  SaveToFile(r.text, filename);

  return filename;
}

static void EraseFile(const std::string& path)
{
  std::remove(path.c_str());
}

static std::time_t to_time_t(const std::string& datetime, const char* pattern = "%a %b %d %H:%M:%S +0000 %Y")
{
  std::tm            t{};
  std::istringstream ss{datetime};

  ss >> std::get_time(&t, pattern);
  return mktime(&t);
}

/**
 * to_unixtime
 *
 * @param
 * @returns
 */                                                                                  // Sat Oct 09 00:58:39 +0000 2021
                                                                                     // 2023-04-18T02:02:00.000Z
                                                                                    //  %a %b %d %H:%M:%S +0000 %Y
static const std::string to_unixtime(const std::string& datetime, const char* pattern = "%Y-%m-%dT%H:%M:%S")
{
  std::time_t tm = to_time_t(datetime, pattern);
  auto        time = *localtime(&tm);
  return std::to_string(mktime(&time));
}

static const std::time_t today_start_time()
{
  std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm   tn  = *localtime(&now);
  tn.tm_hour = 0;
  tn.tm_min  = 0;
  tn.tm_sec  = 0;
  return mktime(&tn);
}


static bool is_today(const std::time_t& t)
{
 return (std::difftime(today_start_time(), t) > 0);
}


} // ns kwitter
