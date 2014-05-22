function print_reborder_input(curve)
	file_id = fopen('input_for_rebordering.txt', 'w');
	fprintf(file_id, "%d\n", size(curve)(2));
	for i =1:size(curve)(2)
		fprintf(file_id, "%d\n",size(curve(i).path)(1));
		for j = 1:size(curve(i).path)(1)
			for k = 1:6
				fprintf(file_id, "%f\n", curve(i).path(j, k));
			endfor
		endfor
	endfor
	fflush(file_id);
endfunction