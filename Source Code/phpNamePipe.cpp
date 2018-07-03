// phpNamePipe.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	//��ʼ������
	char sPipeName[]="\\\\.\\pipe\\phpPipeDatenbank";
	// ���Ƹ�ʽ\\.\�ܵ�\�ܵ���\
	//windows��������linux�����µĹܵ�������ʽ��ͬ!
	//������ȫ������
	PSECURITY_DESCRIPTOR pSecurityDesc;
	pSecurityDesc=(PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	if(pSecurityDesc==NULL)return 1;
	ZeroMemory(pSecurityDesc,SECURITY_DESCRIPTOR_MIN_LENGTH);
	//��ʼ����ȫ������
	if(!InitializeSecurityDescriptor(pSecurityDesc,SECURITY_DESCRIPTOR_REVISION))
	{
		free(pSecurityDesc);
		return 2;
	}
	//����DACL(���ɷ��ʿ��Ʊ�)��Ϣ
	if(!SetSecurityDescriptorDacl(pSecurityDesc,TRUE,(PACL)NULL,FALSE))
	{
		free(pSecurityDesc);
		return 3;
	}
	//���ð�ȫ����������
	SECURITY_ATTRIBUTES SecurityAttr;
	SecurityAttr.nLength=sizeof(SECURITY_ATTRIBUTES);
	SecurityAttr.lpSecurityDescriptor=pSecurityDesc;
	SecurityAttr.bInheritHandle=TRUE;//��ȫ�����Ķ����Ƿ��ܱ��´����Ľ��̼̳�
	//���������ܵ�
	HANDLE hPipe=CreateNamedPipeA(sPipeName,
		PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,//˫��ܵ�|��������ܵ������첽����
		PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,
		1,0,0,1000,&SecurityAttr);
	if(hPipe==INVALID_HANDLE_VALUE)return 4;
	//��ȡ����
	char* PipeData;
	DWORD PipeDataLength;
	PipeData=(char*)malloc(128);
	while(1)
	{
		//���ӹܵ�(�ȴ�php������)
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
		//�ͻ��˵���fclose()��ܵ��Ͽ�����
		//��Ҫ���µ���ConnectNamePipe()�ȴ��¿ͻ�������
		DisconnectNamedPipe(hPipe);
	}
	//����û�м�����������...
	free(PipeData);
	CloseHandle(hPipe);
	return 0;
}

