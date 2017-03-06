void ulib_delay(int time)
{
	int i,j,k;
	for(i=0;i<time;++i)
		for(j=0;j<1000;++j)
			for(k=0;k<100;++k);
}