ulimit -s unlimited
cd $1/src/code_image
cmake .
make
cd $1/src
$1/src/code_image/code_image $1/popped_out_boundaries/$2 > $1/src/create_color_code_matrix.m 