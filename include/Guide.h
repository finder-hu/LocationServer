/**
 * 调用者有：Guide.cpp
 */
#ifndef _ASTAR_H_
#define _ASTAR_H_

#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <MsgQueue.h>

using namespace std;

#define INFI_MAX 65535

const int M = 1000;
const int N = 1000;

struct node{     // 地图节点
	string nodename;
	double position_x;
	double position_y;
	int position_lt;
	int  position_lc;
	int m_index;
};

struct louti{     // 地图节点
	string loutiname;
	double louti_x;
	double louti_y;

	int  louti_lc;   //楼层
	int louti_index;
};

struct edge{
	int edgeid;
	double edgeweightid;  //È¨ÖØ
	int frontnodeid;
	int lastnodeid;
};

typedef struct GraphMatrix   //µØÍ¼¾ØÕó
{
	int matrix[N][M];
    vector<int> g_num;
	int louti_num;
	int edge_num;
}Graph;

struct starnode{
	int nodeid;
	double positionx;
	double positiony;
	int positionlt;
	int positionlc;
	double g;
	double h;
	double f;
	starnode* parent;
	starnode(int _nodeid,double _positionx,double _positiony,int _positionlt,int _positionlc):nodeid(_nodeid),positionx(_positionx),positiony(_positiony),positionlt(_positionlt),positionlc(_positionlc){
		g = 0.0;
		h = 0.0;
		f = 0.0;
		parent = NULL;
	}
};

class Guide
{
public:
	Guide(MsgQueue* msgQueue);
	~Guide();
	void initGraph(Graph& g);
	void deleteInOpenVec(int nodeid);
	starnode* isInOpenVec(int nodeid);
	starnode* isInCloseVec(int nodeid);
	void deleteVec();
	bool check(node Anode, starnode* Bnode, starnode* Cnode);
	double distanceFromA2B(starnode* a, starnode* b);
	starnode* findMinNode2EndInOpen(starnode* eNode);
	bool checkPath(node curnode, starnode* parentNode,starnode* eNode);
	list<starnode*> findPath(starnode* eNode);
	list<starnode*> searchpath(int startpoint, int endpoint);
	void loadMap();
//	void loadMap2();
	int getIndexStart(double point_x, double point_y);
	void getLine( list<starnode*> listNode);
	int getIndexStart2(double point_xs, double point_ys,int point_ls );
	int getIndexEnd2(double point_xd, double point_yd,int point_ld );
	int getIndexStart(double point_xs, double point_ys,int point_ls, double point_xd, double point_yd);
	int getIndexEnd(double point_xs, double point_ys,int point_ls, double point_xd, double point_yd);
	double getSum ( list<starnode*> listNode );
	int  get_Xuhao( vector<double> v_sum1,vector<double> v_sum2);

	
	static void* guide(void* arg);

	int StartTime(void);
	int EndTime(void);
	void RecordTime(void);

private:
	MsgQueue *msgQueue;
	
	map<int, int> parent;  //父节点，及可扩展节点
	map<int, int> dist;   //距离

	node newnode[800];  // 创建节点
	edge newedge[1500]; //边数
	louti newlouti[50];	//楼梯数

	Graph g;

	map<int, node> nodeMap;  // 存放节点信息
	map<int, edge> edgeMap; // 存放边信息

	vector <starnode*> v_open; //open 表
	vector <starnode*> v_close; // close 表


	/**
	 * 用来测试导引时间，guideDuring记录每次从请求队列里取出请求时的时间。
	 *receiveBufSize记录此时的请求队列里数据的个数。 
	 */
	vector<struct timeval> startTime;
	vector<struct timeval> endTime;
	vector<int> receiveBufSize;

};
#endif // _ASTAR_H_
