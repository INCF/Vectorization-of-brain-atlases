function [pt, num, adj_pix, vert, pix_to_node_map] = detect_control_points(M)
	control_pt = [];
	nodeCounter = 1;
	num_control_points = 0;
	adj_regions_pix= cell(0,1);
	white_node_map = zeros(size(M)(1), size(M)(2));

	for i = 2:size(M)(1)-1
		for j = 2:size(M)(2)-1
			if(M(i,j,1) == 255 && M(i,j,2) == 255 && M(i,j,3) == 255)
				white_node_map(i, j) = nodeCounter;
				adj_region = [];
				region = [];
				[x adj_region] = check_unique_region_pixel(M(i, j-1, :), adj_region);
				if(x)
					region = [region; [i  (j-1)]];
				endif
				[x adj_region] = check_unique_region_pixel(M(i, j+1, :), adj_region);
				if(x)
					region = [region; [i  (j+1)]];
				endif
				[x adj_region] = check_unique_region_pixel(M(i-1, j-1, :), adj_region);
				if(x)
					region = [region; [i-1  (j-1)]];
				endif
				[x adj_region] = check_unique_region_pixel(M(i-1, j+1, :), adj_region);
				if(x)
					region = [region; [i-1  (j+1)]];
				endif
				[x adj_region] = check_unique_region_pixel(M(i+1, j-1, :), adj_region);
				if(x)
					region = [region; [i+1  (j-1)]];
				endif
				[x adj_region] = check_unique_region_pixel(M(i+1, j+1, :), adj_region);
				if(x)
					region = [region; [i+1  (j+1)]];
				endif
				[x adj_region] = check_unique_region_pixel(M(i-1, j, :), adj_region);
				if(x)
					region = [region; [i-1  (j)]];
				endif
				[x adj_region] = check_unique_region_pixel(M(i+1, j, :), adj_region);
				if(x)
					region = [region; [i+1  (j)]];
				endif

				if(size(region)(1) >= 3)
					num_control_points = num_control_points + 1;
					control_pt = [control_pt ; [i j size(region)(1) nodeCounter]];
					adj_regions_pix(end + 1, 1) = region;
					node(nodeCounter).id = nodeCounter;
					node(nodeCounter).isCntrlPt = 1;
					node(nodeCounter).isIslandPt = 0;
					node(nodeCounter).cntrlPtColNum = num_control_points;
					node(nodeCounter).isUsedUp = 0;
					node(nodeCounter).x = i;
					node(nodeCounter).y = j;
					nodeCounter = nodeCounter + 1;
				else
					adj_regions_pix(end + 1, 1) = region;
					node(nodeCounter).id = nodeCounter;
					node(nodeCounter).isCntrlPt = 0;
					node(nodeCounter).isIslandPt = 0;
					node(nodeCounter).cntrlPtColNum = -1;
					node(nodeCounter).isUsedUp = 0;
					node(nodeCounter).x = i;
					node(nodeCounter).y = j;
					nodeCounter = nodeCounter + 1;
				endif
			endif
		endfor
	endfor
	
	test_cntrl_pts(M, control_pt);
	pt = control_pt;
	num = num_control_points;
	adj_pix = adj_regions_pix;
	vert = node;
	pix_to_node_map = white_node_map;

endfunction