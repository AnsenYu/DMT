#include <vector>

using namespace std;

class singleConf
{
public:
	int pnode_num;
	int pnode_stage_num;
	int stage_capacity;
	int vnet_per_time;
	int time_length;
	unsigned keylen;
	int vnet_size;//平均多少个vnode
	int pipeline_len;//平均pipeline长度
	double stage_capa;//每个virtual stage占p stage的最大百分比
	double link_prob;//任意两个vnode之间link的概率
};

