// SLZ.cpp : 定义控制台应用程序的入口点。
//

/*	
	SLZ for StarOcean First Departure(PSP)

	Version:	v0.99	2008.1.26
	Author:		kid
*/

#include "stdafx.h"
#include "SLZ.h"
#include <io.h>
#include "File.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

#define N        4096   /* size of ring buffer */
#define F          18   /* upper limit for match_length */

char ext1[20];
char ext2[20];

struct _finddata_t c_file; 
long hFile;

string Magic("SLZ");
bool	dumptxt = false;

#define CYC_HIGH_NUM 10;
#define MATCH_HIGH_NUM 20;

using std::size_t;


//template<typename T, typename U> const U FindSame(const T* begin, const T* end, U u = 1)
//{
//	//U u = 1;
//	while (begin + 1 < end && u <  CYC_HIGH_NUM && *begin == *(begin + 1))
//	{
//		u++;begin++;
//	}
//	return u;
//}


//int match(const unsigned char *src,const unsigned char *const beg,const unsigned char *const end,unsigned int &pos)		//查找匹配输出
//{
//	if (src+1>=end) return 0;
//
//	int len=0,max=0;
//	const unsigned char *p=src-1;
//
//	for (;p>=beg && p>src-N;p--) 
//	{
//		if (*p==*src) 
//		{
//			len=1;
//			const unsigned char *q=src+1;
//			const unsigned char *tp=p+1;
//			while ( q<end && len < MATCH_HIGH_NUM && *q==*tp )  {
//				len++;q++;tp++;
//			}
//		}
//		else continue;
//
//		if (len>max) 
//		{
//			pos=unsigned int(src-p);
//			max=len;
//		}
//	}
//
//	return max;
//}

//unsigned long compress1(UCHAR *src, unsigned long srcsize,UCHAR *dst)
//{
//	unsigned int num = FindSame<UCHAR,unsigned int>(src,src+10);
//
//
//}

UCHAR * uncompress1(UCHAR *src,unsigned long size, unsigned long dstsize)
{
	UCHAR *dst = new UCHAR[dstsize];
	memset(dst, 0,dstsize);
	UCHAR *p = dst;
	char flag;
	for (unsigned int i=0;i<size;)
	{
		flag = src[i++];
		//cout<<"======= flag="<<hex<<(int)flag<<" i ="<<i<<endl;
		for (int t=0;t<8;t++)
		{

			if (flag & 1)
			{
				*p++ = src[i++];
			}
			else
			{
				if (i >= size - 1 || (src[i] == 0 && src[i+1] == 0))
					break;
				int num = ((src[i+1] & 0xf0) >>4) ;
				int mask = (src[i+1] & 0x0f) ;
				int pos = src[i] + (mask<<8);
				
				if (num == 0xf)
				{
					if (mask == 0)
					{
						num = mask + 0x12;
						for (int k=0;k<num;k++)
							*p++ = dst[p-dst - pos];
						
					}
					else
					{
						for (int k=0;k < num + 3;k++)
							*p++ = dst[p-dst - pos];
					}
				}
				else 
				{
					for (int k=0;k<num+3;k++)
						*p++ =dst[p-dst - pos];
				}

				i+=2;

			}
			flag>>=1;
		}

	}
	
#ifdef DEBUG
	cout<<hex<<p-dst<<endl;
	cout<<hex<<dstsize<<endl;
#endif

	if (p-dst != dstsize)
	{
		cout<<"Heap error!Please contact me. :)\n";
		exit(-1);
	}
	return dst;
}

