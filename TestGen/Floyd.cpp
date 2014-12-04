#include "stdafx.h"
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

void Dispath(vector<vector<int> >& A,vector<vector<int> >& path,int n, vector<vector<int> >& allpaths);
 
void Floyd(MGraph& G, vector<vector<int> >& allpaths)
{
	//cout<<"Start Floyd"<<endl;
	vector<int> dummy(G.n, 0);
	vector<vector<int> > A(G.n, dummy);
	vector<vector<int> > path(G.n, dummy);

	int i,j,k;
	for (i=0;i<G.n;i++)
	{
		for (j=0;j<G.n;j++)
		{
			A[i][j]=G.edges[i][j];
			path[i][j]=-1;
		}
	}

	//cout<<"Begin Floyd"<<endl;

	for (k=0;k<G.n;k++)
	{
		for (i=0;i<G.n;i++)
		{
			for (j=0;j<G.n;j++)
			{
				if (A[i][j]>A[i][k]+A[k][j])
				{
					A[i][j]=A[i][k]+A[k][j];
					path[i][j]=k;
				}
			}
		}
	}



	//cout<<"finish Floyd, begin to write"<<endl;


	Dispath(A,path,G.n, allpaths);
}
 
void Ppath(vector<vector<int> >& path,int i,int j, vector<int>& curPath)
{
	int k;
	k=path[i][j];
	if (k==-1)
	{
		return;
	}
	Ppath(path,i,k,curPath);
	curPath.push_back(k);
	Ppath(path,k,j,curPath);
}
 
void Dispath(vector<vector<int> >& A,vector<vector<int> >& path,int n, vector<vector<int> >& allpaths)
{
	allpaths.clear();
	vector<int> dummy;
	int i,j;
	for (i=0;i<n;i++)
	{
		for (j=i+1;j<n;j++)
		{
			if (A[i][j]==INF)
			{
				;
			}
			else
			{
				allpaths.push_back(dummy);	   	
				vector<int>& curPath = *(allpaths.end()-1);
				curPath.push_back(i);
				Ppath(path,i,j,curPath);
				curPath.push_back(j);
			}
		}
	}
}
 
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

void FloydAllPaths(int nodenum, vector<vector<unsigned> >& conn, vector<vector<int> >& allpaths)
{
	MGraph G;
	CreateMGraph(G, nodenum, conn);
	Floyd(G, allpaths);
}
