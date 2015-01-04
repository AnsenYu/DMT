#include "stdafx.h"
#include "simu.h"
#include <math.h>
#include <iostream>

#define MOD_JAN4
#define SCALE_FACTOR 10
int MAX_LINE_WIDTH = 640;
int MAXIMUN_TIME_WINDOW = 0; //按可能的最晚结束的vnet的Tend来决定
unsigned method_ID = 0; //
const char* method_name[] = {"NONE", "DMT", "DMTe2e", "DMTe3e", "SMT", "IDEAL", "OVX", "DMT_WO_O2M", "DMT_WO_M2O", "DMT_SPREAD", "DMT_SPREAD_WO_M2O", "SMT_WO_M2O", "DMT_GD", "DMT_GD_WO_O2M", "DMT_GD_WO_M2O"};
int CHILD_TTL = 9999;
bool NO_TOPO = false;
unsigned g_pnet_size;
unsigned default_pnetsize = 980;

/*Class Config**************************/
void Config::AssignConf(rapidjson::Value& val)
{
	assert(val.HasMember("smartkey")&&val["smartkey"].IsBool());
	assert(val.HasMember("stagesplit")&&val["stagesplit"].IsBool());
	assert(val.HasMember("nodesplit")&&val["nodesplit"].IsBool());
	this->smartKey = val["smartkey"].GetBool();
	this->stageSplit = val["stagesplit"].GetBool();
	this->nodeSplit = val["nodesplit"].GetBool();
}

/*class GlobalIDMaster ****************************/
void GlobalIDMaster::RegisterNewVnode(Vnode* ptr)
{
	ptr->SetGID(this->pool.size());
	pool.push_back(ptr);
}

void GlobalIDMaster::DeleteVnode(unsigned globalID)
{
	;//do nothing temperaly
}


/*Class Vnode ********************/

Vnode* Vnode::MakeChildNode(std::vector<int>& depth, std::vector<int>& width)
{
	Vnode* pdst = new Vnode;
	assert(pdst != NULL);
	pdst->depth = depth;
	pdst->width = width;
	pdst->vnetID = this->vnetID;
	pdst->vnodeID = this->vnodeID;
	pdst->pgm = this->pgm;
	pdst->pgm->RegisterNewVnode(pdst);//Register to globalIDmaster
	pdst->palloc = NULL;//记得赋值，否则就成野指针了，在~Vnode的时候会去delete导致出错！
	pdst->pchild = NULL;//同上
	pdst->childTTL = this->childTTL - 1;
	this->pchild = pdst;
	return pdst;
}

void Vnode::AssignNode(std::vector<int> d, std::vector<int> w, unsigned netID, unsigned nodeID, GlobalIDMaster& gm)
{
		this->depth = d;
		this->width = w;
		this->vnetID = netID;
		this->vnodeID = nodeID;
		this->pgm = &gm;
		this->pchild = NULL;
		this->palloc = NULL;
		this->childTTL = CHILD_TTL;//最多往后向分裂次数，childTTL等于0时不能分裂，child节点等于父节点减一
		gm.RegisterNewVnode(this);//Register to globalIDmaster
}

void Vnode::SetAlloc(unsigned pID, std::vector<int>& width, std::vector<int>& allocation)
{
	this->palloc = new AllocVnode2Pnode;
	this->palloc->vnodeID = this->globalID;
	this->palloc->pnodeID = pID;
	std::vector<int>::iterator itr = this->palloc->alloc.begin();
	this->palloc->resource = 0;
	for(size_t i = 0; i<allocation.size(); i++)
	{
		this->palloc->resource += (width[i]*allocation[i]);
	}
	this->palloc->alloc.insert(itr, allocation.begin(), allocation.end());
}


/*Class Pnode ********************/
int Pnode::chooseWidth(int width)
{
	assert(width <= MAX_LINE_WIDTH);
	for(size_t i = 0; i<this->widthOption.size(); i++)
	{
		if(width <= this->widthOption[i])
			return this->widthOption[i];
	}
}

