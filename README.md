# Hugo's Shell

## TODO

- redireccionamiento >, >>, <
- pipe |
- bg &
- multiple commands &&
- cd: ../..,
- which

## Things that (may) work

- Config file: ./.hsllrc
- Path variables expansion
- Prompt (colors and path variables)
- cd (all except ../.. and - and advanced stuff)
- Aliases

## How to access shell variables

At least in my machine env variables such as HOST
are not accessible from hsll. I can fix it by
placing `export HOST` in the parent shell config.
