#include "string.hpp"
#include <cstring>
#include <cctype>

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

bool simpp::string_to_int(string s, int &val){
  constexpr int ascii_number_start = 48;
  bool is_signed = false;
  if(s[0] == '-'){
	s = string::slice(s,1,s.len);
	is_signed = true;
  }
  int decimal_place = 1;
  for(size_t i = 0;i < s.len - 1;i++){
	decimal_place *= 10;
  }
  int result = 0;
  for(size_t i = 0;i < s.len; i++){
	if(!std::isdigit(s[i])){
	  return false;
	}
	int digit = (s[i] - ascii_number_start);
	result += digit * decimal_place;
	decimal_place /= 10;
  }
  if(is_signed)
	result = -result;
  val = result;
  return true;
}

bool simpp::string_to_float(string s, double &val){
  constexpr double ascii_number_start = 48.0f;
  bool is_signed = false;
  if(s[0] == '-'){
	s = string::slice(s,1,s.len);
	is_signed = true;
  }
  int dot_ind = -1;
  int whole_len = 0;
  for(size_t i = 0;i < s.len;i++){
	if(s[i] == '.'){
	  dot_ind = i;
	  break;
	}
	whole_len++;
  }
  auto whole_portion_str = string::slice(s, 0, dot_ind);
  double whole_decimal_place = 1;
  for(int i = 0;i < dot_ind - 1;i++){
	whole_decimal_place *= 10;
  }
  double result = 0;
  for(size_t i = 0;i < whole_portion_str.len;i++){
	if(!std::isdigit(whole_portion_str[i]))
	  return false;
	double digit = (double)whole_portion_str[i] - ascii_number_start;
	result += digit * whole_decimal_place;
	whole_decimal_place /= 10.0f;
  }
  if (dot_ind > 0){
	auto fractional_str = string::slice(s, dot_ind + 1, s.len);
	double fractional_dec_place = 1.0f / 10.0f;
	for(size_t i = 0;i <fractional_str.len;i++){
	  if(!std::isdigit(fractional_str[i]))
		return false;
	  double digit = (double)fractional_str[i] - ascii_number_start;
	  result += digit * fractional_dec_place;
	  fractional_dec_place /= 10.0f;
	}
  }
  if(is_signed)
	result = -result;
  val = result;
  return true;
}

bool simpp::string_to_float(string s, float &val){
  constexpr float ascii_number_start = 48.0f;
  bool is_signed = false;
  if(s[0] == '-'){
	s = string::slice(s,1,s.len);
	is_signed = true;
  }
  int dot_ind = -1;
  int whole_len = 0;
  for(size_t i = 0;i < s.len;i++){
	if(s[i] == '.'){
	  dot_ind = i;
	  break;
	}
	whole_len++;
  }
  auto whole_portion_str = string::slice(s, 0, dot_ind);
  float whole_decimal_place = 1;
  for(int i = 0;i < dot_ind - 1;i++){
	whole_decimal_place *= 10;
  }
  float result = 0;
  for(size_t i = 0;i < whole_portion_str.len;i++){
	if(!std::isdigit(whole_portion_str[i]))
	  return false;
	float digit = (float)whole_portion_str[i] - ascii_number_start;
	result += digit * whole_decimal_place;
	whole_decimal_place /= 10.0f;
  }
  if (dot_ind > 0){
	auto fractional_str = string::slice(s, dot_ind + 1, s.len);
	float fractional_dec_place = 1.0f / 10.0f;
	for(size_t i = 0;i <fractional_str.len;i++){
	  if(!std::isdigit(fractional_str[i]))
		return false;
	  float digit = (float)fractional_str[i] - ascii_number_start;
	  result += digit * fractional_dec_place;
	  fractional_dec_place /= 10.0f;
	}
  }
  if(is_signed)
	result = -result;
  val = result;
  return true;
}

bool simpp::string_to_bool(string s, bool &val){
  auto lowered = string_to_lower(s);
  string truestr = "true";
  string falsestr = "false";
  if(lowered == truestr){
	val = true;
  }else if(lowered == falsestr){
	val = false;
  }else{
	string::destroy(lowered);
	return false;
  }
  string::destroy(lowered);
  return true;
}
