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
	int vnet_size;//ƽ�����ٸ�vnode
	int pipeline_len;//ƽ��pipeline����
	double stage_capa;//ÿ��virtual stageռp stage�����ٷֱ�
	double link_prob;//��������vnode֮��link�ĸ���
};