bool Pnode::push(Vnode& v, Config& c, GlobalIDMaster& gm, Vnode*& leftover){
	std::vector<int> state(this->statePerStage);
	const std::vector<int> capa(this->capaPerStage);
	std::vector<int> depth(this->depthPerStage);
	std::vector<int> width(this->widthPerStage);

	std::vector<int> vdepth(v.depth);
	std::vector<int> vwidth(v.width);
	std::vector<int> alloc;

	size_t i,j;
	bool occupied = false;
	for(i = 0, j = 0; i<depth.size() && j<vdepth.size();){
		if(method_ID == SMT || method_ID == OVX || method_ID == SMT_WO_M2O)// no smart key
		{
			c.smartKey = false;
		}
		else
		{
			c.smartKey = true;
		}
		if(method_ID == OVX || method_ID == DMT_WO_O2M)// no 1-to-many
		{
			c.stageSplit = false;
			c.nodeSplit= false;
		}
		else
		{
			c.stageSplit = true;
			c.nodeSplit= true;
		}

		if(c.smartKey){
			if(state[i])//the stage is still free
			{
				int alignedWidth = this->chooseWidth(vwidth[j]);
				if(capa[i]/alignedWidth >= vdepth[j] ){
					width[i] = alignedWidth;
					depth[i] = capa[i]/alignedWidth - vdepth[j];
					alloc.push_back(vdepth[j]);
					state[i] = false;
					i++;j++;
					occupied = true;
					continue;
				}
				if(c.stageSplit){
					width[i] = alignedWidth;
					vdepth[j] -= capa[i]/alignedWidth;
					alloc.push_back(capa[i]/alignedWidth);
					depth[i] = 0;
					state[i] = false;
					i++;
					occupied = true;
					continue;
				}
				else{
					return false;
				}
			}
			else
			{
				if(c.stageSplit){
					if(width[i] < vwidth[j])
					{
						alloc.push_back(0);
						i++; //put off to next physical stage
						continue;
					}
					if(depth[i] < vdepth[j])
					{
						vdepth[j] -= depth[i];
						alloc.push_back(depth[i]);

						if(depth[i] == 0)
						{
							;
						}
						else
						{
							depth[i] = 0;
							occupied = true;
						}
						i++;
						continue;
					}
					depth[i] -= vdepth[j];
					alloc.push_back(vdepth[j]);
					i++;j++;
					occupied = true;
					continue;
				}
				else{
					if(width[i] < vwidth[j])
						return false;
					if(depth[i] < vdepth[j])
						return false;
					alloc.push_back(vdepth[j]);
					depth[i] -= vdepth[j];
					i++;j++;
					occupied = true;
					continue;
				}
			}
		}
		else{// all entries use full tuple
			if(depth[i]>=vdepth[j]){
				depth[i] -= vdepth[j];
				alloc.push_back(vdepth[j]);
				state[i] = false;//占用
				i++;j++;
				occupied = true;
				continue;
			}
			if(c.stageSplit){
				vdepth[j] -= depth[i];
				alloc.push_back(depth[i]);
				if(depth[i] == 0)
				{
					;
				}
				else
				{
					occupied = true;
					state[i] = false;//占用
					depth[i] = 0;
				}
				i++;
				
			}
			else{
				return false;
			}
		}
	}
	if(j<vdepth.size()){
		if(c.nodeSplit && occupied && v.IsSplitable()){ // 意味着Vnode占用了pnode的资源 且Vnode还可以分裂
			std::vector<int> leftDepth(vdepth.begin()+j, vdepth.end());
			std::vector<int> leftWidth(vwidth.begin()+j, vwidth.end());
			leftover = v.MakeChildNode(leftDepth, leftWidth);//The new child is not yet added to its vnet during
															 // generation.
		}
		else{
			return false;
		}
	}
	v.SetAlloc(this->GetID(), width, alloc);//在vnode上记录分配
	this->vnodeAlloc.insert(VNODE2STAGE::value_type(v.GetGID(),alloc));
	this->vnodes.push_back(v.GetGID());
	this->statePerStage = state;
	this->depthPerStage = depth;
	this->widthPerStage = width;
	return true;
}


void Pnode::popVnode(unsigned vnodeID)
{
	assert(this->vnodeAlloc.find(vnodeID) != this->vnodeAlloc.end());
	std::vector<int>& alloc = this->vnodeAlloc[vnodeID];
	for(size_t i = 0; i<alloc.size() && i<this->depthPerStage.size(); i++)
	{
		if(alloc[i] == 0) continue;
		this->depthPerStage[i] += alloc[i];
		if(this->depthPerStage[i] == this->capaPerStage[i]/this->widthPerStage[i])
			this->statePerStage[i] = true;
	}
	this->vnodeAlloc.erase(vnodeID);
}

