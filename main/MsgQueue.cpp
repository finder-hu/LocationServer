 #include <MsgQueue.h>
#include <define.h>
#include <json/json.h>
#include <TcpUser.h>

void MsgQueue::addLocItem(int id,TcpUser* ptcpuser,const Json::Value &value)
{
	users.insert(std::make_pair(id,ptcpuser));
	LocateVec.push_back(std::make_pair(id,value));
	debug("%d request in LocateVec now",LocateVec.size());
}

int MsgQueue::startLoc(int &id, string &rssi)
{	
	if(LocateVec.size() == 0)
		return -1;

	//从待定位请求队列里取出
	auto req = LocateVec.front();
	id = req.first;
	rssi = (req.second)["rssi"].asString();
	debug("start Locating"); 
	return 0;
}

void MsgQueue::finishLoc(int id, std::tuple<int,int,int> loc)
{
	auto front = LocateVec.front();
	if(id != front.first)
		return;
	
	Json::Value value;
	value["x"] = std::get<0>(loc);
	value["y"] = std::get<1>(loc);
	value["z"] = std::get<2>(loc);
	for(auto len = LocatedVec.size();len <= id; len++)	//增加LocatedVec
	{
		std::queue<Json::Value> temp;
		LocatedVec.push_back(temp);
	}
	LocatedVec[id].push(value);
	LocateVec.erase(LocateVec.begin());

	users[id]->PendingWriteEvt();

	debug("add an resolut to LocatedVec[%d]",id);
}

void MsgQueue::addGuideItem(int id,TcpUser* ptcpuser,const Json::Value &value)
{
	users.insert(std::make_pair(id,ptcpuser));
	GuideVec.push_back(std::make_pair(id,value));
	debug("%d request int GuideVec now", GuideVec.size());
}


int MsgQueue::startGuide(int &id, int &point_xs, int &point_ys,int &point_ls,int &point_xd, int &point_yd, int &point_ld)
{	
	if(GuideVec.size() == 0)
		return -1;

	auto req = GuideVec.front();
	id = req.first;
	Json::Value value = req.second;

	point_xs = value["sour"]["x"].asInt();
	point_ys = value["sour"]["y"].asInt();
	point_ls = 1;	//for test
	#if TEST
	debug("sx:%d,sy:%d,sl:%d",point_xs,point_ys,point_ls);
	#endif
	point_xd = value["dest"]["x"].asInt();
	point_yd = value["dest"]["y"].asInt();
	point_ld = 2;	//for test
	#if TEST
	debug("dx:%d,dy:%d,dl:%d",point_xd,point_yd,point_ld);
	debug("start Guiding");
	#endif
	return 0;
}

void MsgQueue::finishGuide(int id, const std::vector<std::pair<int,int> > &path)

{
	auto front = GuideVec.front();
	if(id != front.first)
		return;
	Json::Value jitem,jpath,jMsg;
	for(auto item = path.begin();item != path.end();item++)
	{
		jitem["x"] = item->first;
		jitem["y"] = item->second;
		jpath.append(jitem);
	}
	jMsg["path"] = jpath;
	jMsg["z"] = 2;

	//增加GuidedVec,确保id对应有空间，用户的第一个定位请求时起作用。
	for(auto len = GuidedVec.size();len <= id; len++)	
	{
		std::queue<Json::Value> temp;
		GuidedVec.push_back(temp);
	}

	GuidedVec[id].push(jMsg);
	GuideVec.erase(GuideVec.begin());

	users[id]->PendingWriteEvt();

	debug("add an resolut to GuideeVec[%d]",id);
}

bool MsgQueue::getMsg(int id,Json::Value &jMsg)
{
	if((LocatedVec.size() > id) && (LocatedVec[id].size() > 0))
	{
		jMsg = LocatedVec[id].front();
		jMsg["typecode"] = 1540;
		LocatedVec[id].pop();
		return true;
	}
	else if((GuidedVec.size() > id) && (GuidedVec.size() > 0))
	{
		jMsg = GuidedVec[id].front();
		jMsg["typecode"] = 1510;
		GuidedVec[id].pop();
		return true;
	}
	return false;
}

void MsgQueue::clearRqst(int id)
{
	// for(size_t len = 0;len<LocateVec.size();len++)
	// {
	// 	if(id == LocateVec[len] )
	// 		LocateVec.erase(len);
	// }
	// debug("clean LocateVec");

	// for(size_t len = 0;len<GuidedVec.size();len++)
	// {
	// 	if(id == GuidedVec[len] )
	// 		GuidedVec.erase(len);
	// }
	// debug("clean GuideVec");
	
	// LocatedVec[id] = NULL;
	// GuidedVec[id] = NULL;
	// debug("clean LocatedVec and GuideVec");
}

