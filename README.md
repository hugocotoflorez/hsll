# Hugo's Shell

## TODO

- redireccionamiento >, >>, <
- bg &
- multiple commands &&, ;
- completion
- which (dont expand alias)

## Things that (may) work

- Config file: ./.hsllrc
- Path variables expansion
- Prompt (colors and path variables)
- cd ('-' and strange stuff is not implemented)
- Aliases
- Up arrow restore prev command (history lenght of 1)

## How to access shell variables

At least in my machine env variables such as HOST
are not accessible from hsll. I can fix it by
placing `export HOST` in the parent shell config.
