function print_points_for_path(node, line_seg, island_line_seg, path_to_)
	file_id = fopen([path_to_ "/data_files/paths.txt"], 'w');
	fprintf(file_id, "%d\n", size(line_seg)(2) + size(island_line_seg)(2));
	for i = 1:size(line_seg)(2)
		fprintf(file_id, "%d\n", size(line_seg(i).a)(2));
		for j = 1:size(line_seg(i).a)(2)
			fprintf(file_id, "%d %d\n", node(line_seg(i).a(j) + 1).y, node(line_seg(i).a(j) + 1).x);
		endfor
	endfor

	for i = 1:size(island_line_seg)(2)
		fprintf(file_id, "%d\n", size(island_line_seg(i).a)(2));
		for j = 1:size(island_line_seg(i).a)(2)
			fprintf(file_id, "%d %d\n", node(island_line_seg(i).a(j) + 1).y, node(island_line_seg(i).a(j) + 1).x);
		endfor
	endfor
	fflush(file_id);
endfunction