void Pnode::popVnet(unsigned vnetID)
{
	std::vector<unsigned> other;
	for(size_t i = 0; i<this->vnodes.size(); i++)
	{
		if(this->pgm->pool[this->vnodes[i]]->GetNID() == vnetID)
		{
			this->popVnode(this->vnodes[i]);
		}
		else{
			other.push_back(this->vnodes[i]);
		}
	}
	
	this->vnodes = other;
}

void Pnode::AssignNode(unsigned ID, std::vector<int>& capacity, std::vector<int> width, GlobalIDMaster* pgm)
{
	this->pgm = pgm;
	this->nodeID = ID;
	for(size_t i = 0; i<capacity.size(); i++){
		this->statePerStage.push_back(true);
		this->capaPerStage.push_back(capacity[i]);
		this->depthPerStage.push_back(capacity[i]/MAX_LINE_WIDTH);// default depth
		this->widthPerStage.push_back(MAX_LINE_WIDTH);//default width
	}
	this->widthOption = width;
}

unsigned Pnode::GetAbsLoad()
{
	unsigned load = 0;
	for(size_t i = 0; i<this->statePerStage.size(); i++)
	{
		if(!this->statePerStage[i])//非空闲
		{
			load += (this->capaPerStage[i] - (this->depthPerStage[i] * this->widthPerStage[i]));
		}
	}
	return load == 0?0:load-1;
}

unsigned Pnode::GetAbsCapacity()
{
	unsigned capacity = 0;
	for(size_t i = 0; i<this->statePerStage.size(); i++)
	{
		capacity += this->capaPerStage[i];
	}
	return capacity;
}

double Pnode::GetLoadRatio()
{
	return ((double)this->GetAbsLoad())/this->GetAbsCapacity();
}

/*Class Vnet ********************/

void Vnet::PushNewVnode(Vnode* p)
{
	assert(p != NULL);
	p->pvnet = this;
	this->vnodes.push_back(p);
	this->vnodesStack.push(p);
}

Vnode* Vnet:: PopVnode()
{
	Vnode* re = this->vnodesStack.top();
	this->vnodesStack.pop();
	return re;
}

bool Vnet:: Empty()
{
	return this->vnodesStack.empty();
}

const unsigned Vnet::GetAllocResource()
{
	unsigned allocated = 0;
	if(method_ID == IDEAL)
	{
		allocated = this->resource;
	}
	else
	{
		for(size_t i = 0; i<this->vnodes.size(); i++)
		{
			allocated += this->vnodes[i]->GetAlloc()->resource;
		}
	}
	return allocated;
}

//void Vnet::AssignNet(unsigned netID, unsigned num, std::vector<std::vector<int> >& depth, 
//	std::vector<std::vector<int> >& width, GlobalIDMaster& gm, unsigned Tb, unsigned Te)
//{
//	this->Tbegin = Tb;
//	this->Tend = Te;
//	this->vnetID = netID;
//	for(size_t i = 0; i<num; i++){
//		Vnode* newNode = new Vnode;
//		newNode->AssignNode(depth[i], width[i], this->vnetID, i, gm);
//		this->PushNewVnode(newNode);
//	}
//}

unsigned WidthAligning(unsigned width, std::vector<unsigned>& widthsOption)
{
	for(size_t i = 0; i<widthsOption.size(); i++)
	{
		if(width <= widthsOption[i]){
			return widthsOption[i];
		}
	}
	return *(widthsOption.end()-1);
}

