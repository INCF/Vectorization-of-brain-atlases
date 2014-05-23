clear;
x = input("");
out = ["output_" x ".svg"];
x = [x ".png"];
img = imread(x);
M = popout_boundaries_color(img);
[cntrl_pt num_cntrl_pt adj_pix_coord node pix_to_node_map] = detect_control_points(M);
# x = input("run c++ code to create and store color code matrix with recursion limit = unlimited and read from popped_out_boundaries.png[argument to main] and write to create_color_code_matrix.m [indirection]");
# From here run your create_color_code_matrix
cd src
system("chmod +x script.sh");
system("./script.sh");
cd ..
run create_color_code_matrix.m
newNode = assign_adjacent_region_code(adj_pix_coord , node, matrix_code );
node = newNode;
node = form_adjacency(M, node, pix_to_node_map);
print_adj_list(node, num_cntrl_pt, adj_pix_coord);
# x = input("run c++ code to to get line segments and island line segments , input from adjacency.txt[indirection] , output to line_segments.m[indirection]");
# From here run your c++ code to get line segments and island line segments
system("g++ line_seg_formation.cpp -o lineSegments");
system("./lineSegments < adjacency.txt > line_segments.m");
run line_segments.m
run display_line_segments.m
run display_islands.m
run assign_line_seg_to_regions.m
print_points_for_path(node, line_seg, island_line_seg);
# x = input("run c++ code to to get optimize paths [gcc Bezcurvs.m GGVecLib.c -lm] input paths.txt ouput opt_curves.m");
system("gcc bezCurves.c GGVecLib.c -lm -o curve");
system("./curve < paths.txt > opt_curves.m");
run opt_curves.m
print_reborder_input(curve, regions, col_code, M);
# x = input("run rebordering code input input_rebordering... output output_rebordering...");
# x = input("Now copy the content of output_rebordering to svg file with proper height and width given");
system("g++ reborder_and_print_final_paths.cpp -o reborder");
myCmd = ["./reborder < input_for_rebordering.txt > " out];
system(myCmd);