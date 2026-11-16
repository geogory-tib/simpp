#pragma once
#include "memory.hpp"
#include "containers.hpp"
#include <cstring>

/*
  string.h
  for now this just contains a typedef
  but in the future I will add more
  string maniuplation features

 */
namespace simpp{
  typedef cslice<char> string;
  template<typename Allocator = malloc_allocator>
  char *string_to_cstr(string str, Allocator al = default_alloc){
	char *buf = (char *)al.alloc(str.len + 1);
	memcpy(buf, str.buffer, str.len);
	buf[str.len] = '\0';
	return buf;
  }
}