UCHAR * uncompress2(UCHAR *src,unsigned long size, unsigned long dstsize)
{
	UCHAR *dst = new UCHAR[dstsize];
	memset(dst,0,dstsize);
	UCHAR *p = dst;
	const UCHAR* const pend = dst + dstsize;
	char flag;
	for (unsigned int i=0;i<size && p < pend;)
	{
		flag = src[i++];
		cout<<"======= flag="<<hex<<(int)flag<<" i ="<<i<<endl;
		
		for (int t=0;t<8;t++)
		{
			if (flag & 1)
			{
				*p++ = src[i++];
			}
			else
			{
				if (i == size || (src[i] == 0 && src[i+1] == 0))
					break;
				int num = ((src[i+1] & 0xf0) >>4) ;
				int mask = (src[i+1] & 0x0f) ;
				int pos = src[i] + (mask<<8);
				
				if (num == 0xf)
				{
					if (mask == 0)
					{
						num = src[i] + 0x13;
						for (int k=0;k<num;k++)
							*p++ = src[i+2];
						i++;
					}
					else
					{
						for (int k=0;k < mask + 3;k++)
							*p++ = src[i];
					}
				}
				else 
				{
					for (int k=0;k<num+3;k++)
						*p++ =dst[p-dst - pos];
				}

				i+=2;

			}
			flag>>=1;
		}

	}
	
#ifdef DEBUG
	cout<<hex<<p-dst<<endl;
	cout<<hex<<dstsize<<endl;
#endif

	/*if (p-dst != dstsize)
	{
		cout<<"Heap error!Please contact me. :)\n";
		exit(-1);
	}*/
	return dst;
}

UCHAR * SO3_uncompress2(UCHAR *src,unsigned long size, unsigned long dstsize)
{
	UCHAR *dst = new UCHAR[dstsize];
	memset(dst,0,dstsize);
	UCHAR *p = dst;
	const UCHAR* const pend = dst + dstsize;
	char flag;
	for (unsigned int i=0;i<size && p < pend;)
	{
		flag = src[i++];
		cout<<"======= flag="<<hex<<(int)flag<<" i ="<<i<<endl;
		
		for (int t=0;t<8;t++)
		{
			if (flag & 1)
			{
				*p++ = src[i++];
			}
			else
			{
				if (i == size || (src[i] == 0 && src[i+1] == 0))
					break;
				int num = ((src[i+1] & 0xf0) >>4) ;
				int mask = (src[i+1] & 0x0f) ;
				int pos = src[i] + (mask<<8);
				
				if (num == 0xf)
				{
					if (mask == 0)
					{
						num = src[i] + 0x13;
						for (int k=0;k<num;k++)
							*p++ = src[i+2];
						i++;
					}
					else
					{
						for (int k=0;k < mask + 3;k++)
							*p++ = src[i];
					}
				}
				else 
				{
					for (int k=0;k<num+3;k++)
						*p++ =dst[p-dst - pos];
				}

				i+=2;

			}
			flag>>=1;
		}

	}
	
#ifdef DEBUG
	cout<<hex<<p-dst<<endl;
	cout<<hex<<dstsize<<endl;
#endif

	/*if (p-dst != dstsize)
	{
		cout<<"Heap error!Please contact me. :)\n";
		exit(-1);
	}*/
	return dst;
}

void Decode(struct _finddata_t f)   /* Just the reverse of Encode(). */
{
	char   drive[_MAX_DRIVE];   
    char   dir[_MAX_DIR];   
    char   ffname[_MAX_FNAME];   
    char   ext[_MAX_EXT];   
	_splitpath_s(f.name,drive,dir,ffname,ext);


	UCHAR *src;
	unsigned long size;
	src = ReadAllFromFile(f.name,size);
	
	string magic(src,src+3);
	if (magic.compare(Magic))
	{
		printf("Error SLZ file. %s\n",f.name);
		delete[] src;
		return;
	}

	unsigned long srcSize = *(unsigned int*)(src + 4);
	unsigned long dstSize = *(unsigned int*)(src + 8);
	UCHAR *dst = NULL;

	switch (src[3])
	{
		case 0:
			break;
		case 1:
			dst = uncompress1(src + 0x10, srcSize, dstSize);
			break;
		case 2:
			dst = uncompress2(src + 0x10, srcSize, dstSize);
			break;
		default:
			printf("Unknow SLZ type. %s\n",f.name);
	}

	if (NULL != dst)
	{
		char name[255];
		sprintf_s(name,255,"%s.%s",ffname,ext2);
		WriteToFile(name,dst,dstSize,0,"wb+");
		printf("Uncompress file %s to %s ok... type:%x\n",f.name, name, src[3]);
	}
	else 
	{
		printf("Uncompress file %s error...\n",f.name);
	}

	delete[] src;
	delete[] dst;

}

static BYTE *pData;
static BYTE *pData2;
static char path[255 + 1];
static int filenum = 0;

