function [retVal1, retVal2] = check_unique_region_pixel(x, y)
	retVal1 = 0;
	if(!(x(1) == 255 && x(2) == 255 && x(3) == 255))
		if(isempty(y))
			y = [y; [x(1) x(2) x(3)]];
			retVal1 = 1;
		else
			flag = 0;
			for i = 1:size(y)(1)
				if(x(1) == y(i,1) && x(2) == y(i,2) && x(3) == y(i,3))
					flag = 1;
					break;
				endif
			endfor
			if(flag == 0)
				y = [y; [x(1) x(2) x(3)]];
				retVal1 = 1;
			endif
		endif
	endif
	retVal2 = y;
end