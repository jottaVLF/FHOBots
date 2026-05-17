# VSSScript

VSSScript is a small DSL for describing FHOBots VSS strategies with domain-level rules.

## Build

The parser is generated with Bison and the lexer with Flex through the normal Makefile:

```bash
make bin_dir
make
```

Generated files are ignored by git:

- `strategy/dsl/VssParser.cpp`
- `strategy/dsl/VssParser.hpp`
- `strategy/dsl/VssLexer.cpp`

## Validate a Strategy

```bash
./fhobotsTeam --check-strategy strategy/scripts/default.vss
```

This parses the file, runs semantic validation, and prints the interpreted strategy.

## Run With a Strategy

```bash
./fhobotsTeam sim strategy/scripts/default.vss
```

If no `.vss` file is passed, the program uses `strategy/scripts/default.vss`.

## Grammar Summary

```ebnf
program          ::= "STRATEGY" identifier statement_list
statement_list   ::= statement | statement_list statement
statement        ::= if_statement | action_statement
if_statement     ::= "IF" condition "THEN" action_list "END"
action_statement ::= action

condition        ::= "KEY" "SPACE"
                   | "BALL" "IN" field_area
                   | robot "HAS_BALL"
                   | robot "NEAR" "BALL"
                   | robot "IN" field_area

action           ::= robot "USE" identifier
                   | robot "GO_TO" "BALL"
                   | robot "GO_TO" "(" number "," number ")"
                   | robot "KICK" "GOAL"
                   | robot "KICK" "CENTER"
                   | robot "DEFEND" "GOAL"
                   | robot "HOLD" "DEFENSE"
                   | robot "MARK" "BALL"
                   | robot "SUPPORT" support_area

robot            ::= "ATTACKER" | "DEFENDER" | "GOALKEEPER"
                   | "ROBOT" number
field_area       ::= "ATTACK_AREA" | "DEFENSE_AREA" | "CENTER_AREA"
support_area     ::= "LEFT" | "RIGHT" | "CENTER"
```

`ROBOT 1`, `ROBOT 2`, and `ROBOT 3` map to goalkeeper, defender, and attacker respectively, matching the project message positions.

## Semantic Rules

- `ROBOT` numbers must be 1, 2, or 3.
- `USE <state>` must reference a state that exists for that robot role.
- `GOALKEEPER KICK CENTER` is rejected by the current semantic rules.
- `GO_TO (x,y)` is accepted and executed as a direct movement command to that field coordinate.

## Example

```txt
STRATEGY ataque_basico

IF BALL IN ATTACK_AREA THEN
  ATTACKER KICK GOAL
  DEFENDER HOLD DEFENSE
  GOALKEEPER DEFEND GOAL
END

IF ATTACKER HAS_BALL THEN
  ATTACKER KICK GOAL
END

IF BALL IN CENTER_AREA THEN
  ROBOT 3 GO_TO (320, 240)
END
```
