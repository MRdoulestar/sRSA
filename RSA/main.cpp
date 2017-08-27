/*
	分布式的RSA加密、解密实现
	时间:2017.6.4
	作者：DoubleStar
	单位:浙江工业大学
*/

#include "main.h"
static bign p(0);
static bign q(0);
static bign e(0);
static bign d(0);
static bign n(0);
static bign f_n(0);
int stop_flag_q = 0;	//结束进程标志
int stop_flag_p = 0;
int c_p = 0;	//上线的p服务器数量计数
int c_q = 0;	//上线的q服务器数量计数
int len_p = 78;	//78	p和q的位数
int len_q = 77;	//77	p和q的位数

struct PQ_ip
{
	string ip = "123.206.199.115";
};
//p和q分别为2^257和 2^255左右的大数，生成的n=p*q约为512位
//用以生成e的素数表
int primeNum[] = { 3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,
				157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,
				313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,
				487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,
				673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,
				877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,
				1061,1063,1069,1087,1091,109,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,
				1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,
				1423,1427,1429,1433,1439,1447,1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,
				1567,1571,1579,1583,1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,1709,1721,1723,
				1733,1741,1747,1753,1759,1777,1783.1787,1789,1801,1811,1823,1831.1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,
				1913,1931,1933,1949,1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,2063,2069,2081,2083,2087,2089,
				2099,2111,2113,2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,
				2293,2297,2309,2311,2333,2339,2341,2347,2351,2357,2371,2377,2381,2383,2389,2393,2399,2411,2417,2423,2437,2441,2447,
				2459,2467,2473,2477,2503,2521,2531,2539,2543,2549,2551,2557,2579,2591
};

char* pqRand(int len) {	//生成指定位数的char*型大数
	char* t;
	t = new char[len];
	for (int i = 0; i <len; i++) {
		if (i == 0) {
			t[i] = char((rand() % 9) + 49);
		}
		t[i] = char((rand() % 10) + 48);
	}
	return t;
}

bign getPandQ(int len) {	//转换成bing类型大数后的p和q
	char* t;
	string n;
	t = pqRand(len);
	n = t;
	n = n.substr(0,len);
	while (bign(n.data())%bign(2)!=bign(1)) {
		t = pqRand(len);
		n = t;
		n = n.substr(0, len);
	}
	return bign(n.data());
}
//二分法快速幂取模(递归) 实际测试，速度快于非递归法

bign quickmod(bign a, bign n, bign c)
{
	bign t(0);
	if (n == 0) return bign(1) % c;
	if (n == 1) return a % c;
	t=quickmod(a, n / 2, c);//递归调用，采用二分递归算法，注意这里n/2会带来奇偶性问题  
	t = (t * t) % c;//二分，乘上另一半再求模  
	if (n % 2 == 1)//n是奇数，因为n/2还少乘了一次a  
		t = t * a %c;
	return t;
}

//二分法快速幂取模(非递归)
/*
bign quickmod(bign a, bign b, bign m)//a 的 b 次方 对 m 取模 
{
	bign s(1);
	while (b != 0)//b 次方就循环 b 次 
	{
		if (b % 2 == 1)//等价于 b%2，如果 b 是奇数 
		{
			s = s * a % m;//乘以 a 后对 m 取模
			b = b - 1;
		}
		a = a * a % m;
		b /= 2;//等价于 b/= 2 
	}
	return s;
}
*/
//米勒罗宾判别法
int Miller_Rabbin(bign  n)
{
	int row[] = {2,3,5,7,11,13,17,19};		//(1/5)^8次方的概率误差，准确率很高
	bign nn(n - 1);
	bign s(0);
	bign ff_n(0);
	bign t(0);
	while (nn%bign(2) == bign(0)) {
		s++;
		nn /= bign(2);
	}
	ff_n = nn;
	//cout <<"s:"<< s << endl;
	//cout <<"d:"<< d << endl;
	//以上求出s和d，其中2^s*d == n-1
	//以下，使用随机几个数a，进行判断
	for (int i = 0; i < sizeof(row)/4;i++) {
		t = quickmod(row[i], ff_n, n);
		//cout <<"t:"<< t << endl;
		if (t != 1) {
			for (bign r(0); r <= (s - bign(1)); r++) {
				bign tt(quickmod(row[i], bign(bign(2).pow(r))*ff_n, n));
				if (tt == n-1) {
					//cout << "r:" << r << endl;
					//cout << "tt:" << tt << endl;
					return 1;
				}
			}return 0;
		}
	}
	return 1;
}

