// TestGen.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <cstdio>
#include <iostream>
//#include <io.h>

#include <limits>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "GraphElements.h"
#include "Graph.h"
#include "DijkstraShortestPathAlg.h"
#include "YenTopKShortestPathsAlg.h"

#include "conf.h"
#include "possion.h"
#include "Floyd.h"

#define SHORTEST_KEY 8
#define LONGEST_KEY 640
#define MAX_TIME_WINDOW 30

using namespace std;
using namespace rapidjson;
Document d;

int max_time_wnd = 0;

int STRICT_PIPELINE_LIMIT = 0;

void ReadTestConfig();
void PrintDOM(Document& dom, const char* fileName);
void SetConfManual(singleConf& c, unsigned, unsigned, unsigned, unsigned);

void ModifyPnet(Value& v, singleConf& c)
{
	assert(v.HasMember("node"));
	v["node"].SetInt(c.pnode_num);
	Value& pl = v["pipeline"];
	pl[0]["capacity"].SetInt(c.stage_capacity);
	for(int i = 1; i<c.pnode_stage_num; i++)
	{
		Value tmp(pl[0], d.GetAllocator());
		pl.PushBack(tmp, d.GetAllocator());
	}
}


void testYenAlg( const int& k, 
	             std::vector<Link>& lk, 
				 const int& size,
				 const int& nodes,
				 std::vector<std::vector<int> >& paths)
{
	std::vector<int> dummy;

	Graph my_graph;

	my_graph.set_number_vertices( nodes );

	for ( int i = 0; i < size; i++ )
	{
		my_graph.add_link( lk[ i ].u, lk[ i ].v, lk[ i ].weight );
	}	

	my_graph.setv();
	
	for(int s = 0; s<nodes; s++)
	{
		for(int d = s+1; d<nodes; d++)
		{
			YenTopKShortestPathsAlg yenAlg(my_graph, 
										   my_graph.get_vertex(s), 
										   my_graph.get_vertex(d) );

			// Output the k-shortest paths
			int i = 0;
			while( yenAlg.has_next() && i < k )
			{
				++i;
				paths.push_back(dummy);
				yenAlg.next()->PrintToVector(*(paths.end()-1));
			}
		}
	}
}

void GenerateLinkAndPaths(const int nodenum, const double prob, Value& net)
{
	std::vector<std::vector<int> > paths;
	std::vector<unsigned>  dummy(nodenum, 0);
	std::vector<std::vector<unsigned> > conn(nodenum, dummy);

	for(size_t i = 0; i<nodenum; i++)
	{
		for(size_t j = i+1; j<nodenum; j++)
		{
			if(U_Random() < prob)
			{
				conn[i][j] = 1;
				conn[j][i] = 1;
			}
		}
	}

	FloydAllPaths(nodenum, conn, paths);

	
#if 0
	int k = 3;
	int array_size = lks.size();
	int nodes = nodenum;


	testYenAlg( k, 
		        lks, 
				array_size, 
				nodes,
				paths);
#endif



	assert(net.HasMember("paths"));
	Value p(net["paths"][0], d.GetAllocator());
	net["paths"].PopBack();
	for(size_t i = 0; i<paths.size(); i++)
	{
		Value newpath(p, d.GetAllocator());
		newpath["path"].SetArray();
		for(size_t j = 0; j<paths[i].size(); j++)
		{
			newpath["path"].PushBack(paths[i][j], d.GetAllocator());
		}
		net["paths"].PushBack(newpath, d.GetAllocator());
	}

}

