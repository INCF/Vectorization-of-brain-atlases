clear;
img = imread("atlas_310.png");
M = popout_boundaries_color(img);
[cntrl_pt num_cntrl_pt adj_pix_coord node pix_to_node_map] = detect_control_points(M);
x = input("run c++ code to create and store color code matrix with recursion limit = unlimited and read from popped_out_boundaries.png[argument to main] and write to create_color_code_matrix.m [indirection]");
# From here run your create_color_code_matrix
run create_color_code_matrix.m
newNode = assign_adjacent_region_code(adj_pix_coord , node, matrix_code );
node = newNode;
node = form_adjacency(M, node, pix_to_node_map);
print_adj_list(node, num_cntrl_pt, adj_pix_coord);
x = input("run c++ code to to get line segments and island line segments , input from adjacency.txt[indirection] , output to line_segments.m[indirection]");
# From here run your c++ code to get line segments and island line segments
run line_segments.m
run display_line_segments.m
run display_islands.m
run assign_line_seg_to_regions.m
print_points_for_path(node, line_seg, island_line_seg);
x = input("run c++ code to to get optimize paths [gcc Bezcurvs.m GGVecLib.c -lm] input paths.txt ouput opt_curves.m");
run opt_curves.m
print_reborder_input(curve, regions, col_code);
x = input("run rebordering code input input_rebordering... output output_rebordering...");
x = input("Now copy the content of output_rebordering to svg file with proper height and width given");