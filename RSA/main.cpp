/*
	�ֲ�ʽ��RSA���ܡ�����ʵ��
	ʱ��:2017.6.4
	���ߣ�DoubleStar
	��λ:�㽭��ҵ��ѧ
*/

#include "main.h"
static bign p(0);
static bign q(0);
static bign e(0);
static bign d(0);
static bign n(0);
static bign f_n(0);
int stop_flag_q = 0;	//�������̱�־
int stop_flag_p = 0;
int c_p = 0;	//���ߵ�p��������������
int c_q = 0;	//���ߵ�q��������������
int len_p = 78;	//78	p��q��λ��
int len_q = 77;	//77	p��q��λ��

struct PQ_ip
{
	string ip = "123.206.199.115";
};
//p��q�ֱ�Ϊ2^257�� 2^255���ҵĴ��������ɵ�n=p*qԼΪ512λ
//��������e��������
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

char* pqRand(int len) {	//����ָ��λ����char*�ʹ���
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

bign getPandQ(int len) {	//ת����bing���ʹ������p��q
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
//���ַ�������ȡģ(�ݹ�) ʵ�ʲ��ԣ��ٶȿ��ڷǵݹ鷨

bign quickmod(bign a, bign n, bign c)
{
	bign t(0);
	if (n == 0) return bign(1) % c;
	if (n == 1) return a % c;
	t=quickmod(a, n / 2, c);//�ݹ���ã����ö��ֵݹ��㷨��ע������n/2�������ż������  
	t = (t * t) % c;//���֣�������һ������ģ  
	if (n % 2 == 1)//n����������Ϊn/2���ٳ���һ��a  
		t = t * a %c;
	return t;
}

//���ַ�������ȡģ(�ǵݹ�)
/*
bign quickmod(bign a, bign b, bign m)//a �� b �η� �� m ȡģ 
{
	bign s(1);
	while (b != 0)//b �η���ѭ�� b �� 
	{
		if (b % 2 == 1)//�ȼ��� b%2����� b ������ 
		{
			s = s * a % m;//���� a ��� m ȡģ
			b = b - 1;
		}
		a = a * a % m;
		b /= 2;//�ȼ��� b/= 2 
	}
	return s;
}
*/
//�����ޱ��б�
int Miller_Rabbin(bign  n)
{
	int row[] = {2,3,5,7,11,13,17,19};		//(1/5)^8�η��ĸ�����׼ȷ�ʺܸ�
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
	//�������s��d������2^s*d == n-1
	//���£�ʹ�����������a�������ж�
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

bign gcd(bign a, bign b) {	//ŷ����÷�����������жϻ���
	if (a%b == 0) {
		return b;
	}
	else {
		return gcd(b, a%b);
	}
}

bign e_gcd(bign a, bign b, bign &x, bign &y) {	//ŷ�������չ��
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

bign cal(bign a,bign m) {	//����Ԫ����d
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
	srand((unsigned)time(NULL));	//���������
	bign temp(0);
	temp = getPandQ(len_p);	//������������p
	while (Miller_Rabbin(temp) != 1) {	//���p��������
		if (stop_flag_p == 0) {
			//temp = getPandQ(len_p);
			temp += 2;	//��������2017.6.4������Ż�������   ֱ�����������ķֲ����������»��һ�δ�������
		}
		else {  return 0; }
	}
	cout << "�Ѿ��ҵ�p" << endl;
	stop_flag_p = 1;
	p = temp;
	return 1;
}

DWORD WINAPI complex_q(LPVOID lpParamter) {
	srand((unsigned)time(NULL));	//���������
	bign temp(0);
	temp = getPandQ(len_q);	//������������q
	while (Miller_Rabbin(temp) != 1) {	//���q��������
		if (stop_flag_q == 0) {
			//temp = getPandQ(len_q);	//���»���������p
			temp += 2;	//��������2017.6.4������Ż�������   ֱ�����������ķֲ����������»��һ�δ�������
		}
		else { return 0; }
	}
	cout << "�Ѿ��ҵ�q" << endl;
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
		cout << "���ӵ�����������" << endl;
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		cout << "���ӵ�����������" << endl;
		return 0;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ip_a->ip.data());	//IP��ַΪ����ṹ������ڵ�����
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(40086);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	int flag = send(sockClient, "78", strlen("78") + 1, 0);
	char recvBuf[78];
	if (flag == -1) {
		cout << "���ӵ�p������ʧ�ܣ�" << endl;
		closesocket(sockClient);
		WSACleanup();
		return 0;
	}
	cout << "���ӵ�p��������" <<++c_p<< endl;
	recv(sockClient, recvBuf, 78, 0);
	string test = recvBuf;
	test = test.substr(0, 78);

	if (!stop_flag_p ) {	//��������߳��Ѿ��ҵ����˳�
		cout << "�Ѿ��ҵ�p" << endl;
		stop_flag_p = 1;	//�Ѿ��ҵ�p��ֹͣ���������߳�
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
		cout << "���ӵ�����������" << endl;
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		cout << "���ӵ�����������" << endl;
		return 0;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ip_b->ip.data());	//IP��ַΪ����ṹ������ڵ�����
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(40086);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	int flag = send(sockClient, "77", strlen("77") + 1, 0);
	char recvBuf[77];
	if (flag == -1) {
		cout << "���ӵ�q������ʧ�ܣ�" << endl;
		closesocket(sockClient);
		WSACleanup();
		return 0;
	}
	cout << "���ӵ�q��������" << ++c_q << endl;
	recv(sockClient, recvBuf, 77, 0);
	string test = recvBuf;
	test = test.substr(0, 77);

	if (!stop_flag_q) {	//��������߳��Ѿ��ҵ����˳�
		cout << "�Ѿ��ҵ�q" << endl;
		stop_flag_q = 1;	//�Ѿ��ҵ�p��ֹͣ���������߳�
		q = bign(test.data());
	}
	else {
		closesocket(sockClient);
		WSACleanup();
		return 1;
	}
}

//�ж�pq.txt�Ƿ��Ѿ�����,����Ѿ����ڣ����ļ�������õ�p��q
int load_file() {
	char pp[79];	//����+1�����\0
	char qq[78];
	ifstream infile("pq.txt", ios::in);
	if (!infile.is_open()) {
		cerr << "û�л�����õ�p��q,��ͨ���ֲ�ʽ�����̼߳���õ���������" << endl;
		return 0;
	}
	cout << "�ҵ�������õ�p��q��������" << endl;
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
		cerr << "���������ļ�ʧ�ܡ�����" << endl;
		return 0;
	}
	strcpy(pp, p.toStr().data());
	strcpy(qq, q.toStr().data());

	outfile.write(pp,78);
	outfile << "\n";
	outfile.write(qq,77);
	return 1;
}


int get_plain(bign &plain,int* index) {		//��ȡҪ���ܵ����ĺ���������,���ҷ��ػ�ԭ������ַ�������
	string h;	//�û�������ַ�������
	string rh = "";	//ת��Ϊascii��
	cout << "������Ҫ���ܵ����ģ�" << endl;
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

int get_md5_plain(bign &md5, int* index) {		//����MD5�Ͷ�Ӧ����,���ҷ��ػ�ԭ������ַ�������
	string h;	//�û�������ַ�������
	string rh = "";	//ת��Ϊascii��
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
	//ͨ����ȡ�ļ��ж��Ƿ��Ѿ�������p��q������Ѿ����ڣ����ļ�������õ�p��q
	pq_exist = load_file();

	if (!pq_exist) {
		srand((unsigned)time(NULL));	//���������
										//�������߳��ھ������p��q
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

		//Զ�̷����������߳�
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
		cout << "Զ���߳���ȫ����ʼ����" << endl;

		//���ؼ����߳�
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

		cout << "����6���߳���ȫ����ʼ����" << endl;
		//��ʱ��¼
		time_t begintime;
		time(&begintime);
		int count = 0;

		//���߳̿�ʼ�ȴ����
		while (stop_flag_p == 0) { Sleep(1000); }//��ֹ����ѭ����ռ��CPU
		cout << "p:" << p << endl;
		while (stop_flag_q == 0) { Sleep(1000); }
		cout << "q:" << q << endl;

		time_t endtime;
		time(&endtime);
		count = endtime - begintime;
		cout << "����ʱ�䣺" << count << endl;
		//cout << Miller_Rabbin(p) << endl;
		//cout << Miller_Rabbin(q) << endl;
		//����õ�p��q�������ļ�
		if (save_file()) {
			cout << "�ɹ�����˴�p��q������" << endl;
		}

	}


	//ͨ���������裬�Ѿ����p��q,��ʼ����n��f_n��e��d
	//if (n==0 && f_n==0 && e==0 && d==0){	//����Ѿ���ȡ�����������ȡ
		n = p * q;
		f_n = (p - 1)*(q - 1);
		for (int i = 0; i < sizeof(primeNum) / 4; i++) {
			if (gcd(primeNum[i], f_n) == 1) {
				e = primeNum[i];
				break;
			}
			if (i == (sizeof(primeNum) / 4) - 1 && e == 0) {
				cout << "��������û�п��Ժ�f_n���ʵ�e��" << endl;
			}
		}d = cal(e, f_n);//ŷ�������չ����d,d��e����f_n�ĳ˷���Ԫ
	//}
	
	cout << "e��f_n�Ƿ���:" << gcd(e, f_n) << endl;
	cout << "n:" << n << endl;
	cout << "f_n:" << f_n << endl;
	cout << "e:" << e << endl;
	cout << "d:" << d << endl;

	//RSA���ܽ���,ʹ�����Ʊ������֧���ַ����ļ���
	bign plain(0);
	bign c(0);
	bign result(0);
	int len = 0;
	int index[512] = { 0 };
	len = get_plain(plain, index);	//��ȡҪ���ܵ����ĺ���������,���ҷ��ػ�ԭ������ַ�������

	c = quickmod(plain, e, n);	//����
	cout << "����c��" << c << endl;
	cout << endl;
	cout << "--------------------------------------" << endl;
	cout << endl;
	result = quickmod(c, d, n);	//����
	cout << "δ�������ģ�" << result << endl;

	string temp = result.toStr();
	//�����ܵ��ַ������뻹ԭ
	cout << "���뻹ԭ�����ģ�";
	for (int i = 0, j = 0; i < len; i++) {
		string a = temp.substr(j, index[i]);
		int ascii = std::atoi(a.c_str());
		cout << char(ascii);
		j += index[i];
	}cout << endl;

	system("pause");
}


string get_md5(string plain) {		//ͨ��get HTTP����������PHP���MD5���
	WORD wVersionRequested;
	WSADATA wsaData;
	char* ip = "121.42.52.116";
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		cout << "���ӵ�����������" << endl;
		return "0";
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		cout << "���ӵ�����������" << endl;
		return "0";
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);	//IP��ַΪ����ṹ������ڵ�����
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
		cout << "���ӵ�md5������ʧ�ܣ�" << endl;
		closesocket(sockClient);
		WSACleanup();
		return "0";
	}
	recv(sockClient, recvBuf, 512, 0);
	string md5 = recvBuf;
	md5 = md5.substr(204, 32);
	cout <<"�ӷ��������md5��"<<md5 << endl;
	closesocket(sockClient);
	WSACleanup();
	return md5;
}



void Sign() {		//����ǩ��
	int pq_exist = 0;
	//ͨ����ȡ�ļ��ж��Ƿ��Ѿ�������p��q������Ѿ����ڣ����ļ�������õ�p��q
	pq_exist = load_file();

	if (!pq_exist) {
		srand((unsigned)time(NULL));	//���������
										//�������߳��ھ������p��q
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
		//Զ�̷����������߳�
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
		cout << "Զ���߳���ȫ����ʼ����" << endl;

		//���ؼ����߳�
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

		cout << "����8���߳���ȫ����ʼ����" << endl;
		//��ʱ��¼
		time_t begintime;
		time(&begintime);
		int count = 0;

		//���߳̿�ʼ�ȴ����
		while (stop_flag_p == 0) { Sleep(1000); }//��ֹ����ѭ����ռ��CPU
		cout << "p:" << p << endl;
		while (stop_flag_q == 0) { Sleep(1000); }
		cout << "q:" << q << endl;

		time_t endtime;
		time(&endtime);
		count = endtime - begintime;
		cout << "����ʱ�䣺" << count << endl;
		//cout << Miller_Rabbin(p) << endl;
		//cout << Miller_Rabbin(q) << endl;
		//����õ�p��q�������ļ�
		if (save_file()) {
			cout << "�ɹ�����˴�p��q������" << endl;
		}

	}


	//ͨ���������裬�Ѿ����p��q,���֮ǰδ��ȡ��n��f_n��e��d,��ʼ����n��f_n��e��d
	if (n==0 && f_n==0 && e==0 && d==0) {
		n = p * q;
		f_n = (p - 1)*(q - 1);
		for (int i = 0; i < sizeof(primeNum) / 4; i++) {
			if (gcd(primeNum[i], f_n) == 1) {
				e = primeNum[i];
				break;
			}
			if (i == (sizeof(primeNum) / 4) - 1 && e == 0) {
				cout << "��������û�п��Ժ�f_n���ʵ�e��" << endl;
			}
		}
		d = cal(e, f_n);//ŷ�������չ����d,d��e����f_n�ĳ˷���Ԫ
	}
	cout << "e��f_n�Ƿ���:" <<  endl;
	cout << "n:" << n << endl;
	cout << "f_n:" << f_n << endl;
	cout << "e:" << e << endl;
	cout << "d:" << d << endl;

	//RSA���ܽ���,ʹ�����Ʊ������֧���ַ����ļ���
	bign plain(0);	//ԭ��
	bign Bmd5(0);
	bign c(0);
	bign result(0);
	string md5;	//ժҪ
	int len = 0;
	int index[512] = { 0 };
	
	get_plain(plain, index);	//��ȡҪ���ܵ����ĺ���������,���ҷ��ػ�ԭ������ַ�������
	//��ȡ���ĵ�MD5ժҪֵ
	md5 = get_md5(plain.toStr());	//����PHP���MD5
	cout << "MD5ժҪ:" << md5 << endl;
	Bmd5 = bign(md5.data());
	len = get_md5_plain(Bmd5, index);
	//��MD5ժҪ����RSA����
	c = quickmod(Bmd5, d, n);	//����
	cout << "���ܵ�MD5����c��" << c << endl;
	cout << endl;
	cout << "--------------------------------------" << endl;
	cout << endl;
	result = quickmod(c, e, n);	//����
	cout << "δ�����MD5���ģ�" << result << endl;
	
	string temp = result.toStr();
	//�����ܵ��ַ������뻹ԭ
	cout << "���뻹ԭ���MD5���ģ�";
	for (int i = 0, j = 0; i < len; i++) {
		string a = temp.substr(j, index[i]);
		int ascii = std::atoi(a.c_str());
		cout << char(ascii);
		j += index[i];
	}cout << endl;
	system("pause");
	
}


void PKey() {
	cout << "�ҵĹ�Կ(e,n)��(" << e << "," << n << ")" << endl;
	system("pause");
}

void SKey() {
	cout << "�ҵ�˽Կ(d,n)��(" << d << "," << n << ")" << endl;
	system("pause");
}

void menu() {	//�˵�����
	int flag = 1;
	cout << "--------------------------------------------------------" << endl;
	cout << "            ��ӭʹ��RSA�ӽ��ܡ�����ǩ������           " << endl;
	cout << "                 ��������DoubleStar����           " << endl;
	cout << "                    ����ʵ���о���;           " << endl;
	cout << "                      �㽭��ҵ��ѧ           " << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "�������ԣ�" << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "                   1������512λRSA           " << endl;
	cout << "                   2��֧�ֲַ�ʽ����           " << endl;
	cout << "                   3��֧�ֶ��߳̽��м���           " << endl;
	cout << "                   4������API,ʹ��MD5��ȡժҪ           " << endl;
	cout << "                   5��֧����Կ����           " << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "��ʵ�֣�" << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "                   1���ļ����ܺ�ǩ��           " << endl;
	cout << "                   2������CUDA����GPU���ټ���           " << endl;
	cout << "                   3������MD5����           " << endl;
	cout << "--------------------------------------------------------" << endl;
	cout << "�Ķ�������Ϣ�������������������" << endl;
	system("pause");
	Sleep(500);
	while (1) {
		flag = 0;
		system("cls");
		cout << "--------------------------------------------------------" << endl;
		cout << "ѡ�" << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "                  1��512λRSA����          " << endl;
		cout << "                  2������RSA������ǩ��           " << endl;
		cout << "                  3���鿴�ҵĹ�Կ           " << endl;
		cout << "                  4���鿴�ҵ�˽Կ           " << endl;
		cout << "                  5���˳�����           " << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "�����룺" << endl;
		cin >> flag;
		switch (flag)
		{
			case 1:RSA(); break;
			case 2:Sign(); break;
			case 3:PKey(); break;
			case 4:SKey(); break;
			case 5:exit(1); break;
			default:cout << "��������ȷѡ�����" << endl;
				Sleep(1000);break;
		}
	}
}

//main����
int main() {
	menu();	//��ʾ��ʼ�˵�
	return 0;
}