void Vnet::AssignNet(rapidjson::Value& val, GlobalIDMaster& gm, 
		std::vector<unsigned>& widthsOptin)
{
	assert(val.HasMember("vnetID"));
	assert(val.HasMember("start"));
	assert(val.HasMember("end"));
	assert(val.HasMember("switches"));

	assert(val.HasMember("config"));
	assert(val["config"].IsObject());
	this->conf.AssignConf(val["config"]);

	assert(val["switches"].IsArray());
	this->originNodeNum = val["switches"].Size();

	this->resource = 0;

	std::vector<int> dummy;
	std::vector<std::vector<int> > depthPerSwitch, widthPerSwitch;
	for(rapidjson::SizeType i = 0; i<val["switches"].Size(); i++){
		assert(val["switches"][i].HasMember("pipeline"));
		rapidjson::Value& v = val["switches"][i]["pipeline"];
		depthPerSwitch.push_back(dummy);
		widthPerSwitch.push_back(dummy);
		for(rapidjson::SizeType j = 0; j<v.Size(); j++)
		{
			//第i个vswitch的第j级pipeline的宽度和深度
			int d = v[j]["depth"].GetInt();
			if(d == 0){ v[j]["depth"].SetInt(1);} 
			else{
				v[j]["depth"].SetInt(d*SCALE_FACTOR);
			}
			depthPerSwitch[i].push_back(v[j]["depth"].GetInt());
			widthPerSwitch[i].push_back(v[j]["width"].GetInt());

			this->resource += v[j]["depth"].GetInt() * WidthAligning(v[j]["width"].GetUint(), widthsOptin);
		}
		assert(val["switches"][i].HasMember("neighbor"));
		rapidjson::Value& nb = val["switches"][i]["neighbor"];
		this->neighbor.push_back(dummy);
		std::vector<int> &Nb = *(this->neighbor.end() - 1);
		for(rapidjson::SizeType j = 0; j<nb.Size(); j++)
		{
			Nb.push_back(nb[j].GetInt());
		}
	}
	//this->AssignNet(val["vnetID"].GetUint(),val["switches"].Size(), depthPerSwitch, 
	//	widthPerSwitch, gm, val["start"].GetUint(), val["end"].GetUint());

	this->Tbegin = val["start"].GetUint();
	this->Tend = val["end"].GetUint();


	this->vnetID = val["vnetID"].GetUint();
	for(size_t i = 0; i<val["switches"].Size(); i++){
		Vnode* newNode = new Vnode;
		newNode->AssignNode(depthPerSwitch[i], widthPerSwitch[i], this->vnetID, i, gm);
		this->PushNewVnode(newNode);
	}

#if 0
	assert(val.HasMember("paths"));
	assert(val["paths"].IsArray());
	rapidjson::Value & paths = val["paths"];
	std::vector<unsigned> udummy;
	for(rapidjson::SizeType i = 0; i<paths.Size(); i++)
	{
		this->paths.push_back(udummy);
		for(rapidjson::SizeType j = 0; j<paths[i]["path"].Size(); j++)
		{
			this->paths[i].push_back(paths[i]["path"][j].GetUint());
		}
	}
#endif
}

unsigned Vnet::CountPhysicalNodes()
{
	std::vector<unsigned> usedPnode;
	for(size_t i = 0; i<this->vnodes.size(); i++)
	{
		unsigned pnodeID = this->vnodes[i]->GetAlloc()->pnodeID;
		if(find(usedPnode.begin(), usedPnode.end(), pnodeID) == usedPnode.end())
		{
			usedPnode.push_back(pnodeID);
		}
	}
	return usedPnode.size();
}


/*Class Pnet ********************/
void Pnet::AssignNet(unsigned num, std::vector<int>& capacity, std::vector<int>& width)
{
	for(size_t i = 0; i<num; i++)
	{
		Pnode* newNode = new Pnode;
		newNode->AssignNode(i, capacity, width, this->pgm);
		pnodes.push_back(newNode);
	}
}

void Pnet::AssignNet(rapidjson::Value& val, GlobalIDMaster* pgm)
{
	this->pgm = pgm;
	this->resource = 0;

	assert(val.HasMember("pipeline"));
	assert(val.HasMember("node"));
	assert(val.HasMember("widths"));
	assert(val["pipeline"].IsArray());
	assert(val["widths"].IsArray());
	std::vector<int> cap, width;
	std::vector<unsigned> Uwidth;
	long long resource_per_switch = 0;
	for(rapidjson::SizeType i = 0; i<val["pipeline"].Size(); i++)
	{
		rapidjson::Value& v = val["pipeline"][i];
		assert(v.HasMember("capacity"));
		cap.push_back(v["capacity"].GetInt());
		resource_per_switch += v["capacity"].GetInt();
	}
	unsigned sw_num;
	if(NO_TOPO)
	{
		sw_num = g_pnet_size;	
	}
	else
	{
		sw_num = val["node"].GetUint();
	}
	this->resource = resource_per_switch * sw_num;

	for(rapidjson::SizeType i = 0; i<val["widths"].Size(); i++)
	{
		rapidjson::Value& v = val["widths"][i];
		width.push_back(v.GetInt());
		Uwidth.push_back(v.GetUint());
	}
	this->widthsOption = Uwidth;
	MAX_LINE_WIDTH = *(Uwidth.end()-1); 
	this->AssignNet(sw_num, cap, width);
}

