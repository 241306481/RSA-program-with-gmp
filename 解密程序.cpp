//-std=c++11 -Wl,--stack=67108864 -pthread -finput-charset=GBK -fexec-charset=GBK -Ofast -s -fstack-protector-all
//-static -lgmpxx -lgmp -lbcrypt
#include<bits/stdc++.h>
#include<io.h>
#include<gmpxx.h>
#include<windows.h>
using namespace std;
const int M=200;int cnt=max(4u,min(thread::hardware_concurrency(),16u)),totol,nex,cntt;atomic<int>now,tot;
mt19937 rnd;mpz_class n,d;queue<string>input[256];string a,in="密文.txt",out="明文.txt",output2[256];bool silent;
queue<pair<short,string>>output[256];ifstream fin;ofstream fout;atomic<bool>running(true);double sstim,stim,tim;
inline mpz_class fast_pow(const mpz_class &x,const mpz_class &y){mpz_class ans;
	mpz_powm(ans.get_mpz_t(),x.get_mpz_t(),y.get_mpz_t(),n.get_mpz_t());return ans;}
const char c[95]="w7)OE$_AT%l;\'smgLu,2krNFieWVJ`p-#{ZQ.t0q}j5K8!\\G=XI*M?S4hbc9nf/]|vz@:6~B(<U^Co\"3H[+x>RdPaYyD1&";
const char c2[65]="3dIgAVkjcrH+8wxPSU5vTJBzNReYMtO6mlqQh92uKpW7FGoZnb1yEfL=sD4CX0ia";
inline mpz_class decompress(const string &s){mpz_class num=0;for(char a:s)num=num*94+mpz_class(int(find(c,c+95,a)-c));return num;}
inline mpz_class decompress2(const string &s){mpz_class num=0;for(char a:s)num=(num<<6)+mpz_class(int(find(c2,c2+64,a)-c2));return num;}
inline void Output(const wstring s){
	DWORD bytesWritten;HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleW(hConsole,s.c_str(),s.size(),&bytesWritten,NULL);
}
inline wstring to_wstring(const string &s){
	int len=MultiByteToWideChar(CP_ACP,0,s.c_str(),s.size(),nullptr,0);if(len<=0)return L"";
	wstring ws(len,L'\0');MultiByteToWideChar(CP_ACP,0,s.c_str(),s.size(),&ws[0],len);return ws;
}
struct Print{
	Print(int x=0):err(x){}
	~Print(){flush();}
	bool err;wostringstream wsout;
	template<class T>Print& operator<<(const T &x){wsout<<x;return *this;}
	Print& operator<<(const string &s){wsout<<to_wstring(s);return *this;}
	Print& operator<<(const char *s){wsout<<to_wstring(s);return *this;}
	void tie(int x){}
	Print& operator<<(Print& (*manip)(Print&)){return manip(*this);}
	Print& flush(){if(err||!silent)Output(wsout.str());wsout.clear();wsout.str(L"");return *this;}
}print(1);
Print& flush(Print &a){return a.flush();}
inline void sleep(const int &x){this_thread::sleep_for(chrono::milliseconds(x));}
inline long long get_time(){using namespace chrono;return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();}
inline wstring to_process(const long long &a,const long long &b){
	wostringstream wsout;wsout<<fixed<<setprecision(3)<<L"进度: "<<(b==0?100:a*100.0/b)<<"% ( "<<a<<" / "<<b<<" )";return wsout.str();
}
inline wstring to_time(){wostringstream wsout;
	wsout<<fixed<<setprecision(3)<<L"耗时: "<<tim-stim<<"s ( "<<int((tim-stim)/3600)<<"h "<<int((tim-stim)/60)%60<<"m "<<fmod((tim-stim),60)<<L"s )";
	if(sstim)wsout<<L" | 总耗时："<<tim-sstim<<"s ( "<<int((tim-sstim)/3600)<<"h "<<int((tim-sstim)/60)%60<<"m "<<fmod(tim-sstim,60)<<"s )";
	return wsout.str();
}
[[noreturn]] inline void my_pause(){
	print<<L"请按回车键继续..."<<flush;long long a,b;if(!silent)do a=get_time(),getline(cin,::a),b=get_time();while(b-a<1);exit(0);
}
template<typename T>inline void open(T &f,const string &s){
	f.open(s);if(!f.is_open())print<<L"无法打开 \""<<s<<L"\" ，请检查访问权限，确保文件存在\n",my_pause();
}
template<class T>inline void create(T &&f){
	for(int i=0;i<cnt;i++){try{thread(f,i).detach();}catch(const exception &e){running=false,
		print<<L"创建第 "<<i+1<<L" 个线程失败\n请尝试重启此程序\n详细信息："<<e.what()<<"\n"<<flush,my_pause();}}
}
inline mpz_class zh(const string &x){return fast_pow(decompress(x),d);}
inline mpz_class read(const string &s){mpz_class x;istringstream sin(s);sin>>x;return x;}
inline int read2(const string &s){int x;istringstream sin(s);sin>>x;return x;}
inline void read3(const string&s,mpz_class f(const string&)){n=f(s.substr(1,s.find(',')-1));d=f(s.substr(s.find(',')+1,s.size()-s.find(',')-2));}
inline bool check(const string &s){return !s.empty()&&s.front()!='0'&&s.find_first_not_of("0123456789")==string::npos;}
inline bool check1(const string &s){
	mpz_class n,d;return s.front()=='('&&s.back()==')'&&s.find(',')!=string::npos&&s.find_first_of(',')==s.find_last_of(',')&&
		check(s.substr(1,s.find(',')-1))&&check(s.substr(s.find(',')+1,s.size()-s.find(',')-2))&&(n=read(s.substr(1,s.find(',')-1)))>
		mpz_class(1)<<2000&&(d=read(s.substr(s.find(',')+1,s.size()-s.find(',')-2)),1<d&&d<n);
}
inline bool check2(const string &s){
	mpz_class n,d;return s.front()=='('&&s.back()==')'&&s.find(',')!=string::npos&&s.find_first_of(',')==s.find_last_of(',')&&s.substr(1,s.find(',')-1)
		.find_first_not_of(c2)==string::npos&&s.substr(s.find(',')+1,s.size()-s.find(',')-2).find_first_not_of(c2)==string::npos&&
		(n=decompress2(s.substr(1,s.find(',')-1)))>mpz_class(1)<<2000&&(d=decompress2(s.substr(s.find(',')+1,s.size()-s.find(',')-2)),1<d&&d<n);
}
inline unsigned int rndd(){unsigned int x=321564596;BYTE b[4];
	if(!BCRYPT_SUCCESS(BCryptGenRandom(NULL,b,sizeof(b),BCRYPT_USE_SYSTEM_PREFERRED_RNG)))
	x^=random_device{}()^get_time();else for(BYTE c:b)x=x<<8|c^x>>24;return x;
}
inline void init_mt19937(mt19937 &rnd){
	unsigned int data[624];for(unsigned int &x:data)x=rndd();seed_seq seed(data,data+624);rnd.seed(seed);
}
void work(int id){
	while(!input[id].empty()){
		mpz_class x=zh(input[id].front());int size=mpz_class(x>>8*M&255).get_si();
		input[id].pop();output[id].push(make_pair(mpz_class(x>>8*M+8&255).get_si(),string()));
		for(int i=M*8-8;i>=max(0,M*8-size*8);i-=8)output[id].back().second.push_back(mpz_class(x>>i&255).get_si());
		++now,tot+=size;if(!running)break;
	}
}
inline string tolower(string s){for(char &c:s)c=tolower(c);return s;}
inline void read_arg(const int argc,const char *const argv[]){
	for(int i=1,j=1;i<argc;j=++i){
		if(a=argv[i],a=="-h"||a=="-help"){
			print<<LR"helpinfo(    -h, -help              显示帮助
    -silent                不输出信息
    -threads <N>           指定线程数（ 1-256 ），若不指定或无效则使用默认值
    -in   <filepath>       指定密文路径
    -out  <filepath>       指定明文路径
    -key                   指定 (n,d) 的十进制
    -n                     指定 n 的十进制
    -d                     指定 d 的十进制
    -KEY                   指定 (n,d) 的压缩串
    -N                     指定 n 的压缩串
    -D                     指定 d 的压缩串
        (-s) <string>      指定 key, n, d 的读取字符串，必须紧跟于相应参数后
        -f <filepath>      指定 key, n, d 的读取文件，必须紧跟于相应参数后
    若有 -h, -help ，则忽略其它参数
    有效 key 会被解析为 n, d ，并按照相应方式处理
    若没有指定 -in, -out ，则默认
    若出现重复项，以最后生效的为准
    本程序不会检查 <filepath>, <N>, <string> 是否为参数，而是将其当做值，只会在无法使用时报错
    有效的 n 需满足：大于 2^2000
    本程序不会检查 d 的有效性
    你可以传入 nul 作为 <filepath> 以拒绝明文输出以测试性能等
)helpinfo"<<flush,exit(0);
		}else if(a=="-silent")silent=1;
		else if(a=="-threads"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数：-threads 后期望 <N> ，但没有找到\n"<<flush;continue;}
			if(!check(a=argv[i])||a.size()>3||(j=read2(a),j==0||j>256))
				{print<<L"警告：位于第 "<<i-1<<" 个参数：-threads 后 <N> 无效\n"<<flush;continue;}cnt=j;
		}else if(a=="-in"||a=="-out"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数："<<a<<" 后期望 <filepath> ，但没有找到\n"<<flush;continue;}
			if(a=="-in")in=argv[i];else out=argv[i];
		}else if(a=="-key"||a=="-KEY"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数："<<a<<" 后期望 (-s) <string>, -f <filepath> ，但没有找到\n"<<flush;continue;}
			if(string(argv[i])=="-f"){
				if(++i==argc){print<<L"警告：位于第 "<<i-1<<" 个参数：-f 后期望 <filepath> ，但没有找到\n"<<flush;continue;}ifstream fin(argv[i]);
				if(!fin.is_open()){print<<L"警告：位于第 "<<i<<" 个参数：打开 -f 指定的文件 "<<argv[i]<<" 失败\n"<<flush;continue;}
				if(!(fin>>a)){print<<L"警告：读取文件 "<<argv[i]<<" 失败\n"<<flush;continue;};fin.close();
			}else{if(string(argv[i])=="-s"&&++i==argc){
				print<<L"警告：位于第 "<<i-1<<" 个参数：-s 后期望 <string> ，但没有找到\n"<<flush;continue;}a=argv[i];}
			if(string(argv[j])=="-key"&&!check1(a)||string(argv[j])=="-KEY"&&!check2(a)){
				print<<L"警告：位于第 "<<j<<" 个参数："<<argv[j]<<" 指定的值无效\n"<<flush;continue;}
			if(string(argv[j])=="-key")read3(a,read);else read3(a,decompress2);
		}else if(a=tolower(a),a=="-n"||a=="-d"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数："<<a<<" 后期望 (-s) <string>, -f <filepath> ，但没有找到\n"<<flush;continue;}
			if(string(argv[i])=="-f"){
				if(++i==argc){print<<L"警告：位于第 "<<i-1<<" 个参数：-f 后期望 <filepath> ，但没有找到\n"<<flush;continue;}ifstream fin(argv[i]);
				if(!fin.is_open()){print<<L"警告：位于第 "<<i<<" 个参数：打开 -f 指定的文件 "<<argv[i]<<" 失败\n"<<flush;continue;}
				if(!(fin>>a)){print<<L"警告：读取文件 "<<argv[i]<<" 失败\n"<<flush;continue;};fin.close();
			}else{if(string(argv[i])=="-s"&&++i==argc){
				print<<L"警告：位于第 "<<i-1<<" 个参数：-s 后期望 <string> ，但没有找到\n"<<flush;continue;}a=argv[i];}
			static mpz_class x;if(islower(argv[j][1]))x=read(a);else x=decompress2(a);if(tolower(argv[j][1])=='n'){
			if(x<=mpz_class(1)<<2000){print<<L"警告：位于第 "<<j<<L" 个参数："<<argv[j]<<" 指定的值无效\n"<<flush;continue;}n=x;}else d=x;
		}else print<<L"警告：位于第 "<<i<<" 个参数：未知的 "<<a<<"\n"<<flush;
	}
	if(n&&!d)n=0,print<<L"警告：指定了 n ，但没有指定 d\n"<<flush;if(!n&&d)d=0,print<<L"警告：指定了 d ，但没有指定 n\n"<<flush;
}
int main(int argc,char *argv[]){
	read_arg(argc,argv);print.err=0;sstim=get_time()/1000.0;init_mt19937(rnd);
	if(!n){
		if(!_isatty(_fileno(stdin)))print.err=1,print<<L"错误：检测到当前非交互式终端，交互模式不可用，"
			L"请使用命令行参数或使用交互式终端启动本程序\n"<<flush,exit(0);
		print<<L"请选择输入的私钥类别：\n1. 输入自定义私钥 (n,d)\n2: 输入加密程序提供的私钥 (n,d)\n"<<flush;
		getline(cin,a),cin.clear();while(a!="1"&&a!="2")print<<L"输入无效，请重新输入："<<flush,getline(cin,a),cin.clear();
		if(a=="1"){
			print<<L"请输入自定义私钥 (n,d)："<<flush,getline(cin,a),cin.clear();
			while(!check1(a))print<<L"输入无效，请重新输入："<<flush,getline(cin,a),cin.clear();
			n=read(a.substr(1,a.find(',')-1)),d=read(a.substr(a.find(',')+1,a.size()-a.find(',')-2));
		}else{
			print<<L"请输入加密程序提供的私钥 (n,d)："<<flush,getline(cin,a),cin.clear();
			while(!check2(a))print<<L"输入无效，请重新输入："<<flush,getline(cin,a),cin.clear();
			n=decompress2(a.substr(1,a.find(',')-1)),d=decompress2(a.substr(a.find(',')+1,a.size()-a.find(',')-2));
		}
	}
	print<<L"统计中...\n";open(fin,in);open(fout,out);stim=get_time()/1000.0;
	while(fin>>a){input[totol%cnt].push(a);
		if(++totol>=nex)nex+=rnd()%100000,tim=get_time()/1000.0,print<<L"进度: 已找到 "<<totol<<L" 块 | "<<to_time()<<"\r"<<flush;
	}
	tim=get_time()/1000.0;fin.close();nex=0;print<<L"进度: 已找到 "<<totol<<L" 块 | "<<to_time()
		<<L"\n统计完毕，解密中（目标线程数："<<cnt<<L"）...\n"<<flush;stim=get_time()/1000.0;create(work);
	while(1){
		const int copy=now;if(copy==totol)break;if(copy>=nex)nex=min(totol,max(copy,nex)+int(rnd()%1000)),
		tim=get_time()/1000.0,print<<to_process(copy,totol)<<" | "<<to_time()<<"\r"<<flush;sleep(10);
	}
	tim=get_time()/1000.0,print<<to_process(totol,totol)<<" | "<<to_time()<<L"\n解密完毕，写入文件中...\n"<<flush;nex=0;stim=get_time()/1000.0;
	for(int i=0;i<totol;i++){const auto p=output[i%cnt].front();output[i%cnt].pop();output2[p.first]+=p.second,cntt=max(cntt,p.first+1);}
	for(int i=0;i<tot;i++){if(i/cntt>=output2[i%cntt].size())fout<<char(rnd()&255);else fout<<output2[i%cntt][i/cntt];
		if(i>=nex)nex=min(int(tot),nex+int(rnd()%1000000)),tim=get_time()/1000.0,print<<to_process(i,tot)<<" | "<<to_time()<<"\r"<<flush;
	}
	for(int i=0;i<cntt;i++)string().swap(output2[i]);fout.close();tim=get_time()/1000.0;
	print<<to_process(tot,tot)<<" | "<<to_time()<<L"\n写入完毕！可以关闭此程序\n",my_pause();
	return 0;
}
