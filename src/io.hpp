#pragma once
#include "memory.hpp"
#include "containers.hpp"
#include "string.hpp"
#include <cstdint>
extern "C"{
#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#endif
}
/*
  io.hpp:
  
  This file contains common functions/structures used for IO.
  Currently only POSIX systems are supported sorry windows folks
  please consult the comments above functions/structures for futher
  documentation.

*/
namespace simpp{
  typedef std::int64_t offset_t;
  typedef std::uint8_t byte;
  typedef cslice<byte> byteslice;
  // these will be declared something else for windows systems
  constexpr int FILE_READ = O_RDONLY;
  constexpr int FILE_WRITE = O_WRONLY;
  constexpr int FILE_RDWR = O_RDWR;
  constexpr int WHENCE_SET = SEEK_SET;
  constexpr int WHENCE_CUR = SEEK_CUR;
  constexpr int WHENCE_END = SEEK_END;
  constexpr int stdin_fileno = STDIN_FILENO;
  constexpr int stdout_fileno = STDOUT_FILENO;
  constexpr int stderr_fileno = STDERR_FILENO;
  /*
	struct file:

	this struct implements non buffered file handling.
	Currently the methods for error handling are not implemented
	The struct implements the following methods
	static int open_read(string path,file& f); // opens a file for reading returns -1 on failure
	static int open_write(string path,file& f); // opens a file for writing returns -1 on failure
	static int open(string path,file& f,int perms); // opens a file with your custom flags returns -1 on failure
	int write(cslice<byte> buf); // writes the slices content into the file returns n written on success and -1 on failure
	int read(cslice<byte> buf); // reads the file contents into the slice returns n read on success and -1 on failure
	int read(cslice<byte> buf,offset_t offset); // reads the slices content at offset in the file returns n read on succes -1 on failure
	(it does not advance the files current offset)
	int write(cslice<byte> buf,offset_t offset); // writes the slices content at offset in the file returns n wrote on success -1 on failure
	(it does not advance the files current offset)
	bool eof(); // checks if file has reached EOF;
	offset_t offset(); // gets the file's current offset.
	offset_t set_offset(offset_t offset); // set's the file's offset relative to the start of the file
	int get_fd(); // returns the underlying fd
	int close(); // closes the file
  */
  struct file{
  public:
	static int open_read(string path,file& f);
	static int open_write(string path,file& f);
	static int open(string path,file& f,int perms);
	static file fdtofile(int fd);
	// TODO -- implement error methods
	int write(cslice<byte> buf);
	int read(cslice<byte> buf);
	int read(cslice<byte> buf,offset_t offset);
	int write(cslice<byte> buf,offset_t offset);
	bool eof();
	offset_t offset();
	offset_t set_offset(offset_t offset);
	int get_fd();
	int close();
  private:
	int fd = 0;
	string error_str;
	int error_code = 0;
	string name;
	bool encountered_eof = false;
  };
  constexpr size_t DEFUALT_BUFFERED_STREAM_SIZE = 4096;
  // had to use a constructor because the compiler was throwing some error;
  template<typename Reader>
  class bufferedreader{
  public:
	Reader& base;
	template<typename Allocator = malloc_allocator>
	bufferedreader(Reader& base,size_t buf_size,Allocator& al = default_alloc) : base(base){
	  buffer = byteslice::make(buf_size,al);
	  read_in = 0;
	  read_ptr = 0;
	}
	template<typename Allocator = malloc_allocator>
	bufferedreader(Reader& base,Allocator& al = default_alloc) : base(base){
	  buffer = byteslice::make(DEFUALT_BUFFERED_STREAM_SIZE, al);
	  read_in = 0;
	  read_ptr = 0;
	}
	template <typename Allocator = malloc_allocator>
	void destroy(Allocator& al = default_alloc){
	  byteslice::destroy(buffer, al);
	  read_in = 0;
	  read_ptr = 0;
	  return;
	}
	int read(cslice<byte> buf){
	  if(read_in == read_ptr){
		read_in = base.read(buffer);
		if(read_in < 0){
		  return -1;
		}
		read_ptr = 0;
	  }
	  int i;
	  for(i = 0;i < buf.len && read_ptr < read_in;i++){
		buf[i] = buffer[i + read_ptr];
	  }
	  return i;
	}
	int remaing(){
	  return read_ptr - read_in;
	}
	int filled(){
	  return read_in;
	}
	template<typename Allocator = malloc_allocator>
	int resize(size_t buf_size, Allocator& al = default_alloc){
	  byteslice::resize(buffer, buf_size, al);
	  return buf_size;
	}
  private:
	byteslice buffer;
	int read_in = 0;
	int read_ptr = 0;
  };
  template<typename Writer>
  class bufferedwriter{
  public:
	Writer& base;
	template<typename Allocator = malloc_allocator>
	bufferedwriter(Writer& base,size_t buf_size,Allocator& al = default_alloc) : base(base){
	  buffer = byteslice::make(buf_size, al);
	  write_in = 0;
	}
	template<typename Allocator = malloc_allocator>
	bufferedwriter(Writer& base,Allocator& al = default_alloc) : base(base){
	  buffer = byteslice::make(DEFUALT_BUFFERED_STREAM_SIZE, al);
	  write_in = 0;
	}
	int flush(){
	  auto filled_data = byteslice::slice(buffer, 0, write_in);
	  int n = base.write(filled_data);
	  write_in = 0;
	  return n;
	}
	int write(cslice<byte> buf){
	  if(buf.len > buffer.len){
		int flushed_bytes = flush();
		if(flushed_bytes < 0){
		  return -1;
		}
		int n = base.write(buf);
		if(n < 0)
		  return -1;
		return n + flushed_bytes;
	  }
	  int i;
	  for(i = 0; i < buf.len;i++){
		buffer[write_in] = buf[i];
		write_in++;
		if(write_in >= buffer.len){
		  int flushed_bytes = flush();
		  if(flushed_bytes < 0)
			return -1;
		}
	  }
	  return i;
	}
	template<typename Allocator = malloc_allocator>
	int resize(size_t buf_size, Allocator& al = default_alloc){
	  byteslice::resize(buffer, buf_size, al);
	  return buf_size;
	}
	template<typename Allocator = malloc_allocator>
	void destroy(Allocator& al = default_alloc){
	  byteslice::destroy(buffer, al);
	  write_in = 0;
	}
	int filled(){
	  return write_in;
	}
  private:
	byteslice buffer;
	size_t write_in;
  };
  size_t print(string str);
  size_t fprint(file& f, string str);
}
