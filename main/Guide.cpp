/*
修改人：胡珑怀
时间：2015/7/7
内容：把代码封装成类
*/

#include <Guide.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <define.h>
#include <Database.h>

#include <sys/time.h>
#include <stdio.h>

#include <string>
using std::string;

#include <tuple>
using std::tuple;

#include <json/json.h>
using Json::FastWriter;
using Json::Value;

Guide::Guide(MsgQueue *msgQueue):msgQueue(msgQueue)
{
	initGraph(g);
	loadMap();
	pthread_t p;
	if(pthread_create(&p, NULL, guide, this) == -1)
	{
		#if TEST
		debug("create Guide error");
		#endif
	}
	else
	{
		#if TEST
		debug("create Guide success");
		#endif
	}
}

Guide::~Guide()
{
	dist.clear();
	parent.clear();

	#if TEST
	RecordTime();
	#endif
}
void Guide::initGraph(Graph& g){   // ³õÊ¼»¯µØÍ¼ -1£¬ ÊýÖµ´ú±í±ßid
	for (int i = 0; i<N; i++)
	{
		for (int j = 0; j<M; j++)
		{
			g.matrix[i][j] = -1;     //注意写法，M是前一个节点编号，N是后一个节点编号，矩阵的值代表两节点之间的权重
		}
	}
	vector<int> g_num;
	g.edge_num = 0;
	g.louti_num = 0;
}

/*************open  É¾³ý½áµãid********/    //ÔõÃ´É¾³ýÄØ£¿£¿£¿£¿£¿£¿£¿£¿£¿ÐÞ¸Ä
void Guide::deleteInOpenVec(int nodeid){        //²ÎÊý nodeid ´ú±íÐÇ½Úµã£¿£¿£¿ ÔÚ±éÀúÊ±×îÐ¡µÄ£¬½«Ö®É¾³ý
	vector<starnode*>::iterator iter;
	for (iter = v_open.begin(); iter != v_open.end(); iter++)
    {
		if (nodeid == (*iter)->nodeid)
		{
			v_open.erase(iter);  //×¢ÒâÐ´·¨
			break;
		}
	}
}

starnode* Guide::isInOpenVec(int nodeid)      //Èç¹û½ÚµãÔÚopen±íÖÐ£¬·µ»ØµÄÊÇÒ»¸öÐÇ½Úµã£¨¿ÉÀ©Õ¹½Úµã£©
{
	vector<starnode*>::iterator iter;
	starnode* result = NULL;
	for (iter = v_open.begin(); iter != v_open.end(); iter++)
	{
		if (nodeid == (*iter)->nodeid)
		{
			result = *iter;
			break;
		}
	}
	return result;
}
/************close ÀïÊÇ·ñÓÐÎ»ÖÃindexµÄ½áµã******/
starnode* Guide::isInCloseVec(int nodeid)     //Èç¹ûÐÇ½ÚµãÔÚclose±íÖÐ£¬Ôò·µ»ØÆä½Úµã
{
	vector<starnode*>::iterator iter;
	starnode* result = NULL;
	for (iter = v_close.begin(); iter != v_close.end(); iter++)
	{
		if (nodeid == (*iter)->nodeid)
		{
			result = *iter;
			break;
		}
	}
	return result;
}

void Guide::deleteVec(){      //É¾³ýÇå¿Õopen ºÍclose ±í

	for (int i = 0; i < (int)v_open.size(); i++){
		delete v_open[i];
	}
	for (int i = 0; i < (int)v_close.size(); i++){
		delete v_close[i];
	}
	v_open.clear();
	v_close.clear();

}

