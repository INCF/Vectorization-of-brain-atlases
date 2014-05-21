function [line_seg, new_node] = form_segments(control_pt, old_node)
	
	lines = cell(0,1);
	node = old_node;
	for i = 1:size(control_pt)(1)
		n = control_pt(i, 4);
		n
		node(n).isUsedUp = 1;
			for j = 1:size(node(n).adjacent_nodes)(2)
				temp = [];
				if(!node(node(n).adjacent_nodes(1,j)).isUsedUp)
					mum = "first loop in "
					node(n).adjacent_nodes(1,j)
					node(node(n).adjacent_nodes(1,j)).isUsedUp = 1;
					[node, temp] = move_to_node(node, node(node(n).adjacent_nodes(1,j)).id, temp, n);
					if(!isempty(temp))
						lines(end+1, 1) = temp;
					endif
				endif
			endfor
	endfor
	line_seg = lines;
	new_node = node;
endfunction