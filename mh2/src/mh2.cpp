// mh2.cpp : Defines the entry point for the console application.
//

/*
	lz_word 双字节压缩/解压程序
	Writen By kid V1.0 2006.12.5
*/

#include "stdafx.h"
#include "mh2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define N        2048   /* size of ring buffer */
#define F        2048   /* upper limit for match_length */ 
WORD			 text_windows[N+F-1];
FILE *infile,*outfile;


/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int match(WORD *src,int s,long *pos,long textsize)			//寻找最大匹配长度
{
	int len=0,p1=0,p2=0,max=0;
	for (int k=(s-2047)>=0?(s-2047):0;k<s;k++)
	{
		if (src[k]==src[s])
		{
			len++;p1=k+1;p2=s+1;
			while (p2<textsize&&src[p1++]==src[p2++]) len++;
		}
		else continue;
		if (len>=0xffff) AfxMessageBox("wrong");
		if (len>max) 
		{
			max=len;
			*pos=k;
			len=0;
		}
		len=0;

	}
	return max;
}

void Encode()
{
	fseek(infile,0l,SEEK_END);
	long len=ftell(infile)/2;			//得到原文件大小
	rewind(infile);
	
	WORD *src=new WORD[len];
	fread(src,2l*len,1,infile);

	long pos=0,maxlen=0,k=0;
	WORD flag=0;
	WORD dst[32]={0};
	int pit=0;

	while (k<len)
	{
		flag=0;
		pit=0;
		for (int i=0;i<16;i++)
		{
			if (k>=len)					//读到文件末尾,写入结束标记
			{
				flag=(flag<<1)|1;
				flag=flag<<(16-i-1);
				dst[pit++]=0;
				dst[pit++]=0;
				break;
			}

			maxlen=match(&src[0],k,&pos,len);
			if (maxlen<=1)				//原样输出
			{
				dst[pit++]=src[k];
				flag<<=1;
				k++;
				continue;
			}
			else						//压缩双字节
			{
				dst[pit++]=k-pos;
				if (maxlen>31)			//循环次数	第二字节 高5位
				{
					dst[pit++]=maxlen;
				}
				else
				{
					dst[pit-1]=dst[pit-1]|(maxlen<<11);
				}
				flag=(flag<<1)|1;
				k+=maxlen;
			}
		}

		fwrite(&flag,2l,1,outfile);
		for (int j=0;j<pit;j++)
			fwrite(&dst[j],2l,1,outfile);

//printf("flag=%lx\n",flag);
	}

	free(src);
}

void Decode()
{
	int  i, j, k, r;
    int  flags=0;		//压缩标记	1表示压缩 0未压缩

	for (i = 0; i < N - F; i++) text_windows[i] = '\x0';
    r = N - F;     

    for ( ;!feof(infile); ) 
	{
		fread(&flags,2l,1,infile);
//printf("%lx\t",flags);
		for (int t=0;t<16;t++)
		{

			if (flags & 0x8000)			//遇到压缩位为1 
			{
				if ((i = getc(infile)) == EOF) break;
				if ((j = getc(infile)) == EOF) break;

				int temp=i;
				//i 第二字节 高5位 				循环次数
				//j 第二字节 低3位＋第一字节 	窗口位置
				i=j>>3;j=((j%8)<<8)+temp;
				if (i==0) fread(&i,2l,1,infile);
				if (i==0&&j==0) return;			//终止标记
				for (k = 0; k < i; k++) 
				{
					WORD p=text_windows[(r-j) & (N - 1)];
					fwrite(&p,2l,1,outfile);
					text_windows[r]=p;
					r++;r &= (N - 1);
				}
			} 
			else 
			{
				if (!feof(infile))
				{
				//读取双字节,原样输出
				WORD temp;
				fread(&temp,2l,1,infile); 
				fwrite(&temp,2l,1,outfile);
				text_windows[r]=temp;r++;r &= (N - 1);
				}
			}
			flags<<=1;
		}
		flags=0;
	}
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		printf("lz_word v1.0 By kid 2006.12.5\n\n");
		char  *s;
    
		if (argc != 4) {
			printf("'lz_word e file1 file2' encodes file1 into file2.\n"
				   "'lz_word d file2 file1' decodes file2 into file1.\n");
			return EXIT_FAILURE;
		}
		if ((s = argv[1], s[1] || strpbrk(s, "DEde") == NULL)
		 || (s = argv[2], (infile  = fopen(s, "rb")) == NULL)
		 || (s = argv[3], (outfile = fopen(s, "wb")) == NULL)) {
			printf("??? %s\n", s);  return EXIT_FAILURE;
		}
		if (toupper(*argv[1]) == 'E') Encode();  else Decode();
		fclose(infile);  fclose(outfile);
		printf("Success\n");
		return EXIT_SUCCESS;
	}

	return nRetCode;
}