bool Guide::check(node Anode, starnode* Bnode, starnode* Cnode) {     //²é¿´½ÚµãÊÇ·ñÔÚÕýÏò
    if ((Cnode->positionx-Anode.position_x)*(Cnode->positionx-Bnode->positionx) + (Cnode->positiony-Anode.position_y)*(Cnode->positiony-Bnode->positiony) < 0)
        return false;
    return true;
}
/*********Á½¸ö²»Í¬indexÖ®¼äÂ·³Ì´ú¼Û*********/
double Guide::distanceFromA2B(starnode* a, starnode* b){      //Á½¸öÐÇ½ÚµãÖ®¼äµÄ¾àÀë

	double distance = sqrt((double)(a->positionx - b->positionx)*(a->positionx - b->positionx)\
		+ (a->positiony - b->positiony)*(a->positiony - b->positiony));
	return distance;
}

starnode* Guide::findMinNode2EndInOpen(starnode* eNode){   //±éÀúopen±í¡£ÕÒ³öÓëeNode¾àÀë×îÐ¡µÄ½Úµã£¨g£©£¬²¢½«Ö®´Óopen±íÄÚÉ¾³ý£¬²¢·µ»ØÐÇ½Úµã  eNode´ú±í×îºó½Úµã
	int min = 65535;      //¾àÀë¶¨ÒåÎª²»¿É´ï
	starnode* result = NULL;

	int deletenodeid = -1;
	int f = 0;
	int minf = 65535;
	starnode* p;    //ÐÇ½Úµã
	for (int i = 0; i < v_open.size(); i++) {
		p = v_open[i];    //±éÀúopen±í
		f = (int)distanceFromA2B(p, eNode);   //Á½¸ö½ÚµãÖ®¼äµÄ¾àÀëh
		f += p->g;   //Ëã³ög




		if(f<minf)
		{
			minf =f;
			result = p;

		}
		deletenodeid = result->nodeid;//printf("minf,nodeid:%d,f=%d,minf=%d\n",p->nodeid,f,minf);   //ÒÑ¾­ÐÞ¸Ä
	}

	deleteInOpenVec(deletenodeid);

	//cout<<"g"<<result->m_index<<endl;
	return result;
}

bool Guide::checkPath(node curnode, starnode* parentNode,starnode* eNode)
{   //寻路。如果现在的节点在close表中，则结束，如果不在
	//printf("enter checkPath\n");

	int curnodeid = curnode.m_index;
	if(isInCloseVec(curnodeid))
	{
		return false;
	}
   if(isInOpenVec(curnodeid))
   {// 更新
		//if(parentNode->g+)
	   starnode* tmp = isInOpenVec(curnodeid);
       double newg = parentNode->g+newedge[g.matrix[parentNode->nodeid][tmp->nodeid]].edgeweightid;  //父节点的G加上父节点到现在open表节点的G
	   if(newg < tmp->g)
	   {
		   tmp->parent = parentNode;       //更新父节点，更新f
		   parent[tmp->nodeid] = parentNode->nodeid;
		   tmp->g = newg;
		   tmp->h = distanceFromA2B(tmp,eNode);
		   tmp->f = tmp->g+tmp->h;
		   dist[tmp->nodeid] = tmp->f;  //现在节点的距离
	   }
	    //printf("%d is in v_open g = %d\n",curnodeid, tmp->g);
	}
	else
	{   //不在open表里面，定义父节点，g，h,f,并加入open表中
		// 添加到开启列表
		starnode* tmpnode = new starnode(curnodeid,curnode.position_x,curnode.position_y,curnode.position_lt,curnode.position_lc);
		tmpnode->parent = parentNode;
		parent[tmpnode->nodeid] = parentNode->nodeid;
		tmpnode->g = parentNode->g+newedge[g.matrix[parentNode->nodeid][curnodeid]].edgeweightid;
		tmpnode->h = distanceFromA2B(tmpnode,eNode);
		tmpnode->f = tmpnode->g+tmpnode->h;
		dist[tmpnode->nodeid] = tmpnode->f;
		v_open.push_back(tmpnode);
		//printf("add %d v_open,g = %d\n",curnodeid,tmpnode->g);
	}
	return true;

}