bign gcd(bign a, bign b) {	//欧几里得法求最大公因数判断互素
	if (a%b == 0) {
		return b;
	}
	else {
		return gcd(b, a%b);
	}
}

bign e_gcd(bign a, bign b, bign &x, bign &y) {	//欧几里得拓展法
	if (b==0) {
		x = 1;
		y = 0;
		return a;
	}
	bign ans(0);
	ans = e_gcd(b,a%b,x,y);
	bign temp(0);
	temp = x;
	x = y;
	y = temp - a / b*y;
	return ans;
}

bign cal(bign a,bign m) {	//求逆元乘数d
	bign x(0);
	bign y(0);
	bign gcd(0);
	gcd = e_gcd(a,m,x,y);
	if (bign(1)%gcd != 0) {
		cout <<"gcd:"<<gcd << endl;
		return 0;
	}
	x *= bign(1) / gcd;
	if (m<0) {
		m = m + bign(2)*m;
	}
	bign ans(0);
	ans = x%m;
	if (ans <= 0) { ans += m; }
	return ans;
}

DWORD WINAPI complex_p(LPVOID lpParamter) {	
	srand((unsigned)time(NULL));	//随机数种子
	bign temp(0);
	temp = getPandQ(len_p);	//获得随机大奇数p
	while (Miller_Rabbin(temp) != 1) {	//如果p不是素数
		if (stop_flag_p == 0) {
			//temp = getPandQ(len_p);
			temp += 2;	//！！！！2017.6.4晚更新优化！！！   直接利用质数的分布，不再重新获得一次大奇数。
		}
		else {  return 0; }
	}
	cout << "已经找到p" << endl;
	stop_flag_p = 1;
	p = temp;
	return 1;
}

DWORD WINAPI complex_q(LPVOID lpParamter) {
	srand((unsigned)time(NULL));	//随机数种子
	bign temp(0);
	temp = getPandQ(len_q);	//获得随机大奇数q
	while (Miller_Rabbin(temp) != 1) {	//如果q不是素数
		if (stop_flag_q == 0) {
			//temp = getPandQ(len_q);	//重新获得随机奇数p
			temp += 2;	//！！！！2017.6.4晚更新优化！！！   直接利用质数的分布，不再重新获得一次大奇数。
		}
		else { return 0; }
	}
	cout << "已经找到q" << endl;
	stop_flag_q = 1;
	q = temp;
	return 1;
}


DWORD WINAPI Socket_p(LPVOID lpParamter) {
	WORD wVersionRequested;
	WSADATA wsaData;
	PQ_ip* ip_a = (PQ_ip*)lpParamter;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		cout << "连接到服务器出错！" << endl;
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		cout << "连接到服务器出错！" << endl;
		return 0;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ip_a->ip.data());	//IP地址为传入结构体参数内的数据
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(40086);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	int flag = send(sockClient, "78", strlen("78") + 1, 0);
	char recvBuf[78];
	if (flag == -1) {
		cout << "连接到p服务器失败！" << endl;
		closesocket(sockClient);
		WSACleanup();
		return 0;
	}
	cout << "连接到p服务器！" <<++c_p<< endl;
	recv(sockClient, recvBuf, 78, 0);
	string test = recvBuf;
	test = test.substr(0, 78);

	if (!stop_flag_p ) {	//如果其他线程已经找到，退出
		cout << "已经找到p" << endl;
		stop_flag_p = 1;	//已经找到p，停止其他所有线程
		p = bign(test.data());
	}
	else {
		closesocket(sockClient);
		WSACleanup();
		return 1;
	}
	
	
}


