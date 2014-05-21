function [code_mat, col_code] = get_image_code_matrix(img)
	for i = 1:size(img)(1)
		for j = 1:size(img)(2)
			code_matrix(i,j).code = -1;
		endfor
	endfor
	counter = 1;
	for i = 1:size(img)(1)
		for j = 1:size(img)(2)
			if(code_matrix(i,j).code == -1)
				color_code(counter).r = img(i,j,1);
				color_code(counter).g = img(i,j,2);
				color_code(counter).b = img(i,j,3);
				moveTo(img, code_matrix, i, j, counter);
				counter = counter + 1;
			endif
		endfor
	endfor
	code_mat = code_matrix;
	col_code = color_code;
endfunction