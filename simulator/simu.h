#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <assert.h>
#include <algorithm>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

using namespace std;
enum{NONE=0, DMT, DMTe2e, DMTe3e, SMT, IDEAL, OVX, DMT_WO_O2M, DMT_WO_M2O, DMT_SPREAD, DMT_SPREAD_WO_M2O, SMT_WO_M2O, DMT_GD, DMT_GD_WO_O2M, DMT_GD_WO_M2O};

extern int MAXIMUN_TIME_WINDOW;
extern unsigned method_ID;
extern const char* method_name[];
extern int CHILD_TTL;
extern bool NO_TOPO;
extern unsigned g_pnet_size;
extern unsigned default_pnetsize;


extern int MAX_LINE_WIDTH;
const int MAX_GLOBAL_VNODE = 65535;



class Config{
public:
	bool smartKey;
	bool stageSplit;
	bool nodeSplit;
	void AssignConf(rapidjson::Value& val);
};

class GlobalIDMaster;
class Vnet;
class Vnode;

class GlobalIDMaster{
public:
	std::vector<Vnode*> pool;
	void RegisterNewVnode(Vnode* ptr);
	void DeleteVnode(unsigned globalID);
};

class AllocVnode2Pnode
{
public:
	unsigned vnodeID; //global ID for a vnode
	unsigned pnodeID; 
	std::vector<int> alloc;//allocated lines per stage,长度可能由于stage split造成跟vnode和pnode均不等
	unsigned resource;
};

class Vnode{
private:
	unsigned vnetID;
	unsigned vnodeID;
	unsigned globalID;
	GlobalIDMaster* pgm;
	Vnode* pchild;//指向分裂出去的后继子node
	AllocVnode2Pnode* palloc;
	//std::vector<int> nexthop; //a vector of next hop's local vnodeID to all other virtual nodes, -1 for itself
	unsigned childTTL;

public:
	bool IsSplitable(){return this->childTTL>0;}
	Vnode* GetPchild(){return this->pchild;}

	std::vector<int> depth;
	std::vector<int> width; 
	Vnet* pvnet;
	unsigned GetGID(){return this->globalID;}
	unsigned GetNID(){return this->vnetID;}
	void SetGID(unsigned gid){ this->globalID = gid;}
	void SetAlloc(unsigned pID, std::vector<int>& width, std::vector<int>& alloc);
	AllocVnode2Pnode* GetAlloc(){return palloc;}

	Vnode* MakeChildNode(std::vector<int>& depth, std::vector<int>& width);
	void AssignNode(std::vector<int> d, std::vector<int> w, 
		unsigned netID, unsigned nodeID, GlobalIDMaster& gm);
	~Vnode(){
		this->pgm->DeleteVnode(this->globalID);
		if(palloc)
			delete this->palloc;
	}
};



typedef std::map<unsigned,std::vector<int> > VNET2STAGE;
typedef std::map<unsigned,std::vector<int> > VNODE2STAGE;

class Pnode{
private:
	std::vector<int> statePerStage;//"true" means free, "false" means occupied
	std::vector<int> capaPerStage;// total capacity
	std::vector<int> depthPerStage;// free depth
	std::vector<int> widthPerStage;// valid when state is "false".
	std::vector<int> widthOption;
	VNODE2STAGE vnodeAlloc;// record the allocation on all stage for each vnode
	std::vector<unsigned> vnodes;
	unsigned nodeID;

	
	void popVnode(unsigned vnodeID);
	int chooseWidth(int width);
public:
	bool CalCost(Vnode v, double& cost);
	GlobalIDMaster* pgm;
	unsigned GetAbsLoad();
	unsigned GetAbsCapacity();
	double GetLoadRatio();
	unsigned GetID(){return this->nodeID;}
	bool push(Vnode& v, Config& c, GlobalIDMaster& gm, Vnode*& leftover);
	void popVnet(unsigned vnetID);
	void AssignNode(unsigned nodeID, std::vector<int>& capacity, std::vector<int> width, GlobalIDMaster* pgm);
	bool empty()
	{
		return this->vnodes.empty();
	}
};



class Vnet{
private:
	unsigned vnetID;
	unsigned Tbegin;
	unsigned Tend;
	unsigned originNodeNum;
	std::vector<Vnode*> vnodes;
	std::stack<Vnode*> vnodesStack;
	std::vector<std::vector<unsigned> > paths;
	unsigned resource;
public:
	
	const unsigned GetStackDepth(){return vnodesStack.size();}
	std::vector<std::vector<int> > neighbor;
	Config conf;
	unsigned CountPhysicalNodes();
	const unsigned GetOriginalVnetSize() {return this->originNodeNum;}
	const unsigned GetChangedVnetSize() {return this->vnodes.size();}
	const unsigned GetResource(){return this->resource;}
	const unsigned GetAllocResource();
	const unsigned GetTbegin(){return this->Tbegin;}
	const unsigned GetTend(){return this->Tend;}
	std::vector<std::vector<unsigned> >& GetPaths(){return this->paths;}
	const unsigned GetOriginalNodeNum(){return this->originNodeNum;}
	const std::vector<Vnode*>& GetVnodes(){return this->vnodes;}

