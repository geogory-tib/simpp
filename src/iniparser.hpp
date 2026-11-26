#pragma once
#include "memory.hpp"
#include "containers.hpp"
#include "string.hpp"
#include "io.hpp"
/*
  iniparser.hpp:
  This header contains the .INI parser
  Please consult the comments above functions/structures for documentation

  INI FILE SYNTAX

  Sections:

  [Section]
  
  A section is a just a header the divides the ini file into parts
  and seperates fields. A section's name can have white space like the following
  [ Hello I Am a Section ]
  when the file is parsed, the leading and trailing whitespace is trimmed
  and will have the following name "Hello I am A Section". If you a programatticaly
  creating a ini file with the parser, it does not trim the section and must be accessed
  by the exact name you supplied. This parser does not have "subsections", but it can be
  emulated by declaring sections like this
  
  [Primary]
  ...
  [Primary.Secondary]
  ...
  
  Fields:
  
  Field = Value
  
  A field is just a key value pair shown as above. They are not "typed" and are just
  plain text values. A key or value cannot be split by white space like the following

  Cute Cat = Super Cute ; invaild

  Cat = Cute ; valid

  if you wish to have split words as a key or value you must wrap them in qoutes like this

  "Cute Cat" = "Super Cute"

  Cat = "(TAB CHAR)Super Cute "

  anything inbetween those qoutes will be included in the key's name or the value string.
  These qoutes cannot contain new lines.

  Global Section:

  This parser supports "global fields". This means any fields that precede a section
  header are put into the "global section". The parser will implicitly create a global
  section and and put all the fields that precede a header into that section.
  The const string global_section_name contains the name the parser uses for the global
  section. This section will not be made if you are creating a file programatticaly
  and do not explcitly create the section.

  Comments:

  This parser only supports single line comments which are start with either

  #Hello I am a comment
  or
  ;Hello I am a comment

 */
namespace simpp{
  const string global_section_name = "SIMPP_INIPARSER_GLOBAL_SECTION";
  // Error Codes retured from functions that can fail.
  enum iniparser_errors{
	INIPARSER_ERR_FILE_FAILURE = 0,
	INIPARSER_ERR_PARSE_FAILURE,
	INIPARSER_ERR_FIELD_NOT_FOUND,
	INIPARSER_ERR_SECTION_NOT_FOUND,
	INIPARSER_ERR_CONV_ERROR
  };
  /*
	struct iniparser_err:

	This structure is always returned as a pointer from a
	routine that can fail. Do not free any pointer to this struct returned from
	any routine in this library, it will mess with memory or abort the program

   */
  struct iniparser_err{
	string strerr;
	int code;
  };
  // internally used for lexing
  enum initokentype{
  TOK_EQ = 0,
  TOK_SECTION,
  TOK_SYMBOL,
  TOK_LNEND,
  TOK_EOF
  };
  // internally used for lexing.
  struct ini_token{
	simpp::string raw;
	initokentype type;
  };
  // internal lexing struct
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
  // interal parsing state
  struct iniparser_state{
   dynarray<ini_token>contents = {};
	size_t pos = 0;
	string current_section = {};
	ini_token pull_tok();
	ini_token peek_tok();
  };
  /*
	struct iniparser:
	This structure is the ini file parser.
	It stores all of it's allocated data in it's
	internal_allocator(growable arena).
	All allocated memory is freed when the destroy method is called.
	Any strings that are given to you by this struct are allocated by it's internal
	allocator and should not be destroyed. Also, strings
	given to you by the parser should be copied or converted to
	a int/bool/float/double if you wish to keep them
	around longer than the parser.
	String values you pass to the parser are copied 
	by the parser's own internal allocator and are freed
	when the parser is destoryed.
  */
  struct iniparser{
	// map to a map containg key value pairs
	map<string,map<string,string>> sections = {};
	/*
	  iniparsererr *parsefile(string filepath):

	  This method opens the file at string filepath
	  and parses the file contents.
	  On success it returns nullptr 
	  This function can return the following errors
	  
	  INIPARSER_ERR_FILE_FAILURE
	  INIPARSER_ERR_PARSE_FAILURE
	 */
	iniparser_err *parsefile(string filepath);
	/*
	  iniparser_err *writefile(string path):

	  This method open/creates a file to write the
	  parser's contents as a ini file.
	  On Success it returns nullptr
	  This function can return the following errord
	  
	  INIPARSER_ERR_FILE_FAILURE
	 */
	iniparser_err *writefile(string path);
	/*
	  iniparsererr *set_field(string section,string field,string value):
	  
	  This method will modify a existing field in a section.
	  If the section does not exist it will return an error.
	  On Success it returns nullptr
	  This function can return the following errors
	  
	  INIPARSER_ERR_FIELD_NOT_FOUND
	  
	  INIPARSER_ERR_SECTION_NOT_FOUND
	  
	 */
	iniparser_err *set_field(string section,string field,string value);
	/*
	  void create_section(string name):
	  
	  This function creates a section named with the
	  string provided in string name.
	  It cannot fail.
	 */
	void create_section(string name);
	/*
	  iniparser_err *create_field(string section,string field,string val);

	  This method creates a field within the provided section.
	  if the field already exists, it's value is replaced with the
	  provided value.
	  On Success it returns nullptr
	  This function can return the following error

	  INIPARSER_ERR_SECTION_NOT_FOUND
	  
	 */
	iniparser_err *create_field(string section,string field,string val);
	/*
	  iniparser_err *get_field(string section,string field,string&||bool&||int&||double& val);

	  This method places the fields value into the refrence provided in argument val.
	  Any string given from this function, is allocated by it's internal allocator
	  and will be destroyed  with the parser. For the other types provided the parser
	  will attempt to convert the field's value string into the requested type.
	  On success this function returns nullptr
	  This function can return the following errors
	  
	  INIPARSER_ERR_FIELD_NOT_FOUND,
	  
	  INIPARSER_ERR_SECTION_NOT_FOUND,
	  
	  INIPARSER_ERR_CONV_ERROR
	
	 */
	iniparser_err *get_field(string section,string field,string& val);
	iniparser_err *get_field(string section,string field,int& val);
	iniparser_err *get_field(string section,string field,float& val);
	iniparser_err *get_field(string section,string field,double& val);
	iniparser_err *get_field(string section,string field,bool& val);
	/*
	  void destroy():

	  This method frees the memory held by the
	  interal allocator and zeros the structure.
	  It can be used again after this.
	 */
	void destroy();

  private:
	void write_section(map<string,string>& section,string section_name, stringbuilder& builder);
	iniparser_err *parse_toks();
	iniparser_err *lex_contents();
	iniparser_state pstate = {};
	inilexer lexer = {};
	file inifile;
	garena internal_alloc;
  };
}
