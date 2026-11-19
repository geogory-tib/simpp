#include "string.hpp"
#include "io.hpp"
#include <cstring>
namespace simpp{
  inline void print_val(string val){
	print(val);
  }
  inline void print_val(int val){
	auto val_str = string_int_conv(val);
	print(val_str);
	string::destroy(val_str);
  }
  inline void print_val(uint64_t val){
	auto val_str = string_int_conv(val);
	print(val_str);
	string::destroy(val_str);
  }
  inline void print_val(int64_t val){
	auto val_str = string_int_conv(val);
	print(val_str);
	string::destroy(val_str);
  }
  inline void print_val(float val){
	auto val_str = string_float_conv(val);
	print(val_str);
	string::destroy(val_str);
  }
  inline void print_val(double val){
	auto val_str = string_float_conv(val);
	print(val_str);
	string::destroy(val_str);
  }
  inline void print_val(void *ptr){
	auto val_str = string_pointer_conv(ptr);
	print(val_str);
	string::destroy(val_str);
  }
  inline void print_val(const char *cstr){
	auto val = STRLIT_TO_STRING(cstr);
	print_val(val);
  }
  template<typename T>
  inline void print_val(cslice<T> s){
	print(STRLIT_TO_STRING("[ "));
	for(size_t i = 0;i < s.len;i++){
	  print_val(s[i]);
	  if(i != s.len - 1)
		print(STRLIT_TO_STRING(", "));
	}

	print(STRLIT_TO_STRING(" ]"));
  }
  
  template<typename T>
  inline void print_val(dynarray<T> s){
	print(STRLIT_TO_STRING("[ "));
	for(size_t i = 0;i < s.len;i++){
	  print_val(s[i]);
	  if(i != s.len - 1)
		print(STRLIT_TO_STRING(", "));
	}

	print(STRLIT_TO_STRING(" ]"));
  }
  inline void printf(string fmt){
	auto nonestr = STRLIT_TO_STRING("(none)");
	size_t last = 0;
	for(size_t i = 0; i < fmt.len;i++){
	  if(fmt[i] == '%'){
		auto part = string::slice(fmt, last, i);
		print(part);
		print(nonestr);
		last = i + 1;
	  }
	}
	auto last_part = string::slice(fmt, last, fmt.len);
	print(last_part);
  }
  template<typename T, typename ...Args>
  void printf(string fmt,T val,Args... args ){
	for(size_t i = 0; i < fmt.len;i++){
	  size_t last = 0;
	  if(fmt[i] == '%'){
		auto part = string::slice(fmt, last, i);
		print(part);
		print_val(val);
		last = i + 1;
		auto remaing = string::slice(fmt, last, fmt.len);
		printf(remaing, args...);
		return;
	  }
	}
  }

}