	void AssignNet(rapidjson::Value& val, GlobalIDMaster& gm,
			std::vector<unsigned>& widthsOption);
	void PushNewVnode(Vnode* p);
	bool Empty();
	unsigned GetVnetID(){
		return vnetID;
	}
	Vnode* PopVnode();
	~Vnet(){
		for(size_t i = 0; i<this->vnodes.size(); i++)
		{
			delete vnodes[i];
		}
	}
};

class Pnet{
private:
	std::vector<Pnode*> pnodes;
	std::vector<Vnet*> vnets;
	long long resource;
public:
	std::vector<unsigned> widthsOption;
	GlobalIDMaster* pgm;
	bool FindBestFitPnode(Vnode& vnode, unsigned& pnodeIdx, const std::vector<bool>& NodesUsed);
	void NodeLoadDistribution(std::vector<double>& dstribution);
	void NodeLoadSort(std::vector<unsigned>& sorted_index);
	unsigned NodeSumLoad();
	unsigned NodeSumCapacity();
	void AssignNet(unsigned num, std::vector<int>& capacity, std::vector<int>& width);
	void AssignNet(rapidjson::Value& val, GlobalIDMaster* pgm);
	~Pnet()
	{
		for(size_t i = 0; i<this->pnodes.size(); i++)
		{
			delete this->pnodes[i];
		}
	}
	bool PushVnet(Vnet& vnet, GlobalIDMaster&);
	bool PushVnetIdeal(Vnet& vnet, GlobalIDMaster&);
	void PopVnet(Vnet* pvnet);
	void PopVnetIdeal(Vnet* pvnet);
	void GetUsedNodes(std::vector<Pnode*>& usedNode);
};

class Profile
{
public:
	std::vector<double> vpathStretchRaw;
	std::vector<double> physicalpathStretchRaw;
	//std::vector<double> vStretchDis;
	//std::vector<double> pStretchDis;
	long long WorkingRequest;
	long long MaxWorkingRequest;
	std::vector<long long> vec_workingRequest;

	std::vector<double> LoadDistribution;//按10%分档
	std::vector<double> LoadDistributionHD;//按1%分档
	std::vector<double> MemoryRatio;
	double AverageMemoryRatio;
	unsigned maxUsedPnodes;
	unsigned maxServingVnets;
	std::vector<unsigned> successVnetSize;
	std::vector<unsigned> allVnetSize;
	std::vector<unsigned> acVnetPerTime;
	unsigned acVnetCnt;

	std::vector<unsigned> vLinkDistr;

	std::vector<unsigned> allocationPerVnet;
	

	Profile()
	{
		acVnetCnt = 0;
		maxServingVnets = 0;
		maxUsedPnodes = 0;
		WorkingRequest = 0;
		MaxWorkingRequest = 0;
		for(int i = 0; i<20; i++) vLinkDistr.push_back(0);
	}
	void regular(std::vector<double>& v)
	{
		double sum = 0;
		for(int i = 0; i<v.size(); i++){
			sum += v[i];
		}
		for(int i = 0; i<v.size(); i++){
			v[i]/=sum;
		}
	}
	void Process(char* test_name, double acceptRatio)
	{
		if(method_ID == DMT || method_ID == SMT || method_ID == IDEAL || method_ID == OVX || method_ID == DMT_WO_O2M || method_ID == DMT_WO_M2O || method_ID == DMT_SPREAD || method_ID == DMT_SPREAD_WO_M2O || method_ID == SMT_WO_M2O || method_ID == DMT_GD || method_ID == DMT_GD_WO_O2M || method_ID == DMT_GD_WO_M2O)
		{
			// output TCAM size
			string str(test_name);
			str = str + "_" + method_name[method_ID] + "_TCAM.txt";
			ofstream fout(str.c_str());
			for(size_t i = 0; i<this->allocationPerVnet.size(); i++)
			{
				fout<<allocationPerVnet[i]<<std::endl;
			}
			fout.close();

			// output max serving nets
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_MaxConcurrentVnets.txt";
			fout.open(str.c_str());
			fout<<this->maxServingVnets;
			fout.close();
		
			// output ac vnet per time window
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_acVnetPerTime.txt";
			fout.open(str.c_str());
			for(size_t i = 0; i<this->acVnetPerTime.size(); i++)
			{
				fout<<this->acVnetPerTime[i]<<std::endl;
			}
			fout.close();

			// output vlink length distribution
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_vLinkLenDistr.txt";
			fout.open(str.c_str());
			for(size_t i = 0; i<this->vLinkDistr.size(); i++)
			{
				fout<<this->vLinkDistr[i]<<std::endl;
			}
			fout.close();

		
#if 0
			// output max serving nets
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_MaxWorkingRequest.txt";
			fout.open(str.c_str());
			fout<<this->MaxWorkingRequest;
			fout.close();
#endif
		
#if 0
			// output physical paths length
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_PhysicalPaths.txt";
			fout.open(str.c_str());
			for(size_t i = 0; i<this->physicalpathStretchRaw.size(); i++)
			{
				fout<<this->physicalpathStretchRaw[i]<<std::endl;
			}
			fout.close();
#endif

			// output working request
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_workingRequest.txt";
			fout.open(str.c_str());
			for(size_t i = 0; i<this->vec_workingRequest.size(); i++)
			{
				fout<<this->vec_workingRequest[i]<<std::endl;
			}
			fout.close();

#if 0
			// output success vnet size
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_VnetSize.txt";
			fout.open(str.c_str());
			for(size_t i = 0; i<this->successVnetSize.size(); i++)
			{
				fout<<this->successVnetSize[i]<<std::endl;
			}
			fout.close();
#endif

#if 0
			// output success vnet size
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_allVnetSize.txt";
			fout.open(str.c_str());
			for(size_t i = 0; i<this->allVnetSize.size(); i++)
			{
				fout<<this->allVnetSize[i]<<std::endl;
			}
			fout.close();
#endif
		}

		if(method_ID == DMT || method_ID == DMTe2e || method_ID == DMTe3e || method_ID == DMT_WO_M2O || method_ID == DMT_SPREAD || method_ID == DMT_SPREAD_WO_M2O || method_ID == DMT_GD )
		{
#if 0
			// output virtual path stretch ratio
			string str(test_name);
			str = str + "_" + method_name[method_ID] + "_VpathStretch.txt";
			ofstream fout(str.c_str());
			for(int i = 0; i<=this->vpathStretchRaw.size(); i++){
				fout<<this->vpathStretchRaw[i]<<std::endl;
			}
			fout.close();
#endif

#if 0
			// output accept ratio
			str.clear();
			str = str + test_name + "_" + method_name[method_ID] + "_AcceptRatio.txt";
			fout.open(str.c_str());
			fout<<acceptRatio;
			fout.close();
#endif
		}
	}
		
};

