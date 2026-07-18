//-std=c++11 -Wl,--stack=67108864 -pthread -finput-charset=GBK -fexec-charset=GBK -Ofast -s -fstack-protector-all
//-static -lgdi32 -lcomctl32 -lgmpxx -lgmp -lcomdlg32 -lbcrypt
#include<bits/stdc++.h>
#include<io.h>
#include<gmpxx.h>
#include<windows.h>
#include<commctrl.h>
#include<shlobj.h>
using namespace std;
const int N=100,M=200;int cnt=max(4u,min(thread::hardware_concurrency(),16u)),totol,nex,tot;atomic<bool>running(true),isrun;
mpz_class n,p,q,e,d,prime[N];gmp_randstate_t state;ifstream fin;ofstream fout;double sstim,stim,tim;char a;atomic<int>now;bool automod,silent;
string input[256],in="明文.txt",out="密文.txt",s,mod,savepath;queue<string>output[256];mt19937 rnd;atomic<HWND>table;INITCOMMONCONTROLSEX icex;
const char c[95]="w7)OE$_AT%l;\'smgLu,2krNFieWVJ`p-#{ZQ.t0q}j5K8!\\G=XI*M?S4hbc9nf/]|vz@:6~B(<U^Co\"3H[+x>RdPaYyD1&";
const char c2[65]="3dIgAVkjcrH+8wxPSU5vTJBzNReYMtO6mlqQh92uKpW7FGoZnb1yEfL=sD4CX0ia";
inline string compress(mpz_class x){
	if(x==0)return {c[0]};string s;while(x)s+=c[mpz_class(x%94).get_si()],x/=94;reverse(s.begin(),s.end());return s;
}
inline string compress2(mpz_class x){
	if(x==0)return {c2[0]};string s;while(x)s+=c2[mpz_class(x&63).get_si()],x>>=6;reverse(s.begin(),s.end());return s;
}
inline mpz_class decompress(const string &s){mpz_class num=0;for(char a:s)num=(num<<6)+int(find(c2,c2+64,a)-c2);return num;}
inline string to_string(const mpz_class &x){ostringstream sout;sout<<x;return sout.str();}
inline void copy(HWND hWnd){
	HWND hList=GetDlgItem(hWnd,1006);if(!hList)return;
	int selectedIndex=ListView_GetNextItem(hList,-1,LVNI_SELECTED);
	if(selectedIndex==-1){MessageBox(hWnd,"请先选择一个素数","提示",MB_OK|MB_ICONINFORMATION);return;}
	if(selectedIndex<0||selectedIndex>=N){MessageBox(hWnd,"选择的素数无效","错误",MB_OK|MB_ICONERROR);return;}
	string clipboardText=to_string(prime[selectedIndex]);
	if(OpenClipboard(hWnd)){
		EmptyClipboard();HGLOBAL hMem=GlobalAlloc(GMEM_MOVEABLE,clipboardText.size()+1);
		if(hMem){char* pMem=(char*)GlobalLock(hMem);strcpy(pMem,clipboardText.c_str());
			GlobalUnlock(hMem);SetClipboardData(CF_TEXT,hMem);}CloseClipboard();
	}
	MessageBox(hWnd,"已复制到剪贴板","成功",MB_OK|MB_ICONINFORMATION);
}
LRESULT CALLBACK PrimeTableWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam){
	switch(msg){
		case WM_CREATE:{
			HWND hList=CreateWindowEx(0,WC_LISTVIEW,"",WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_REPORT|LVS_SINGLESEL|
				LVS_SHOWSELALWAYS|LVS_OWNERDATA,10,10,400,400,hWnd,(HMENU)1006,GetModuleHandleW(NULL),NULL);
			ListView_SetExtendedListViewStyle(hList,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
			char text1[]="序号",text2[]="随机出的素数（最多显示前 200 位）";
			LVCOLUMN lvc;lvc.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;
			lvc.iSubItem=0;lvc.cx=40;lvc.pszText=text1;ListView_InsertColumn(hList,0,&lvc);
			lvc.iSubItem=1;lvc.cx=360;lvc.pszText=text2;ListView_InsertColumn(hList,1,&lvc);
			ListView_SetItemCount(hList,N);HFONT hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
			if(hFont)SendMessage(hList,WM_SETFONT,(WPARAM)hFont,TRUE);break;
		}
		case WM_COMMAND:{if(LOWORD(wParam)==1008)copy(hWnd);break;}
		case WM_NOTIFY:{
			LPNMHDR nmhdr=(LPNMHDR)lParam;
			if(nmhdr->idFrom==1006&&nmhdr->code==LVN_GETDISPINFO){
				NMLVDISPINFO* plvdi=(NMLVDISPINFO*)lParam;long long itemIndex=plvdi->item.iItem;
				if(plvdi->item.mask&LVIF_TEXT)switch(plvdi->item.iSubItem){
					case 0:snprintf(plvdi->item.pszText,200,"%lld",itemIndex);break;
					case 1:snprintf(plvdi->item.pszText,200,"%s",to_string(prime[itemIndex]).c_str());break;
				}
			}
			break;
		}
		case WM_CONTEXTMENU:{HWND hList=GetDlgItem(hWnd,1006);if((HWND)wParam==hList){
				HMENU hMenu=CreatePopupMenu();char copyText[]="复制";AppendMenuA(hMenu,MF_STRING,1008,copyText);
				int selectedIndex=ListView_GetNextItem(hList,-1,LVNI_SELECTED);
				if(selectedIndex==-1)EnableMenuItem(hMenu,1008,MF_GRAYED);POINT pt={LOWORD(lParam),HIWORD(lParam)};
				if(pt.x==-1&&pt.y==-1){RECT rect;GetWindowRect(hList,&rect);pt.x=rect.left+10;pt.y=rect.top+10;}
				TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN,pt.x,pt.y,0,hWnd,NULL);DestroyMenu(hMenu);
			}break;
		}
		case WM_SIZE:{HWND hList=GetDlgItem(hWnd,1006);if(hList)ListView_SetColumnWidth(hList,1,LOWORD(lParam)-80),
			ListView_SetColumnWidth(hList,0,40),MoveWindow(hList,10,10,LOWORD(lParam)-20,HIWORD(lParam)-20,TRUE);break;
		}
		case WM_CLOSE:DestroyWindow(hWnd);break;
		case WM_DESTROY:table=NULL;isrun=false;PostQuitMessage(0);break;
		default:return DefWindowProcA(hWnd,msg,wParam,lParam);
	}
	return 0;
}
DWORD WINAPI PrimeTableThread(LPVOID lpParam){
	WNDCLASSA wc={};wc.lpfnWndProc=PrimeTableWndProc;wc.hInstance=GetModuleHandle(NULL);
	wc.lpszClassName="PrimeTableWindow";wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);wc.lpszMenuName=NULL;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);wc.cbClsExtra=0;wc.cbWndExtra=0;wc.style=CS_HREDRAW|CS_VREDRAW;
	if(!RegisterClassA(&wc)){DWORD err=GetLastError();if(err!=ERROR_CLASS_ALREADY_EXISTS){isrun=false;return 0;}}
	table=CreateWindowA("PrimeTableWindow","素数表查看器",WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_CLIPCHILDREN,
		CW_USEDEFAULT,CW_USEDEFAULT,440,460,NULL,NULL,GetModuleHandle(NULL),NULL);if(!table){isrun=false;return 0;}
	ShowWindow(table,SW_SHOW);SetForegroundWindow(table);UpdateWindow(table);MSG msg;
	while(GetMessage(&msg,NULL,0,0)){TranslateMessage(&msg);DispatchMessage(&msg);}isrun=false;return 0;
}
inline void Output(const wstring s){
	DWORD bytesWritten;WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),s.c_str(),s.size(),&bytesWritten,NULL);
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
inline void showPrimeTable(){
	if(isrun&&table){if(IsIconic(table))ShowWindow(table,SW_RESTORE);SetForegroundWindow(table);
		print<<L"素数表查看器正在运行，已为您提至前台\n您可以使用右键菜单复制选中的素数\n";return;}
	if(isrun){print<<L"素数表查看器正在启动，请耐心等待\n";return;}
	print<<L"正在启动素数表查看器...\n";isrun=true;HANDLE h;if(h=CreateThread(NULL,0,PrimeTableThread,NULL,0,0))
	print<<L"素数表查看器已启动\n您可以选择合适的素数用于密钥生成\n您还可以使用右键菜单复制选中的素数\n",CloseHandle(h);
	else print<<L"启动素数表查看器失败\n",isrun=false;
}
inline void setcolor(WORD x){SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),x);}
inline bool save(){
	if(savepath.empty()||savepath=="-auto"){
		time_t t=time(NULL);tm *tt=localtime(&t);char name[MAX_PATH];sprintf(name,"%04d-%02d-%02d_%02d-%02d-%02d_private_key.txt",
			1900+tt->tm_year,1+tt->tm_mon,tt->tm_mday,tt->tm_hour,tt->tm_min,tt->tm_sec);
		if(savepath!="-auto"){
			char dir[MAX_PATH];OPENFILENAME ofn={};SHGetFolderPath(NULL,CSIDL_PERSONAL,NULL,0,dir);ofn.lpstrInitialDir=dir;
			ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=GetConsoleWindow();ofn.lpstrFile=name;ofn.nMaxFile=sizeof(name);
			ofn.lpstrFilter="Text Files\0*.txt\0All Files\0*.*\0\0";ofn.nFilterIndex=1;ofn.lpstrDefExt="txt";
			ofn.Flags=OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT;if(!GetSaveFileName(&ofn)){DWORD err=CommDlgExtendedError();
				if(err)return print<<L"保存失败，错误码："<<err<<"\n",1;return print<<L"保存被取消\n",1;}
		}savepath=name;
	}
	ofstream fout(savepath);if(!fout.is_open())return print<<L"保存失败：打开文件失败\n",1;
	if(!(fout<<s))return print<<L"保存失败：写入文件失败\n",1;fout.close();return print<<L"保存成功\n",0;
}
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
[[noreturn]] inline void my_pause(){gmp_randclear(state);
	print<<L"请按回车键继续..."<<flush;long long a,b;if(!silent)do a=get_time(),getline(cin,s),b=get_time();while(b-a<1&&cin.good());exit(0);
}
template<typename T>inline void open(T &f,const string &s){
	f.open(s);if(!f.is_open())print<<L"无法打开 \""<<s<<L"\" ，请检查访问权限，确保文件存在\n"<<flush,my_pause();
}
template<class T>inline void create(T &&f){
	for(int i=0;i<cnt;i++){try{thread(f,i).detach();}catch(const exception &e){running=false,
		print<<L"创建第 "<<i+1<<L" 个线程失败\n请尝试重启此程序\n详细信息："<<e.what()<<"\n"<<flush,my_pause();}}
}
inline mpz_class fast_pow(const mpz_class &x,const mpz_class &y){
	mpz_class ans;mpz_powm(ans.get_mpz_t(),x.get_mpz_t(),y.get_mpz_t(),n.get_mpz_t());return ans;
}
inline string zh(const mpz_class &x){return compress(fast_pow(x,e));}
inline mpz_class gcd(const mpz_class &x,const mpz_class &y){mpz_class g;mpz_gcd(g.get_mpz_t(),x.get_mpz_t(),y.get_mpz_t());return g;}
inline tuple<mpz_class,mpz_class>exgcd(const mpz_class &x,const mpz_class &y){
	mpz_class g,a,b;mpz_gcdext(g.get_mpz_t(),a.get_mpz_t(),b.get_mpz_t(),x.get_mpz_t(),y.get_mpz_t());return {a,b};
}
inline mpz_class get_rand(){mpz_class x;mpz_urandomb(x.get_mpz_t(),state,2048);return x;}
inline void gete(){while(true)if(e=get_rand()%n,1<e&&e<n&&gcd(e,(p-1)*(q-1))==1)break;}
inline mpz_class getd(){mpz_class x,y,m=(p-1)*(q-1);tie(x,y)=exgcd(e,m);return (x%m+m)%m;}
inline void get(){
	int x,y;do{while((x=rnd()%N)<2);p=prime[x],y=rnd()%(x-1),q=prime[y];}while(abs(p-q)>>100==0);n=p*q;gete();d=getd();
}
inline void get2(){
	do{mpz_urandomb(p.get_mpz_t(),state,1024),mpz_nextprime(p.get_mpz_t(),p.get_mpz_t());
		mpz_urandomb(q.get_mpz_t(),state,1024),mpz_nextprime(q.get_mpz_t(),q.get_mpz_t());Sleep(0);
	}while(abs(p-q)>>100==0||p>>1000==0||q>>1000==0);n=p*q;gete();d=getd();
}
inline mpz_class read(const string &s){mpz_class x;istringstream sin(s);sin>>x;return x;}
inline int read2(const string &s){int x;istringstream sin(s);sin>>x;return x;}
inline void read3(const string&s,mpz_class f(const string&)){n=f(s.substr(1,s.find(',')-1));p=q=0;e=f(s.substr(s.find(',')+1,s.size()-s.find(',')-2));}
inline bool check(const string &s){return !s.empty()&&s.find_first_not_of("0123456789")==string::npos;}
inline bool check1(const string &s){mpz_class x;return check(s)&&(x=read(s),0<=x&&x<N);}
inline bool check2(const string &s){mpz_class x;return check(s)&&(x=read(s),x>>1000!=0&&mpz_probab_prime_p(x.get_mpz_t(),25));}
inline bool check3(const string &s){mpz_class x;return check(s)&&(x=read(s),1<x&&x<n&&gcd(x,(p-1)*(q-1))==1);}
inline bool check4(const string &s){
	mpz_class n,e;return s.front()=='('&&s.back()==')'&&s.find(',')!=string::npos&&s.find_first_of(',')==s.find_last_of(',')&&
		check(s.substr(1,s.find(',')-1))&&check(s.substr(s.find(',')+1,s.size()-s.find(',')-2))&&
		(n=read(s.substr(1,s.find(',')-1)))>>2000!=0&&(e=read(s.substr(s.find(',')+1,s.size()-s.find(',')-2)),1<e&&e<n);
}
inline bool check5(const string &s){
	mpz_class n,e;return s.front()=='('&&s.back()==')'&&s.find(',')!=string::npos&&s.find_first_of(',')==s.find_last_of(',')&&s.substr(1,s.find(',')-1)
		.find_first_not_of(c2)==string::npos&&s.substr(s.find(',')+1,s.size()-s.find(',')-2).find_first_not_of(c2)==string::npos&&
		(n=decompress(s.substr(1,s.find(',')-1)))>>2000!=0&&(e=decompress(s.substr(s.find(',')+1,s.size()-s.find(',')-2)),1<e&&e<n);
}
inline unsigned int rndd(){unsigned int x=321564596;BYTE b[4];
	if(!BCRYPT_SUCCESS(BCryptGenRandom(NULL,b,sizeof(b),BCRYPT_USE_SYSTEM_PREFERRED_RNG)))
	x^=random_device{}()^get_time();else for(BYTE c:b)x=x<<8|c^x>>24;return x;
}
inline void init_mt19937(mt19937 &rnd){
	unsigned int data[624];for(unsigned int &x:data)x=rndd();seed_seq seed(data,data+624);rnd.seed(seed);
}
inline void init_state(gmp_randstate_t &state){
	mpz_class seed;gmp_randinit_default(state);for(int i=0;i<32;i++)seed=seed<<32|rndd();gmp_randseed(state,seed.get_mpz_t());
}
void init(const int &id){
	gmp_randstate_t state;init_state(state);mpz_class x;
	for(int i=id;i<N;i+=cnt){do mpz_urandomb(x.get_mpz_t(),state,1024);while(x>>1000==0);
		mpz_nextprime(x.get_mpz_t(),x.get_mpz_t());prime[i]=x,++now,Sleep(0);if(!running)break;}
	gmp_randclear(state);
}
void work(const int &id){
	gmp_randstate_t state;init_state(state);mpz_class y;
	for(int i=0;i<input[id].size();i+=M){
		mpz_class x;mpz_urandomb(y.get_mpz_t(),state,1984-8*M);
		for(int j=i;j<i+M;j++)x=(x<<8)|(j<input[id].size()?(unsigned char)(input[id][j]):0);
		x|=(mpz_class(min(M,int(input[id].size())-i))<<8*M)|(mpz_class(id)<<8*M+8)|(y<<8*M+16);
		if(i+M>input[id].size())mpz_urandomb(y.get_mpz_t(),state,i+M-input[id].size()<<3),x|=y;
		output[id].push(zh(x)),++now,Sleep(0);if(!running)break;
	}
	string().swap(input[id]);gmp_randclear(state);
}
inline void print_help(){
	print<<L"选项：\n1: 手动输入 p 的 id\n2: 手动输入 q 的 id\n3: 手动输入 p\n4: 手动输入 q\n5: 手动输入 e\n6: 全部随机\n7: 随机 p\n\
8: 随机 q\n9: 随机 e\n10: 输入自定义公钥 (n,e)\n11: 输入本程序的公钥 (n,e)\n0: 查看素数表\nh: 查看此菜单\n注意：\n\
1. 请确保手动输入的 p,q 大于 2^1000\n2. 当手动输入 (n,e) 时，你应该有匹配的私钥 (n,d)，并确保输入的 n 大于 2^2000 且 (n,e) 合法\n"<<flush;
}
inline bool check_mod(const string &mod){int x;return mod=="h"||check(mod)&&mod.size()<3&&(x=read2(mod),0<=x&&x<=11);}
inline void clear(mpz_class &x){mpz_ptr z=x.get_mpz_t();if(z->_mp_d)SecureZeroMemory(z->_mp_d,z->_mp_alloc*sizeof(mp_limb_t));x=0;}
inline string tolower(string s){for(char &c:s)c=tolower(c);return s;}
inline void read_arg(const int argc,const char *const argv[]){
	for(int i=1,j=1;i<argc;j=++i){
		if(mod=argv[i],mod=="-h"||mod=="-help"){
			print<<LR"helpinfo(
    -h, -help              显示帮助
    -auto                  自动随机生成密钥
    -silent                不输出信息
    -threads <N>           指定线程数（ 1-256 ），若不指定或无效则使用默认值
    -in   <filepath>       指定明文路径
    -out  <filepath>       指定密文路径
    -save -no              不保存私钥（如果有）到文件，而是输出到控制台
    -save -null            不保存私钥（如果有）到文件，也不输出到控制台
    -save -auto            自动保存私钥（如果有）到文件，只在失败时输出到控制台
    -save <filepath>       指定私钥（如果有）保存路径，如果失败则输出到控制台
    -key                   指定 (n,e) 的十进制
    -p                     指定 p 的十进制
    -q                     指定 q 的十进制
    -n                     指定 n 的十进制
    -e                     指定 e 的十进制
    -KEY                   指定 (n,e) 的压缩串
    -P                     指定 p 的压缩串
    -Q                     指定 q 的压缩串
    -N                     指定 n 的压缩串
    -E                     指定 e 的压缩串
        (-s) <string>      指定 key, p, q, n, e 的读取字符串，必须紧跟于相应参数后
        -f <filepath>      指定 key, p, q, n, e 的读取文件，必须紧跟于相应参数后
    若有 -h, -help ，则忽略其它参数
    若有 -auto ，则忽略 key, n, p, q, e
    有效 key 会被解析为 n, e ，并按照相应方式处理
    若有指定有效 n ，则忽略 p, q
    若没有指定 -in, -out, -save ，则默认
    若出现重复项，以最后生效的为准
    本程序不会检查 <filepath>, <N>, <string> 是否为参数，而是将其当做值，只会在无法使用时报错
    有效的 n 需满足：大于 2^2000
    有效的 p, q 需满足：大于 2^1000 且为质数
    本程序不会检查 n, e 的有效性
    你可以传入 nul 作为 <filepath> 以拒绝密文输出以测试性能等
)helpinfo"<<flush,exit(0);
		}else if(mod=="-auto")automod=1;else if(mod=="-silent")silent=1;
		else if(mod=="-threads"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数：-threads 后期望 <N> ，但没有找到\n"<<flush;continue;}
			if(!check(mod=argv[i])||mod.size()>3||(j=read2(mod),j==0||j>256))
				{print<<L"警告：位于第 "<<i-1<<" 个参数：-threads 后 <N> 无效\n"<<flush;continue;}cnt=j;
		}else if(mod=="-in"||mod=="-out"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数："<<mod<<" 后期望 <filepath> ，但没有找到\n"<<flush;continue;}
			if(mod=="-in")in=argv[i];else out=argv[i];
		}else if(mod=="-save"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数：-save 后期望 -no, -null, -auto, <filepath> ，但没有找到\n"<<flush;continue;}
			savepath=argv[i];
		}else if(mod=="-key"||mod=="-KEY"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数："<<mod<<" 后期望 (-s) <string>, -f <filepath> ，但没有找到\n"<<flush;continue;}
			if(string(argv[i])=="-f"){
				if(++i==argc){print<<L"警告：位于第 "<<i-1<<" 个参数：-f 后期望 <filepath> ，但没有找到\n"<<flush;continue;}ifstream fin(argv[i]);
				if(!fin.is_open()){print<<L"警告：位于第 "<<i<<" 个参数：打开 -f 指定的文件 "<<argv[i]<<" 失败\n"<<flush;continue;}
				if(!(fin>>s)){print<<L"警告：读取文件 "<<argv[i]<<" 失败\n"<<flush;continue;};fin.close();
			}else{if(string(argv[i])=="-s"&&++i==argc){
				print<<L"警告：位于第 "<<i-1<<" 个参数：-s 后期望 <string> ，但没有找到\n"<<flush;continue;}s=argv[i];}
			if(mod=="-key"&&!check4(s)||mod=="-KEY"&&!check5(s)){print<<L"警告：位于第 "<<j<<" 个参数："<<mod<<" 指定的值无效\n"<<flush;continue;}
			if(mod=="-key")read3(s,read);else read3(s,decompress);
		}else if(s=tolower(mod),s=="-p"||s=="-q"||s=="-n"||s=="-e"){
			if(++i==argc){print<<L"警告：位于第 "<<j<<" 个参数："<<mod<<" 后期望 (-s) <string>, -f <filepath> ，但没有找到\n"<<flush;continue;}
			if(string(argv[i])=="-f"){
				if(++i==argc){print<<L"警告：位于第 "<<i-1<<" 个参数：-f 后期望 <filepath> ，但没有找到\n"<<flush;continue;}ifstream fin(argv[i]);
				if(!fin.is_open()){print<<L"警告：位于第 "<<i<<" 个参数：打开 -f 指定的文件 "<<argv[i]<<" 失败\n"<<flush;continue;}
				if(!(fin>>s)){print<<L"警告：读取文件 "<<argv[i]<<" 失败\n"<<flush;continue;}fin.close();
			}else{if(string(argv[i])=="-s"&&++i==argc){
				print<<L"警告：位于第 "<<i-1<<" 个参数：-s 后期望 <string> ，但没有找到\n"<<flush;continue;}s=argv[i];}
			if(islower(mod[1])){if(check(s))d=read(s);else{print<<L"警告：位于第 "<<j<<" 个参数："<<mod<<" 指定的值无效\n"<<flush;continue;}}
			else d=decompress(s);if(s=tolower(mod[1]),(s=="p"||s=="q")&&(d>>1000==0||!mpz_probab_prime_p(d.get_mpz_t(),25))||
			s=="n"&&d>>2000==0){print<<L"警告：位于第 "<<j<<" 个参数："<<mod<<" 指定的值无效\n"<<flush;continue;}
			if(s=="p")p=d;else if(s=="q")q=d;else if(s=="n")n=d;else e=d;
		}else print<<L"警告：位于第 "<<i<<" 个参数：未知的 "<<mod<<"\n"<<flush;
	}
	if(n)p=q=0;else if(p&&q)n=p*q;else if(p)p=e=0,print<<L"警告：指定了 p ，但没有指定 q\n"<<flush;
	else if(q)q=e=0,print<<L"警告：指定了 q ，但没有指定 p\n"<<flush;else if(e)e=0,print<<L"警告：指定了 e ，但没有指定 n 或 p, q\n"<<flush;
}
int main(int argc,char *argv[]){
	read_arg(argc,argv);print.err=0;setcolor(0x07);stim=get_time()/1000.0;init_state(state);init_mt19937(rnd);
	if(automod)print<<L"自动模式模式初始化中...\n"<<flush,get2();
	else if(n){if(p&&!e){while(true)if(e=get_rand()%n,1<e&&e<n&&gcd(e,(p-1)*(q-1))==1)break;d=getd();}}
	else if(!_isatty(_fileno(stdin)))print.err=1,print<<L"警告：检测到当前非交互式终端，交互模式不可用，已切换到自动模式\n"<<flush,print.err=0,get2();
	else{
		print<<L"初始化中（目标线程数："<<cnt<<L"）...\n"<<flush;icex.dwICC=ICC_LISTVIEW_CLASSES;
		icex.dwSize=sizeof(INITCOMMONCONTROLSEX);InitCommonControlsEx(&icex);create(init);
		while(1){
			const int copy=now;if(copy==N)break;if(copy>=nex)nex=min(N,max(copy,nex)+int(rnd()%100)),tim=get_time()/1000.0,
			print<<to_process(copy,N)<<" | "<<to_time()<<"\r"<<flush;sleep(10);
		}
		sort(prime,prime+N);tim=get_time()/1000.0;now=nex=0;print<<to_process(N,N)<<" | "<<to_time()
			<<L"\n初始化完毕，请选择密钥生成模式（1. 自动随机生成；2. 手动输入或手动随机）："<<flush;
		getline(cin,mod),cin.clear();while(mod!="1"&&mod!="2")print<<L"输入无效，请重新输入："<<flush,getline(cin,mod),cin.clear();
		if(mod=="1")get(),print<<L"使用自动生成的随机";
		if(mod=="2"){
			print_help();print<<L"请选择："<<flush;getline(cin,mod);while(!check_mod(mod))print<<L"输入无效，请重新输入："<<flush,getline(cin,mod);
			while(mod!="-1"){
				#define Input(h,ch,op...) {print<<L"请输入" h L": "<<flush,getline(cin,s);if(ch)op else print<<L"输入无效\n";}
				if(mod=="1")Input(L" p 的 id",check1(s),{p=prime[read2(s)],n=e=0;if(abs(p-q)>>100==0)q=0;})
				if(mod=="2")Input(L" q 的 id",check1(s),{q=prime[read2(s)],n=e=0;if(abs(p-q)>>100==0)p=0;})
				if(mod=="3")Input(L" p",check2(s),{p=read(s),n=e=0;if(abs(p-q)>>100==0)q=0;})    if(mod=="6")get();
				if(mod=="4")Input(L" q",check2(s),{q=read(s),n=e=0;if(abs(p-q)>>100==0)p=0;})    if(mod=="h")print_help();
				if(mod=="5")if(p!=0&&q!=0)Input(L" e",check3(s),e=read(s);)else print<<L"请先填写 p 与 q 再选择此步\n";
				if(mod=="7"){e=0;do p=prime[rnd()%N];while(abs(p-q)>>100==0);}    if(mod=="8"){e=0;do q=prime[rnd()%N];while(abs(p-q)>>100==0);}
				if(mod=="9")if(p!=0&&q!=0)gete();else print<<L"请先填写 p 与 q 再选择此步\n";    if(mod=="0")showPrimeTable();
				if(mod=="10")Input(L"自定义 (n,e)",check4(s),read3(s,read);)    if(mod=="11")Input(L"本程序的 (n,e)",check4(s),read3(s,decompress);)
				if(p&&q)n=p*q;bool ok=n&&e;print<<L"当前数据：\n{";if(p)print<<"\n\tp="<<compress2(p);if(q)print<<(p?",":"")<<"\n\tq="<<compress2(q);
				if(!p&&n)print<<"\n\tn="<<compress2(n);if(ok)print<<",\n\te="<<compress2(e);if(ok&&p)print<<",\n\td="<<compress2(d=getd());
				print<<L"\n}\n请选择下一步";if(ok)print<<L"（当前可键入 -1 以确定并退出）";print<<L"："<<flush;getline(cin,mod),cin.clear();
				while((mod!="-1"||!ok)&&!check_mod(mod))print<<L"输入无效，请重新输入："<<flush,getline(cin,mod),cin.clear();
			}
			if(table)SendMessage(table,WM_CLOSE,0,0);
		}
	}
	sstim=stim,print<<L"密钥：\n{\n\tn="<<compress2(n)<<",\n\te="<<compress2(e)<<(p?",\n\td="+compress2(d):"")<<"\n}\n";
	if(p)s="("+compress2(p*q)+","+compress2(d)+")";else s.clear();clear(p);clear(q);clear(d);for(int i=0;i<N;++i)clear(prime[i]);
	print<<L"统计中...\n"<<flush;open(fin,in);open(fout,out);stim=get_time()/1000.0;
	while(fin.get(a)){input[totol%cnt].push_back(a);
		if(++totol>=nex)nex+=rnd()%100000,tim=get_time()/1000.0,print<<L"进度: 已找到 "<<totol<<L" 字符 | "<<to_time()<<"\r"<<flush;
	}
	tim=get_time()/1000.0;fin.close();nex=0;print<<L"进度: 已找到 "<<totol<<L" 字符 | "<<to_time()
		<<L"\n统计完毕，加密中（目标线程数："<<cnt<<L"）...\n"<<flush;stim=get_time()/1000.0;
	tot=totol%cnt*(totol/cnt%M==0)+(totol/cnt+M-1)/M*cnt;create(work);
	while(1){
		const int copy=now;if(copy==tot)break;if(copy>=nex)nex=min(totol,max(copy,nex)+int(rnd()%1000)),
		tim=get_time()/1000.0,print<<to_process(copy,tot)<<" | "<<to_time()<<"\r"<<flush;sleep(10);
	}
	tim=get_time()/1000.0,print<<to_process(tot,tot)<<" | "<<to_time()<<L"\n加密完毕，写入文件中...\n"<<flush;nex=0;stim=get_time()/1000.0;
	for(int i=0;i<tot;i++){
		fout<<output[i%cnt].front()<<" ",output[i%cnt].pop();if(i>=nex)nex=min(totol,nex+int(rnd()%10000)),
		tim=get_time()/1000.0,print<<to_process(i,tot)<<" | "<<to_time()<<"\r"<<flush;
	}
	fout.close();tim=get_time()/1000.0,print<<to_process(tot,tot)<<" | "<<to_time()<<L"\n写入完毕，"<<flush;
	if(!s.empty()){
		if(savepath.empty()&&!_isatty(_fileno(stdin)))print<<L"警告：检测到当前非交互式终端，已切换为自动保存\n"<<flush,savepath="-auto";
		if(savepath=="-null")a=0;else if(savepath=="-no")a=1;else if(!savepath.empty())print<<L"自动保存私钥中..."<<flush,a=save();
		else{print<<L"是否保存私钥到文件？(Yes/No)（只检测第一个字符）："<<flush;getline(cin,mod),cin.clear();
			while(mod.empty()||(mod=toupper(mod[0]),mod!="Y"&&mod!="N"))print<<L"输入无效，请重新输入："<<flush,getline(cin,mod),cin.clear();
			if(mod=="Y")a=save();else a=1;
		}
		if(a)print.err=1,print<<L"私钥为：\n"<<flush,setcolor(0x04),
			print<<s<<flush,setcolor(0x07),print<<L"\n请妥善保存\n",print.err=0;
	}print<<L"现在可以关闭此程序。\n"<<flush;my_pause();
	return 0;
}