list<starnode*> Guide::findPath(starnode* eNode){
	//printf("enter find Path\n");
	int lc=eNode->positionlc;
	//cout<<lc<<"lt"<<endl;
    int j=0;
    int max=0;
    if (lc>1)
    {
        for (int i=0;i<lc-1;i++)
        {
            j=j+g.g_num[i];
        }
    }
    for(int i=0;i<lc;i++)
    {
    max=max+g.g_num[i];
    }
    // cout<<j<<"cccc"<<max<<"fdsf"<<endl;
	bool isFind = false;
    list<starnode*> resultvec;
	starnode* Node = NULL;
	while(v_open.size()>0)
	{
		Node = findMinNode2EndInOpen(eNode) ;    //找出下个扩展节点
		//printf("Node:%d\n",Node->nodeid);
		int nodeid = Node->nodeid;
		int endpoint = eNode->nodeid;
		if(nodeid == endpoint)
		{
			isFind = true;
			break;
		}

		for(int i = j; i < max; i++ )
		{
		        //修改该层比较！！！！！！！！！！！！！！！
			if(g.matrix[nodeid][i]>=0&&g.matrix[nodeid][i]<INFI_MAX)
			{    //与节点之间有路存在
				map<int,node>::iterator iter = nodeMap.find(i);
				//if (check(iter->second, eNode, Node))    //并且满足是在前方的节点  新节点：目的节点，当前已节点
				checkPath(iter->second,Node,eNode);      //进入寻路的三种状态：在open表，在close表，或重新加入新节点
			}
		}
		//deletOpenVec(index);
		v_close.push_back(Node);
	}
	//cout << "isFind is " << isFind << endl;
	if(isFind)
	{
			//getPath(resultvec,node);
		while(Node!= NULL)
		{
			resultvec.push_front(Node);
			Node = Node->parent;
		}
	}
	//cout << "done!" << endl;
	return resultvec;
}

list<starnode*> Guide::searchpath(int startpoint, int endpoint){  // ËÑË÷Â·¾¶
	deleteVec();

	dist[startpoint] = 0;
	parent[startpoint] = -1;
	map<int,node>::iterator iter = nodeMap.find(startpoint);
	//printf("%d:%d,%d\n",startpoint, iter->second.position_x,iter->second.position_y);
	starnode* sNode = new starnode(startpoint,iter->second.position_x,iter->second.position_y,iter->second.position_lt,iter->second.position_lc);
	iter = nodeMap.find(endpoint);
	//printf("%d:%d,%d\n",endpoint, iter->second.position_x,iter->second.position_y);
	starnode* eNode = new starnode(endpoint,iter->second.position_x,iter->second.position_y,iter->second.position_lt,iter->second.position_lc);
	v_open.push_back(sNode);   //将开始节点放入open表
	return findPath(eNode);     //开始寻路
}


/***************ÔÚopen ÀïÕÒÒ»¸öµ½ÖÕµã×îÐ¡µÄ½áµã**********/


/*********³·ÏúÓÃnew¿ª±ÙµÄ¿Õ¼ä**********/