void ExtractTxt(unsigned long offset,unsigned long srcsize)
{
	BYTE *p = (offset > 0x20000000) ? pData2 : pData;
	UCHAR *src = *(unsigned long*)(p + offset + 0x20) + offset + pData;

	string magic(src,src + 3);
	if (magic.compare(Magic))
		return;

	int i = 1;
	char fname[255];
	while (true)
	{
		unsigned long srcSize = *(unsigned long*)(src + 4);
		unsigned long dstSize = *(unsigned long*)(src + 8);
		unsigned long nextAddr = *(unsigned long*)(src + 12);

		UCHAR *dst = NULL;
		switch (src[3])
		{
			case 0:
				sprintf_s(fname,255,"%s\\%08x_%08x_0.%d",path,offset,dstSize,i);
				WriteToFile(fname,src + 0x10,dstSize,0,"wb+");
				break;

			case 1:
				dst = uncompress1(src + 0x10, srcSize, dstSize);
				sprintf_s(fname,255,"%s\\%08x_%08x_1.%d",path,offset,dstSize,i);
				if (NULL != dst && WriteToFile(fname,dst,dstSize,0,"wb+"))
					printf("Uncompress to %08x_%08x ok... type:%x\n", offset,dstSize, src[3]);
				else
					printf("Uncompress error.offset=%lx\n",offset);
				break;

			case 2:
				dst = uncompress2(src + 0x10, srcSize, dstSize);
				sprintf_s(fname,255,"%s\\%08x_%08x_2.%d",path,offset,dstSize,i);
				if (NULL != dst && WriteToFile(fname,dst,dstSize,0,"wb+"))
					printf("Uncompress to %08x_%08x ok... type:%x\n", offset,dstSize, src[3]);
				else
					printf("Uncompress error.offset=%lx\n",offset);
				break;

			default:
				printf("Unknow SLZ type. %lx, type=%d\n", src - pData, src[3]);
		}

		delete[] dst;
		if (!nextAddr) break;
		src += nextAddr;
		i++;
	}



}

void extract(unsigned long offset,unsigned long srcsize)
{
	char name[255];
	char fname[255];
	sprintf_s(name,255,"%08x_%08x",offset,srcsize);
	UCHAR *src = pData + offset;
	
	filenum++;
	string magic(src,src+3);
	if (magic.compare(Magic))
	{
		printf("Not SLZ file. Dump to file %s\n",name);
		sprintf_s(fname,255,"%s\\%s.bin",path,name);
		WriteToFile(fname,pData + offset,srcsize,0,"wb+");
		return;
	}

	unsigned long srcSize = *(unsigned int*)(src + 4);
	unsigned long dstSize = *(unsigned int*)(src + 8);
	UCHAR *dst = NULL;

	switch (src[3])
	{
		case 0:
			sprintf_s(fname,255,"%s\\%08x_%08x",path,offset,dstSize);
			WriteToFile(fname,src,dstSize,0,"wb+");
			return;
		case 1:
			dst = uncompress1(src + 0x10, srcSize, dstSize);
			break;
		case 2:
			dst = uncompress2(src + 0x10, srcSize, dstSize);
			break;
		default:
			printf("Unknow SLZ type. %s\n",name);
	}

	/*CString ffname;
	ffname.Format("%s\\%08x_%08x",path,offset,dstSize);*/
	sprintf_s(fname,255,"%s\\%08x_%08x",path,offset,dstSize);
	if (NULL != dst && WriteToFile(fname,dst,dstSize,0,"wb+"))
	{
		printf("Uncompress to %08x_%08x ok... type:%x\n", offset,dstSize, src[3]);
	}
	else 
	{
		printf("Uncompress file %x error...\n",offset);
	}

	delete[] dst;

	unsigned long additional = *(unsigned int*)(src + 0xc);
	if (additional)
		extract(additional + offset,0);


}

