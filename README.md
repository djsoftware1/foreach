# foreach
A small, portable Unix-style utility that executes a command once per input line, with predictable argument substitution and no shell evaluation.

## Example use

```sh
$ cat repo_list.txt | foreach git clone
```