DWORD WINAPI Socket_q(LPVOID lpParamter) {
	WORD wVersionRequested;
	WSADATA wsaData;
	PQ_ip* ip_b = (PQ_ip*)lpParamter;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		cout << "连接到服务器出错！" << endl;
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		cout << "连接到服务器出错！" << endl;
		return 0;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ip_b->ip.data());	//IP地址为传入结构体参数内的数据
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(40086);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	int flag = send(sockClient, "77", strlen("77") + 1, 0);
	char recvBuf[77];
	if (flag == -1) {
		cout << "连接到q服务器失败！" << endl;
		closesocket(sockClient);
		WSACleanup();
		return 0;
	}
	cout << "连接到q服务器！" << ++c_q << endl;
	recv(sockClient, recvBuf, 77, 0);
	string test = recvBuf;
	test = test.substr(0, 77);

	if (!stop_flag_q) {	//如果其他线程已经找到，退出
		cout << "已经找到q" << endl;
		stop_flag_q = 1;	//已经找到p，停止其他所有线程
		q = bign(test.data());
	}
	else {
		closesocket(sockClient);
		WSACleanup();
		return 1;
	}
}

//判断pq.txt是否已经存在,如果已经存在，从文件读入可用的p和q
int load_file() {
	char pp[79];	//长度+1，存放\0
	char qq[78];
	ifstream infile("pq.txt", ios::in);
	if (!infile.is_open()) {
		cerr << "没有缓存可用的p和q,将通过分布式、多线程计算得到。。。。" << endl;
		return 0;
	}
	cout << "找到缓存可用的p和q。。。。" << endl;
	infile.getline(pp, 79);
	infile.getline(qq, 78);
	p = bign(pp);
	q = bign(qq);
	cout <<"p:"<< p << endl;
	cout <<"q:"<< q << endl;
	infile.close();
	return 1;
}

int save_file() {
	char pp[79];
	char qq[78];
	ofstream outfile("pq.txt",ios::out);
	if (!outfile.is_open()) {
		cerr << "创建缓存文件失败。。。" << endl;
		return 0;
	}
	strcpy(pp, p.toStr().data());
	strcpy(qq, q.toStr().data());

	outfile.write(pp,78);
	outfile << "\n";
	outfile.write(qq,77);
	return 1;
}


int get_plain(bign &plain,int* index) {		//获取要加密的明文和明文索引,并且返回还原编码的字符串长度
	string h;	//用户输入的字符串明文
	string rh = "";	//转化为ascii码
	cout << "请输入要加密的明文：" << endl;
	cin.get();
	getline(cin, h);
	for (int i = 0; i < h.length(); i++) {
		int tmp_int = int(h[i]);
		if (tmp_int>99) {
			index[i] = 3;
		}
		else { index[i] = 2; }
		string tmp_str = to_string(tmp_int);
		rh += tmp_str;
	}
	plain =  bign(rh.data());
	return h.length();
}

int get_md5_plain(bign &md5, int* index) {		//编码MD5和对应索引,并且返回还原编码的字符串长度
	string h;	//用户输入的字符串明文
	string rh = "";	//转化为ascii码
	h = md5.toStr();
	for (int i = 0; i < h.length(); i++) {
		int tmp_int = int(h[i]);
		if (tmp_int>99) {
			index[i] = 3;
		}
		else { index[i] = 2; }
		string tmp_str = to_string(tmp_int);
		rh += tmp_str;
	}
	md5 = bign(rh.data());
	return h.length();
}

