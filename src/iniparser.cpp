#include "iniparser.hpp"
#include "fmt.hpp"
#include "string.hpp"
#include <cctype>
#include <cassert>


static simpp::string tok_str_buf[] = {"TOK_EQ","TOK_SECTION","TOK_SYMBOL","TOK_LNEND","TOK_EOF"};

simpp::ini_token simpp::iniparser_state::pull_tok(){
  if(pos >= contents.len)
	return (ini_token){.raw = "EOF",.type = TOK_EOF};
  auto tok = contents[pos];
  pos++;
  return tok;
}
simpp::ini_token simpp::iniparser_state::peek_tok(){
  if(pos >= contents.len)
	return (ini_token){.raw = "EOF",.type = TOK_EOF};
  auto tok = contents[pos];
  return tok;
}
char simpp::inilexer::pull_ch(){
  if(pos >= contents.len)
	return '\0';
  char ch = contents[pos];
  pos++;
  return ch;
}

char simpp::inilexer::peek_ch(){
  if(pos >= contents.len)
	return '\0';
  return contents[pos];
}

void  simpp::inilexer::eat_comment(){
  char ch = pull_ch();
  while(ch != '\0' && ch != '\n'){
	ch = pull_ch();
  }
}
simpp::ini_token simpp::inilexer::parse_section_header(simpp::iniparser_err*& err){
  ini_token ret = {};
  ret.type = TOK_SECTION;
  size_t raw_start = pos;
  char ch = pull_ch();
  if(ch == ']'){
	return ret;
  }
  bool found_closing_bracket = false;
  while(ch != '\0' && ch != '\n'){
	ch = pull_ch();
	if(ch == ']'){
	  size_t raw_end = pos - 1;
	  auto untrimmed_raw = simpp::string::slice(contents, raw_start,raw_end);
	  ret.raw = simpp::string_trim(" ", untrimmed_raw);
	  found_closing_bracket = true;
	  break;
	}
  }
  if(!found_closing_bracket){
	err = (iniparser_err *)internal_alloc->alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_PARSE_FAILURE;
	err->strerr =simpp::sprintf("Expected closing ] after %", *internal_alloc, string::slice(contents, raw_start, pos -1));
	return ini_token{};
  }
  return ret;
}

simpp::ini_token simpp::inilexer::parse_qoutes(simpp::iniparser_err*& err){
  simpp::ini_token ret = {};
  ret.type = simpp::TOK_SYMBOL;
  size_t start_ind = pos;
  bool found_closing_qoute = false;
  char ch = pull_ch();
  if(ch == '"')
	return ini_token{.raw = "",.type = TOK_SYMBOL};
  while(ch != '\0' && ch != '\n' && !found_closing_qoute){
	ch = pull_ch();
	if(ch == '"')
	  found_closing_qoute = true;
  }
  if(!found_closing_qoute){
	err = (iniparser_err *)internal_alloc->alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_PARSE_FAILURE;
	err->strerr =simpp::sprintf("Expected closing \" after %", *internal_alloc, string::slice(contents,start_ind, pos -1));
	return ini_token{};
  }
  size_t end_pos = pos - 1;
  ret.raw = simpp::string::slice(contents,start_ind,end_pos);
  return ret;
}

simpp::ini_token simpp::inilexer::parse_symbol(size_t start_index){
  ini_token ret = {};
  ret.type = TOK_SYMBOL;
  char ch = peek_ch();
  while(!isblank(ch) && ch != '\n' && ch != '\r' && ch != '\0'){
	ch = pull_ch();
	ch = peek_ch();
  }
  size_t end_index = pos;
  ret.raw = simpp::string::slice(contents, start_index, end_index);
  return ret;
}

simpp::iniparser_err *simpp::iniparser::lex_contents(){
  //  simpp::dynarray<ini_token> pstate.contents;
  iniparser_err *err = nullptr;
  for(char ch = lexer.pull_ch() ;ch != '\0';ch = lexer.pull_ch()){
	if(err != nullptr){
	  return err;
	}
	switch(ch){
	case ';':
	case '#':
	  {
		lexer.eat_comment();
		continue;
		break;
	  }
	case '[':
	  pstate.contents.append(lexer.parse_section_header(err),internal_alloc);
	  continue;
	  break;
	case '=':
	  pstate.contents.append((ini_token){.raw = "=",.type = TOK_EQ},internal_alloc);
	  continue;
	  break;
	case '"':
	  pstate.contents.append(lexer.parse_qoutes(err), internal_alloc);
	  continue;
	  break;
	case '\r':
	  continue;
	case '\n':
	  pstate.contents.append((ini_token){.raw = "\\n",.type = TOK_LNEND}, internal_alloc);
	  continue;
	  break;
	}
	if(isalnum(ch) || ispunct(ch))
	  pstate.contents.append(lexer.parse_symbol(lexer.pos - 1), internal_alloc);
	else if(isblank(ch))
	  continue;
  }
  pstate.contents.append((ini_token){.raw = "EOF",TOK_EOF}, internal_alloc);
  return nullptr;
}


