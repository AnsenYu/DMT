#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 2) 
	{
		cout<<"fullmesh nodenum"<<endl;
		return 0;
	}
	int N = atoi(argv[1]);
	ofstream fout("fullmesh.txt");
	fout<<N<<" ";
	for(size_t i = 0; i<N; i++)
	{
		for(size_t j = 0; j<N; j++)
		{
			if(i == j){fout<<0<<" ";}
			else{fout<<1<<" ";}
		}
	}
	fout.close();
	return 0;
}