void RSA() {
	int pq_exist = 0;
	//通过读取文件判断是否已经缓存有p和q，如果已经存在，从文件读入可用的p和q
	pq_exist = load_file();

	if (!pq_exist) {
		srand((unsigned)time(NULL));	//随机数种子
										//开启多线程挖掘大素数p和q
		PQ_ip *ip_a[5];
		ip_a[0] = new PQ_ip();
		ip_a[0]->ip = "123.206.199.115";
		ip_a[1] = new PQ_ip();
		ip_a[1]->ip = "121.42.52.116";
		ip_a[2] = new PQ_ip();
		ip_a[2]->ip = "115.230.124.26";
		ip_a[3] = new PQ_ip();
		ip_a[3]->ip = "58.53.94.165";
		ip_a[4] = new PQ_ip();
		ip_a[4]->ip = "58.53.94.146";

		PQ_ip *ip_b[2];
		ip_b[0] = new PQ_ip();
		ip_b[0]->ip = "192.210.178.122";
		ip_b[1] = new PQ_ip();
		ip_b[1]->ip = "58.53.94.154";

		//远程服务器计算线程
		for (int i = 0; i < 5; i++) {
			HANDLE hThread_p_10 = CreateThread(NULL, 0, Socket_p, ip_a[i], 0, NULL);
			CloseHandle(hThread_p_10);
			Sleep(1500);
		}
		for (int i = 0; i < 2; i++) {
			HANDLE hThread_q_10 = CreateThread(NULL, 0, Socket_q, ip_b[i], 0, NULL);
			CloseHandle(hThread_q_10);
			Sleep(1500);
		}
		cout << "远程线程已全部开始工作" << endl;

		//本地计算线程
		for (int i = 0; i < 3; i++) {
			HANDLE hThread_p_1 = CreateThread(NULL, 0, complex_p, NULL, 0, NULL);
			CloseHandle(hThread_p_1);
			Sleep(1200);
		}

		for (int i = 0; i < 3; i++) {
			HANDLE hThread_q_1 = CreateThread(NULL, 0, complex_q, NULL, 0, NULL);
			CloseHandle(hThread_q_1);
			Sleep(1200);
		}

		cout << "本地6个线程已全部开始工作" << endl;
		//耗时记录
		time_t begintime;
		time(&begintime);
		int count = 0;

		//主线程开始等待结果
		while (stop_flag_p == 0) { Sleep(1000); }//防止过快循环假占用CPU
		cout << "p:" << p << endl;
		while (stop_flag_q == 0) { Sleep(1000); }
		cout << "q:" << q << endl;

		time_t endtime;
		time(&endtime);
		count = endtime - begintime;
		cout << "消耗时间：" << count << endl;
		//cout << Miller_Rabbin(p) << endl;
		//cout << Miller_Rabbin(q) << endl;
		//将获得的p和q缓存至文件
		if (save_file()) {
			cout << "成功缓存此次p和q。。。" << endl;
		}

	}


	//通过上述步骤，已经获得p、q,开始计算n、f_n、e、d
	//if (n==0 && f_n==0 && e==0 && d==0){	//如果已经获取过，则无需获取
		n = p * q;
		f_n = (p - 1)*(q - 1);
		for (int i = 0; i < sizeof(primeNum) / 4; i++) {
			if (gcd(primeNum[i], f_n) == 1) {
				e = primeNum[i];
				break;
			}
			if (i == (sizeof(primeNum) / 4) - 1 && e == 0) {
				cout << "素数表内没有可以和f_n互质的e！" << endl;
			}
		}d = cal(e, f_n);//欧几里得拓展法求d,d是e关于f_n的乘法逆元
	//}
	
	cout << "e和f_n是否互质:" << gcd(e, f_n) << endl;
	cout << "n:" << n << endl;
	cout << "f_n:" << f_n << endl;
	cout << "e:" << e << endl;
	cout << "d:" << d << endl;

	//RSA加密解密,使用自制编码表，以支持字符串的加密
	bign plain(0);
	bign c(0);
	bign result(0);
	int len = 0;
	int index[512] = { 0 };
	len = get_plain(plain, index);	//获取要加密的明文和明文索引,并且返回还原编码的字符串长度

	c = quickmod(plain, e, n);	//加密
	cout << "密文c：" << c << endl;
	cout << endl;
	cout << "--------------------------------------" << endl;
	cout << endl;
	result = quickmod(c, d, n);	//解密
	cout << "未编码明文：" << result << endl;

	string temp = result.toStr();
	//将解密的字符串编码还原
	cout << "编码还原后明文：";
	for (int i = 0, j = 0; i < len; i++) {
		string a = temp.substr(j, index[i]);
		int ascii = std::atoi(a.c_str());
		cout << char(ascii);
		j += index[i];
	}cout << endl;

	system("pause");
}


