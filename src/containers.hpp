#pragma once
#include "memory.hpp"
#include <cstring>
#include <cstdint>
/*
  containers.hpp: V0.01
  Containers is a simple template library with common data structures
  (Currently only contains a dynamic array(dynarray) and slice types (cslice))
  By default they use the memory.hpp's malloc_allocator. Consult memory.hpp's main
  comment for more information on diffrent allocators that can be used within this
  library.
*/

namespace simpp{
  typedef uint8_t byte;
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
	bool operator==(cslice<T>& other){
	  if(other.len != len)
		return false;
	  for(size_t i = 0;i < len;i++){
		if(other[i] != (*this)[i])
		  return false;
	  }
	  return true;
	}
	template<typename Allocator = malloc_allocator>
	static cslice<T>make(size_t size,Allocator& al = default_alloc){
	  cslice<T> ret = {.buffer = (T *)al.alloc(size * sizeof(T)),.len = size};
	  return ret;
	}
	template<typename Allocator = malloc_allocator>
	static void resize(cslice<T>&s, size_t size,Allocator& al = default_alloc){
	  s.buffer = al.realloc(s.buffer, size * sizeof(T));
	  s.len = size;
	}
	//DO NOT CALL ON A SLICE FROM A SLICE WILL CURRUPT MEMORY OR SEGFAULT
	template<typename Allocator = malloc_allocator>
	static void destroy(cslice<T>& s,Allocator& al = default_alloc){
	  al.free(s.buffer);
	  s = {};
	}
	static int memcpy(cslice<T>& src,cslice<T>&dst){
	  size_t copy_len;
	  if(src.len >= dst.len)
		copy_len = dst.len;
	  else
		copy_len = src.len;
	  std::memcpy(dst.buffer, src.buffer, copy_len);
	  return copy_len;
	}
	template<typename Allocator = malloc_allocator>
	static cslice<T>copy(cslice<T> other,Allocator al = default_alloc){
	  cslice<T> ret;
	  ret.buffer = (T*)al.alloc(other.len);
	  ret.len = other.len;
	  std::memcpy(ret.buffer, other.buffer, other.len * (sizeof(T)));
	  return ret;
	}
	static cslice<T>slice(cslice<T> s,size_t begin,size_t end){
	  cslice<T> ret;
	  ret.buffer = &s[begin];
	  ret.len = end - begin;
	  return ret;
	}
  };

  
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
	T pop(){
	  len--;
	  return buffer[len];
	}
	void delete_at(size_t index){
	  for(size_t i = index;i <len;i++){
		buffer[i] = buffer[i + 1];
	  }
	  len--;
	}
	// simple linear search
	int index_of(T element){
	  for(int i = 0;i < len;i++){
		if(buffer[i] == element){
		  return i;
		}
	  }
	  return -1;
	}
	template<typename Allocator = malloc_allocator>
	void append(T element, Allocator& al = default_alloc){
	  if(len >= cap){
		buffer = (T *)al.realloc(buffer,sizeof(T) * ((cap + 1) * 2));
		cap = ((cap + 1) * 2);
	  }
	  buffer[len] = element;
	  len++;
	}
	template<typename Allocator = malloc_allocator>
	void insert_at(T element,size_t index,Allocator& al = default_alloc){
	  if(len >= cap){
		buffer = (T *)al.realloc(buffer,sizeof(T) * ((cap + 1) * 2));
		cap = ((cap + 1) * 2);
	  }
	  T holder1 = buffer[index];
	  T holder2;
	  buffer[index] = element;
	  for(size_t i = index + 1;i < len + 1;i++){
		holder2 = buffer[i];
		buffer[i] = holder1;
		holder1 = holder2;
	  }
	  len++;
	}
	
	template<typename Allocator = malloc_allocator>
	void append_arr(T *elements,size_t elements_len,Allocator& al = default_alloc){
	  for(int i = 0;i < elements_len;i++){
		append(elements[i],al);
	  }
	}
	
	template<typename Allocator = malloc_allocator>
	void append_arr(cslice<T> elements,Allocator&al = default_alloc){
	  for(int i = 0;i < elements.len;i++){
		append(elements[i],al);
	  }
	}
	
	template<typename Allocator = malloc_allocator>
	void append_arr(dynarray<T> elements,Allocator&al = default_alloc){
	  for(int i = 0;i < elements.len;i++){
	   append(elements[i],al);
	  }
	}
	template<typename Allocator = malloc_allocator>
	void insert_arr_at(T *elements,size_t len,size_t index, Allocator& al = default_alloc){
	  for(int i = 0;i < len;i++){
		insert_at(elements[i],index + i,al);
	  }
	}
	template<typename Allocator = malloc_allocator>
	void insert_arr_at(cslice<T> elements,size_t index, Allocator& al = default_alloc){
	  for(int i = 0;i < elements.len;i++){
		insert_at(elements[i],index + i,al);
	  }
	}
	template<typename Allocator = malloc_allocator>
	void insert_arr_at(dynarray<T> elements,size_t index, Allocator& al = default_alloc){
	  for(int i = 0;i < elements.len;i++){
		insert_at(elements[i],index + i,al);
	  }
	}
	cslice<T> toslice(){
	  return cslice<T>{.buffer = buffer,.len = len};
	}
	
	//If using an arena allocator, this will pretty much do nothing
	//Your arena will free all of the allocated memory 
	template<typename Allocator = malloc_allocator>
	void destroy(Allocator& al = default_alloc){
	  al.free(buffer);
	  *this = {};
	}
  };

  // uses FNV-1 hash converts base cslice type to byte slice
  template<typename T>
  uint64_t map_hash(cslice<T> key){
	cslice<byte> key_data = {.buffer = (byte *)key.buffer,.len = (key.len * sizeof(T))};
	uint64_t hash = 0xcbf29ce484222325ull;
	for(size_t i = 0;i < key_data.len;i++){
	  hash ^= key_data[0];
	  hash *= 0x100000001b3ull;
	}
	return hash;
  }
  inline uint64_t map_hash(int key){
	byte * data = (byte *)&key;
	uint64_t hash = 0xcbf29ce484222325ull;
	for(size_t i = 0;i < sizeof(key);i++){
	  hash ^= data[0];
	  hash *= 0x100000001b3ull;
	}
	return hash;
  }
  
  template<typename Key,typename T>
  struct map_entry{
	Key key;
	T data;
	bool filled = false;
	bool deleted = false;
  };
  
  template<typename Key,typename T>
  struct map{
	cslice<map_entry<Key,T>> entries;
	size_t entries_filled = 0;
	template<typename Allocator = malloc_allocator>
	static map<Key,T> make(size_t size,Allocator& al = default_alloc){
	  map<Key,T> ret;
	  ret.entries = cslice<map_entry<Key,T>>::make(size,al);
	  std::memset(ret.entries.buffer, 0, sizeof(map_entry<Key,T>) * ret.entries.len);
	  ret.entries_filled = 0;
	  return ret;
	}
	template<typename Allocator = malloc_allocator>
	void grow(Allocator& al = default_alloc){
	  auto new_entry_buf = cslice<map_entry<Key,T>>::make(entries.len * 2,al);
	  std::memset(new_entry_buf.buffer, 0, sizeof(map_entry<Key,T>) * new_entry_buf.len);
	  for(size_t i = 0; i < entries.len;i++){
		if(!entries[i].filled || entries[i].deleted)
		  continue;
		auto entry = entries[i];
		auto hash = map_hash(entry.key);
		auto ind = hash % new_entry_buf.len;
		if(new_entry_buf[ind].filled){
		  do{
			ind = ind + 1 % new_entry_buf.len;
		  }while(new_entry_buf[ind].filled);
		}
		new_entry_buf[ind] = entry;
	  }
	  entries.destroy(entries, al);
	  entries = new_entry_buf;
	}
	bool if_needs_resized(){
	  float percent_used = (float)(entries_filled) / (float)(entries.len);
	  if(percent_used >= 0.50f)
		return true;
	  return false;
	}
	template<typename Allocator = malloc_allocator>
	void set(Key key,T data,Allocator& al = default_alloc){
	  if(if_needs_resized()){
		grow(al);
	  }
	  auto hash = map_hash(key);
	  auto ind = hash % entries.len;
	  if(entries[ind].filled){
		do{
		  ind = ind + 1 % entries.len;
		}while(entries[ind].filled);
	  }
	  entries[ind].data = data;
	  entries[ind].key = key;
	  entries[ind].filled = true;
	  entries_filled++;
	}
	T *get(Key key){
	  auto hash = map_hash(key);
	  auto ind = hash % entries.len;
	  if(!(entries[ind].key == key)){
		do{
		  ind = ind + 1 % entries.len;
		  if(!entries[ind].filled)
			return nullptr;
		}while(!(entries[ind].key == key));
	  }
	  if(entries[ind].deleted)
		return nullptr;
	  return &entries[ind].data;
	}
	void del(Key key){
	  auto hash = map_hash(key);
	  auto ind = hash % entries.len;
	  if(!(entries[ind].key == key)){
		do{
		  ind = ind + 1 % entries.len;
		}while(!(entries[ind].key == key));
	  }
	  entries[ind].deleted = true;
	}
	template<typename Allocator = malloc_allocator>
	void destroy(Allocator& al = default_alloc){
	  entries.destroy(entries, al);
	  entries_filled = 0;
	}
  };
}