void Guide::loadMap()
{
	 ifstream fin("1015.txt");
  string line;
  int num;
  getline(fin, line);
  stringstream linebuf;
  linebuf << line;
    while(linebuf>>num)
    {
         g.g_num.push_back(num);
    }   //µÃµ½¸÷Â¥²ã½ÚµãÊý
    int  num_i=g.g_num.size();
    // for (int j=0;j<num_i;j++)
    //cout<<g.g_num[j]<<endl;
    getline(fin, line);
    stringstream linebuf2;
  linebuf2 << line;
  linebuf2 >>  g.edge_num >>g.louti_num;
  //cout << g.edge_num << ","  <<g.louti_num << endl;   //Êä³ö½ÚµãÊýÄ¿ºÍ±ßÊýÄ¿
  for (int i = 0; i < g.edge_num; i++)
  {     //Êä³ö±ßÐÅÏ¢
    getline(fin, line);
    stringstream buf;
    buf << line;
    buf >> newedge[i].edgeid >> newedge[i].frontnodeid >> newedge[i].lastnodeid >> newedge[i].edgeweightid;
    //cout << newedge[i].edgeid << ":" << newedge[i].frontnodeid << "," << newedge[i].lastnodeid << "," << newedge[i].edgeweightid << endl;
    g.matrix[newedge[i].frontnodeid][newedge[i].lastnodeid] = newedge[i].edgeid;    //¶¨ÒåµÄ±ßµÄÐÅÏ¢
    edgeMap.insert(pair<int, edge>(newedge[i].edgeid, newedge[i]));
  }

   int j=0;
   for (int i=0; i<g.g_num.size(); i++)
   {
       j=j+g.g_num[i];
   }
     for (int i = 0; i<j; i++)   //Êä³ö½ÚµãÐÅÏ¢
    {
        getline(fin, line);
        stringstream nodebuf;
        nodebuf << line;
        nodebuf >> newnode[i].m_index >> newnode[i].position_x >> newnode[i].position_y >>newnode[i].position_lt >>newnode[i].position_lc >> newnode[i].nodename;
        //cout << newnode[i].nodename << " (" << newnode[i].position_x << "," << newnode[i].position_y << ","<< newnode[i].position_lt<< ","<< newnode[i].position_lc  <<  ")" << endl;
          nodeMap.insert(pair<int, node>(newnode[i].m_index, newnode[i]));//ÏòÈÝÆ÷ÖÐ²åÈë½ÚµãÔªËØ
      }

    for (int i = 0; i<g.louti_num; i++)
        {   //Êä³öÂ¥ÌÝ½ÚµãÐÅÏ¢
    getline(fin, line);
    stringstream nodebuf2;
    nodebuf2 << line;
    nodebuf2 >> newlouti[i].louti_index >> newlouti[i].louti_x >> newlouti[i].louti_y >>newlouti[i].louti_lc>> newlouti[i].loutiname;
      //cout<<newlouti[i].loutiname<<endl;
      //loutiMap.insert(pair<int, louti>(newlouti[i].louti_index, newlouti[i]));//ÏòÈÝÆ÷ÖÐ²åÈë½ÚµãÔªËØ
      }
  fin.close();
}


void Guide::getLine( list<starnode*> listNode)
{
  if (listNode.empty()==0)
  {
    cout << "the path is ";
    for (list<starnode*>::iterator it=listNode.begin(); it!=listNode.end(); ++it)
    {
      if (parent[(**it).nodeid] != -1)
      {
       // cout<<newnode[(**it).nodeid].nodename<<"if"<<endl;
        cout << "->" << " ("<<newnode[(**it).nodeid].position_x<<","<<newnode[(**it).nodeid].position_y<<") ";  //Êä³öÆäËûµã×ø±ê
      }
      else
      {
        cout << " ("<<newnode[(**it).nodeid].position_x<<","<<newnode[(**it).nodeid].position_y<<") ";   //Êä³öÆðµã×ø±ê
      }
    }
  }
  cout<<endl;
}


int Guide::getIndexStart2(double point_xs, double point_ys,int point_ls )
{
    double minv = -1;
    int ans = -1;
    int j=0;
    int max=0;
    if (point_ls>1)
        {
          for (int i=0;i<point_ls-1;i++)
              {
                j=j+g.g_num[i];
               }
        }
    for(int i=0;i<point_ls;i++)
        {
            max=max+g.g_num[i];
        }
   //cout<<j<<max<<"df"<<endl;

   for (int i=j; i<max; i++)
       {

        double d = sqrt((newnode[i].position_x-point_xs)*(newnode[i].position_x-point_xs)+
                        (newnode[i].position_y-point_ys)*(newnode[i].position_y-point_ys));
        if (minv == -1 || d < minv)
           {
               minv = d, ans = i;
            }
        }

    return ans;
}

