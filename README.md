# Hugo's Shell

## Things that (may) work

- Config file: ~/.hsllrc
- Path variables expansion
- Prompt (with colors and path variables)
- cd ('-' and strange stuff is not implemented)
- Aliases
- History nav! with up and down arrows
- History: ~/.hsll-hist to perserve hist between sessions
- History: reset (clear) input box on min offset
- Completion: file, dir+file, command, command options
- Completion: autocomplete on unique command and same prefix
- Background commands using `&`
- Command concatenation: `&&`, `;`
- Interactive apps from terminal (vi, less, ...)

## Compiling and installing

### Using hbuild

- Requisites: [hbuild](https://github.com/hugocotoflorez/hbuild)

- For installing globaly: `hbuild install`.
- for compile locally: `hbuild compile`

### Using make

- For installing globaly: `make install`.
- for compile locally: `make`

> [!NOTE]
> `~/.local/bin/` have to be on PATH

## How to access shell variables

At least in my machine env variables such as HOST
are not accessible from hsll. I can fix it by
placing `export HOST` in the parent shell config.

## TODO

- redireccionamiento >, >>, <
- completion for alias
- which (dont expand alias)
- left and right arrows

