#include "string.hpp"
#include <cstring>

const char ascii_lookup_tables[][3] = {
	  {"0"},{"1"},{"2"},{"3"},{"4"},
	  {"5"},{"6"},{"7"},{"8"},{"9"},
	  {"10"},{"11"},{"12"},{"13"},{"14"},
	  {"15"},{"16"},{"17"},{"18"},{"19"},
	  {"20"},{"21"},{"22"},{"23"},{"24"},
	  {"25"},{"26"},{"27"},{"28"},{"29"},
	  {"30"},{"31"},{"32"},{"33"},{"34"},
	  {"35"},{"36"},{"37"},{"38"},{"39"},
	  {"40"},{"41"},{"42"},{"43"},{"44"},
	  {"45"},{"46"},{"47"},{"48"},{"49"},
	  {"50"},{"51"},{"52"},{"53"},{"54"},
	  {"55"},{"56"},{"57"},{"58"},{"59"},
	  {"60"},{"61"},{"62"},{"63"},{"64"},
	  {"65"},{"66"},{"67"},{"68"},{"69"},
	  {"70"},{"71"},{"72"},{"73"},{"74"},
	  {"75"},{"76"},{"77"},{"78"},{"79"},
	  {"80"},{"81"},{"82"},{"83"},{"84"},
	  {"85"},{"86"},{"87"},{"88"},{"89"},
	  {"90"},{"91"},{"92"},{"93"},{"94"},
	  {"95"},{"96"},{"97"},{"98"},{"98"}
};

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
simpp::string simpp::string_triml(string trimset,string s){
  bool found_in_set = true;
  size_t i = 0;
  while(i < s.len){
	auto ch = s[i];
	for(size_t tset_i = 0; tset_i < trimset.len;tset_i++){
	  if(trimset[tset_i] == ch){
		found_in_set = true;
		break;
	  }
	  found_in_set = false;
	}
	if(!found_in_set)
	  break;
	i++;
  }
  if(i == s.len){
	return string{};
  }
  return string::slice(s, i, s.len);
}

simpp::string simpp::string_trimr(string trimset, string s){
  bool found_in_set = true;
  size_t i = s.len - 1;
  while(i < s.len){
	auto ch = s[i];
	for(size_t tset_i = 0; tset_i < trimset.len;tset_i++){
	  if(trimset[tset_i] == ch){
		found_in_set = true;
		break;
	  }
	  found_in_set = false;
	}
	if(!found_in_set)
	  break;
	i--;
  }
  if(i == 0){
	return string{};
  }
  return string::slice(s, 0,i + 1);
}

simpp::string simpp::string_trim(string trimset, string s){
  auto rtrimmed = simpp::string_trimr(trimset, s);
  return simpp::string_triml(trimset,rtrimmed);
}


