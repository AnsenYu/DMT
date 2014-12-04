#include <fstream>

using namespace std;

const	int K = 28;
const	int halfK = K/2;
const	int all_node_num = K*K*5/4;
const	int pod_node_num = K;
const	int all_pod_node_num = K*K;

class PodSW
{
public:
	int podNo;
	bool edge;
	int pos;
	PodSW(int no)
	{
		podNo = no/K;	
		edge = (no%2 == 0);
		pos = no%K/2;// the local position
	}
	int Dist(PodSW& sw2)
	{
		if(this->podNo == sw2.podNo)
		{
			return this->edge == sw2.edge?2:1;
		}
		else
		{
			if(this->edge != sw2.edge) return 3;
			else if(this->edge == true)//edge switch 
			{
				return 4;
			}
			else if(this->pos == sw2.pos) 
				return 2;
			else
				return 4;
		}
	}
};
class CoreSW
{
public:
	int grpNo;
	CoreSW(int no)
	{
		grpNo = (no - all_pod_node_num)/halfK;
	}
	int Dist(CoreSW& sw)
	{
		return this->grpNo == sw.grpNo? 2:4;	
	}
	int Dist(PodSW& sw)
	{
		if(sw.edge) return 2;
		if(this->grpNo == sw.pos) return 1;
		else return 3;
	}
};

int main()
{
	ofstream fout("fattree.txt");
	fout<<all_node_num<<" ";
	for(size_t i = 0; i<all_node_num; i++)
		for(size_t j = 0; j<all_node_num; j++)
		{
			if(i == j)
			{ 
			  fout<<0<<" ";
			  continue;
			}		
			if(i<all_pod_node_num && j<all_pod_node_num)			
			{
				PodSW sw1(i), sw2(j);
				  fout<<sw1.Dist(sw2)<<" ";
				  continue;
			}

			else if(i<all_pod_node_num && j>=all_pod_node_num)			
			{
				PodSW sw1(i); CoreSW sw2(j);
				  fout<<sw2.Dist(sw1)<<" ";
				  continue;
			}
			else if(j<all_pod_node_num && i>=all_pod_node_num)			
			{
				PodSW sw1(j); CoreSW sw2(i);
				  fout<<sw2.Dist(sw1)<<" ";
				  continue;
			}
			else if(j>=all_pod_node_num && i>=all_pod_node_num)			
			{
				CoreSW sw1(j); CoreSW sw2(i);
				  fout<<sw2.Dist(sw1)<<" ";
				  continue;
			}
		}
	fout.close();
	return 0;
}

