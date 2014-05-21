function retVal = popout_boundaries_color(img)
	img = [img; zeros(1, size(img)(2), 3)];
	img = [zeros(1, size(img)(2), 3); img];
	img = [img, zeros(size(img)(1), 1, 3)];
	img = [zeros(size(img)(1), 1, 3), img];
	M = zeros(2*size(img)(1), 2*size(img)(2), 3);
	M = uint8(M);
	for i = 2:size(img(:,1,1))(1)
		count_i = 2 * i;
		for j = 2:size(img(1,:,1))(2)
			count_j = 2 * j;
			M(count_i,count_j, 1) = img(i,j,1);
			M(count_i,count_j, 2) = img(i,j,2);
			M(count_i,count_j, 3) = img(i,j,3);
			if((img(i,j,1) != img(i,j-1,1) || img(i,j,2) != img(i,j-1,2) || img(i,j,3) != img(i,j-1,3)))
				M(count_i,count_j-1, 1) = 255;
				M(count_i,count_j-1, 2) = 255;
				M(count_i,count_j-1, 3) = 255;
			else
				M(count_i,count_j-1, 1) = img(i,j-1,1);
				M(count_i,count_j-1, 2) = img(i,j-1,2);
				M(count_i,count_j-1, 3) = img(i,j-1,3);
			endif
			if((img(i,j,1) != img(i-1,j-1,1) || img(i,j,2) != img(i-1,j-1,2) || img(i,j,3) != img(i-1,j-1,3)))
				M(count_i-1,count_j-1, 1) = 255;
				M(count_i-1,count_j-1, 2) = 255;
				M(count_i-1,count_j-1, 3) = 255;
			else
				M(count_i-1,count_j-1, 1) = img(i-1,j-1,1);
				M(count_i-1,count_j-1, 2) = img(i-1,j-1,2);
				M(count_i-1,count_j-1, 3) = img(i-1,j-1,3);
			endif
			if((img(i,j,1) != img(i-1,j,1) || img(i,j,2) != img(i-1,j,2) || img(i,j,3) != img(i-1,j,3)))
				M(count_i-1,count_j, 1) = 255;
				M(count_i-1,count_j, 2) = 255;
				M(count_i-1,count_j, 3) = 255;
			else
				M(count_i-1,count_j, 1) = img(i-1,j,1);
				M(count_i-1,count_j, 2) = img(i-1,j,2);
				M(count_i-1,count_j, 3) = img(i-1,j,3);
			endif
		endfor
	endfor
	imwrite(M, "popped_out_boundaries.png");
	imwrite(M, "popped_out_boundaries.bmp");
	retVal = M;
endfunction