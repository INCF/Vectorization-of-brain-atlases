for i = 1:size(col_code)(1)
	regions(i).path = [];
endfor

for i = 1:size(line_seg)(2)
	if(!node(line_seg(i).a(1) + 1).isCntrlPt)
		printf("Some ERROR\n");
	else
		if(size(node(line_seg(i).a(2) + 1).adj_regions)(2) != 2)
			printf("Maybe Some ERROR\n");
		else
			regions(node(line_seg(i).a(2)+1).adj_regions(1,1)).path = [regions(node(line_seg(i).a(2)+1).adj_regions(1,1)).path , i];
			regions(node(line_seg(i).a(2)+1).adj_regions(1,2)).path = [regions(node(line_seg(i).a(2)+1).adj_regions(1,2)).path , i];
		endif
	endif
endfor