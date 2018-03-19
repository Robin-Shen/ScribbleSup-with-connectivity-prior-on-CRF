Depends on:
1) Gurobi (http://www.gurobi.com/)
2) Boost (http://www.boost.org/)
3) OpenCV (https://opencv.org/)

Please edit the Makefile appropriately, and specify the directories of Boost and Gurobi.

The code used C++ 2014 and hence needs at least G++-4.9. However, Gurobi only officially support G++-4.8 and works apparently with G++-4.9 as well.
Therefore, G++-4.9 is the only supported compiler.


You have two compiling options.. You can use "make manual" to compile a more manual version of the code.
This has the option to manually draw scribbles instead of loading them from file, also output is shown in the terminal as well.
Usage: ./main_manual image.jpg superpixel_file scribble_file brushwidth timelimit lambda scribble_option scribble_file(optional)
If you choose to manually select scribbles, you can use the left mouse button to enforce connectivity.
Use the right mouse button if you do not want to enforce connectivity (e.g. background label).
You can use scribble_option "2" to load from file AND additionally draw new scribbles.


Use "make auto" for a more automated version. There is no check whether the call of main is correct. There is no output to the terminal. This might be a proper option if you need to run several images. The easiest way is to use a bashscript. There is an example included.
Usage: ./main_auto image.jpg superpixel.csv scribble.txt brushwidth timelimit lambda


A superpixel file can be generated using e.g.:
https://github.com/davidstutz/superpixel-benchmark
The superpixel csv file give the superpixel ID for every pixel.

Typical options:
Brushwidth between "1" and "3". Lambda: "0.2" Timelimit: "10"s.
One superpixel must only have one scribble/label. Otherwise the l0 heuristic will fail with a segfault.
Occasionally, the l0 heuristic fails to converge or produces some arbitrary segfaults. Retry with reduces brushwidth.
There is no check whether the provided input files exist. If they don't, the programm will fail silently.

Models:
You have to define the models to run in the main files! You have the following options.
"0": ILP-C
"1": ILP-PC (automatically select when there is NO unconnected label)
"2": LP-PC
"3": ILP-P
"4": ILP-PCB (automatically added when there is at least one unconnected label (e.g. background))
The l0-heuristic will always be called at first.

Example:
There are some example images, superpixel segs and scribbles from the Pascal VOC set in the example subfolder. To run them automatically, chmod +x example.sh and then ./example.sh
