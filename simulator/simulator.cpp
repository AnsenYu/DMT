// simulator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "simu.h"




#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <cstdio>
#include <iostream>

using namespace rapidjson;
Document d;

extern int MAXIMUN_TIME_WINDOW;

void ReadTestConfig(char* fname);
void PrintDOM(Document& dom, char* fileName);


int main(int argc, char* argv[]) {
	if(argc != 7)	
	{
		printf("\nsimulator keylen speed vnetsize pnetsize method topologyfile\n");
		printf("OR: simulator keylen speed vnetsize pnetsize method NO_TOPO \n");
		return 0;
	}
	unsigned keylen, speed, vnetsize, pnetsize;
	keylen = atoi(argv[1]);
	speed = atoi(argv[2]);
	vnetsize = atoi(argv[3]);
	pnetsize = atoi(argv[4]);

	string str(argv[5]);
	if(str == "DMT")
	{
		method_ID = DMT;
	}
	else if(str == "DMTe2e")
	{
		method_ID = DMTe2e;
		CHILD_TTL = 1;
	}
	else if(str == "DMTe3e")
	{
		method_ID = DMTe3e;
		CHILD_TTL = 2;
	}
	else if(str == "SMT")
	{
		method_ID = SMT;
	}
	else if(str == "IDEAL")
	{
		method_ID = IDEAL;
	}
	else if(str == "OVX")
	{
		method_ID = OVX;
	}
	else if(str == "DMT_WO_O2M")
	{
		method_ID = DMT_WO_O2M;
	}
	else if(str == "DMT_WO_M2O")
	{
		method_ID = DMT_WO_M2O;
	}
	else if(str == "DMT_SPREAD")
	{
		method_ID = DMT_SPREAD;
	}
	else if(str == "DMT_SPREAD_WO_M2O")
	{
		method_ID = DMT_SPREAD_WO_M2O;
	}
	else if(str == "SMT_WO_M2O")
	{
		method_ID = SMT_WO_M2O;
	}
	else if(str == "DMT_GD")
	{
		method_ID = DMT_GD;
	}
	else if(str == "DMT_GD_WO_O2M")
	{
		method_ID = DMT_GD_WO_O2M;
	}
	else if(str == "DMT_GD_WO_M2O")
	{
		method_ID = DMT_GD_WO_M2O;
	}
	else
	{
		printf("Error method name");
		return 0;
	}
	char* topofile = argv[6];
	if(0 == strcmp(topofile, "NO_TOPO"))
	{
		NO_TOPO = true;	
		g_pnet_size = pnetsize;
		pnetsize = default_pnetsize;
	}


	char fname[256];
	sprintf(fname,"../TestGen/Data/keylen%d_speed%d_vnet%d_pnet%d.json",
		keylen, speed, vnetsize, pnetsize);
	printf("Read data from \"../TestGen/Data/keylen%d_speed%d_vnet%d_pnet%d.json\"\n",
		keylen, speed, vnetsize, pnetsize);


	ReadTestConfig(fname);
	printf("Load config file success!\n");

	Test test(d);
	printf("Test init success!\n");

	if(!NO_TOPO)
	{
		unsigned toposize = test.SetDistFromFile(topofile);
		if(toposize < pnetsize)
		{
			printf("Error: topo is smaller than pnet");
			return 0;
		}
	}

	test.PeriodRun();

	sprintf(fname,"Result/keylen%d_speed%d_vnet%d_pnet%d",
		keylen, speed, vnetsize, NO_TOPO?g_pnet_size:pnetsize);

	test.pf.Process(fname, test.GetSuccessRatio());
	printf("\nEnd!\n");

    return 0;
}

void ReadTestConfig(char* fname){
	/* Read from JSON file. */
	FILE* fp = fopen(fname, "rb"); // non-Windows use "r"
	assert(fp);
	char readBuffer[65536*2];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	/* Parse from stream*/
	d.ParseStream(is);
	fclose(fp);
}

void PrintDOM(Document& dom, char* fileName)
{
	assert(0 == access(fileName, 0));
	FILE* fpo = fopen(fileName, "wb"); // non-Windows use "r"
	char writeBuffer[65536];
	FileWriteStream os(fpo, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer3(os);

	dom.Accept(writer3);
	fclose(fpo);
}
