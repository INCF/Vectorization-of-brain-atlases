test_M = M;
for i = 1:size(node)(2)
		test_M(node(i).x, node(i).y, 1) = 0;
		test_M(node(i).x, node(i).y, 2) = 0;
		test_M(node(i).x, node(i).y, 3) = 0;
	endfor
imwrite(test_M, "check_nodes.png");
imwrite(test_M, "check_nodes.bmp");