bool Pnet::PushVnetIdeal(Vnet& vnet, GlobalIDMaster& gm)
{
	unsigned res = vnet.GetResource();
	if(this->resource > res)
	{
		this->resource -= res;
		return true;
	}
	return false;
}

void Pnet::PopVnetIdeal(Vnet* pvnet)
{
	unsigned res = pvnet->GetResource();
	this->resource += res;
}

bool Pnode::CalCost(Vnode v, double& cost)
{
	bool touched = false;
	unsigned alloc = 0;
	unsigned occupy = 0;
	size_t j = 0, i = 0;
	std::vector<unsigned> uWidOpt;
	for(size_t k = 0; k<this->widthOption.size(); k++)
	{
		uWidOpt.push_back(this->widthOption[k]);
	}
	while(i<v.depth.size() && j<this->depthPerStage.size())
	{
		unsigned wid = WidthAligning(v.width[i], uWidOpt);
		if(this->statePerStage[j])//free
		{
			unsigned availableLine = this->capaPerStage[j]/wid;
			if(availableLine >= v.depth[i])
			{
				alloc += v.depth[i] * wid;
				occupy += v.depth[i] * wid;
				i++;j++;
			}
			else
			{
				alloc += availableLine * wid;
				occupy += availableLine * wid;
				v.depth[i] -= availableLine;
				j++;
			}
			continue;
		}
		
		if(wid > this->widthPerStage[j])
		{
			j++;
		}
		else{
			unsigned availableLine = this->depthPerStage[j];
			if(availableLine >= v.depth[i])
			{
				alloc += v.depth[i] * wid;
				occupy += v.depth[i] * this->widthPerStage[j];
				i++;j++;
			}
			else
			{
				alloc += availableLine * wid;
				occupy += availableLine * this->widthPerStage[j];
				v.depth[i] -= availableLine;
				j++;
			}
			continue;
		}
	}
	if(method_ID == DMT_GD_WO_O2M && i < v.depth.size())
	{
		return false;
	}
	if(alloc == 0) return false;
	
	cost = (double)occupy / alloc;
	return true;
}

bool Pnet::FindBestFitPnode(Vnode& vnode, unsigned& pnodeIdx, const std::vector<bool>& NodesUsed)
{
	double mincost = 99999;
	unsigned minIdx = 0;
	bool found = false;
	for(size_t i = 0; i<this->pnodes.size(); i++)
	{
		double cost;
		if( this->pnodes[i]->CalCost(vnode,cost) && cost < mincost)
		{
			if(method_ID == DMT_GD_WO_M2O && NodesUsed[i]) continue;
			found = true;
			mincost = cost;
			minIdx = i; 
			if(fabs(mincost-1)<0.0001)
			{
				break;	
			}
		}
	}
	if(found)
	{
		pnodeIdx = minIdx;
		return true;
	}
	else
	{
		return false;
	}
}

