#pragma once
#ifndef UTIL_H
#define UTIL_H
#include <vector>
#include <map>
#include <string>
using namespace std;

class Util
{
public:
	Util();
	virtual ~Util();
	//�ַ���ת����double
	static double stringToNum(const string& str);
	static string numToString(const double& a);

	static vector<string> split(const string& content, const char splitchar);
	static map<string, double> StringToMap(string content);
	// static map<string,double> StringToMap(string content);
	static string MapToString(const map<string, double>& smap);
	//map1 map2�����ϵ��ƥ��
	static double MapMatch(const map<string, double>& map1, const map<string, double>& map2,int m);
	static double MapMatchW(const map<string, double>& map1, const map<string, double>& map2);
	//map1,map2 ����ƥ��
	static double MapMathErr2(const map<string, double>& map1, const map<string, double>& map2);
	//��ֵ
	static double mean(const vector<double>& v);
	//����
	static double var(const vector<double>& v);
	//Э����
	static double cov(const vector<double>& v1, const vector<double>& v2);
	//�˻�
	static double module(const vector<double>& v);
	//sum += v1 * v2
	static double dotProduct(const vector<double>& v1, const vector<double>& v2);
	
	
	
    //Ƥ��ѷϵ��
	static double corr(const vector<double>& v1, const vector<double>& v2);
	//�н�����
	static double cosine(const vector<double>& v1, const vector<double>& v2);
	
	//���ϵ��
	static double coefficient(const vector<double>& v1, const vector<double>& v2);
    //Dice ϵ��
	static double dice(const vector<double>& v1, const vector<double>& v2);
	//���Ͼ���
	static double minkowsky(const vector<double>& v1, const vector<double>& v2, double n);
	//ŷʽ����
	static double euclidean(const vector<double>& v1, const vector<double>& v2);
	static double euclidean1(const vector<double>& v1, const vector<double>& v2);
protected:
private:
};

#endif // UTIL_H