string get_md5(string plain) {		//通过get HTTP数据来调用PHP获得MD5结果
	WORD wVersionRequested;
	WSADATA wsaData;
	char* ip = "121.42.52.116";
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		cout << "连接到服务器出错！" << endl;
		return "0";
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		cout << "连接到服务器出错！" << endl;
		return "0";
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);	//IP地址为传入结构体参数内的数据
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(80);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	string str = "GET /rsaapi/md5.php?str="+plain+" HTTP/1.1\r\n";
	str.append("Connection:Keep-Alive\r\n");
	//str.append("Accept-Encoding:gzip, deflate\r\n");
	str.append("Accept-Language:zh-CN,en,*\r\n");
	str.append("host:121.42.52.116\r\n");
	str.append("User-Agent:Mozilla / 5.0\r\n\r\n");

	int flag = send(sockClient, str.data(), strlen(str.data()) + 1, 0);
	char recvBuf[512];
	if (flag == -1) {
		cout << "连接到md5服务器失败！" << endl;
		closesocket(sockClient);
		WSACleanup();
		return "0";
	}
	recv(sockClient, recvBuf, 512, 0);
	string md5 = recvBuf;
	md5 = md5.substr(204, 32);
	cout <<"从服务器获得md5："<<md5 << endl;
	closesocket(sockClient);
	WSACleanup();
	return md5;
}



void Sign() {		//数字签名
	int pq_exist = 0;
	//通过读取文件判断是否已经缓存有p和q，如果已经存在，从文件读入可用的p和q
	pq_exist = load_file();

	if (!pq_exist) {
		srand((unsigned)time(NULL));	//随机数种子
										//开启多线程挖掘大素数p和q
		PQ_ip *ip_a[2];
		ip_a[0] = new PQ_ip();
		ip_a[0]->ip = "123.206.199.115";
		ip_a[1] = new PQ_ip();
		ip_a[1]->ip = "121.42.52.116";
		ip_a[2] = new PQ_ip();
		ip_a[2]->ip = "115.230.124.26";
		ip_a[3] = new PQ_ip();
		ip_a[3]->ip = "58.53.94.165";
		ip_a[4] = new PQ_ip();
		ip_a[4]->ip = "58.53.94.146";

		PQ_ip *ip_b[2];
		ip_b[0] = new PQ_ip();
		ip_b[0]->ip = "192.210.178.122";
		ip_b[1] = new PQ_ip();
		ip_b[1]->ip = "58.53.94.154";
		//远程服务器计算线程
		for (int i = 0; i < 5; i++) {
			HANDLE hThread_p_10 = CreateThread(NULL, 0, Socket_p, ip_a[i], 0, NULL);
			CloseHandle(hThread_p_10);
			Sleep(1500);
		}
		for (int i = 0; i < 2; i++) {
			HANDLE hThread_q_10 = CreateThread(NULL, 0, Socket_q, ip_b[i], 0, NULL);
			CloseHandle(hThread_q_10);
			Sleep(1500);
		}
		cout << "远程线程已全部开始工作" << endl;

		//本地计算线程
		for (int i = 0; i < 4; i++) {
			HANDLE hThread_p_1 = CreateThread(NULL, 0, complex_p, NULL, 0, NULL);
			CloseHandle(hThread_p_1);
			Sleep(1200);
		}

		for (int i = 0; i < 4; i++) {
			HANDLE hThread_q_1 = CreateThread(NULL, 0, complex_q, NULL, 0, NULL);
			CloseHandle(hThread_q_1);
			Sleep(1200);
		}

		cout << "本地8个线程已全部开始工作" << endl;
		//耗时记录
		time_t begintime;
		time(&begintime);
		int count = 0;

		//主线程开始等待结果
		while (stop_flag_p == 0) { Sleep(1000); }//防止过快循环假占用CPU
		cout << "p:" << p << endl;
		while (stop_flag_q == 0) { Sleep(1000); }
		cout << "q:" << q << endl;

		time_t endtime;
		time(&endtime);
		count = endtime - begintime;
		cout << "消耗时间：" << count << endl;
		//cout << Miller_Rabbin(p) << endl;
		//cout << Miller_Rabbin(q) << endl;
		//将获得的p和q缓存至文件
		if (save_file()) {
			cout << "成功缓存此次p和q。。。" << endl;
		}

	}


	//通过上述步骤，已经获得p、q,如果之前未获取过n、f_n、e、d,开始计算n、f_n、e、d
	if (n==0 && f_n==0 && e==0 && d==0) {
		n = p * q;
		f_n = (p - 1)*(q - 1);
		for (int i = 0; i < sizeof(primeNum) / 4; i++) {
			if (gcd(primeNum[i], f_n) == 1) {
				e = primeNum[i];
				break;
			}
			if (i == (sizeof(primeNum) / 4) - 1 && e == 0) {
				cout << "素数表内没有可以和f_n互质的e！" << endl;
			}
		}
		d = cal(e, f_n);//欧几里得拓展法求d,d是e关于f_n的乘法逆元
	}
	cout << "e和f_n是否互质:" <<  endl;
	cout << "n:" << n << endl;
	cout << "f_n:" << f_n << endl;
	cout << "e:" << e << endl;
	cout << "d:" << d << endl;

	//RSA加密解密,使用自制编码表，以支持字符串的加密
	bign plain(0);	//原文
	bign Bmd5(0);
	bign c(0);
	bign result(0);
	string md5;	//摘要
	int len = 0;
	int index[512] = { 0 };
	
	get_plain(plain, index);	//获取要加密的明文和明文索引,并且返回还原编码的字符串长度
	//获取明文的MD5摘要值
	md5 = get_md5(plain.toStr());	//调用PHP获得MD5
	cout << "MD5摘要:" << md5 << endl;
	Bmd5 = bign(md5.data());
	len = get_md5_plain(Bmd5, index);
	//对MD5摘要进行RSA加密
	c = quickmod(Bmd5, d, n);	//加密
	cout << "加密的MD5密文c：" << c << endl;
	cout << endl;
	cout << "--------------------------------------" << endl;
	cout << endl;
	result = quickmod(c, e, n);	//解密
	cout << "未编码的MD5明文：" << result << endl;
	
	string temp = result.toStr();
	//将解密的字符串编码还原
	cout << "编码还原后的MD5明文：";
	for (int i = 0, j = 0; i < len; i++) {
		string a = temp.substr(j, index[i]);
		int ascii = std::atoi(a.c_str());
		cout << char(ascii);
		j += index[i];
	}cout << endl;
	system("pause");
	
}


