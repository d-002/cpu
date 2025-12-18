T_SPACE ::= [ \t]+
T_IDENTIFIER ::= [a-zA-Z_][a-zA-Z0-9_]*
T_NUMBER_BIN ::= 0b[01]+
T_NUMBER_HEX ::= 0x[0-9a-f]+
T_NUMBER_DEC ::= [0-9]+
T_REGISTER ::= %r[0-9]+
T_MEMORY ::= %m[0-9]+
T_PORT ::= %p[0-9]+
T_COMMENT ::= ;.*
T_OPCODE ::= (NOP|ADD|SUB|...)

<line> ::=
  <instruction_line> <optional_space> <comment>
  | <instruction_assignation> <optional_space> <comment>

<instruction_line> ::= <label> <instruction>
<instruction_assignation> ::= <T_IDENTIFIER> <optional_space> = <optional_space> <argument>

<optional_space> ::=
  <T_SPACE>
  | \epsilon

<optional_colon> ::=
  :
  | \epsilon

<label> ::=
  . <optional_space> <T_IDENTIFIER> <optional_colon>
  | \epsilon

<comma> ::= <optional_space> , <optional_space>

<instruction> ::=
  <T_SPACE> <T_OPCODE> <instruction_arg1>
  | \epsilon
<instruction_arg1> ::=
  <comma> <argument> <instruction_arg2>
  | \epsilon
<instruction_arg2> ::=
  <comma> <argument>
  | \epsilon

<number> ::=
  <T_NUMBER_BIN>
  | <T_NUMBER_HEX>
  | <T_NUMBER_DEC>

<argument> ::=
  <number>
  | <T_REGISTER>
  | <T_MEMORY>
  | <T_PORT>
  | <T_IDENTIFIER>

<comment> ::=
  <T_COMMENT>
  | \epsilon