bool Pnet::PushVnet(Vnet& vnet, GlobalIDMaster& gm)
{
	this->vnets.push_back(&vnet);
	Config& conf = vnet.conf;
	std::vector<bool> pnodeUsed(this->pnodes.size(), false);
	while(!vnet.Empty())
	{
		Vnode* pvnode = vnet.PopVnode();
		size_t i = 0, idx = 0;
		std::vector<unsigned> sorted_index;
		if(method_ID == DMT_SPREAD || method_ID == DMT_SPREAD_WO_M2O)
		{
			this->NodeLoadSort(sorted_index);
		}

		if(method_ID == DMT_GD || method_ID == DMT_GD_WO_O2M || method_ID == DMT_GD_WO_M2O)
		{
			unsigned bestnode;
			if(this->FindBestFitPnode(*pvnode, bestnode, pnodeUsed))
			{
				Vnode* childVnode = NULL;
				if(this->pnodes[bestnode]->push(*pvnode, conf, gm, childVnode))
				{
					if(childVnode != NULL)
					{
						vnet.PushNewVnode(childVnode); // add to virtual net
					}
					pnodeUsed[bestnode] = true;
					continue; // next vnode
				}
				std::cout<<"exception!";
				assert(0);
			}
			else
			{
				this->PopVnet(&vnet);
				return false;
			}
		}

		//尽可能地把序号小的pnode先塞满，然后再选择后面的
		while(1)
		{
			if(method_ID == DMT_SPREAD || method_ID == DMT_SPREAD_WO_M2O)
			{
				if(idx >= this->pnodes.size()) break;
				i = sorted_index[idx];
			}
			else
			{
				if(idx >= this->pnodes.size()) break;
				i = idx;
			}

			// no many-to-1
			if((method_ID == OVX || method_ID == DMT_WO_M2O || method_ID == DMT_SPREAD_WO_M2O || method_ID == SMT_WO_M2O) && pnodeUsed[i] == true) 
			{
				idx += 1;
				continue;
			}
			Vnode* childVnode = NULL;
			if(this->pnodes[i]->push(*pvnode, conf, gm, childVnode))
			{
				pnodeUsed[i] = true;
				if(childVnode != NULL)
				{
					vnet.PushNewVnode(childVnode); // add to virtual net
				}
				break; //get out of FOR loop
			}
			idx++;
		}
		if(idx<this->pnodes.size())
		{
			continue;
		}
		else
		{
			//no physical node can hold current vnode, current vnet fails
			this->PopVnet(&vnet);
			return false;
		}
	}
	return true;
}

void Pnet::PopVnet(Vnet* pvnet)
{
	// remove from vnet address list
	std::vector<Vnet*>::iterator itr = std::find(this->vnets.begin(),this->vnets.end(), pvnet);
	if(itr == this->vnets.end())
	{
		return;
	}
	this->vnets.erase(itr);
	// release occupied resource on physical nodes
	for(size_t i = 0; i<this->pnodes.size(); i++)
	{
		this->pnodes[i]->popVnet(pvnet->GetVnetID());
	}
}

void Pnet::GetUsedNodes(std::vector<Pnode*>& usedNode)
{
	for(size_t i = 0; i<this->pnodes.size(); i++)
	{
		if(!(this->pnodes[i]->empty()))
			usedNode.push_back(this->pnodes[i]);
	}
}

void Pnet::NodeLoadDistribution(std::vector<double>& distribution)
{
	for(size_t i = 0; i<this->pnodes.size(); i++)
	{
		distribution.push_back(this->pnodes[i]->GetLoadRatio());
	}
}

unsigned Pnet::NodeSumLoad()
{
	unsigned sum = 0;
	for(size_t i = 0; i<this->pnodes.size(); i++)
	{
		sum += this->pnodes[i]->GetAbsLoad();
	}
	return sum;
}

typedef struct
{
	int idx;
	unsigned ld;
}node_cap;
std::vector<node_cap> load(1000);
bool loadCmp(const node_cap n1, const node_cap n2){return n1.ld < n2.ld;}

void Pnet::NodeLoadSort(std::vector<unsigned>& sorted_index)
{
	if(this->pnodes.size() > 1000)
	{
		std::cout<<"pnode must < 1000\n";
		assert(this->pnodes.size()<1000);
	}
	sorted_index.clear();
	for(size_t i = 0; i<this->pnodes.size(); i++)
	{
		load[i].idx = i;
		load[i].ld = (this->pnodes[i]->GetAbsLoad());
	}
	sort(load.begin(),load.begin()+this->pnodes.size(), loadCmp);
	for(size_t i = 0; i<this->pnodes.size(); i++)
	{
		sorted_index.push_back(load[i].idx);
	}
}

unsigned Pnet::NodeSumCapacity()
{
	unsigned sum = 0;
	for(size_t i = 0; i<this->pnodes.size(); i++)
	{
		sum += this->pnodes[i]->GetAbsCapacity();
	}
	return sum;
}

/* class Test******************/
void Test::SimpleRun(void)
{
	//Push all virtual nets onto pnet one-by-one at a time w/o considering start and end time
	for(size_t i = 0; i<this->virtualNets.size(); i++)
	{
		if(this->physicalNet.PushVnet(*(this->virtualNets[i]), this->gm))
		{
			this->succedVnets.push_back(i);//记录成功插入的虚拟网络
		}
	}
	std::vector<Pnode*> usedNode;
	this->physicalNet.GetUsedNodes(usedNode);
	std::cout<<"Use "<<usedNode.size()<<" physical nodes";
}

