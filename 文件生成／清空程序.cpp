#include<bits/stdc++.h>
using namespace std;
void create(string s){
	cout<<"正在生成/清空 \""<<s<<"\" ...";ofstream f(s);
	if(f.is_open())cout<<"   ->成功\n";else cout<<"   ->失败，请检查权限\n";
}
int main(){
	create("明文.txt");create("密文.txt");cout<<"程序运行完毕\n";system("pause");
	return 0;
}
