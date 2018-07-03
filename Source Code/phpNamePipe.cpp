// phpNamePipe.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	//初始化变量
	char sPipeName[]="\\\\.\\pipe\\phpPipeDatenbank";
	// 名称格式\\.\管道\管道名\
	//windows环境下与linux环境下的管道命名方式不同!
	//创建安全描述符
	PSECURITY_DESCRIPTOR pSecurityDesc;
	pSecurityDesc=(PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	if(pSecurityDesc==NULL)return 1;
	ZeroMemory(pSecurityDesc,SECURITY_DESCRIPTOR_MIN_LENGTH);
	//初始化安全描述符
	if(!InitializeSecurityDescriptor(pSecurityDesc,SECURITY_DESCRIPTOR_REVISION))
	{
		free(pSecurityDesc);
		return 2;
	}
	//设置DACL(自由访问控制表)信息
	if(!SetSecurityDescriptorDacl(pSecurityDesc,TRUE,(PACL)NULL,FALSE))
	{
		free(pSecurityDesc);
		return 3;
	}
	//设置安全描述符属性
	SECURITY_ATTRIBUTES SecurityAttr;
	SecurityAttr.nLength=sizeof(SECURITY_ATTRIBUTES);
	SecurityAttr.lpSecurityDescriptor=pSecurityDesc;
	SecurityAttr.bInheritHandle=TRUE;//安全描述的对象是否能被新创建的进程继承
	//创建命名管道
	HANDLE hPipe=CreateNamedPipeA(sPipeName,
		PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,//双向管道|允许这个管道进行异步操作
		PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,
		1,0,0,1000,&SecurityAttr);
	if(hPipe==INVALID_HANDLE_VALUE)return 4;
	//读取数据
	char* PipeData;
	DWORD PipeDataLength;
	PipeData=(char*)malloc(128);
	while(1)
	{
		//连接管道(等待php端连接)
		ConnectNamedPipe(hPipe,NULL);
		if(PeekNamedPipe(hPipe,NULL,0,NULL,&PipeDataLength,0))
		{
			ZeroMemory(PipeData,128);
			ReadFile(hPipe,PipeData,PipeDataLength<128?PipeDataLength:128,
				&PipeDataLength,0);
			printf("Message:[%s]\r\n",PipeData);
			if(strcmp(PipeData,"TestFrage")==0)
				WriteFile(hPipe,"TestAntwort",strlen("TestAntwort"),&PipeDataLength,0);
		}
		//客户端调用fclose()后管道断开连接
		//需要重新调用ConnectNamePipe()等待新客户端连接
		DisconnectNamedPipe(hPipe);
	}
	//这里没有加入跳出代码...
	free(PipeData);
	CloseHandle(hPipe);
	return 0;
}

