#pragma once
#include "memory.hpp"
/*
  containers.hpp: V0.01
  Containers is a simple template library with common data structures
  (Currently only contains a dynamic array(dynarray) and slice types (cslice))
  All of the data structures currently only use static methods as I felt that
  made the most sense since they don't really carry a lot of state (this might change).
  By default they use the memory.hpp's malloc_allocator. Consult memory.hpp's main
  comment for more information on diffrent allocators that can be used within this
  library.
*/

namespace simpp{
/*
  dynarray:
  
  Simple dynamic array implementation
  Has no methods but static ones so an allocator
  does not have to be stored internally
  This was done so you could switch allocators midway through
  using the container
  Allocator parameter has to provide the methods
  void free();
  void *alloc(size_t size);
  void *realloc(void *addr, size_t size);
  the default allocator for these functions is
  the malloc_allocator default_alloc; it stores
  no state and is just a wrapper around c stdlib
  malloc.
  All functions that can allocate memory need a refrence
  to an allocator provided so if they do have to allocate
  memory they can do so. If no argumenet is provided for an allocator
  malloc/realloc/free will be used.
*/
  template<typename T>
  struct dynarray{
	T *buffer = nullptr;
	size_t cap = 0;
	size_t len = 0;
	T& operator[](size_t index) {
#ifdef CONTAINERS_OUT_OF_BOUNDS
	  if (index >= len) {
		std::fprintf(stderr, "FAILURE::OUT OF BOUND INDEXING\n");
		std::exit(-1);
	  }
#endif
	  return buffer[index];
	}
	const T& operator[](size_t index) const {
#ifdef CONTAINERS_OUT_OF_BOUNDS
	  if (index >= len) {
		std::fprintf(stderr, "FAILURE::OUT OF BOUND INDEXING\n");
		std::exit(-1);
	  }
#endif	  
	  return buffer[index];
	}
	template <typename Allocator = malloc_allocator>
	static dynarray<T>make(size_t size,Allocator al = default_alloc){
	  dynarray<T> ret{};
	  ret.buffer = (T *)al.alloc(sizeof(T) * size);
	  ret.cap = size;
	  ret.len = 0;
	  return ret;
	}
	static T pop(dynarray<T>& arr){
	  arr.len--;
	  return arr.buffer[arr.len];
	}
	static void delete_at(dynarray <T> arr,size_t index){
	  for(size_t i = index;i < arr.len;i++){
		arr[i] = arr[i + 1];
	  }
	  arr.len--;
	}
	// simple linear search
	static int index_of(dynarray<T> arr,T element){
	  for(int i = 0;i < arr.len;i++){
		if(arr[i] == element){
		  return i;
		}
	  }
	  return -1;
	}
	template<typename Allocator = malloc_allocator>
	static void append(dynarray<T>& arr,T element, Allocator& al = default_alloc){
	  if(arr.len >= arr.cap){
		arr.buffer = (T *)al.realloc(arr.buffer,sizeof(T) * ((arr.cap + 1) * 2));
		arr.cap = ((arr.cap + 1) * 2);
	  }
	  arr.buffer[arr.len] = element;
	  arr.len++;
	}
	template<typename Allocator = malloc_allocator>
	static void insert_at(dynarray<T>& arr,T element,size_t index,Allocator& al = default_alloc){
	  if(arr.len >= arr.cap){
		arr.buffer = (T *)al.realloc(arr.buffer,sizeof(T) * ((arr.cap + 1) * 2));
		arr.cap = ((arr.cap + 1) * 2);
	  }
	  T holder1 = arr[index];
	  T holder2;
	  arr[index] = element;
	  for(size_t i = index + 1;i < arr.len + 1;i++){
		holder2 = arr[i];
		arr[i] = holder1;
		holder1 = holder2;
	  }
	  arr.len++;
	}
	template<typename Allocator = malloc_allocator>
	static void append_arr(dynarray<T>& arr,T *elements,size_t elements_len,Allocator& al = default_alloc){
	  for(int i = 0;i < elements_len;i++){
		dynarray::append(arr, elements[i],al);
	  }
	}
	//If using an arena allocator, this will pretty much do nothing
	//Your arena will free all of the allocated memory 
	template<typename Allocator = malloc_allocator>
	static void destroy(dynarray<T>&arr, Allocator& al = default_alloc){
	  al.free(arr.buffer);
	  arr = {};
	}
  };


  /*
	cslice:
	
	A slice is just a pointer with a length
	It does not do dynamic allocations or anything else
	can be "sliced" into smaller sections
	as an  example this could be usefull in situtions like this
	::PSEUDO CODE::
	simpp::slice<byte>read_buf = simpp::slice<byte>::make(4096);
	int n_read = sock.read(read_buf.buffer);
	// this returns a new slice that just contains data from 0...n_read - 1
	auto read_in = simpp::slice<byte>::slice(0,n_read);
	.......
   */ 
  template <typename T>
  struct cslice{
	T *buffer;
	size_t len;
	T& operator[](size_t index) {
#ifdef CONTAINERS_OUT_OF_BOUNDS
	  if (index >= len) {
		std::fprintf(stderr, "FAILURE::OUT OF BOUND INDEXING\n");
		std::exit(-1);
	  }
#endif
	  return buffer[index];
	}
	const T& operator[](size_t index) const {
#ifdef CONTAINERS_OUT_OF_BOUNDS
	  if (index >= len) {
		std::fprintf(stderr, "FAILURE::OUT OF BOUND INDEXING\n");
		std::exit(-1);
	  }
#endif	  
	  return buffer[index];
	}
	template<typename Allocator = malloc_allocator>
	static cslice<T>make(size_t size,Allocator& al = default_alloc){
	  cslice<T> ret = {.buffer = (T *)al.alloc(size),.len = size};
	  return ret;
	}
	//DO NOT CALL ON A SLICE FROM A SLICE WILL CURRUPT MEMORY OR SEGFAULT
	template<typename Allocator = malloc_allocator>
	static void destroy(cslice<T> s,Allocator& al = default_alloc){
	  al.free(s.buffer);
	  s = {};
	}
	static cslice<T>slice(cslice<T> s,size_t begin,size_t end){
	  cslice<T> ret;
	  ret.buffer = &s[begin];
	  ret.len = end - begin;
	  return ret;
	}
  };
}

