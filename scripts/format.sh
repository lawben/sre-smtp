#!/usr/bin/env bash

unamestr=$(uname)
if [[ "$unamestr" == 'Darwin' ]]; then
	clang_format="/usr/local/opt/llvm/bin/clang-format"
	$clang_format --version | grep "version 6.0" >/dev/null
	if [ $? -ne 0 ]; then
		echo "incompatible clang-format version detected in $clang_format"
	fi
	format_cmd="$clang_format -i -style=file '{}'"
elif [[ "$unamestr" == 'Linux' ]]; then
	format_cmd="clang-format -i -style=file '{}'"
fi

find src -iname "*.cpp" -o -iname "*.hpp" -not -path "src/catch/*" | xargs -I{} sh -c "${format_cmd}"
