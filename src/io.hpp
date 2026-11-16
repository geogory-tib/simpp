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
namespace simpp{
  typedef std::int64_t offset_t;
  typedef std::uint8_t byte;
  typedef cslice<byte> byteslice;
  constexpr int FILE_READ = O_RDONLY;
  constexpr int FILE_WRITE = O_WRONLY;
  constexpr int FILE_RDWR = O_RDWR;
  constexpr int WHENCE_SET = SEEK_SET;
  constexpr int WHENCE_CUR = SEEK_CUR;
  constexpr int WHENCE_END = SEEK_END;
  constexpr int stdin_fileno = STDIN_FILENO;
  constexpr int stdout_fileno = STDOUT_FILENO;
  constexpr int stderr_fileno = STDERR_FILENO;
  struct file{
  public:
	static int open_read(string path,file& f);
	static int open_write(string path,file& f);
	static int open(string path,file& f,int perms);
	static file fdtofile(int fd);
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
	template<typename Allocator>
	bufferedreader(Reader& base,size_t buf_size,Allocator& al = default_alloc) : base(base){
	  buffer = byteslice::make(buf_size,al);
	  read_in = 0;
	  read_ptr = 0;
	}
	template<typename Allocator>
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
	template<typename Allocator>
	bufferedwriter(Writer& base,size_t buf_size,Allocator& al = default_alloc) : base(base){
	  buffer = byteslice::make(buf_size, al);
	  write_in = 0;
	}
	template<typename Allocator>
	bufferedwriter(Writer& base,Allocator& al = default_alloc) : base(base){
	  buffer = byteslice::make(DEFUALT_BUFFERED_STREAM_SIZE, al);
	  write_in = 0;
	}
	int flush(){
	  auto filled_data = byteslice::slice(buffer, 0, write_in);
	  int n = base.write(filled_data);
	  write_in = 0;
	}
	int write(cslice<byte> buf){
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
