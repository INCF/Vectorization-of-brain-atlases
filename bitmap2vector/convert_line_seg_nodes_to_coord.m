for i=1:size(line_seg)(2) + size(island_line_seg)(2)
	points(i).a = [];
	if(i <= size(line_seg)(2))
		for j = 1:size(line_seg(i).a)(2)
			points(i).a = [points(i).a ;[node(line_seg(i).a(1, j) + 1).x, node(line_seg(i).a(1, j) + 1).y]];
		endfor
	else
		for j = 1:size(island_line_seg(i - size(line_seg)(2)).a)(2)
			points(i).a = [points(i).a; [node(island_line_seg(i- size(line_seg)(2)).a(1, j) + 1).x, node(island_line_seg(i- size(line_seg)(2)).a(1, j) + 1).y]];
		endfor
	endif
endfor