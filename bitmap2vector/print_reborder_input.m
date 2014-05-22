function print_reborder_input(curve, regions, col_code)
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
	fprintf(file_id, "%d\n", size(regions)(2));
	for i =1:size(regions)(2)
		fprintf(file_id, "%d\n",size(regions(i).path)(2));
		for j = 1:size(regions(i).path)(2)
			fprintf(file_id, "%d\n", regions(i).path(1, j) - 1);
		endfor
		fprintf(file_id, "%d\n%d\n%d\n", col_code(i,1), col_code(i,2), col_code(i, 3));
	endfor
	fflush(file_id);
endfunction