void Dump()
{
	SYSTEM_INFO s;
	GetSystemInfo(&s);

	CFileDialog   Filedlg(TRUE,   
							NULL,   
							"so1pack.bin",   
							OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT   |   
							OFN_ALLOWMULTISELECT|OFN_EXPLORER,     
							"StarOcean First Departure(PSP)|*.bin||",   
							NULL);     

	//  Filedlg.m_ofn.lpstrFile   =   lpstrFilename;     

	Filedlg.m_ofn.lpstrTitle   =   "打开bin文件";//改变标题   

	//Filedlg.m_ofn.lpstrInitialDir   =   ".\\";//设定打开初始目录   

	//  Filedlg.m_ofn.lStructSize   =   structsize;   

	Filedlg.m_ofn.nMaxFile   =   MAX_PATH;   

	if(Filedlg.DoModal()   ==   IDOK)   
	{      
		CString fn;
		fn=Filedlg.GetPathName();

		HANDLE hFile; 
 
		hFile = CreateFile(fn ,					  // file to open
						   GENERIC_READ,          // open for reading
						   FILE_SHARE_READ,       // share for reading
						   NULL,                  // default security
						   OPEN_EXISTING,         // existing file only
						   FILE_ATTRIBUTE_NORMAL, // normal file
						   NULL);                 // no attr. template
		if (hFile == INVALID_HANDLE_VALUE) 
		{ 
			printf("Could not open file (error %d)\n", GetLastError());
			exit(-1);
		}

		DWORD size = GetFileSize(hFile,0);
		HANDLE hFileMapping = CreateFileMapping(hFile,NULL,PAGE_READONLY, 
								0, size,  NULL);

		//PVOID pvFile = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
		CloseHandle(hFile);
		
		//pData=(BYTE*)MapViewOfFile(hFileMapping,FILE_MAP_READ,0,0,0); 
		pData = (BYTE*)MapViewOfFile(hFileMapping,FILE_MAP_READ,0,0,0x20000000);       
		pData2 = (BYTE*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0x20000000, 0);

		if   (pData ==   NULL || pData2 ==   NULL)   
		{   
		   TCHAR szBuf[80]; 
			LPVOID lpMsgBuf;
			DWORD dw = GetLastError(); 

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL );

			wsprintf(szBuf, 
				"%s failed with error %d: %s", 
				"map", dw, lpMsgBuf); 
		 
			MessageBox(NULL, szBuf, "Error", MB_OK); 


			CloseHandle(hFileMapping); 
			return;
		}

		unsigned long cnt = *(unsigned long*)pData;
		unsigned long pos = *(unsigned long*)(pData + 4);
		unsigned long length = *(unsigned long*)(pData + 8);
		unsigned long offset = *(unsigned long*)(pData + 0xc);

		unsigned long *addr = new unsigned long[cnt];
		memcpy(addr,pData + pos, cnt * 4);

		unsigned long *len = new unsigned long[cnt];
		memcpy(len,pData + length, cnt * 4);

		for (unsigned long i = 0; i<cnt; i++)
		{
			if (dumptxt)
			{
				if (i > 2405 && i < 3310)
				{	
					//printf("%8x,%d\n",addr[i],len[i]);
					ExtractTxt(addr[i],len[i]);
				}
			}
			else
				extract(addr[i],len[i]);
		}

		delete[] addr;
		delete[] len;

		UnmapViewOfFile(pData);
		CloseHandle(hFileMapping);

	}
}

void PrintHelp()
{
	printf("'SLZ dump all' dump bin files.\n"
				"'SLZ dump txt' dump txt files.\n"
			  "'SLZ d extname1 extname2' decode files.\n");
	exit(0);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
		printf("SLZ for StarOcean First Departure(PSP) v0.99a By kid 2008.1.26\n\n");

		if (argc == 3 && strcmp(argv[1], "dump") == 0)
		{
			if (strcmp(argv[2], "txt") == 0)
				dumptxt = true;
			else if (strcmp(argv[2], "all") != 0)
				PrintHelp();
			GetCurrentDirectory(255,path);
			Dump();
			cout<<"\nComplete!\n";
			cout<<"Press Enter to Exit...\n";
			cin.get();
			return nRetCode;
		}

		if (argc!=4)
		{
			PrintHelp();
		}
		
		sprintf_s(ext1,20,"*.%s",argv[2]);
		sprintf_s(ext2,20,"%s",argv[3]);
		//if (strpbrk(argv[1], "4")) isoffset=true;

		if ((hFile = _findfirst(ext1, &c_file )) == -1L )
		{
			printf( "No files in current directory!\n" ); 
			AfxMessageBox("未找到文件！");
			return nRetCode;
		}
		else
		{
			Decode(c_file);
			while ( _findnext( hFile, &c_file ) == 0 )
			{
				Decode(c_file);
			}
			_findclose( hFile ); 
		}
	}

	cout<<"\nComplete!\n";
	cout<<"Press Enter to Exit...\n";
	cin.get();

	return nRetCode;
}
