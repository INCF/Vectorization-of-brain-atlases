clear;
img = imread("atlas_219.png");
M = popout_boundaries_color(img);
[cntrl_pt num_cntrl_pt adj_pix_coord node pix_to_node_map] = detect_control_points(M);
x = input("run c++ code to create and store color code matrix");
# From here run your create_color_code_matrix
run create_color_code_matrix.m
newNode = assign_adjacent_region_code(adj_pix_coord , node, matrix_code );
node = newNode;
node = form_adjacency(M, node, pix_to_node_map);
print_adj_list(node, num_cntrl_pt, adj_pix_coord);
x = input("run c++ code to to get line segments and island line segments");
# From here run your c++ code to get line segments and island line segments
run line_segments.m
run display_line_segments.m
run display_islands.m