function print_all_disjointed_line_segment(node, line_seg, island_line_seg, M)
	for i = 1:size(line_seg)(2)
		test_M = M;
		for j = 1:size(line_seg(i).a)(2)
			if(node(line_seg(i).a(1,j)+1).isCntrlPt)
				test_M(node(line_seg(i).a(j)+1).x, node(line_seg(i).a(j)+1).y, 1) = 255;
				test_M(node(line_seg(i).a(j)+1).x, node(line_seg(i).a(j)+1).y, 2) = 0;
				test_M(node(line_seg(i).a(j)+1).x, node(line_seg(i).a(j)+1).y, 3) = 0;
			else
				test_M(node(line_seg(i).a(j)+1).x, node(line_seg(i).a(j)+1).y, 1) = 0;
				test_M(node(line_seg(i).a(j)+1).x, node(line_seg(i).a(j)+1).y, 2) = 0;
				test_M(node(line_seg(i).a(j)+1).x, node(line_seg(i).a(j)+1).y, 3) = 0;
			endif
		endfor
		imwrite(test_M, ["/home/dhruv/bitmap2vector/segments/line_seg" num2str(i)  ".png"]);
	endfor
	for i = 1:size(island_line_seg)(2)
		test_M = M;
		for j = 1:size(island_line_seg(i).a)(2)
			if(node(island_line_seg(i).a(1,j)+1).isCntrlPt)
				test_M(node(island_line_seg(i).a(j)+1).x, node(island_line_seg(i).a(j)+1).y, 1) = 255;
				test_M(node(island_line_seg(i).a(j)+1).x, node(island_line_seg(i).a(j)+1).y, 2) = 0;
				test_M(node(island_line_seg(i).a(j)+1).x, node(island_line_seg(i).a(j)+1).y, 3) = 0;
			else
				test_M(node(island_line_seg(i).a(j)+1).x, node(island_line_seg(i).a(j)+1).y, 1) = 0;
				test_M(node(island_line_seg(i).a(j)+1).x, node(island_line_seg(i).a(j)+1).y, 2) = 0;
				test_M(node(island_line_seg(i).a(j)+1).x, node(island_line_seg(i).a(j)+1).y, 3) = 0;
			endif
		endfor
		imwrite(test_M, ["/home/dhruv/bitmap2vector/segments/island_line_seg" num2str(i)  ".png"]);
	endfor

endfunction