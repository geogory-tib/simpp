#pragma once
#include "memory.hpp"
#include "containers.hpp"
#include <cstring>
#include <cstdint>
#include <cmath>
/*
  string.h
  simple string helper functions
  This library provides no dedicated string types
  a string is a typedef of a cslice<char>
  currently there is no utf-8 support though I plan on
  implmenting helpers for that as well.
  Please read each function's documentation listed
  on the comment above it.
 */
namespace simpp{
  //typedef cslice<char> string;
  typedef dynarray<char> stringbuilder;
  //extern const char ascii_lookup_tables[][3];
  /*
	constexpr string STRLIT_TO_STRING(const char *str):
	
	this is a compile time functions that converts
	a string literal into a string(cslice<char>).
	Usefull for when you want to hardcode string values
	to functions at compile time
   */

  // constexpr string STRLIT_TO_STRING(const char *str){
  // 	size_t str_len = 0;
  // 	for(int i = 0;str[i] != '\0';i++){
  // 	  str_len++;
  // 	}
  // 	string ret = {.buffer = (char *)str,.len = str_len};
  // 	return ret;
  // }
  struct string : cslice<char>{
	string() = default;
	string(cslice<char> base) : cslice<char>(base) {}
	string(const char *str){
	  buffer = (char *)str;
	  len = strlen(str);
	}
	template <size_t N>
	string(const char (&lit)[N]) {
	  buffer = const_cast<char*>(lit);
	  len = N - 1;
	}
	
	static string from_ptr(char *ptr,size_t ptr_len){
	  string ret;
	  ret.buffer = ptr;
	  ret.len = ptr_len;
	  return ret;
	}
  };
  // just a nice little default if you so use to use it
  const string SIMPP_TRIM_LIST(" \r\n\t");
  /*
	char *string_to_cstr(str, Allocator& al = default_alloc):
	
	this function allocates a new null terminated c style string
	You must manually dealloc this yourself or use an arena to make
	handling these easy.
	If you are interfacing with C code directly you really have no reason to use this
   */
  template<typename Allocator = malloc_allocator>
  char *string_to_cstr(string str, Allocator& al = default_alloc){
	char *buf = (char *)al.alloc(str.len + 1);
	memcpy(buf, str.buffer, str.len);
	buf[str.len] = '\0';
	return buf;
  }
  /*
	cslice<string> string_split(string substr, string s, Allocator& al = default_alloc):
	
	this functions splits string s on every occurence of string substr within string s ommiting the sub-string.
	the resulting slice of strings is slices into the provided s and not fresh allocations.
	if you wish to have copies of the slices you must do that manually.
   */
  template<typename Allocator = malloc_allocator>
  cslice<string> string_split(string substr, string s, Allocator& al = default_alloc){
	if(substr.len > s.len){
	  return cslice<string>{};
	}
	dynarray<string>toks = dynarray<string>::make(2, al);
	int last = 0;
	for(int i = 0;i < s.len;i++){
	  auto ssub = string::slice(s, i, i + substr.len);
	  if(ssub == substr){
		if(i != 0){
		  auto tok = string::slice(s, last, i);
		  toks.append(tok, al);
		}
		// gets incremented after this.
		i += substr.len - 1;
		last = i + 1;
	  }
	}
	auto tok = string::slice(s, last, s.len);
	if(tok.len > 0){
	  toks.append(tok, al);
	}
	return toks.toslice();
  }
  /*
	bool string_contains(string needle,string haystack):

	This function returns true if the string haystack contains string needle.
	It will stop on the first occurence of string needle.
  */
  bool string_contains(string needle,string haystack);
    /*
	int string_sub_str(string needle,string haystack):

	This function returns the starting index of the string haystack within string needle.
	if the string needle is not found within string haystack it will return -1;
	It will stop on the fist occurence of string needle.
  */
  int string_contains_index(string needle,string haystack);
  /*
	bool string_contains_any(string subset, string s):

	This function will return true if any character within string subset is found within string s.
	It will stop on the first occurence of any character within string subset.
   */
  bool string_contains_any(string subset, string s);
  /*
	bool string_has_prefix(string prefix,string s):

	This function will return true if string s begins with string string prefix.
   */
  bool string_has_prefix(string prefix,string s);
    /*
	bool string_has_suffix(string prefix,string s):

	This function will return true if string s ends with string string suffix.
   */
  bool string_has_suffix(string suffix,string s);
  /*
	string string_trim(string trimset,string s):
	
	This function returns a slice of string s excluding left hand trailing characters in string tremset.
	Do not using string::destroy() on this slice, it will more than break your program if using malloc
  */
  string string_triml(string trimset,string s);
  /*
	string string_trim(string trimset,string s):
	
	This function returns a slice of string s excluding right hand trailing characters in string trimset.
	It is probably safe to use string::destroy() on the returned string
  */
  string string_trimr(string trimset,string s);
  /*
	string string_trim(string trimset,string s):
	
	This function returns a slice of string s excluding trailing and leading characters in string trimset.
	Do not using string::destroy() on this slice, it will more than break your program if using malloc
   */
  string string_trim(string trimset,string s);
  // this is kind of slow but it will be okay
  template<typename Allocator = malloc_allocator>
  string string_int_conv(std::int64_t num,Allocator& al = default_alloc){
	const char ascii_lookup_tables[] = {
	  '0','1','2','3','4',
	  '5','6','7','8','9',
	};
	string ret;
	if(num == 0){
	  ret = string::make(1, al);
	  ret[0] = '0';
	  return ret;
	}

	stringbuilder builder = stringbuilder::make(3, al);
	std::int64_t abs_val = (num < 0) ? -num : num;
	while(abs_val!= 0){
	  std::int64_t last_val = abs_val % 10;
	  builder.insert_at(ascii_lookup_tables[last_val],0,al);
	  abs_val /= 10;
	}
	if(num < 0)
	  builder.insert_at('-',0, al);
	ret = builder.toslice();
	return ret;
  }
  template<typename Allocator = malloc_allocator> 
  string string_float_conv(float num, Allocator& al = default_alloc){
	stringbuilder builder = stringbuilder::make(3, al);
	if(isnan(num)){
	  builder.append_arr(string("NaN"), al);
	  return builder.toslice();
	}else if(isinf(num)){
	  if(num < 0)
		builder.append('-', al);
	  builder.append_arr(string("Inf"), al);
	  return builder.toslice();
	}
	//  float val = num;
	float modifedf = num;
	int power = 0;
	while(modifedf != (int)modifedf){
	  modifedf *= (10);
	  power++;
	}
	int conv_int = (num < 0) ? (int)(-modifedf) : modifedf;
	auto conv = string_int_conv(conv_int,al);
	auto whole_number = string::slice(conv, 0,conv.len - power);
	auto fractional_number = string::slice(conv,(conv.len) - power, conv.len);
	if(num < 0){
	  builder.append('-', al);
	}
	builder.append_arr(whole_number, al);
	builder.append('.', al);
	builder.append_arr(fractional_number, al);
	string::destroy(conv, al);
	return builder.toslice();
  }