int Guide::getIndexEnd2(double point_xd, double point_yd,int point_ld )
{
    double minv = -1;
    int ans = -1;
    int j=0;
    int max=0;
    if (point_ld>1)
        {
        for (int i=0;i<point_ld-1;i++)
            {
               j=j+g.g_num[i];
            }
        }
    for(int i=0;i<point_ld;i++)
        {
                max=max+g.g_num[i];
        }
      //cout<<j<<max<<"df"<<endl;

   for (int i=j; i<max; i++)
       {
        double d = sqrt((newnode[i].position_x-point_xd)*(newnode[i].position_x-point_xd)+
                        (newnode[i].position_y-point_yd)*(newnode[i].position_y-point_yd));
                        //cout<<i<<d<<endl;
        if (minv == -1 || d < minv)
           {
            minv = d, ans = i;
           //cout<<i<<"change"<<endl;
            }
        }

    return ans;
}

int Guide::getIndexStart(double point_xs, double point_ys,int point_ls, double point_xd, double point_yd)
{
    double minv = -1;
    int ans = -1;
    int j=0;
    int max=0;
    if (point_ls>1)
        {
        for (int i=0;i<point_ls-1;i++)
            {
                j=j+g.g_num[i];
            }
        }
    for(int i=0;i<point_ls;i++)
        {
            max=max+g.g_num[i];
        }
  //cout<<j<<max<<"df"<<endl;
   for (int i=j; i<max; i++)
   {
       //cout<<newnode[i].position_x<<"dffdssssssa"<<endl;

        if((newnode[i].position_x-point_xs)*(point_xd-point_xs)+(newnode[i].position_y-point_ys)*(point_yd-point_ys)>=0)
           {
               double d = sqrt((newnode[i].position_x-point_xs)*(newnode[i].position_x-point_xs)+(newnode[i].position_y-point_ys)*(newnode[i].position_y-point_ys));
                        //cout<<i<<"df "<<d<<endl;
              if (minv == -1 || d < minv)
                 {
                     minv = d, ans = i;
                     //cout<<ans<<endl;
                 }
        }
   }
   if (ans==-1)
      {
         for (int i=j; i<max; i++)
            {
              double d = sqrt((newnode[i].position_x-point_xs)*(newnode[i].position_x-point_xs)+
                        (newnode[i].position_y-point_ys)*(newnode[i].position_y-point_ys));
              if (minv == -1 || d < minv)
                 {
                     minv = d, ans = i;
                 }
            }
       }
     return ans;
}

int Guide::getIndexEnd(double point_xs, double point_ys,int point_ls, double point_xd, double point_yd)
{
    double minv = -1;
    int ans = -1;
    int j=0;
    int max=0;
    if (point_ls>1)
        {
        for (int i=0;i<point_ls-1;i++)
            {
                j=j+g.g_num[i];
            }
        }
        for(int i=0;i<point_ls;i++)
            {
                max=max+g.g_num[i];
            }
   for (int i=j; i<max; i++)
        {
           // cout<<newnode[i].position_x<<"dffdssssssa"<<endl;
          if((newnode[i].position_x-point_xd)*(point_xs-point_xd)+(newnode[i].position_y-point_yd)*(point_ys-point_yd)>=0)
             {
                double d = sqrt((newnode[i].position_x-point_xd)*(newnode[i].position_x-point_xd)+(newnode[i].position_y-point_yd)*(newnode[i].position_y-point_yd));
         // cout<<d<<"dd"<<endl;
             if (minv == -1 || d < minv)
                 {
                     minv = d, ans = i;
                 }
            }
         }
   if (ans==-1)
      {
       for (int i=j; i<max; i++)
           {
             double d = sqrt((newnode[i].position_x-point_xd)*(newnode[i].position_x-point_xd)+
                        (newnode[i].position_y-point_yd)*(newnode[i].position_y-point_yd));
             if (minv == -1 || d < minv)
                {
                    minv = d, ans = i;
                }
        }
     }
    return ans;
}

