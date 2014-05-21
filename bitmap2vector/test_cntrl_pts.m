function test_cntrl_pts(M, control_pt)
	test_M = M;
	for i = 1:size(control_pt)(1)
		test_M(control_pt(i, 1), control_pt(i, 2), 1) = 0;	
		test_M(control_pt(i, 1), control_pt(i, 2), 2) = 0;
		test_M(control_pt(i, 1), control_pt(i, 2), 3) = 139;
	endfor
	imwrite(test_M, "test_cntrl_pts.png");
	imwrite(test_M, "test_cntrl_pts.bmp");
endfunction