void Test::PeriodRun()
{
	for(int i = 0; i<MAXIMUN_TIME_WINDOW; i++)
	{
		for(size_t j = 0; j<this->VnetLeavePerTimeWindow[i].size(); j++)
		{
			unsigned vnetid = this->VnetLeavePerTimeWindow[i][j];
			if(method_ID != IDEAL)
			{
				this->physicalNet.PopVnet(this->virtualNets[vnetid]);
			}
			else
			{
				this->physicalNet.PopVnetIdeal(this->virtualNets[vnetid]);
			}
			this->pf.WorkingRequest -= this->virtualNets[vnetid]->GetResource();
			std::vector<unsigned>::iterator itr = find(this->workingVnets.begin(),
				this->workingVnets.end(), vnetid);
			if(itr != this->workingVnets.end())
				this->workingVnets.erase(itr);
		}
		for(size_t j = 0; j<this->VnetArrivalPerTimeWindow[i].size(); j++)
		{
			unsigned vnetid = this->VnetArrivalPerTimeWindow[i][j];
			unsigned allocation = 0;
			bool result;
			if(method_ID != IDEAL)
			{
				result  = (this->physicalNet.PushVnet(*(this->virtualNets[vnetid]), this->gm));
			}
			else
			{
				result = (this->physicalNet.PushVnetIdeal(*(this->virtualNets[vnetid]), this->gm));
			}
			if(result)
			{
				this->succedVnets.push_back(vnetid);//记录成功插入的虚拟网络
				this->workingVnets.push_back(vnetid);
				this->VnetLeavePerTimeWindow[this->virtualNets[vnetid]->GetTend()].push_back(vnetid);//插入到弹出队列
				this->ProcessPerPushVnet(vnetid);
				allocation = this->virtualNets[vnetid]->GetAllocResource();
				//this->pf.successVnetSize.push_back(this->virtualNets[vnetid]->GetOriginalVnetSize());
				this->pf.WorkingRequest += this->virtualNets[vnetid]->GetResource();
				this->pf.acVnetCnt ++;
			}
			this->pf.allocationPerVnet.push_back(allocation);
			//this->pf.allVnetSize.push_back(this->virtualNets[vnetid]->GetOriginalVnetSize());
		}
		this->ProcessPerTimeWindow();
	}
}


