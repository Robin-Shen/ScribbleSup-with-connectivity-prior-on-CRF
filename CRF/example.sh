#!/bin/bash

for filename in ./example/*_scribble.txt; do
	name=${filename##*/}
	base=${name%_scribble.txt}
	timeout 30s ./main_auto ./example/"$base.jpg" ./example/"$base".csv ./example/"$base""_scribble.txt" 2 10 0.2
done
