#include "main.h"
char* pqRand(int len) {	//éú3é???¨??êyμ?char*Dí′óêy
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

bign getPandQ(int len) {	//×a??3ébingààDí′óêyoóμ?poíq
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
//?t·?·¨?ì?ù?Yè??￡(μY1é)
bign quickmod(bign a, bign n, bign c)
{
	bign t(0);
	if (n == 0) return bign(1) % c;
	if (n == 1) return a % c;
	t=quickmod(a, n / 2, c);//μY1éμ÷ó?￡?2éó??t·?μY1é??·¨￡?×￠òa?aà?n/2?á′?à′????D??êìa  
	t = (t * t) % c;//?t·?￡?3?é?áíò?°??ù?ó?￡  
	if (n % 2 == 1)//nê???êy￡?òò?an/2?1éù3?á?ò?′?a  
		t = t * a %c;
	return t;
}


//?×à??T±??D±e·¨
int Miller_Rabbin(bign  n)
{
	int row[] = {2,3,5,7,11,13,17,19};		//(1/5)^8′?·?μ????ê?ó2?￡?×?è·?êoü??
	bign nn(n - 1);
	bign s(0);
	bign d(0);
	bign t(0);
	while (nn%bign(2) == bign(0)) {
		s++;
		nn /= bign(2);
	}
	d = nn;
	//cout <<"s:"<< s << endl;
	//cout <<"d:"<< d << endl;
	//ò?é??ó3?soíd￡????D2^s*d == n-1
	//ò???￡?ê1ó????ú????êya￡???DD?D??
	for (int i = 0; i < sizeof(row)/4;i++) {
		t = quickmod(row[i], d, n);
		//cout <<"t:"<< t << endl;
		if (t != 1) {
			for (bign r(0); r <= (s - bign(1)); r++) {
				bign tt(quickmod(row[i], bign(bign(2).pow(r))*d, n));
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

int main(){  
while(1){
    //创建套接字  
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
  
    //将套接字和IP、端口绑定  
    struct sockaddr_in serv_addr;  
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充  
    serv_addr.sin_family = AF_INET;  //使用IPv4地址  
    serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");  //具体的IP地址  
    serv_addr.sin_port = htons(40086);  //端口  
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));  
  
    //进入监听状态，等待用户发起请求  
    listen(serv_sock, 20);  
  
    //接收客户端请求  
    struct sockaddr_in clnt_addr;  
    socklen_t clnt_addr_size = sizeof(clnt_addr);  
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);  
    char rev[2];
    read(clnt_sock, rev, sizeof(rev));	//获得生成的大素数长度
    int len;
    len = int(rev[0]-48)*10+int(rev[1]-48);
    printf("%d\n", len);
	//调用求大素数  
    bign q(0);
    srand((unsigned)time(NULL));	//随机数种子
    q = getPandQ(len);	//获得随机大奇数q
	while (Miller_Rabbin(q) != 1) {	//如果q不是素数
		// p = getPandQ(len);	//重新获得随机奇数p
		q += 2;	//！！！！2017.6.4晚更新优化！！！   直接利用质数的分布，不再重新获得一次大奇数。
	}

    //向客户端发送数据  
    string qs = q.toStr();
    // char* str = ps.data();  
    write(clnt_sock, qs.data(), 78);  
    printf("%s\n", qs.data());
    //关闭套接字  
    close(clnt_sock);  
    close(serv_sock);  
}
    return 0;  
}  
