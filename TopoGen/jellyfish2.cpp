#include <iostream>
#include <fstream>
#include <string>   
#include <stdio.h>   
#include <vector>
#include <stdlib.h>

using namespace std;   
#define MaxVertexNum 1000   
#define INF 32767   
typedef struct  
{   
    char vertex[MaxVertexNum];   
    int edges[MaxVertexNum][MaxVertexNum];   
    int n,e;   
}MGraph;   
 
void CreateMGraph(MGraph &G, int node, vector<vector<unsigned> >& conn)   
{   
    int i,j,k,p;   
    G.n = node;

    for (i=0;i<G.n;i++)   
    {   
        for (j=0;j<G.n;j++)   
        {   
	    if(conn[i][j] == 0 && i!=j)
	    {
	    	G.edges[i][j]=INF;   
		continue;
	    }
            if (i==j)   
            {   
                G.edges[i][j]=0;   
		continue;
            }   
	    G.edges[i][j]=1;
        }   
    }      
}   
//void Dispath(int A[][MaxVertexNum],int path[][MaxVertexNum],int n);
 
void Floyd(MGraph& G)
{
	cout<<"Start Floyd"<<endl;
	int A[MaxVertexNum][MaxVertexNum];
//,path[MaxVertexNum][MaxVertexNum];
	int i,j,k;
	for (i=0;i<G.n;i++)
	{
		for (j=0;j<G.n;j++)
		{
			A[i][j]=G.edges[i][j];
	//		path[i][j]=-1;
		}
	}

	cout<<"Begin Floyd"<<endl;

	for (k=0;k<G.n;k++)
	{
		for (i=0;i<G.n;i++)
		{
			for (j=0;j<G.n;j++)
			{
				if (A[i][j]>A[i][k]+A[k][j])
				{
					A[i][j]=A[i][k]+A[k][j];
				//	path[i][j]=k;
				}
			}
		}
	}
	cout<<"finish Floyd, begin to write"<<endl;

	ofstream fout("jellyfish.txt");
	fout<<G.n<<" ";
	for (i=0;i<G.n;i++)
	{
		for (j=0;j<G.n;j++)
		{
			fout<<A[i][j]<<" ";
		}
	}
	fout.close();
	//Dispath(A,path,G.n);
}
 
#if 0
void Ppath(int path[][MaxVertexNum],int i,int j)
{
	int k;
	k=path[i][j];
	if (k==-1)
	{
		return;
	}
	Ppath(path,i,k);
	printf("%d,",k);
	Ppath(path,k,j);
}
 
void Dispath(int A[][MaxVertexNum],int path[][MaxVertexNum],int n)
{
	int i,j;
	for (i=0;i<n;i++)
	{
		for (j=0;j<n;j++)
		{
			if (A[i][j]==INF)
			{
				if (i!=j)
				{
					printf("从%d到%d没有路径\n",i,j);
				}
			}
			else
			{
				printf("  从%d到%d=>路径长度:%d路径:",i,j,A[i][j]);
				printf("%d,",i);
				Ppath(path,i,j);
				printf("%d\n",j);
			}
		}
	}
}
#endif
 
int vecMax(vector<unsigned>& v, unsigned& pos)
{
	int max = -1;
	pos = 0;
	for(size_t i = 0; i<v.size(); i++)
	{
		if(v[i] > max)
		{
			max = v[i];
			pos = i;
		}
	}
	return max;
}

void ConstructJellyfish(vector<vector<unsigned> >& conn, unsigned port)
{
	vector<unsigned> RemainPort(conn.size(), port);
	for(size_t i = 0; i<conn.size()*conn.size(); i++)
	{
		unsigned sw1 = rand()%conn.size();
		unsigned sw2 = rand()%conn.size();
		if(RemainPort[sw1] == 0) continue;
		if(RemainPort[sw2] == 0) continue;
		conn[sw1][sw2] += 1; 
		conn[sw2][sw1] += 1;
		RemainPort[sw1] -= 1;
		RemainPort[sw2] -= 1;
	}
	unsigned pos;
	while(vecMax(RemainPort, pos)>1)
	{
		unsigned sw1 = rand()%conn.size();
		unsigned sw2 = rand()%conn.size();
		if(!conn[sw1][sw2]) continue;
		if(sw1 == pos || sw2 == pos) continue;
		conn[sw1][sw2] -= 1;
		conn[sw2][sw1] -= 1;

		conn[pos][sw1] += 1;
		conn[pos][sw2] += 1;
		conn[sw1][pos] += 1;
		conn[sw2][pos] += 1;
		RemainPort[pos] -= 2;
	}
}

int main()
{
	int nodenum = 980;
	int k = 1;//kth shortest path
	unsigned port = 24;//no of ports to connect switch
	vector<unsigned> dummy(nodenum, 0);
	vector<vector<unsigned> > conn(nodenum, dummy);
	
	cout<<"Begin to construct"<<endl;
	ConstructJellyfish(conn, port);
	cout<<"Construction completed"<<endl;

	MGraph G;
	CreateMGraph(G, nodenum, conn);
	cout<<"create complete"<<endl;
	Floyd(G);

	return 0;
}
