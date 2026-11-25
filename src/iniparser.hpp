#pragma once
#include "memory.hpp"
#include "containers.hpp"
#include "string.hpp"
#include "io.hpp"
namespace simpp{
  const string global_section_name = "SIMPP_INIPARSER_GLOBAL_SECTION";
  enum iniparser_errors{
	INIPARSER_ERR_FILE_FAILURE = 0,
	INIPARSER_ERR_PARSE_FAILURE,
	INIPARSER_ERR_FIELD_NOT_FOUND,
	INIPARSER_ERR_SECTION_NOT_FOUND,
	INIPARSER_ERR_CONV_ERROR
  };
  struct iniparser_err{
	string strerr;
	int code;
  };
  
  enum initokentype{
  TOK_EQ = 0,
  TOK_SECTION,
  TOK_SYMBOL,
  TOK_LNEND,
  TOK_EOF
  };
  struct ini_token{
	simpp::string raw;
	initokentype type;
  };
  
  struct inilexer{
	simpp::string contents;
	size_t pos;
	garena *internal_alloc;
	char pull_ch();
	char peek_ch();
	void eat_comment();
	ini_token parse_section_header(iniparser_err*& err);
	ini_token parse_qoutes(iniparser_err*& err);
	ini_token parse_symbol(size_t start_index);
  };

  struct iniparser_state{
   dynarray<ini_token>contents = {};
	size_t pos = 0;
	string current_section = {};
	ini_token pull_tok();
	ini_token peek_tok();
  };
  
  struct iniparser{
	map<string,map<string,string>> sections = {};
	iniparser_err *parsefile(string filepath);
	iniparser_err *writefile(string path);
	iniparser_err *set_field(string section,string field,string value);
	void create_section(string name);
	iniparser_err *create_field(string section,string field,string val);
	iniparser_err *get_field(string section,string field,string& val);
	iniparser_err *get_field(string section,string field,int& val);
	iniparser_err *get_field(string section,string field,float& val);
	iniparser_err *get_field(string section,string field,double& val);
	iniparser_err *get_field(string section,string field,bool& val);
	void destroy();

  private:
	void write_section(map<string,string>& section,string section_name, stringbuilder& builder);
	iniparser_err *parse_toks();
	iniparser_err *lex_contents();
	iniparser_state pstate = {};
	inilexer lexer = {};
	file inifile;
	garena internal_alloc;
	string strerror = {};	
	int errorcode = 0;
  };
}
