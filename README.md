# Hugo's Shell

## TODO

- redireccionamiento >, >>, <
- bg &
- multiple commands &&, ;
- completion for alias and not unique command names
- which (dont expand alias)

## Things that (may) work

- Config file: ./.hsllrc
- Path variables expansion
- Prompt (colors and path variables)
- cd ('-' and strange stuff is not implemented)
- Aliases
- History nav! with up and down arrows
- History: hist-file to perserve hist between sessions
- History: reset (clear) input box on min offset
- Completion: file, dir+file, command, command options
- Completion: autocomplete on unique command and same prefix

## How to access shell variables

At least in my machine env variables such as HOST
are not accessible from hsll. I can fix it by
placing `export HOST` in the parent shell config.