class Test
{
private:
	Pnet physicalNet;
	std::vector<Vnet*> virtualNets;
	std::vector<unsigned> succedVnets;
	std::vector<unsigned> workingVnets;
	
	std::vector<std::vector<unsigned> > VnetArrivalPerTimeWindow;
	std::vector<std::vector<unsigned> > VnetLeavePerTimeWindow;

	std::vector<std::vector<unsigned> > Dist; // distance between pnode pairs
	

public:
	GlobalIDMaster gm;
	Profile pf;
	double GetSplitedSurvivedRatio()
	{
		unsigned cnt = 0;
		for(size_t i = 0; i<this->succedVnets.size(); i++)
		{
			unsigned vnetID = this->succedVnets[i];
			cnt += (this->virtualNets[vnetID]->GetOriginalNodeNum() == 
				this->virtualNets[vnetID]->GetVnodes().size())?0:1;
		}
		return (double)cnt/this->succedVnets.size();
	}
	double GetSuccessRatio()
	{
		return ((double)this->succedVnets.size())/this->virtualNets.size();
	}
	Test(rapidjson::Document& d)
	{
		assert(d.HasMember("pnet"));
		assert(d.HasMember("vnets"));
		
		assert(d["pnet"].IsObject());
		assert(d["vnets"].IsArray());

		assert(d.HasMember("max_time_window"));
		MAXIMUN_TIME_WINDOW = d["max_time_window"].GetInt();
		
		this->physicalNet.AssignNet(d["pnet"], &(this->gm));
		std::vector<unsigned> dummy;
		std::vector<std::vector<unsigned> > dummy2(MAXIMUN_TIME_WINDOW,dummy);
		this->VnetArrivalPerTimeWindow = dummy2;
		this->VnetLeavePerTimeWindow = dummy2;

		for(rapidjson::SizeType i = 0; i<d["vnets"].Size(); i++)
		{
			Vnet* newVnet = new Vnet;
			newVnet->AssignNet(d["vnets"][i], this->gm, 
					this->physicalNet.widthsOption);
			this->virtualNets.push_back(newVnet);
			this->VnetArrivalPerTimeWindow[newVnet->GetTbegin()].push_back(newVnet->GetVnetID());
		}
	}
	unsigned SetDistFromFile(char* fname)
	{
		// all figure are in a line
		// first number is the number of pnode, N
		// next is N^2 number, which is the dist of links from (0,0) to (N,N)
		ifstream fin(fname);
		int N;
		fin>>N;
		for(size_t i = 0; i<N; i++)
		{
			std::vector<unsigned> dummy;
			this->Dist.push_back(dummy);
			for(size_t j = 0; j<N; j++)
			{
				unsigned d;
				fin>>d;
				this->Dist[i].push_back(d);
			}
		}
		return N;
	}
	~Test()
	{
		for(size_t i = 0; i<this->virtualNets.size(); i++)
		{
			delete this->virtualNets[i];
		}
	}
	void SimpleRun();
	void PeriodRun();
	void ProcessPerPushVnet(unsigned vnetid);
	void ProcessPerTimeWindow();
};

