#pragma once  
#include <iostream>  
#include <iosfwd>  
#include <cmath>  
#include <cstring>  
#include <stdlib.h>    
#include <cstring>  
#include <ctime>
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
  
#define MAX_L 2005   
using namespace std;

class bign
{
public:
	int len, s[MAX_L];//���ĳ��ȣ���¼����  
					  //���캯��  
	bign();
	bign(const char*);
	bign(int);
	bool sign;//���� 1���� 0����  
	string toStr() const;//ת��Ϊ�ַ�������Ҫ�Ǳ������  
	friend istream& operator >> (istream &, bign &);//����������  
	friend ostream& operator<<(ostream &, bign &);//���������  
												  //���ظ���  
	bign operator=(const char*);
	bign operator=(int);
	bign operator=(const string);
	//���ظ��ֱȽ�  
	bool operator>(const bign &) const;
	bool operator>=(const bign &) const;
	bool operator<(const bign &) const;
	bool operator<=(const bign &) const;
	bool operator==(const bign &) const;
	bool operator!=(const bign &) const;
	//������������  
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
	//�����������������  
	bign operator%(const bign&)const;//ȡģ��������  
	bign factorial()const;//�׳�  
	bign Sqrt()const;//��������������ȡ����  
	bign pow(const bign&)const;//�η�  
							   //�����ĺ���  
	void clean();
	~bign();
};
