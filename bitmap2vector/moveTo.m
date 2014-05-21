function moveTo(img, code_matrix, i, j, counter)
	if(j - 1 >= 1)
		if(code_matrix(i, j-1).code == -1 && sum(img(i, j, :) == img(i, j-1, :)) == 3)
			code_matrix(i, j-1).code = counter;
			moveTo(img, code_matrix, i, j-1, counter);
		endif
	endif
	if(j + 1 <= size(img)(2))
		if(code_matrix(i, j+1).code == -1 && sum(img(i, j, :) == img(i, j+1, :)) == 3)
			code_matrix(i, j+1).code = counter;
			moveTo(img, code_matrix, i, j+1, counter);
		endif
	endif
	if(i - 1 >= 1)
		if(code_matrix(i-1, j).code == -1 && sum(img(i, j, :) == img(i - 1, j, :)) == 3)
			code_matrix(i-1, j).code = counter;
			moveTo(img, code_matrix, i-1, j, counter);
		endif
		if(j - 1 >= 1)
			if(code_matrix(i-1, j-1).code == -1 && sum(img(i, j, :) == img(i - 1, j-1, :)) == 3)
				code_matrix(i-1, j-1).code = counter;
				moveTo(img, code_matrix, i-1, j-1, counter);
			endif
		endif
		if(j + 1 <= size(img)(2))
			if(code_matrix(i-1, j+1).code == -1 && sum(img(i, j, :) == img(i - 1, j+1, :)) == 3)
				code_matrix(i-1, j+1).code = counter;
				moveTo(img, code_matrix, i-1, j+1, counter);
			endif
		endif
	endif
	if(i + 1 <= size(img)(1))
		if(code_matrix(i+1, j).code == -1 && sum(img(i, j, :) == img(i + 1, j, :)) == 3)
			code_matrix(i+1, j).code = counter;
			moveTo(img, code_matrix, i+1, j, counter);
		endif
		if(j - 1 >= 1)
			if(code_matrix(i+1, j-1).code == -1 && sum(img(i, j, :) == img(i + 1, j-1, :)) == 3)
				code_matrix(i+1, j-1).code = counter;
				moveTo(img, code_matrix, i+1, j-1, counter);
			endif
		endif
		if(j + 1 <= size(img)(2))
			if(code_matrix(i+1, j+1).code == -1 && sum(img(i, j, :) == img(i + 1, j+1, :)) == 3)
				code_matrix(i+1, j+1).code = counter;
				moveTo(img, code_matrix, i+1, j+1, counter);
			endif
		endif
	endif
endfunction