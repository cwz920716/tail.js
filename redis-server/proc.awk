BEGIN {
	sum_all = 0;
	sum_comput = 0;
	sum_io = 0;
	n = 0;
}
{
	sum_all += $1;
	sum_compute += $2;
	sum_io += $3;
	n += 1;
}
END {
	printf "%f\t%f\t%f\n", sum_all / n, sum_compute / n, sum_io / n
}
