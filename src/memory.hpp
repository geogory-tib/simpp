#pragma once
#include <cstdlib>
#include <cstdio>
/*
  memory.hpp:

  This header contains various allocators that can be used for
  dynamic memory allocations
  all of the allocators must implement three methods, they work exactly like libc's malloc.
  void free(void *); // for arenas this is a no-op and does nothing
  void *alloc(size_t);  
  void *realloc(void *,size_t); // on nullptr it must allocate a new block of memory 
  If these methods are not implemented any functions/structures in the library can
  not use your custom allocator without throwing a stupidly complex c++ error message.
  (This is written for C++ 11 so most compilers can use it, so there is no constraints)
  Any function/structure that requires an allocator, said allocator should be passed as a refrence.
  Currently this module implements:
  struct malloc_allocator;
  class arena;
  class garena;
  For documentation on each type consult the comments above their declarations
*/


namespace simpp{
  /*
	struct malloc_allocator:
	
	This is a struct wrapper around standard libc malloc()/free().
	I made this a struct so it can be passed into the Allocator field
	of the templates in the containers.hpp module.
   */
  struct malloc_allocator{
	void free(void *addr){
	  std::free(addr);
	}
	void *alloc(size_t size){
	  return std::malloc(size);
	}
	void *realloc(void *addr,size_t size){
	  return std::realloc(addr,size);
	}
  };
  /*
	extern malloc_allocator default_alloc:
	
	This is the defintion of the default allocator of the
	simpp library. it is a struct wrapper that stores no
	state of libc malloc(size_t),free(void *) and realloc(void *,size_t).
	It was done this way so it could be passed as a default argument to
	functions/structures that need to allocate memory and have the possibility
	to use other allocators.
  */
  extern malloc_allocator default_alloc;
  /*
	struct allocated_t & class allocation_tracker:
	
	allocated_t is used purely so the arena types can keep track of previous allocations' sizes.
	The allocation tracker is just a dynamic array with containg the allocated_t(s).
	push() -  pushes an allocation into it's buffer creating an allocated_t struct.
	get()  -  will do a linear lookup on an address to find return it's allocated_t struct.
	This is used within the arena and garena allocators to ensure that realloc(void *,size_t)
	will not cause memory issues  when the previous allocation's data get's memcpy(ed)
	if the realloc'd data shrinks or grows from it's previous size
	(I could also provide the malloc allocator with this so people can look up previous allocations
	but that might be too much memory overhead);
 */
  struct allocated_t{
	void *addr;
	size_t size;
  };
  class allocation_tracker{
  public:
	void push(void *addr,size_t size);
	allocated_t get(void *addr);
	void destroy();
  private:
	void append(allocated_t element);
	allocated_t *buffer;
	size_t len;
	size_t cap;
  };
  /*
	class arena:

	This is a simple fixed size arena.
	It preallocates a buffer with libc malloc and gives out chunks from that buffer
	If the there is an attempt to allocate more than the capacity
	of the arena it will just return a nullptr.
	it implements methods:
	void free();
	void *alloc(size_t);
	void *realloc(void *,size_t);
	void destory() // frees all memory used by the arena
	size_t usage() // returns total free memory in bytes
  */
  class arena{
  public:
	void *buffer = nullptr;
	size_t brk = 0;
	size_t cap = 0;
	static arena new_arena(size_t size);
	// no op
	void free(void *addr);
	void *alloc(size_t size);
	void *realloc(void *addr,size_t size);
	void destroy();
	size_t usage();
  private:
	allocation_tracker tracker;
  };
  /*
	class garena_page:

	This is class that contains a preallocated buffer that
	the garena will return chunks of memory from.
	when it gets full or cannot meet the demand of the requested allocation
	the garena will create a new one to allocate from.
  */
  class garena_page{
  public:
	void *buffer = nullptr;
	size_t brk = 0;
	size_t cap = 0;
	static garena_page new_page(size_t size);
	void *alloc(size_t size);
	void destroy();
  };

  /*
	class garena:

	This is a growable arena type.
	the garena stores a dynamic array of class garena_page.
	the garena will return a chunk of memory from its current page.
	if the current page cannot meet the requested allocation it will create
	a new page and append it to it's garena_page *pages.
	it implements methods:
	void free();
	void *alloc(size_t);
	void *realloc(void *,size_t);
	void destory() // frees all memory used by the arena
  */
  constexpr size_t default_garena_page_size = 4096;
  class garena{
  public:
	garena_page *pages = nullptr;
	size_t pages_len = 0;
	size_t pages_cap = 0;
	size_t page_size = default_garena_page_size;
	static garena new_garena(size_t page_size);
	void *alloc(size_t size);
	// no-op like on the arena
	void free(void *addr);
	void *realloc(void *addr,size_t size);
	void destroy();
  private:
	allocation_tracker tracker = {};
	garena_page* new_page();
  };
/*
  struct iallocator:
  
  This is the interface for allocators if runtime
  polymorphism is required for some reason
  this actually used in the string implementation
  because it would be a headache to actually use templates
  for each time a function needs to use a string
*/

}