int  Guide::get_Xuhao( vector<double> v_sum1,vector<double> v_sum2)
{
    double sumx=65535;
    int i=-1;
    for(int j=0;j<(int)v_sum1.size(); j++)
       {
        double sumx2=v_sum1[j]+v_sum2[j];
        if( sumx2<sumx)
          {
            sumx=sumx2;
            i=j;
             //cout<<"sumx2changge  "<<sumx<<endl;

          }
      }
    return i;
}


double Guide::getSum ( list<starnode*> listNode )
{
    double   sum = 0;
    for (list<starnode*>::iterator it=listNode.begin(); it!=listNode.end(); ++it)
       {
           if (parent[(**it).nodeid] != -1)
              {
                sum += newedge[g.matrix[parent[(**it).nodeid]][(**it).nodeid]].edgeweightid;
                    //cout<<sum<<"sum"<<endl;
              }
        }
    //cout<<"sumd"<<sum<<endl;
    return sum;
}

void *Guide::guide(void *arg)
{
	Guide *guide = (Guide*)arg;
	double sum;
	double point_x, point_y;
	list<starnode*> listNode;
	std::pair<int,int> sour,dest;
	int startPoint_index;   //开始节点
	int endPoint_index;     //目的节点

	int point_xs, point_ys,point_ls;
    int point_xd, point_yd,point_ld;

	int id;
	/*while (true){
		//获取数据    
		if((guide->msgQueue)->startGuide(id,sour,dest) == -1)	//no guide request
			continue;
		guide->startPoint_index = guide->getIndexStart(sour.first,sour.second);
		guide->endPoint_index = guide->getIndexStart(dest.first,dest.second);
      
        // A* 查找算法
        guide->dist.clear();
        guide->parent.clear();
        listNode = guide->searchpath(guide->startPoint_index,guide->endPoint_index);

	   //输出结果
		std::vector<std::pair<int,int> > path;
        for (auto it=listNode.begin(); it!=listNode.end(); ++it) 
        {
        	path.push_back(std::make_pair((*it)->positionx,(*it)->positiony));
			cout<<(*it)->positionx<<" "<<(*it)->positiony<<"->"<<endl;	
        }
		(guide->msgQueue)->finishGuide(id,path);
        debug("finish one Guide");

        //测试导引时间
		#if TEST
   		if(guide->test()==-1)
   			cout<<"error in run"<<endl;
		#endif
	}*/

	 while (true)
        {    //定义输入输出方式

		   // cout << "input 当前位置(包括坐标和楼层)startPoint_index:";
		   // cin >> point_xs >> point_ys>>point_ls;

		   // cout << "input 目的位置(包括坐标和楼层)endPoint_index:";
     //       cin >> point_xd >> point_yd>>point_ld;

    //获取数据    
		if((guide->msgQueue)->startGuide(id,point_xs, point_ys,point_ls,point_xd, point_yd,point_ld) == -1)	//no guide request
			continue;
 
    //记录导引开始时间
    guide->StartTime();

    //开始导引
    while(1)
    {
        if (point_ls==point_ld)    //同一层
        {
            startPoint_index = guide->getIndexStart(point_xs, point_ys,point_ls,point_xd, point_yd);   //找最近点
            //cout<<startPoint_index <<"start"<<endl;
              endPoint_index = guide->getIndexEnd(point_xs, point_ys,point_ls,point_xd, point_yd);
              //cout<<endPoint_index <<"end"<<endl;
            guide->dist.clear();
            guide->parent.clear();
            listNode = guide->searchpath(startPoint_index,endPoint_index);
            guide->getLine( listNode) ;

            //输出结果
  	        std::vector<std::pair<int,int> > path;
            for (auto it=listNode.begin(); it!=listNode.end(); ++it) 
            {
        	     path.push_back(std::make_pair((*it)->positionx,(*it)->positiony));
            }

            //结束导引
		        (guide->msgQueue)->finishGuide(id,path);

			        //测试导引时间
        		#if TEST
                debug("finish one Guide");
           		if(guide->EndTime()==-1)
           			cout<<"error in run"<<endl;
        		#endif
                     break;
                   }

                else if (point_ls!=point_ld)   //不同一层 
                       {
                        int panduan=0;
                        for ( int i=0;i<(guide->g).louti_num; i++)
                            {
                              if (guide->newlouti[i].louti_x==point_xs&&guide->newlouti[i].louti_y==point_ys )
                                 {
                                    endPoint_index=guide->newlouti[i].louti_index;
                                    panduan==1;   //X为楼梯,找Y的最近点
                                    startPoint_index = guide->getIndexStart(point_ys, point_xs,point_ld,point_yd, point_xd);
                                    guide->dist.clear();
                                    guide->parent.clear();
                                    listNode = guide->searchpath(startPoint_index,endPoint_index);
                                    guide->getLine( listNode) ;

                                    //输出结果
									std::vector<std::pair<int,int> > path;
							        for (auto it=listNode.begin(); it!=listNode.end(); ++it) 
							        {
							        	path.push_back(std::make_pair((*it)->positionx,(*it)->positiony));
							        }
									(guide->msgQueue)->finishGuide(id,path);

							        //测试导引时间
									#if TEST
							        debug("finish one Guide");
							   		if(guide->EndTime()==-1)
							   			cout<<"error in run"<<endl;
									#endif

                                    break;
                                 }
                             if (guide->newlouti[i].louti_x==point_xd &&guide->newlouti[i].louti_y==point_yd )  //Y为楼梯 ,找x的最近点
                                {
                                    endPoint_index=guide->newlouti[i].louti_index;
                                    panduan==1;
                                    startPoint_index = guide->getIndexStart(point_xs, point_ys,point_ls,point_xd, point_yd);
                                    guide->dist.clear();
                                    guide->parent.clear();
                                    listNode = guide->searchpath(startPoint_index,endPoint_index);
                                    guide->getLine( listNode) ;

                                    //输出结果
									std::vector<std::pair<int,int> > path;
							        for (auto it=listNode.begin(); it!=listNode.end(); ++it) 
							        {
							        	path.push_back(std::make_pair((*it)->positionx,(*it)->positiony));
							        }
									(guide->msgQueue)->finishGuide(id,path);

							        //测试导引时间
									#if TEST
							        debug("finish one Guide");
							   		if(guide->EndTime()==-1)
							   			cout<<"error in run"<<endl;
									#endif
                                    break;
                                }
                            }
                        if (panduan==0)
                        {
                            startPoint_index = guide->getIndexStart2(point_xs, point_ys,point_ls);
                            //cout<<startPoint_index <<"start"<<endl;
                           	int startPoint_index2= guide->getIndexEnd2(point_xd, point_yd,point_ld);
                          // cout<<startPoint_index2 <<"start2"<<endl;
                           	vector <int>v_ele1;
                           	vector <int>v_ele2;
                           	for(int i=0;i<(guide->g).louti_num; i++)
                              	{
                                	if (guide->newlouti[i].louti_lc==point_ls)
                                   	//cout<<guide->newlouti[i].louti_index<<"louti"<<endl;
                                    	v_ele1.push_back(guide->newlouti[i].louti_index);
                                	if (guide->newlouti[i].louti_lc==point_ld)
                                    	v_ele2.push_back(guide->newlouti[i].louti_index);    //定义了楼梯口新节点   楼梯口的序号直接换过了
                              }
                         	int jj=(int) v_ele2.size();  //
                          	//vector<list<starnode*> > v_lis1;
                          	//vector<list<starnode*> > v_lis2;
                         	vector<double> v_sum1;
                         	vector<double> v_sum2;

                 //找开始节点和目的节点的最近点
                   //for循环找多个list
                       	for (int ii=0;ii<jj;ii++)
                           {
                                listNode = guide->searchpath(startPoint_index, v_ele1[ii]);
                                //guide->getLine( listNode) ;
                                sum=0;
                                sum= guide->getSum (listNode );
                                //cout<<"sum:"<<sum<<endl;
                                v_sum1.push_back(sum);

                                //v_lis1.push_back(listNode);
                                //guide->getLine(v_lis1[ii]);
                                //listNode.clear();  //????
                          }

	                    for (int ii=0;ii<jj;ii++)
	                        {
	                            listNode = guide->searchpath(startPoint_index2, v_ele2[ii]);
	                            sum=0;
	                            sum=guide->getSum (listNode );
	                            //cout<<sum<<"sum2  "<<endl;
	                            v_sum2.push_back(sum);
	                            //v_lis2.push_back(listNode);
	                            listNode.clear();  //????
	                        }

	                    //for(int i=0; i<(int)v_sum2.size();i++){
	                    //cout<<v_sum2[i]<<"yuas"<<endl}

	                    int i_xuhao= guide->get_Xuhao(v_sum1,v_sum2);
	                    //cout<<i_xuhao<<"xuhao"<<endl;
	                    listNode = guide->searchpath(startPoint_index, v_ele1[i_xuhao]);
	                    guide->getLine( listNode) ;

	                     //输出结果
	                    std::vector<std::pair<int,int> > path;
				        while(!listNode.empty())
			        	{
			        		auto item = listNode.front();
				        	path.push_back(std::make_pair(item->positionx,item->positiony));
				        	listNode.pop_front();
			        	}

	                    listNode = guide->searchpath(startPoint_index2, v_ele2[i_xuhao]);
	                    guide->getLine( listNode) ;//getLine( v_lis2[i_xuhao]) ;

	                    //输出结果
	                    listNode.pop_back();
	                    while(!listNode.empty())
			        	{
			        		auto item = listNode.back();
				        	path.push_back(std::make_pair(item->positionx,item->positiony));
				        	listNode.pop_back();
			        	}
						(guide->msgQueue)->finishGuide(id,path);

				        //测试导引时间
						#if TEST
				        debug("finish one Guide");
				   		if(guide->EndTime()==-1)
				   			cout<<"error in run"<<endl;
						#endif

	                    break;

              }

            }
        }
    }


}



