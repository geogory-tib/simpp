#pragma once
#include "memory.hpp"
#include "containers.hpp"
#include <cstring>

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
  typedef cslice<char> string;
  typedef dynarray<char> stringbuilder;
  
  /*
	constexpr string STRLIT_TO_STRING(const char *str):
	
	this is a compile time functions that converts
	a string literal into a string(cslice<char>).
	Usefull for when you want to hardcode string values
	to functions at compile time
   */
  constexpr string STRLIT_TO_STRING(const char *str){
	size_t str_len = 0;
	for(int i = 0;str[i] != '\0';i++){
	  str_len++;
	}
	string ret = {.buffer = (char *)str,.len = str_len};
	return ret;
  }
  // just a nice little default if you so use to use it
  constexpr const string SIMPP_TRIM_LIST = STRLIT_TO_STRING(" \r\n\t");
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
}
