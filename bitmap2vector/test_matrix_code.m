myImg = zeros(504, 644, 3);
myImg = uint8(myImg);
for i = 1:504
for j = 1:644
myImg(i, j, 1) = col_code(matrix_code(i, j) , 1);
myImg(i, j, 2) = col_code(matrix_code(i,j) , 2);
myImg(i, j, 3) = col_code(matrix_code(i,j) , 3);
end
end
imwrite(myImg, "test_code_reverse.png");
