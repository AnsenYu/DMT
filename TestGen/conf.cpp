#include "stdafx.h"
#include "conf.h"

	//vector<int> pnode_num;
	//vector<int> pnode_stage_num;
	//vector<int> stage_capacity;
	////widths不变了。
	//vector<int> vnet_per_time;
	//vector<int> time_length;
	//vector<int> vnet_size;//平均多少个vnode
	//vector<int> pipeline_len;//平均pipeline长度
	//vector<double> stage_capa;//平均每个virtual stage占p stage的大小
	////每个vstage多宽，随机选
	//vector<double> link_prob;//任意两个vnode之间link的概率

	//vector<bool> smartkey;
	//vector<bool> stagesplit;
	//vector<bool> nodesplit;

bool Conf::MoreConf()
{
	if(nomoreconf == true) return false;
	if(pnode_num.size()>1) return true;
	if(pnode_stage_num.size()>1) return true;
	if(stage_capacity.size()>1) return true;
	if(vnet_per_time.size()>1) return true;
	if(time_length.size()>1) return true;
	if(vnet_size.size()>1) return true;
	if(pipeline_len.size()>1) return true;
	if(stage_capa.size()>1) return true;
	if(link_prob.size()>1) return true;
	if(smartkey.size()>1) return true;
	if(stagesplit.size()>1) return true;
	if(nodesplit.size()>1) return true;
	if(splitratio.size()>1) return true;

	nomoreconf = true;//最后一个conf，读取后设置为true
	return true;
}

void Conf::GetNextConf()
{
	this->sconf.pnode_num = pnode_num[pnode_num.size()-1];
	this->sconf.pnode_stage_num = pnode_stage_num[pnode_stage_num.size()-1];
	this->sconf.stage_capacity = stage_capacity[stage_capacity.size()-1];
	this->sconf.vnet_per_time = vnet_per_time[vnet_per_time.size()-1];
	this->sconf.time_length = time_length[time_length.size()-1];
	this->sconf.vnet_size = vnet_size[vnet_size.size()-1];
	this->sconf.pipeline_len = pipeline_len[pipeline_len.size()-1];
	this->sconf.stage_capa = stage_capa[stage_capa.size()-1];
	this->sconf.link_prob = link_prob[link_prob.size()-1];
	this->sconf.smartkey = smartkey[smartkey.size()-1];
	this->sconf.stagesplit = stagesplit[stagesplit.size()-1];
	this->sconf.nodesplit = nodesplit[nodesplit.size()-1];
	this->sconf.splitratio = splitratio[splitratio.size()-1];


	if(pnode_num.size()>1) {pnode_num.pop_back(); return;}
	if(pnode_stage_num.size()>1) {pnode_stage_num.pop_back(); return;}
	if(stage_capacity.size()>1) {stage_capacity.pop_back(); return;}
	if(vnet_per_time.size()>1) {vnet_per_time.pop_back(); return;}
	if(time_length.size()>1) {time_length.pop_back(); return;}
	if(vnet_size.size()>1) {vnet_size.pop_back(); return;}
	if(pipeline_len.size()>1) {pipeline_len.pop_back(); return;}
	if(stage_capa.size()>1) {stage_capa.pop_back(); return;}
	if(link_prob.size()>1) {link_prob.pop_back(); return;}
	if(smartkey.size()>1) {smartkey.pop_back(); return;}
	if(stagesplit.size()>1) {stagesplit.pop_back(); return;}
	if(nodesplit.size()>1) {nodesplit.pop_back();return;}
	if(splitratio.size()>1) {splitratio.pop_back();return;}
}