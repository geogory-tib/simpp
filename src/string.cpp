#include "string.hpp"
#include <cstring>


bool simpp::string_contains(string needle, string haystack){
  if(needle.len > haystack.len)
	return false;
  for(size_t i = 0;i < haystack.len;i++){
	auto sub_str = string::slice(haystack, i, i + needle.len);
	if(sub_str == needle){
	  return true;
	}
  }
  return false;
}

int simpp::string_contains_index(string needle, string haystack){
  if(needle.len > haystack.len)
	return -1;
  for(size_t i = 0;i < haystack.len;i++){
	auto sub_str = string::slice(haystack, i, i + needle.len);
	if(sub_str == needle){
	  return (int)i;
	}
  }
  return -1;
}

bool simpp::string_contains_any(string subset, string s){
  for(size_t i = 0; i < s.len; i++){
	auto c = s[i];
	for(size_t set_i = 0; set_i < subset.len;set_i++){
	  if(c == subset[set_i])
		return true;
	}
  }
  return false;
}

bool simpp::string_has_prefix(string prefix, string s){
  if(prefix.len > s.len)
	return false;
  auto sprefix = string::slice(s, 0, prefix.len);
  return sprefix == prefix;
}
bool simpp::string_has_suffix(string suffix, string s){
  if(suffix.len > s.len)
	return false;
  auto ssuffix = string::slice(s,(s.len) - suffix.len, s.len);
  return ssuffix == suffix;
}
static bool in_trimset(char ch, const simpp::string& trimset) {
  for (size_t j = 0; j < trimset.len; ++j) {
    if (trimset[j] == ch) {
      return true;
    }
  }
  return false;
}

simpp::string simpp::string_triml(string trimset, string s) {
  if (s.len == 0 || trimset.len == 0) {
    return string::slice(s, 0, s.len);
  }

  size_t start = 0;
  while (start < s.len && in_trimset(s[start], trimset)) {
    ++start;
  }

  if (start == s.len) {
    return string{};
  }
  return string::slice(s, start, s.len);
}

simpp::string simpp::string_trimr(string trimset, string s) {
  if (s.len == 0 || trimset.len == 0) {
    return string::slice(s, 0, s.len);
  }

  size_t end = s.len;
  while (end > 0 && in_trimset(s[end - 1], trimset)) {
    --end;
  }

  if (end == 0) {
    return string{};
  }
  return string::slice(s, 0, end);
}

simpp::string simpp::string_trim(string trimset, string s) {
  auto rtrimmed = simpp::string_trimr(trimset, s);
  return simpp::string_triml(trimset, rtrimmed);
}


