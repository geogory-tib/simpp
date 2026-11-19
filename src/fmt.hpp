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

  inline void print_val(cslice<string> s){
	print(STRLIT_TO_STRING("[ "));
	for(size_t i = 0;i < s.len;i++){
	  print(STRLIT_TO_STRING("\""));
	  print_val(s[i]);
	  print(STRLIT_TO_STRING("\""));
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

  inline void print_val(dynarray<string> s){
	print(STRLIT_TO_STRING("[ "));
	for(size_t i = 0;i < s.len;i++){
	  print(STRLIT_TO_STRING("\""));
	  print_val(s[i]);
	  print(STRLIT_TO_STRING("\""));
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
		break;
	  }
	}
  }
  template<typename Allocator = malloc_allocator>
  void sprintf(string fmt,stringbuilder& builder,Allocator& al){
	auto nonestr = STRLIT_TO_STRING("(none)");
	size_t last = 0;
	for(size_t i = 0; i < fmt.len;i++){
	  if(fmt[i] == '%'){
		auto part = string::slice(fmt, last, i);
		builder.append_arr(part, al);
		builder.append_arr(nonestr, al);
		last = i + 1;
	  }
	}
	builder.append_arr(string::slice(fmt, last, fmt.len), al);
  }
  template<typename Allocator = malloc_allocator>
  string sprintf(string fmt,Allocator& al){
	stringbuilder builder = stringbuilder::make(10,al);
	sprintf(fmt,builder,al);
	return builder.toslice();
  }
  template<typename Allocator = malloc_allocator>
  void sprint_val(int val,stringbuilder& builder,Allocator& al = default_alloc){
	auto val_str = string_int_conv(val);
	builder.append_arr(val_str, al);
	string::destroy(val_str);
  }
  template<typename Allocator = malloc_allocator>
  void sprint_val(string val,stringbuilder& builder,Allocator& al = default_alloc){
	builder.append_arr(val, al);
  }
  template<typename Allocator = malloc_allocator>
  void sprint_val(uint64_t val,stringbuilder& builder,Allocator& al = default_alloc){
	auto val_str = string_int_conv(val);
	builder.append_arr(val_str, al);
	string::destroy(val_str);
  }
  template<typename Allocator = malloc_allocator>
  void sprint_val(int64_t val,stringbuilder& builder,Allocator& al = default_alloc){
	auto val_str = string_int_conv(val);
	builder.append_arr(val_str, al);
	string::destroy(val_str);
  }
  template<typename Allocator = malloc_allocator>
  void sprint_val(float val,stringbuilder& builder,Allocator& al = default_alloc){
	auto val_str = string_float_conv(val);
	builder.append_arr(val_str, al);
	string::destroy(val_str);
  }
  template<typename Allocator = malloc_allocator>
  void sprint_val(double val,stringbuilder& builder,Allocator& al = default_alloc){
	auto val_str = string_float_conv(val);
	builder.append_arr(val_str, al);
	string::destroy(val_str);
  }

  template<typename Allocator = malloc_allocator>
  void sprint_val(void *val,stringbuilder& builder,Allocator& al = default_alloc){
	auto val_str = string_pointer_conv(val);
	builder.append_arr(val_str, al);
	string::destroy(val_str);
  }
  template<typename Allocator = malloc_allocator,typename T>
  void sprint_val(cslice<T> val,stringbuilder& builder,Allocator& al = default_alloc){
	builder.append_arr(STRLIT_TO_STRING("[ "), al);
	for(size_t i = 0;i < val.len;i++){
	  sprint_val(val[i], builder, al);
	  if(i != val.len - 1)
		builder.append_arr(STRLIT_TO_STRING(", "),al);
	}
	builder.append_arr(STRLIT_TO_STRING(" ]"), al);
  }

  template<typename Allocator = malloc_allocator>
  void sprint_val(cslice<string> val,stringbuilder& builder,Allocator& al = default_alloc){
	builder.append_arr(STRLIT_TO_STRING("[ "), al);
	for(size_t i = 0;i < val.len;i++){
	  builder.append('"', al);
	  sprint_val(val[i], builder, al);
	  builder.append('"', al);
	  if(i != val.len - 1)
		builder.append_arr(STRLIT_TO_STRING(", "),al);
	}
	builder.append_arr(STRLIT_TO_STRING(" ]"), al);
  }

  template<typename Allocator = malloc_allocator,typename T>
  void sprint_val(dynarray<T> val,stringbuilder& builder,Allocator& al = default_alloc){
	builder.append_arr(STRLIT_TO_STRING("[ "), al);
	for(size_t i = 0;i < val.len;i++){
	  sprint_val(val[i], builder, al);
	  if(i != val.len - 1)
		builder.append_arr(STRLIT_TO_STRING(", "),al);
	}
	builder.append_arr(STRLIT_TO_STRING(" ]"), al);
  }

  template<typename Allocator = malloc_allocator>
  void sprint_val(dynarray<string> val,stringbuilder& builder,Allocator& al = default_alloc){
	builder.append_arr(STRLIT_TO_STRING("[ "), al);
	for(size_t i = 0;i < val.len;i++){
	  builder.append('"', al);
	  sprint_val(val[i], builder, al);
	  builder.append('"', al);
	  if(i != val.len - 1)
		builder.append_arr(STRLIT_TO_STRING(", "),al);
	}
	builder.append_arr(STRLIT_TO_STRING(" ]"), al);
  }
  
  template<typename Allocator = malloc_allocator,typename T,typename ...Args>
  void sprintf(string fmt,stringbuilder& builder,Allocator& al,T val,Args... args){
	size_t last = 0;
	for(size_t i = 0;i < fmt.len;i++){
	  if(fmt[i] == '%'){
		auto part = string::slice(fmt, last, i);
		builder.append_arr(part, al);
		sprint_val(val, builder, al);
		last = i + 1;
		auto remaing = string::slice(fmt, last, fmt.len);
		sprintf(remaing,builder,al,args...);
		break;
	  }
	}
  }
  template<typename Allocator = malloc_allocator,typename T,typename ...Args>
  string sprintf(string fmt,Allocator& al,T val,Args... args){
	stringbuilder builder = stringbuilder::make(10, al);
	sprintf(fmt,builder, al, val, args...);
	return builder.toslice();
  }

}