void PKey() {
	cout << "我的公钥(e,n)：(" << e << "," << n << ")" << endl;
	system("pause");
}

void SKey() {
	cout << "我的私钥(d,n)：(" << d << "," << n << ")" << endl;
	system("pause");
}

void menu() {	//菜单界面
	int flag = 1;
	cout << "--------------------------------------------------------" << endl;
	cout << "            欢迎使用RSA加解密、数字签名工具           " << endl;
	cout << "                 本工具由DoubleStar开发           " << endl;
	cout << "                    用于实验研究用途           " << endl;
	cout << "                      浙江工业大学           " << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "工具特性：" << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "                   1、基于512位RSA           " << endl;
	cout << "                   2、支持分布式计算           " << endl;
	cout << "                   3、支持多线程进行计算           " << endl;
	cout << "                   4、利用API,使用MD5获取摘要           " << endl;
	cout << "                   5、支持秘钥缓存           " << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "待实现：" << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "                   1、文件加密和签名           " << endl;
	cout << "                   2、利用CUDA调用GPU加速计算           " << endl;
	cout << "                   3、本地MD5计算           " << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "阅读以上信息，按任意键继续。。。" << endl;
	system("pause");
	Sleep(500);
	while (1) {
		flag = 0;
		system("cls");
		cout << "--------------------------------------------------------" << endl;
		cout << "选项：" << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "                  1、512位RSA加密          " << endl;
		cout << "                  2、基于RSA的数字签名           " << endl;
		cout << "                  3、查看我的公钥           " << endl;
		cout << "                  4、查看我的私钥           " << endl;
		cout << "                  5、退出程序           " << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "请输入：" << endl;
		cin >> flag;
		switch (flag)
		{
			case 1:RSA(); break;
			case 2:Sign(); break;
			case 3:PKey(); break;
			case 4:SKey(); break;
			case 5:exit(1); break;
			default:cout << "请输入正确选项。。。" << endl;
				Sleep(1000);break;
		}
	}
}

//main函数
int main() {
	menu();	//显示开始菜单
	return 0;
}