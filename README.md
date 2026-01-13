# for-each

**`for-each`** is a small, cross-platform Unix-style utility that executes a command once for each line of input, with positional variable expansion and no shell evaluation.

---

## Quick examples (TL;DR)

```sh
# Do a command for all folders in current directory
ls -d * | for-each echo

# Echo each line
printf "foo\nbar\n" | for-each echo

# Same idea, from a file
cat urls.txt | for-each echo
cat urls.txt | for-each wget

# Show line numbers
cat file.txt | for-each echo '$#'

# Clone many repositories
cat repos.txt | for-each git clone

# Use fields from tab-separated input
printf "apple\tfruit\ncarrot\tvegetable\n" | for-each echo "$1 is a $2"

# Use the full line explicitly
printf "hello world\n" | for-each echo "Input was: $*"
```

Note: Use single quotes around placeholders (to avoid shell expansion before being passed to for-each): ```'$1'   '$*'   '$#'```

## Example use

```sh
# Show help and exit
for-each -h
for-each --help

# Show version number and exit
for-each --version

# Do a command for all folders in current directory
ls -d */ | for-each echo

cat repo_list.txt | for-each echo

# Clone multiple repos
cat repo_list.txt | for-each git clone

s# Run a command with replacement
cat urls.txt | for-each wget

# Line numbers
cat src/main.cpp | for-each echo '$#'

# bulk operations or cowsay
printf "C++\nPython\nRust\nfor-each\nLinux\nWindows\nMac\nUnix\ncowsay\nChatGPT" | for-each sh -c 'echo " I love $1 " | cowsay'
```

### Git & Repository Management

```sh
# Clone many repos (one per line)
cat repos.txt | for-each git clone

# Or with submodules
cat repos.txt | for-each git clone --recurse-submodules
```

### File Processing & Renaming

```sh
# Auto-generate filenames and create files or folders
cat tasknames.txt | for-each touch 'tasks-$1.txt'

# Convert all markdown to HTML
ls *.md | for-each pandoc '$1' -o '$*.html'

# Compress images
ls *.png | for-each convert '$1' -quality 85 'optimized/$1'
```

### Data Processing Pipelines

```
# Process TSV/CSV with multi-field access

cat data.tsv | for-each \
    curl -X POST https://api.example.com -d "user=$1&action=$2&value=$3"

# Generate reports per user
cat users.txt | for-each generate-report --user $1 --output reports/$1.pdf
```

### AI / LLM Workflows

```sh
ls *.* | for-each runai -t "Say hi" --dummy

# Generate prompts for many countries
cat countries.txt | for-each runai -t '"List top 10 longevity research organizations in $1"'
```

### System Administration

```
# Restart services across many hosts (from file)
cat hosts.txt | for-each ssh '$1' sudo systemctl restart myapp

# Check disk usage on remote servers
cat servers.txt | for-each ssh '$1' df -h /

# Kill processes by name on multiple machines
cat machines.txt | for-each ssh '$1' pkill -f naughty-process
```

### Media & Bulk Operations

```sh
# Download many URLs
cat urls.txt | for-each wget
cat urls.txt | for-each wget -q '$1'

# Resize all videos in a directory
ls *.mp4 | for-each ffmpeg -i '$1' -vf scale=1280:720 r
```

## Features

- Reads lines from **stdin**
- Executes a command **once per line**
- Simple variable expansion:
  - `$1`, `$2`, … — positional fields
  - `$*` — entire input line
  - `$#` — line number (1-based)
- one input line → one argument
- Optional tab-separated fields detected automatically
- Works on **Linux, macOS, and Windows**
- No runtime dependencies

`for-each` is intentionally not a CSV (Comma-Separated Values) or text-processing language.

---

## Installation

### Build from source

Build with CMake using the helpers:

```sh
./build.sh
```

On Windows, build with:

```bat
./build.bat
```

To use, install to:

```
~/.local/bin/for-each
```

Ensure ~/.local/bin is in your PATH.

## License

MIT License

Copyright © 2026 David Joffe

## Author
David Joffe
https://djoffe.com
https://davidjoffe.github.io
https://www.tshwanedje.com