simpp::iniparser_err *simpp::iniparser::parse_toks(){
  map<string,string> global_section = {};
  sections.set(global_section_name, global_section, internal_alloc);
  pstate.current_section = global_section_name;
  for(ini_token tok = pstate.pull_tok();tok.type != TOK_EOF;tok = pstate.pull_tok()){
	switch(tok.type){
	case TOK_SECTION:
	  {
		map<string,string> new_section = {};
		sections.set(tok.raw, new_section, internal_alloc);
		pstate.current_section = tok.raw;
		break;
	  }
	case TOK_SYMBOL:
	  {
		auto key = tok.raw;
		if(pstate.peek_tok().type != TOK_EQ){
		  iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
		  err->code = INIPARSER_ERR_PARSE_FAILURE;
		  err->strerr = simpp::sprintf("Expected '=' after % got '%'", internal_alloc,tok.raw,pstate.peek_tok().raw);
		  return err;
		}
		pstate.pull_tok();
		auto val_tok = pstate.pull_tok();
		if(val_tok.type != TOK_SYMBOL && val_tok.type != TOK_LNEND){
		  iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
		  err->code = INIPARSER_ERR_PARSE_FAILURE;
		  err->strerr = simpp::sprintf("Expected symbol or newline  after '=' got '%'", internal_alloc,val_tok.raw);
		  return err;
		}
		auto section = sections.get(pstate.current_section);
		assert(section != nullptr);
		if(val_tok.type == TOK_LNEND)
		  section->set(key,"", internal_alloc);
		else
		  section->set(key, val_tok.raw, internal_alloc);
		break;
	  }
	case TOK_LNEND:
	  break;
	case TOK_EQ:
	  {
		iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
		err->code = INIPARSER_ERR_PARSE_FAILURE;
		err->strerr = simpp::sprintf("Unexpected '=' before token '%'", internal_alloc,pstate.peek_tok().raw);
		return err;
		break;
	  }
	case TOK_EOF:
	  break;
	}
  }
  return nullptr;
}


simpp::iniparser_err *simpp::iniparser::parsefile(simpp::string filepath){
  this->internal_alloc = garena::new_garena(4096);
  int success = file::open_read(filepath,this->inifile);
  if(success < 0){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_FILE_FAILURE;
	err->strerr = simpp::sprintf("Failed to open %: '%'", internal_alloc, filepath,this->inifile.get_errormsg(internal_alloc));
	return err;
  }
  byte buf[512];
  carray<byte> read_buf(buf);
  stringbuilder builder;
  for(;!this->inifile.eof();){
	int n = this->inifile.read(read_buf);
	if(n < 0){
	  iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	  err->code = INIPARSER_ERR_FILE_FAILURE;
	  err->strerr = simpp::sprintf("Failed to open %: '%'", internal_alloc, filepath,this->inifile.get_errormsg(internal_alloc));
	  return err;
	}
	auto read_in = byteslice::slice(read_buf, 0, n);
	builder.append_arr((carray<char>&)(read_in), this->internal_alloc);
  }
  this->lexer.contents = builder.toslice();
  this->lexer.internal_alloc = &internal_alloc;
  auto err = lex_contents();
  if(err != nullptr){
	return err;
  }
  return this->parse_toks();
}

simpp::iniparser_err *simpp::iniparser::get_field(string section, string field, string &val){
  auto sect = sections.get(section);
  if(sect == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_SECTION_NOT_FOUND;
	err->strerr = simpp::sprintf("Section '%' was not found within parsed file",internal_alloc,section);
	return err;
  }
  auto value = sect->get(field);
  if(value == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_FIELD_NOT_FOUND;
	err->strerr = simpp::sprintf("Field '%' not found within Section '%'",internal_alloc,field,section);
	return err;
  }
  val = *value;
  return nullptr;
}
simpp::iniparser_err *simpp::iniparser::get_field(string section, string field, int &val){
  auto sect = sections.get(section);
  if(sect == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_SECTION_NOT_FOUND;
	err->strerr = simpp::sprintf("Section '%' was not found within parsed file",internal_alloc,section);
	return err;
  }
  auto value = sect->get(field);
  if(value == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_FIELD_NOT_FOUND;
	err->strerr = simpp::sprintf("Field '%' not found within Section '%'",internal_alloc,field,section);
	return err;
  }
  bool passed = string_to_int(*value,val);
  if(!passed){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_CONV_ERROR;
	err->strerr = simpp::sprintf("Field '%' with value '%' cannot be converted to integer",internal_alloc,field,value);
	return err;
  }
  return nullptr;
}

simpp::iniparser_err *simpp::iniparser::get_field(string section, string field, double& val){
  auto sect = sections.get(section);
  if(sect == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_SECTION_NOT_FOUND;
	err->strerr = simpp::sprintf("Section '%' was not found within parsed file",internal_alloc,section);
	return err;
  }
  auto value = sect->get(field);
  if(value == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_FIELD_NOT_FOUND;
	err->strerr = simpp::sprintf("Field '%' not found within Section '%'",internal_alloc,field,section);
	return err;
  }
  bool passed = string_to_float(*value,val);
  if(!passed){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_CONV_ERROR;
	err->strerr = simpp::sprintf("Field '%' with value '%' cannot be converted to integer",internal_alloc,field,*value);
	return err;
  }
  return nullptr;
}

