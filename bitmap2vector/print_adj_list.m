function print_adj_list(node, num_cntrl_pt, adj_pix_coord)
	file_id = fopen('adjacency.txt', 'w');
	fprintf(file_id, "%d\n", size(node)(2));
	fprintf(file_id, "%d\n", num_cntrl_pt);
	for i =1:size(node)(2)
		fprintf(file_id, "%d\n%d\n%d\n",size(node(i).adj_regions)(2), node(i).isCntrlPt, size(node(i).adjacent_nodes)(2));
		for j = 1:size(node(i).adjacent_nodes)(2)
			fprintf(file_id, "%d\n", node(i).adjacent_nodes(1,j));
		endfor
		for j = 1:size(node(i).adj_regions)(2)
			fprintf(file_id, "%d\n", node(i).adj_regions(1,j));
		endfor
	endfor
	fflush(file_id);
endfunction