  template<typename Allocator = malloc_allocator> 
  string string_float_conv(double num, Allocator& al = default_alloc){
	stringbuilder builder = stringbuilder::make(3, al);
	if(isnan(num)){
	  builder.append_arr(string("NaN"), al);
	  return builder.toslice();
	}else if(isinf(num)){
	  if(num < 0)
		builder.append('-', al);
	  builder.append_arr(string("Inf"), al);
	  return builder.toslice();
	}
	//  float val = num;
	double modifedf = num;
	int power = 0;
	while(modifedf != (int64_t)modifedf){
	  modifedf *= (10);
	  power++;
	}
	int64_t conv_int = (num < 0) ? (int64_t)(-modifedf) : modifedf;
	auto conv = string_int_conv(conv_int,al);
	auto whole_number = string::slice(conv, 0,conv.len - power);
	auto fractional_number = string::slice(conv,(conv.len) - power, conv.len);
	if(num < 0){
	  builder.append('-', al);
	}
	builder.append_arr(whole_number, al);
	builder.append('.', al);
	builder.append_arr(fractional_number, al);
	string::destroy(conv, al);
	return builder.toslice();
  }
  template<typename Allocator = malloc_allocator> 
  string string_pointer_conv(const void *ptr,Allocator& al = default_alloc){
	const char ascii_lookup_tables[] = {
	  '0','1','2','3','4',
	  '5','6','7','8','9',
	  'a','b','c','d','e',
	  'f'
	};
	stringbuilder builder = stringbuilder::make(3, al);
	if(ptr == nullptr){
	  builder.append_arr(string("(nullptr)"), al);
	}
	uintptr_t ptr_val = (uintptr_t)const_cast<void*>(ptr);
	uintptr_t modifed_ptr = ptr_val;
	builder.append_arr(string("0x"),al);
	while(modifed_ptr != 0x0){
	  int mod_val = modifed_ptr & 0x10 - 1;
	  builder.insert_at(ascii_lookup_tables[mod_val],2,al);
	  modifed_ptr >>= 4;
	}
	return builder.toslice();
  }
}



