function newNode = assign_adjacent_region_code(adj_pix_coord, node, matrix_code)
	for i = 1:size(node)(2)
		temp = [];
		pooh = cell2mat(adj_pix_coord(i));
		for j = 1:size(pooh)(1)
			temp = [temp, [matrix_code(pooh(j,1), pooh(j,2))]];
		endfor
		node(i).adj_regions = temp;
	endfor
	newNode = node;
endfunction
	