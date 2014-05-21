test_M = M;
for i = 1:size(line_seg)(2)
	for j = 1:size(line_seg(i).a)(2)
		test_M(node(line_seg(i).a(j) + 1).x, node(line_seg(i).a(j) + 1).y, 1) = 0;
		test_M(node(line_seg(i).a(j) + 1).x, node(line_seg(i).a(j) + 1).y, 2) = 0;
		test_M(node(line_seg(i).a(j) + 1).x, node(line_seg(i).a(j) + 1).y, 3) = 0;
	endfor
endfor
imwrite(test_M, "line_segments.png");
imwrite(test_M, "line_segments.bmp");