#include <iostream>
#include <string>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
using namespace std;

#define CODESIZE 10000000

void charToBit(char *s, int *text, unsigned long long count) // 字符串转二进制
{
    for (unsigned long long i = 0; i < count; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            text[i * 8 + j] = (s[i] >> j) & 1; // 从低位到高位存储
        }
    }
}

void bitToChar(int *text, char *s, unsigned long long count) // 二进制转字符串
{
    for (unsigned long long i = 0; i < count; i++)
    {
        s[i] = 0; // 确保初始值为 0
        for (int j = 0; j < 8; j++)
        {
            s[i] |= (text[i * 8 + j] << j); // 从低位到高位还原
        }
    }
}

void intToBit(int *text, unsigned long long count) // 整型转字符串
{
    int i = 0;
    for (int i = 0; i < 20; i++)
        text[i] = 0;
    while (count)
    {
        text[i] = count % 2;
        count /= 2;
        i++;
    }
}

unsigned long long bitToInt(int *text) // 二进制转整型
{
    unsigned long long count = 0;
    for (int i = 0; i < 20; i++)
        count += text[i] * pow(2, i);
    return count;
}

bool isBigEnough(int ziplen,int bmplen)
{
    // 已实现
    if(bmplen>=ziplen)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void embedMessage(int* l,int* f,int len)
{
    // 已实现
    for(int i=0;i<len;i++)
    {
        l[i]=f[i]|(l[i]&0xFE);
    }
}

void extractMessage(int* r,int* f,int len)
{
    // 已实现
    for(int i=0;i<len;i++)
    {
        f[i]=r[i]&1;
    }
}

int main()
{
    cout << "Pleasw to choose mode(\"0\" for decryption and \"1\" for encryption): ";
    int mode; // 选择模式，1是加密，二是解密
    cin >> mode;
    if (mode)
    {
        FILE *fin = fopen("pic.bmp", "rb");
        FILE *fout = fopen("secret.bmp", "wb");
        FILE *fin1 = fopen("secret.zip", "rb");
        BITMAPFILEHEADER bmph;                                                // BMP文件头
        BITMAPINFOHEADER bmpf;                                                // BMP文件信息头
        fread(&bmph, sizeof(BITMAPFILEHEADER), 1, fin);                       // 读取BMP文件头
        fread(&bmpf, sizeof(BITMAPINFOHEADER), 1, fin);                       // 读取BMP文件信息头
        long w = bmpf.biWidth;                                                // 图像的横向像素点个数
        long h = bmpf.biHeight;                                               // 图像的纵向像素点个数
        unsigned long long size = static_cast<unsigned long long>(w * h * 3); // 图像RGB信息的字节大小
        
        char *buf1 = static_cast<char *>(malloc(sizeof(char) * size));                         // 储存BMP文件RGB信息
        int *buf2 = static_cast<int *>(malloc(sizeof(int) * size * 8));                        // 储存BMP文件RGB信息的二进制形式
        fseek(fin, static_cast<long>(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)), 0); // 文件指针移动至RGB信息的开头
        fread(buf1, sizeof(char), size, fin);                                                  // 读取RGB信息
        charToBit(buf1, buf2, size);                                                           // 将RGB信息转化为二进制形式
        char *s = static_cast<char *>(malloc(sizeof(char) * CODESIZE));                        // 储存压缩文件的信息
        unsigned long long count = 0;                                                          // 记录字符串s中的有效信息长度
        while (!feof(fin1))
        {
            fread(s + count, sizeof(char), 1, fin1);
            count++;
        }
        
        if (!isBigEnough(count*8,size))
        {
            cout << "The BMP file is not big enough..." << endl;
            return 0;
        }
        else
            cout << "The BMP file is big enough!" << endl;
        
        int *buf3=static_cast<int *>(malloc(sizeof(int) * count * 8));
        charToBit(s,buf3,count);
        embedMessage(buf2,buf3,count*8);
        cout<<"count: "<<count<<endl;
        bitToChar(buf2, buf1, size); //buf2是从隐写后的图片RGB信息，转换为char形式存在buf1中输出文件，也可以自己更换变量或代码逻辑
        fwrite(&bmph, sizeof(BITMAPFILEHEADER), 1, fout);
        fwrite(&bmpf, sizeof(BITMAPINFOHEADER), 1, fout);
        fwrite(buf1, 1, size, fout);
        free(buf1);
        free(buf2);
        free(buf3);
        fclose(fin);
        fclose(fout);
        fclose(fin1);
        cout << "Bingo!" << endl;
    }
    else
    {
        FILE *fin = fopen("secret.bmp", "rb");
        FILE *fout = fopen("ans.zip", "wb");
        unsigned long long l=0;
        cout<<"please input the size of the zip"<<endl;
        cin>>l;
        int* plain=static_cast<int*>(malloc(sizeof(int)*l*8));
        char* plainText=static_cast<char*>(malloc(sizeof(char)*l));
        BITMAPFILEHEADER bmph;                                                // BMP文件头
        BITMAPINFOHEADER bmpf;                                                // BMP文件信息头
        fread(&bmph, sizeof(BITMAPFILEHEADER), 1, fin);                       // 读取BMP文件头
        fread(&bmpf, sizeof(BITMAPINFOHEADER), 1, fin);                       // 读取BMP文件信息头
        fseek(fin, static_cast<long>(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)), 0); // 文件指针移动至RGB信息的开头
        char* tmp=static_cast<char *>(malloc(sizeof(char) * l*8)); 
        int* tmpbits=static_cast<int *>(malloc(sizeof(int) * l*8 * 8));
        fread(tmp,sizeof(char),l*8,fin);
        charToBit(tmp,tmpbits,l*8);
        extractMessage(tmpbits,plain,l*8);
        bitToChar(plain, plainText, l); //plain是从BMP中抽取出的信息的二进制形式，也可以自己更换变量或代码逻辑
        fwrite(plainText, sizeof(char), l, fout); // 输出RAR信息
        free(plain);
        free(plainText);
        free(tmp);
        free(tmpbits);
        fclose(fin);
        fclose(fout);
        cout << "Bingo!" << endl;
    }
    return 0;
}
