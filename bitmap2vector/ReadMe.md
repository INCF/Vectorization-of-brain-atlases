Vectorization-of-brain-atlases
--------------------------------

**Requirements**

For now, octave, cmake and openCV with usual g++ and gcc compilers are required for compilation.
Only octave is required for execution.

In future, only gcc/g++ compilers will be requied for compilation and genral system without octave will suffice for execution.

**How to use**

Copy the atlas bitmap image into the "input_atlas" folder and then run the following terminal commands

- cd /path/to/bitmap2vector/directory
- chmod +x main.sh
- ./main.sh name_of_image_without_extension compile_flag

NOTE:
- compile_flag is either 1 or 0 i.e. to make compilation ON, argument should be 1 else 0
- There must be two arguments to the ./main.sh

Your output svg will be in "output" folder in bitmap2vector directory.