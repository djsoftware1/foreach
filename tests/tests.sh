#!/bin/sh
#
# https://github.com/djsoftware1/foreach
#
# Main basic automatic tests for for-each
#
# tests.sh
# 2026-01 - File created by David Joffe
# 

echo "for-each tests"

# Test if for-each is installed
if command -v for-each >/dev/null 2>&1; then
    HAVE_FOREACH=true
else
    HAVE_FOREACH=false
fi

if [ "$HAVE_FOREACH" = true ]; then
    echo for-each is installed
else
    echo for-each is not installed
fi

echo 'for-each test: ls *.txt | for-each echo'
#pause
ls *.txt | for-each echo
ls *.jpg | for-each echo

# weird recurse
#bash -c 'ls *.txt | for-each echo abc text main'

echo '=================================='

printf "foo\nbar\n" | for-each echo

echo '=================================='
# Show line numbers
ls *.md | for-each echo markdown file: '$# $*'

echo '----------------'
ls *.txt | for-each echo line: '$# $*'

echo '----------------'
cat ../src/main.cpp | for-each echo line '$#' '$*'
cat ../src/main.cpp | for-each --include-empty echo line '$#' '$*'

# This file
echo '----------------'
cat tests.sh | for-each echo Line '$#': '$*'

echo '----------------'
cat countries.txt | for-each echo '$# $*'
echo '=================================='

cat countries.txt | for-each echo
echo '=================================='

#cat words.tsv | for-each echo
#cat list.txt | for-each echo foreach runai -t '"Do X with $1"' --dummy#

#cat repos.txt | for-each echo git clone
cat repos.txt | for-each echo git clone
echo '=================================='

printf "apple\tfruit\ncarrot\tvegetable\n" \
  | for-each echo '"$1 is a $2"'
echo '=================================='
