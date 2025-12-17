T_SPACE ::= [ \t]+
T_IDENTIFIER ::= [a-zA-Z_][a-zA-Z0-9_]*
T_NUMBER ::= [1-9][0-9]*
T_TEXT ::= .*
T_OPCODE ::= (NOP|ADD|SUB|...)

<line> ::=
  <label> <T_SPACE> <instruction> <T_SPACE> <comment> <optional_space>
  | <T_IDENTIFIER> <optional_space> = <optional_space> <argument> <optional_space>

<optional_space> ::=
  <T_SPACE>
  | \epsilon

<optional_colon> ::=
  :
  | \epsilon

<label> ::= . <optional_space> <T_IDENTIFIER> <optional_colon>

<comma> ::= <optional_space> , <optional_space>

<instruction> ::= <T_OPCODE> <instruction_arg1>
<instruction_arg1> ::=
  <comma> <argument> <instruction_arg2>
  | \epsilon
<instruction_arg2> ::=
  <comma> <argument>
  | \epsilon

<argument> ::=
  <T_NUMBER>
  | <T_IDENTIFIER>

<comment> ::=
  ; <T_TEXT>
  | \epsilon
