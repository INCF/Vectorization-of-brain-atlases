
for i = 1:size(island_line_seg)(2)
	for j = 1:size(island_line_seg(i).a)(2)
		test_M(node(island_line_seg(i).a(j) + 1).x, node(island_line_seg(i).a(j) + 1).y, 1) = 255;
		test_M(node(island_line_seg(i).a(j) + 1).x, node(island_line_seg(i).a(j) + 1).y, 2) = 0;
		test_M(node(island_line_seg(i).a(j) + 1).x, node(island_line_seg(i).a(j) + 1).y, 3) = 0;
	endfor
endfor

imwrite(test_M, "islands.png");
imwrite(test_M, "islands.bmp");