#include "io.hpp"
#include "string.hpp"
#include <cerrno>
#include <cstring>
#ifndef _WIN32
extern"C"{
#include <fcntl.h>
#include <unistd.h>
}
#endif
int simpp::file::write(cslice<byte> buf){
  int n = ::write(this->fd, buf.buffer, buf.len);
  if(n < 0){
	strerror_r(errno, this->error_str.buffer, this->error_str.len);
	this->error_code = errno;
  }
  return n;
}

int simpp::file::read(cslice<byte> buf){
   int n = ::read(this->fd, buf.buffer, buf.len);
   if(n == 0){
	 this->encountered_eof = true;
   }else if(n < 0){
	 strerror_r(errno, this->error_str.buffer, this->error_str.len);
	 this->error_code = errno;
   }
   return n;
}

bool simpp::file::eof(){
  return encountered_eof;
}

int simpp::file::get_fd(){
  return this->fd;
}
simpp::offset_t simpp::file::offset(){
  off_t offst = lseek(this->fd, 0, SEEK_CUR);
  if(offst < 0){
	strerror_r(errno, this->error_str.buffer, this->error_str.len);
	this->error_code = errno;
  }
  return offst;
}
simpp::offset_t simpp::file::set_offset(simpp::offset_t offset){
  off_t offst = lseek(this->fd, offset, SEEK_SET);
  if(offst < 0){
	strerror_r(errno, this->error_str.buffer, this->error_str.len);
	this->error_code = errno;
  }
  return offst;
}

int simpp::file::read(cslice<byte> buf,simpp::offset_t offset){
  int n = ::pread(this->fd, buf.buffer, buf.len, offset);
   if(n == 0){
	 this->encountered_eof = true;
   }else if(n < 0){
	 strerror_r(errno, this->error_str.buffer, this->error_str.len);
	 this->error_code = errno;
   }
   return n;
}

int simpp::file::write(cslice<byte> buf, simpp::offset_t offset){
   int n = ::pwrite(this->fd, buf.buffer, buf.len, offset);
   if(n < 0){
	 strerror_r(errno, this->error_str.buffer, this->error_str.len);
	 this->error_code = errno;
   }
   return n;
}

int simpp::file::open_read(string path, file &f){
  char *cstr_path = simpp::string_to_cstr(path);
  f.error_str = string::make(512);
  int fd = ::open(cstr_path, O_RDONLY);
  if(fd < 0){
	strerror_r(errno, f.error_str.buffer, f.error_str.len);
	f.error_code = errno;
  }
  f.name = string::copy(path);
  f.fd = fd;
  free(cstr_path);
  return fd;
}

int simpp::file::open_write(string path, file &f){
  char *cstr_path = simpp::string_to_cstr(path);
  f.error_str = string::make(512);
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP; 
  int fd = ::creat(cstr_path, mode);
  if(fd < 0){
	strerror_r(errno, f.error_str.buffer, f.error_str.len);
	f.error_code = errno;
  }
  f.name = string::copy(path);
  f.fd = fd;
  free(cstr_path);
  return fd;
}

int simpp::file::open(string path, file &f,int flags){
  char *cstr_path = simpp::string_to_cstr(path);
  f.error_str = string::make(512);
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP; 
  int fd = ::open(cstr_path,flags,mode);
  if(fd < 0){
	strerror_r(errno, f.error_str.buffer, f.error_str.len);
	f.error_code = errno;
  }
  f.name = string::copy(path);
  f.fd = fd;
  free(cstr_path);
  return fd;
}

simpp::file simpp::file::fdtofile(int fd){
  file ret;
  ret.fd = fd;
  ret.name = string::make(1);
  ret.error_str = string::make(512);
  return ret;
}

int simpp::file::close(){
  int code = ::close(this->fd);
  string::destroy(this->error_str);
  string::destroy(this->name);
  return code;
}

size_t simpp::print(string str){
  return ::write(stdout_fileno, str.buffer, str.len);
}

size_t simpp::fprint(file &f, string str){
  byteslice bytestr = {.buffer = (simpp::byte*)str.buffer,.len = str.len};
  return f.write(bytestr);
}

int simpp::file::get_errorcode(){
  return error_code;
}