int Guide::StartTime(void)
{
  struct timeval tv;
  if(gettimeofday(&tv,(struct timezone*)NULL)==-1)
  {
    perror("in Location::test, gettimeofday");
    return -1;
  }
  startTime.push_back(tv);
  receiveBufSize.push_back(msgQueue->getLocateNum()); 

}

int Guide::EndTime(void)
{
  struct timeval tv;
  if(gettimeofday(&tv,(struct timezone*)NULL)==-1)
  {
    perror("in Location::test, gettimeofday");
    return -1;
  }
  endTime.push_back(tv);

}


void Guide::RecordTime(void)
{
	ofstream out;
	out.open("./testData/GuideTime.txt");
	if(startTime.size() != receiveBufSize.size())
	{
		cout<<"size of guideDuring , receiveBufSize and sendBufsize are not same!"<<endl;
		return;
	}
	double dur;
	out<<"   dur         receiveBufSize"<<endl;
	for(size_t a = 0;a<startTime.size()-1;++a)
	{
    dur = endTime[a].tv_sec - startTime[a].tv_sec +
          (endTime[a].tv_usec - startTime[a].tv_usec)* 1e-6;
		out<<dur<<"us          "<<receiveBufSize[a]<<"items"<<endl;
	}
	out.close();
}


// int main(void)
// {
// 	Guide guide;
// 	sleep(1000);
// }