simpp::iniparser_err *simpp::iniparser::get_field(string section, string field, float& val){
  auto sect = sections.get(section);
  if(sect == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_SECTION_NOT_FOUND;
	err->strerr = simpp::sprintf("Section '%' was not found within parsed file",internal_alloc,section);
	return err;
  }
  auto value = sect->get(field);
  if(value == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_FIELD_NOT_FOUND;
	err->strerr = simpp::sprintf("Field '%' not found within Section '%'",internal_alloc,field,section);
	return err;
  }
  bool passed = string_to_float(*value,val);
  if(!passed){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_CONV_ERROR;
	err->strerr = simpp::sprintf("Field '%' with value '%' cannot be converted to integer",internal_alloc,field,*value);
	return err;
  }
  return nullptr;
}

simpp::iniparser_err *simpp::iniparser::get_field(string section, string field, bool& val){
  auto sect = sections.get(section);
  if(sect == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_SECTION_NOT_FOUND;
	err->strerr = simpp::sprintf("Section '%' was not found within parsed file",internal_alloc,section);
	return err;
  }
  auto value = sect->get(field);
  if(value == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_FIELD_NOT_FOUND;
	err->strerr = simpp::sprintf("Field '%' not found within Section '%'",internal_alloc,field,section);
	return err;
  }
  bool passed = string_to_bool(*value,val);
  if(!passed){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_CONV_ERROR;
	err->strerr = simpp::sprintf("Field '%' with value '%' cannot be converted to integer",internal_alloc,field,*value);
	return err;
  }
  return nullptr;
}
void simpp::iniparser::create_section(string name){
  map<string,string> new_section = {};
  sections.set(name, new_section, internal_alloc);
}

simpp::iniparser_err *simpp::iniparser::set_field(string section, string field, string val){
  auto sect = sections.get(section);
  if(sect == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_SECTION_NOT_FOUND;
	err->strerr = simpp::sprintf("Section '%' was not found within parsed file",internal_alloc,section);
	return err;
  }
  auto value = sect->get(field);
  if(value == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_FIELD_NOT_FOUND;
	err->strerr = simpp::sprintf("Field '%' not found within Section '%'",internal_alloc,field,section);
	return err;
  }
  auto copied_val = string::copy(val, internal_alloc);
  *value = copied_val;
  return nullptr;
}

simpp::iniparser_err *simpp::iniparser::create_field(string section, string field, string val){
  auto sect = sections.get(section);
  if(sect == nullptr){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_SECTION_NOT_FOUND;
	err->strerr = simpp::sprintf("Section '%' was not found within parsed file",internal_alloc,section);
	return err;
  }
  auto copied_val = string::copy(val, internal_alloc);
  sect->set(field, copied_val, internal_alloc);
  return nullptr;
}

simpp::iniparser_err *simpp::iniparser::writefile(string path){
  int code = file::open_write(path, inifile);
  if(code < 0){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_FILE_FAILURE;
	err->strerr = simpp::sprintf("Failed to open file '%' for writing: '%'", internal_alloc,path,this->inifile.get_errormsg(internal_alloc));
	return err;
  }
  stringbuilder builder = stringbuilder::make(512, internal_alloc);
  auto global_sect = sections.get(global_section_name);
  write_section(*global_sect, global_section_name,builder);
  auto sections_slice = sections.toslice(internal_alloc);
  for(size_t i = 0; i < sections_slice.len;i++){
	if(sections_slice[i].key == global_section_name)
	  continue;
	write_section(sections_slice[i].data, sections_slice[i].key, builder);
  }
  builder.append_arr(string("\n#inifile built by simpp::iniparser"), internal_alloc);
  auto file_slice = builder.toslice();
  int n = inifile.write((cslice<byte>&)file_slice);
  if(n < 0){
	iniparser_err *err = (iniparser_err*)internal_alloc.alloc(sizeof(iniparser_err));
	err->code = INIPARSER_ERR_SECTION_NOT_FOUND;
	err->strerr = simpp::sprintf("Failed to write to %: '%'", internal_alloc,path,this->inifile.get_errormsg(internal_alloc));
	return err;
  }
  return nullptr;
}

void simpp::iniparser::write_section(map<string,string>& section,string section_name,stringbuilder& builder){
  auto sect_slice = section.toslice(internal_alloc);
  if(!((string&)global_section_name == section_name)){
	auto header_str = simpp::sprintf("[%]\n",internal_alloc,section_name);
	builder.append_arr(header_str, internal_alloc);
  }
  for(size_t i = 0; i < sect_slice.len;i++){
	builder.append_arr(sect_slice[i].key, internal_alloc);
	builder.append_arr(string("  =  "), internal_alloc);
	builder.append_arr(sect_slice[i].data, internal_alloc);
	builder.append('\n', internal_alloc);
  }
}

void simpp::iniparser::destroy(){
  internal_alloc.destroy();
  inifile.close();
  *this = {};
  return;
}
