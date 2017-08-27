/*
分布式的RSA加密、解密实现
时间:2017.6.4
作者：DoubleStar
单位:浙江工业大学
*/
#pragma once
#include<string>  
#include<fstream>
#include<iostream>  
#include<iosfwd>  
#include<cmath>  
#include<cstring>  
#include<stdlib.h>  
#include<stdio.h>  
#include <Winsock2.h>
#include<cstring>  
#include<ctime>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#define MAX_L 2005   
using namespace std;

class bign
{
public:
	int len, s[MAX_L];//数的长度，记录数组  
					  //构造函数  
	bign();
	bign(const char*);
	bign(int);
	bool sign;//符号 1正数 0负数  
	string toStr() const;//转化为字符串，主要是便于输出  
	friend istream& operator >> (istream &, bign &);//重载输入流  
	friend ostream& operator<<(ostream &, bign &);//重载输出流  
												  //重载复制  
	bign operator=(const char*);
	bign operator=(int);
	bign operator=(const string);
	//重载各种比较  
	bool operator>(const bign &) const;
	bool operator>=(const bign &) const;
	bool operator<(const bign &) const;
	bool operator<=(const bign &) const;
	bool operator==(const bign &) const;
	bool operator!=(const bign &) const;
	//重载四则运算  
	bign operator+(const bign &) const;
	bign operator++();
	bign operator++(int);
	bign operator+=(const bign&);
	bign operator-(const bign &) const;
	bign operator--();
	bign operator--(int);
	bign operator-=(const bign&);
	bign operator*(const bign &)const;
	bign operator*(const int num)const;
	bign operator*=(const bign&);
	bign operator/(const bign&)const;
	bign operator/=(const bign&);
	//四则运算的衍生运算  
	bign operator%(const bign&)const;//取模（余数）  
	bign factorial()const;//阶乘  
	bign Sqrt()const;//整数开根（向下取整）  
	bign pow(const bign&)const;//次方  
							   //辅助的函数  
	void clean();
	~bign();
};
