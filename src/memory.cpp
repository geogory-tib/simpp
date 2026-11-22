#include "memory.hpp"
#include <cstring>
#define BYTE_SIZE 1
void simpp::allocation_tracker::append(allocated_t element){
  if(this->len >= this->cap){
	buffer = (allocated_t *)std::realloc(buffer,sizeof(allocated_t) * ((this->cap + 1) * 2));
	this->cap = ((this->cap + 1) * 2);
  }
  this->buffer[len] = element;
  this->len++;
}
void simpp::allocation_tracker::push(void *addr, size_t size){
  this->append(allocated_t{.addr = addr,.size = size});
}

namespace simpp{
  simpp::malloc_allocator default_alloc{};
}

simpp::allocated_t simpp::allocation_tracker::get(void *addr){
  simpp::allocated_t prev_alloc = {};
  for(size_t i = 0;i < this->len;i++){
	if(this->buffer[i].addr == addr){
	  prev_alloc = this->buffer[i];
	  break;
	}
  }
  return prev_alloc;
}

void simpp::allocation_tracker::destroy(){
  std::free(this->buffer);
  *this = {};
}

simpp::arena simpp::arena::new_arena(size_t size){
  simpp::arena ret = {};
  ret.buffer = std::calloc(size,BYTE_SIZE);
  ret.cap = size;
  ret.brk =  0;
  return ret;
}

void *simpp::arena::alloc(size_t size){
  if ((this->brk + size) >= this->cap) {
    return NULL;
  }
  void *ret = (((char *)this->buffer) + this->brk);
  this->brk += (size);
  this->tracker.push(ret, size);
  return ret;
}
//!!NO-OP!! data in arena cannot be freed
// only done so it has the same API as malloc
// and can be swapped out for malloc in the container
// library
void simpp::arena::free(void *addr){
  return;
}

void *simpp::arena::realloc(void *addr, size_t size){
  void * ret = this->alloc(size);
  if(addr == nullptr){
	this->tracker.push(ret,size);
	return ret;
  }
  auto prev_page = this->tracker.get(addr);
  size_t copy_size = 0;
  if(size >= prev_page.size){
	copy_size = prev_page.size;
  }else if(prev_page.size > size){
	copy_size = size;
  }
  this->tracker.push(ret, size);
  std::memcpy(ret, addr, copy_size);
  return ret;
}

size_t simpp::arena::usage(){
  if(this->brk >= this->cap){
	return -1;
  }
  return this->cap - this->brk;
}

void simpp::arena::destroy(){
  std::free(this->buffer);
  this->tracker.destroy();
  *this = {};
}
/*
  garena_page is basically the same thing as the arena type
  but without the allocation tracker
 */
simpp::garena_page simpp::garena_page::new_page(size_t size){
  simpp::garena_page ret = {};
  ret.buffer = std::calloc(size,BYTE_SIZE);
  ret.cap = size;
  ret.brk =  0;
  return ret;
}

void *simpp::garena_page::alloc(size_t size){
  if ((this->brk + size) >= this->cap) {
    return NULL;
  }
  void *ret = (((char *)this->buffer) + this->brk);
  this->brk += (size);
  return ret;
}

void simpp::garena_page::destroy(){
  std::free(this->buffer);
  *this = {};
}

simpp::garena simpp::garena::new_garena(size_t page_size){
  simpp::garena ret = {};
  ret.pages = (simpp::garena_page *)std::calloc(4, sizeof(simpp::garena_page));
  ret.pages_cap = 4;
  ret.pages[0] = simpp::garena_page::new_page(page_size);
  ret.pages_len = 1;
  ret.page_size = page_size;
  return ret;
}
simpp::garena_page *simpp::garena::new_page(){
  if(this->pages_len >= this->pages_cap){
	this->pages = (garena_page *)realloc(this->pages, sizeof(garena_page) * ((this->pages_cap + 1) * 2));
	this->pages_cap = (this->pages_cap + 1) * 2;
  }
  garena_page page = simpp::garena_page::new_page(this->page_size);
  this->pages[this->pages_len] = page;
  garena_page *ret = &this->pages[this->pages_len];
  this->pages_len++;
  return ret;
}
void *simpp::garena::alloc(size_t size){
  simpp::garena_page *current_page = &this->pages[this->pages_len - 1];
  if(size >= this->page_size){
	this->page_size = size * 2;
	current_page = this->new_page();
  }else if(current_page->brk + size >= current_page->cap)
	current_page = this->new_page();
  void *ret = current_page->alloc(size);
  this->tracker.push(ret, size);
  return ret;
}

void *simpp::garena::realloc(void *addr, size_t size){
  void * ret = this->alloc(size);
  if(addr == nullptr){
	this->tracker.push(ret, size);
	return ret;
  }
  auto prev_page = this->tracker.get(addr);
  size_t copy_size = 0;
  if(size >= prev_page.size){
	copy_size = prev_page.size;
  }else if(prev_page.size > size){
	copy_size = size;
  }
  std::memcpy(ret, addr, copy_size);
  this->tracker.push(ret, size);
  return ret;
}

void simpp::garena::free(void *addr){
  return;
}

void simpp::garena::destroy(){
  for(int i = this->pages_len - 1;i >= 0;i--){
	this->pages[i].destroy();
  }
  this->tracker.destroy();
  std::free(pages);
  *this = {};
}

