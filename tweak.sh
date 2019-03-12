#!/bin/bash

# This script will take a fractal 'name' (parameter set) and emit it and the
# result of tweaking each of its parmeters by plus or minus one.  Useful for
# experimenting around a parameter set to find related interesting ones.

declare -a coeffs=( $(echo {a..z} {A..Y} ) )
declare -A coeffnext=([Y]=Y)
declare -A coeffprev=([a]=a)
for ((i=0;i<$((${#coeffs[*]}-1));++i)); do
	coeffnext[${coeffs[$i]}]=${coeffs[$((i+1))]}
	coeffprev[${coeffs[$((i+1))]}]=${coeffs[$i]}
done

tweak() {
	local f="$1"
	local i
	echo "$f"
	for ((i=0;i<${#f};++i)); do
		local p=${f:0:$i}
		local c=${f:$i:1}
		local s=${f:$((i+1))}
		echo "${p}${coeffnext[$c]}${s}"
		echo "${p}${coeffprev[$c]}${s}"
	done
}

for arg in "$@" ; do
	tweak "$arg"
done