void Test::ProcessPerPushVnet(unsigned vnetid)
{

	Vnet* pvnet = this->virtualNets[vnetid];

#ifdef MOD_JAN4
	std::vector<unsigned> NodeStart;// the physical node of the first deployed virtual switch
	std::vector<unsigned> NodeEnd;
	const std::vector<Vnode*> & vnodelist = pvnet->GetVnodes();
	for(size_t i = 0; i<pvnet->GetOriginalNodeNum(); i++)
	{
		Vnode* ptr = vnodelist[i];
		NodeStart.push_back(ptr->GetAlloc()->pnodeID);
		
		while(ptr->GetPchild())
		{
			ptr = ptr->GetPchild();
		}
		NodeEnd.push_back(ptr->GetAlloc()->pnodeID);
	}

	for(size_t i = 0; i<pvnet->GetOriginalNodeNum(); i++)
	{
		for(size_t j = 0; j<pvnet->neighbor[i].size(); j++)
		{
			size_t k = pvnet->neighbor[i][j];
			unsigned vLinkLen = this->Dist[NodeEnd[i]][NodeStart[k]];
			this->pf.vLinkDistr[vLinkLen]++;	
		}
	}
#else
	if(method_ID == IDEAL) return;
	

	//统计占用switch个数
	//unsigned num = pvnet->CountPhysicalNodes();
	

	std::vector<unsigned> vnodeStretch;
	const std::vector<Vnode*> & vnodelist = pvnet->GetVnodes();
	for(size_t i = 0; i<pvnet->GetOriginNodeNum(); i++)
	{
		unsigned stretch = 1;
		Vnode* ptr = vnodelist[i];
		while(ptr->GetPchild())
		{
			stretch++;
			ptr = ptr->GetPchild();
		}
		vnodeStretch.push_back(stretch);
	}

	//统计vnode path延展比例
	for(size_t i = 0; i<pvnet->GetPaths().size(); i++)
	{
		std::vector<unsigned> & path = pvnet->GetPaths()[i];
		unsigned stretch_path_len = 0;
		for(size_t j = 0; j<path.size(); j++)
		{
			stretch_path_len += vnodeStretch[path[j]];
		}
		this->pf.vpathStretchRaw.push_back((double)stretch_path_len/path.size());
		//unsigned stretch = floor((double)(stretch_path_len-1)/(path.size()-1));
		//this->pf.vStretchDis[stretch>=10?10:stretch]+=1;
	}


	if(!NO_TOPO)
	{
	//统计pnode path延展比例
	for(size_t i = 0; i<pvnet->GetPaths().size(); i++)
	{
		std::vector<unsigned> & path = pvnet->GetPaths()[i];
		unsigned pnode_path_len = 0;
		unsigned lastpnode = vnodelist[path[0]]->GetAlloc()->pnodeID;
			// or say previous node

		for(size_t j = 0; j<path.size(); j++)
		{
			Vnode* ptr = vnodelist[path[j]];
			unsigned currentnode = ptr->GetAlloc()->pnodeID;
			if(currentnode != lastpnode)
			{
				pnode_path_len += this->Dist[lastpnode][currentnode];
				lastpnode = currentnode;
			}
			while(ptr->GetPchild())
			{
				ptr = ptr->GetPchild();
				currentnode = ptr->GetAlloc()->pnodeID;
				if(currentnode != lastpnode)
				{
					pnode_path_len += this->Dist[lastpnode][currentnode];
					lastpnode = currentnode;
				}
			}
		}
		this->pf.physicalpathStretchRaw.push_back((double)pnode_path_len/(path.size()-1));
	}
	}
#endif

}


void Test::ProcessPerTimeWindow()
{
	if(this->workingVnets.size() > this->pf.maxServingVnets)
	{
		this->pf.maxServingVnets = this->workingVnets.size();
	}
	this->pf.vec_workingRequest.push_back(this->pf.WorkingRequest);

#ifdef MOD_JAN4
	this->pf.acVnetPerTime.push_back(this->pf.acVnetCnt);
	this->pf.acVnetCnt = 0;
#else
	if(this->pf.WorkingRequest > this->pf.MaxWorkingRequest)
	{
		this->pf.MaxWorkingRequest = this->pf.WorkingRequest;
	}
#endif

	if(method_ID == IDEAL) return;
if(0)
{

	std::vector<double> LoadDistribution;
	this->physicalNet.NodeLoadDistribution(LoadDistribution);
	for(size_t i = 0; i<LoadDistribution.size(); i++)
	{
		this->pf.LoadDistributionHD[(int)floor(LoadDistribution[i]*100)]+=1;
		this->pf.LoadDistribution[(int)floor(LoadDistribution[i]*10)]+=1;
	}
	std::vector<Pnode*> usedNode;
	this->physicalNet.GetUsedNodes(usedNode);
	if(usedNode.size() > this->pf.maxUsedPnodes)
	{
		this->pf.maxUsedPnodes = usedNode.size();//记录最多需要的pnode个数
	}



	//统计当前时刻全网物理资源利用效率 = vnet申请总资源/pnet使用总资源
	//GetSumVnetResourceDemand();
	unsigned vnetsDemand = 0;
	for(size_t i = 0; i<this->workingVnets.size(); i++)
	{
		vnetsDemand += this->virtualNets[this->workingVnets[i]]->GetResource();
	}
	unsigned pnetCapacity = this->physicalNet.NodeSumCapacity();
	unsigned pnetLoad = this->physicalNet.NodeSumLoad();
	if(vnetsDemand !=0)
	{
		this->pf.MemoryRatio.push_back(((double)pnetLoad)/vnetsDemand);
	}
	else
	{
		this->pf.MemoryRatio.push_back(1);
	}

	//统计当前时刻全网Revenue
	//unsigned revenue = vnetsDemand;
	//统计当前时刻全网平均path stretch ratio
	//latency 模型，经过一个switch的pipeline需要固定时间Tp，packet传播序列化和反序列化Ts，假设底层二层转发支持
	//   cut-through,那么简化为不同pnode之间传播时延均看做一跳。
}

}