void ModifyVnets(Value& v, singleConf& c)
{
	Value rawVnet(v[0], d.GetAllocator());
	v.PopBack();
	int netID = 0;
	for(int time = 1; time < MAX_TIME_WINDOW; time++)
	{
		int num = possion((double)c.vnet_per_time);
		for(int k = 0; k<num; k++)
		{
			Value net(rawVnet, d.GetAllocator());
			net["vnetID"].SetInt(netID++);
			net["start"].SetInt(time);
			int period = randomExponential(c.time_length);
			while(period == 0)
			{
				period = randomExponential(c.time_length);
			}
			if(time + period > max_time_wnd){max_time_wnd = time + period;}
			net["end"].SetInt(time + period);
			int vnodeNum = U_Random() * c.vnet_size * 2;
			while(vnodeNum < 2){vnodeNum = U_Random() * c.vnet_size * 2;}
			Value sw(net["switches"][0], d.GetAllocator());
			net["switches"].PopBack();
			for(int i = 0; i<vnodeNum; i++)
			{
				Value newSw(sw, d.GetAllocator());
				Value stage(newSw["pipeline"][0], d.GetAllocator());
				newSw["pipeline"].PopBack();
				int pLen = U_Random() * c.pipeline_len * 2;
				while(1){
					pLen = U_Random() * c.pipeline_len * 2;
					if(pLen < 1)continue;
					if(STRICT_PIPELINE_LIMIT && pLen > c.pnode_stage_num) continue;
					break;
				}
				for(int j = 0; j<pLen; j++)
				{
					Value newStage(stage, d.GetAllocator());
					
					//这里是设置virtual stage width的期望宽度
					int width = randomExponential(c.keylen);
					while(width < SHORTEST_KEY || width > LONGEST_KEY){width = randomExponential(c.keylen);}
					int depth = U_Random() * c.stage_capa * c.stage_capacity / width;
					newStage["depth"].SetInt(depth);
					newStage["width"].SetInt(width);
					newSw["pipeline"].PushBack(newStage, d.GetAllocator());
				}
				net["switches"].PushBack(newSw, d.GetAllocator());
			}
			GenerateLinkAndPaths(vnodeNum, c.link_prob, net);
			v.PushBack(net, d.GetAllocator());
		}
	}
}

int main(int argc, char* argv[])
{
	printf("argc is %d\n", argc);
	if(argc != 5)
	{
	    printf("\nEnter in this way, \"TestGen keylen speed vnetsize pnetsize\"\n");
	    return 0;
	}
	unsigned keylen = atoi(argv[1]);
	unsigned speed = atoi(argv[2]);
	unsigned vnetsize = atoi(argv[3]);
	unsigned pnetsize = atoi(argv[4]);
	
	printf("Start!\n keylen = %d, speed = %d, vnetsize = %d, pnetsize = %d\n",
		 keylen, speed, vnetsize, pnetsize);
	Document::AllocatorType& allocator = d.GetAllocator();
	
	singleConf c;
	SetConfManual(c, keylen, speed, vnetsize, pnetsize);

	{
		max_time_wnd = 0;

		ReadTestConfig();

		assert(d.HasMember("pnet"));

		ModifyPnet(d["pnet"], c);

		assert(d.HasMember("vnets"));

		ModifyVnets(d["vnets"], c);

		assert(d.HasMember("max_time_window"));
		d["max_time_window"].SetInt(max_time_wnd+1);

		char buffer[256];
		string fstr("Data/");
		
		sprintf(buffer, "keylen%d", c.keylen);
		fstr += buffer;

		sprintf(buffer, "_speed%d", c.vnet_per_time);
		fstr += buffer;

		sprintf(buffer, "_vnet%d", c.vnet_size);
		fstr += buffer;

		sprintf(buffer, "_pnet%d", c.pnode_num);
		fstr += buffer;

		sprintf(buffer, ".json");
		fstr += buffer;
		PrintDOM(d, fstr.c_str());
		printf("outputfile is %s\n", fstr.c_str());
	}
	printf("End!\n");
	return 0;
}

void ReadTestConfig(){
	/* Read from JSON file. */
	FILE* fp = fopen("raw_config.json", "rb"); // non-Windows use "r"
	assert(fp);
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	/* Parse from stream*/
	d.ParseStream(is);
	fclose(fp);
}

void PrintDOM(Document& dom, const char* fileName)
{
	//assert(0 == access(fileName, 0));
	FILE* fpo = fopen(fileName, "wb"); // non-Windows use "r"
	char writeBuffer[65536];
	FileWriteStream os(fpo, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer3(os);

	dom.Accept(writer3);
	fclose(fpo);
}

void SetConfManual(singleConf& c, unsigned keylen, unsigned speed, unsigned vnetsize, unsigned pnetsize)
{
	c.link_prob = 0.5;// link probability per pair
	c.pipeline_len = 4;// 6 stage per virtual pipeline
	c.pnode_num = pnetsize;// 1k physical node
	c.pnode_stage_num = 4; // 4 stages per physical pipeline
	c.keylen = keylen;

	c.stage_capa = (0.1); // each virtual stage is 0~0.1 time of physical stage capacity
	c.stage_capacity = (1280000);//16X2KX40b per stage
	c.time_length = (3);// vnet duratio, 3 time window expected
	c.vnet_per_time = (speed);// vnet request arriving speed
	c.vnet_size = (vnetsize);// expected vnet size(80 vnodes per vnet)
}
