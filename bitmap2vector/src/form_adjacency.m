function newNode = form_adjacency(M, node, pix_to_node_map)
	for i = 2:size(M)(1)-1
		for j = 2:size(M)(2)-1
			if(M(i,j,1) == 255 && M(i,j,2) == 255 && M(i,j,3) == 255)
				adj = [];
				if(M(i-1,j,1) == 255 && M(i-1,j,2) == 255 && M(i-1,j,3) == 255)
					adj = [adj, [pix_to_node_map(i-1,j)]];
				endif
				if(M(i+1,j,1) == 255 && M(i+1,j,2) == 255 && M(i+1,j,3) == 255)
					adj = [adj, [pix_to_node_map(i+1,j)]];
				endif
				if(M(i-1,j-1,1) == 255 && M(i-1,j-1,2) == 255 && M(i-1,j-1,3) == 255)
					adj = [adj, [pix_to_node_map(i-1,j-1)]];
				endif
				if(M(i-1,j+1,1) == 255 && M(i-1,j+1,2) == 255 && M(i-1,j+1,3) == 255)
					adj = [adj, [pix_to_node_map(i-1,j+1)]];
				endif
				if(M(i+1,j-1,1) == 255 && M(i+1,j-1,2) == 255 && M(i+1,j-1,3) == 255)
					adj = [adj, [pix_to_node_map(i+1,j-1)]];
				endif
				if(M(i+1,j+1,1) == 255 && M(i+1,j+1,2) == 255 && M(i+1,j+1,3) == 255)
					adj = [adj, [pix_to_node_map(i+1,j+1)]];
				endif
				if(M(i,j-1,1) == 255 && M(i,j-1,2) == 255 && M(i,j-1,3) == 255)
					adj = [adj, [pix_to_node_map(i,j-1)]];
				endif
				if(M(i,j+1,1) == 255 && M(i,j+1,2) == 255 && M(i,j+1,3) == 255)
					adj = [adj, [pix_to_node_map(i,j+1)]];
				endif
				node(pix_to_node_map(i,j)).adjacent_nodes = adj;
			endif
		endfor
	endfor
	newNode = node;
endfunction