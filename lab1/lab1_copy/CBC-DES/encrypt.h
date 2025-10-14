#ifndef ENCRPT_H
#define ENCRPT_H
#include "transformation_matrix.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

// 将字符串s转二进制，存储在text中
void charToBit(char *s, bool *text)
{ 
    char tmp[8];
    for (int i = 0; i < 8; i++)
        tmp[i] = s[i];
    for (int i = 0; i < 8; i++)
    {
        for (int j = 7; j >= 0; j--)
        {
            text[i * 8 + j] = tmp[i] & 1;
            tmp[i] = tmp[i] >> 1;
        }
    }
}
// 将二进制text转字符串，存储在s中
void bitToChar(bool *text, char *s)
{ 
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            s[i] = s[i] << 1;
            s[i] = s[i] | text[i * 8 + j];
        }
    }
}
// 将二进制input复制到output中，复制比特长度位len
void copy(bool *output, bool *input, int len)
{
    for (int i = 0; i < len; i++)
        output[i] = input[i];
}
// 对output和parameter按位异或，异或比特长度为len
void XOR(bool *output, bool *parameter, int len)
{ 
    for (int i = 0; i < len; i++)
        output[i] = output[i] ^ parameter[i];
}
// 对text初始换位，将结果存在text中
void initialTransposition(bool *text)
{ 
    bool tmp[64];
    copy(tmp, text, 64);
    for (int i = 0; i < 64; i++)
        text[i] = tmp[IP[i] - 1];
}
// 对text扩展换位，将结果存在text中
void extendTransposition(bool *text)
{ 
    // 待实现
}
// 对text进行左移，time指当前处于第几轮迭代
void leftMove(int time, bool *text)
{ 
    for (int i = 0; i < time; i++)
    {
        bool tmp = text[0];
        for (int i = 0; i < 27; i++)
            text[i] = text[i + 1];
        text[27] = tmp;
    }
}
// 对text进行PC1置换，将结果存在output中
void PC1Transposition(bool *text, bool *output)
{ 
    for(int i=0;i<56;i++) output[i]=text[PC_1[i]-1];
}
// 对text进行PC2置换，将结果存在output中
void PC2Transposition(bool *text, bool *output)
{
    // 待实现
}
// 根据originalkey得到第time轮迭代的子密钥，将该子密钥存在output中
void getKey(bool *originalKey, bool *output, int time)
{ 
    bool tmp[56], A[28], B[28];
    PC1Transposition(originalKey, tmp);
    copy(A, tmp, 28);
    for (int i = 0; i < 28; i++)
        B[i] = tmp[i + 28];
    leftMove(move_time[time], A);
    leftMove(move_time[time], B);
    copy(tmp, A, 28);
    for (int i = 0; i < 28; i++)
        tmp[i + 28] = B[i];
    PC2Transposition(tmp, output);
}
// 对input做S盒换位，将结果存在output中
void SBoxTransposition(bool *input, bool *output)
{ 
    // 待实现
}
// 对text做P盒换位，将结果存在text中
void PBoxTransposition(bool *text)
{ 
    bool tmp[32];
    copy(tmp,text,32);
    for(int i=0;i<32;i++) text[i]=tmp[P_Box[i]-1];
}
// f函数
void f(bool *text, bool *originalKey, int time)
{ 
    bool text1[48];
    copy(text1, text, 32);
    extendTransposition(text1);
    bool key[48];
    getKey(originalKey, key, time);
    XOR(text1, key, 48);
    SBoxTransposition(text1, text);
    PBoxTransposition(text);
}
// 迭代函数
void iteration(bool *text, bool *originalKey, int time)
{ 
    bool L[32], R[32], newL[32], newR[32];
    copy(L, text, 32);
    for (int i = 0; i < 32; i++)
        R[i] = text[i + 32];
    copy(newL, R, 32);
    f(R, originalKey, time);
    XOR(L, R, 32);
    copy(newR, L, 32);
    copy(text, newL, 32);
    for (int i = 0; i < 32; i++)
        text[i + 32] = newR[i];
}
// 对text做32bit换位，将结果存在text中
void halfSwap(bool *text)
{ 
    // 待实现
}
// 对text做逆初始置换，将结果存在text中
void inverseTransposition(bool *text)
{ 
    // 待实现
}
// 加密过程
void encrypt(char *plaintext, char *key, bool *cipher, bool *iv)
{ 
    bool codetext[64], originalKey[64];
    charToBit(plaintext, codetext);
    charToBit(key, originalKey);
    XOR(codetext, iv, 64);
    initialTransposition(codetext);
    for (int i = 0; i < 16; i++)
        iteration(codetext, originalKey, i);
    halfSwap(codetext);
    inverseTransposition(codetext);
    copy(cipher, codetext, 64);
}
// 解密过程
void decrypt(char *ciphertext, char *key, char *plaintext, bool *iv)
{ 
    bool cipher[64], originalKey[64];
    charToBit(ciphertext, cipher);
    charToBit(key, originalKey);
    initialTransposition(cipher);
    for (int i = 0; i < 16; i++)
        iteration(cipher, originalKey, 15 - i);
    halfSwap(cipher);
    inverseTransposition(cipher);
    XOR(cipher, iv, 64);
    bitToChar(cipher, plaintext);
}
// 字符串填充函数，将text长度填充至8的倍数
void pad(string &text)
{ 
    int padding = 8 - text.length() % 8;
    int val = padding;
    char buf;
    itoa(val, &buf, 10);
    for (int i = 0; i < padding; i++)
        text += buf;
}
// 字符串还原函数，将填充后的字符串还原为原字符串
int cut(char *text)
{ 
    int padding = atoi(text + 7);
    if (padding < 1 || padding > 7)
        return 8;
    for (int i = 7; i >= 8 - padding; i--)
        if (text[i] != text[7])
            return 8;
    return 8 - padding;
}

#endif // ENCRPT_H
