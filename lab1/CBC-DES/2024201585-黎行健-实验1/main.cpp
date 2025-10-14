#include <iostream>
#include <fstream>
#include "transformation_matrix.h"
#include "encrypt.h"
using namespace std;

int main()
{
    int mode;
    cout << "Please input the mode you want to check(\"1\" for encryption and \"0\" for decryption): ";
    cin >> mode;
    char key[9] = "12345678", IVText[9] = "12345678"; //密钥和IV，也可以修改为用户输入的模式
    bool IV[64];
    string text;
    charToBit(IVText, IV); // 将IV转换为bit形式
    if (mode)
    {
        fstream fin, fout;
        fin.open("/Users/lxjarctane2/Desktop/lab1/CBC-DES/input.txt", ios_base::in); // 这里和下面文件均是绝对路径，注意修改
        fout.open("/Users/lxjarctane2/Desktop/lab1/CBC-DES/output.txt", ios_base::out);
        while (!fin.eof())
        {
            getline(fin, text); // 读取一行字符串
            pad(text); // 填充字符串
            int count = static_cast<int>(text.length()) / 8;
            for (int i = 0; i < count; i++)
            {
                char plainText[8], cipherText[8];
                bool cipher[64];
                for (int j = 0; j < 8; j++)
                    plainText[j] = text[j + i * 8];
                encrypt(plainText, key, cipher, IV); //进行加密
                copy(IV, cipher, 64); //按照CBC模式更新IV
                bitToChar(cipher, cipherText);
                for (int j = 0; j < 8; j++)
                    fout.put(cipherText[j]);
            }
            fout << endl;
        }
    }
    else
    {
        fstream fin, fout;
        fin.open("/Users/lxjarctane2/Desktop/lab1/CBC-DES/output.txt", ios_base::in);
        fout.open("/Users/lxjarctane2/Desktop/lab1/CBC-DES/input.txt", ios_base::out);
        while (!fin.eof())
        {
            getline(fin, text);
            int count = static_cast<int>(text.length()) / 8;
            for (int i = 0; i < count; i++)
            {
                char cipherText[8], plainText[8];
                for (int j = 0; j < 8; j++)
                    cipherText[j] = text[j + i * 8];
                decrypt(cipherText, key, plainText, IV); // 进行解密
                charToBit(cipherText, IV); //按照CBC模式更新IV
                int cutting = cut(plainText);
                for (int j = 0; j < cutting; j++)
                    fout << plainText[j];
            }
            fout << endl;
        }
    }
}
