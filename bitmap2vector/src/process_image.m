function process_image(x, path_to_, compileFlag)
	if(strcmp(compileFlag, "1"))
		printf("Compilation ON\n");
	else
		printf("Compilation OFF\n");
	endif
	fflush(stdout);

	out = [path_to_ "/output/output_" x ".svg"];
	y = x;
	x = [path_to_ "/input_atlas/" x ".png"];
	img = imread(x);
	M = popout_boundaries_color(img, y, path_to_);
	[cntrl_pt num_cntrl_pt adj_pix_coord node pix_to_node_map] = detect_control_points(M, y, path_to_);

	if(strcmp(compileFlag, "1"))
		system(["chmod +x " path_to_ "/src/compileScript.sh"]);
		system(["" path_to_ "/src/compileScript.sh " path_to_]);
	endif

	system(["chmod +x " path_to_ "/src/executeScript.sh"]);
	system(["" path_to_ "/src/executeScript.sh " path_to_ " " [y ".png"]]);

	run create_color_code_matrix.m
	newNode = assign_adjacent_region_code(adj_pix_coord , node, matrix_code );
	node = newNode;
	node = form_adjacency(M, node, pix_to_node_map);
	print_adj_list(node, num_cntrl_pt, adj_pix_coord, path_to_);

	if(strcmp(compileFlag, "1"))
		system(["g++ " path_to_ "/src/line_seg_formation.cpp -o " path_to_ "/bin/lineSegments"]);
	endif
	system([path_to_ "/bin/lineSegments < " path_to_ "/data_files/adjacency.txt > " path_to_ "/src/line_segments.m"]);

	run line_segments.m
	run display_islands_and_line_seg.m
	run assign_line_seg_to_regions.m
	print_points_for_path(node, line_seg, island_line_seg, path_to_);

	if(strcmp(compileFlag, "1"))
		system(["gcc " path_to_ "/src/bezCurves.c " path_to_ "/src/GGVecLib.c -lm -o " path_to_ "/bin/curve"]);
	endif
	system([path_to_ "/bin/curve < " path_to_ "/data_files/paths.txt > " path_to_ "/src/opt_curves.m"]);

	run opt_curves.m
	print_reborder_input(curve, regions, col_code, M, path_to_);

	if(strcmp(compileFlag, "1"))
		system(["g++ " path_to_ "/src/reborder_and_print_final_paths.cpp -o " path_to_ "/bin/reborder"]);
	endif
	system([path_to_ "/bin/reborder < " path_to_ "/data_files/input_for_rebordering.txt > " out]);
endfunction