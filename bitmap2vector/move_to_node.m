function [retVal1, retVal2] = move_to_node(all_node, id, temp, arrived_from)

	retVal2 = [];

	if(all_node(id).isCntrlPt)
		temp = [temp, [id]];
		retVal2 = temp;
		mum2 = "got a control point"
	else
		mum2 = "NOT a control point"
		for k = 1:size(all_node(id).adjacent_nodes)(2)
			if(!all_node(all_node(id).adjacent_nodes(1, k)).isUsedUp)
				mum2 = "insideAbove"
				all_node(all_node(id).adjacent_nodes(1, k)).isUsedUp = 1;
				all_node(all_node(id).adjacent_nodes(1, k)).id
				[all_node temp2] = move_to_node(all_node, all_node(all_node(id).adjacent_nodes(1, k)).id, temp, id);
				if(!isempty(temp))
					temp = [temp, temp2];
					retVal2 = temp;
				endif
			endif
		endfor
	endif

	retVal1 = all_node;
endfunction