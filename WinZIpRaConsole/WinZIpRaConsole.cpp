﻿#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include "Node.h"
#include <bitset>
#define freqSize 256
#define ioSize 1024 * 1024 * 1

using PriQueue = std::priority_queue<Node::pointer, std::vector<Node::pointer>, LowestPriority>;

std::vector<int> ReadFile(std::string fileName)
{
    std::ifstream ifs(fileName, std::ifstream::binary);
    if (!ifs)
        throw "Can't open file";

    std::vector<int> frequency(freqSize, 0);

    while (!ifs.eof())
    {
        unsigned char ch;
        ifs.read(reinterpret_cast<char*>(&ch), sizeof(ch));
        if (ifs.eof()) break;
        frequency[ch]++;
    }
    return frequency;
}

PriQueue GetTree(PriQueue queue)
{
    while (queue.size() > 1)
    {
        Node::pointer leftNode = queue.top();
        queue.pop();
        Node::pointer rightNode = queue.top();
        queue.pop();

        Node::pointer parentNode = std::make_shared<Node>(0, leftNode->GetFrequency() + rightNode->GetFrequency());
        parentNode->left = leftNode;
        parentNode->right = rightNode;

        leftNode->parent = parentNode;
        rightNode->parent = parentNode;

        queue.push(parentNode);
    }
    return queue;
}

void SetNodeCode(Node::pointer root, std::string str, std::vector<std::string>& codes)
{
    if (root->left != nullptr)
        SetNodeCode(root->left, str + "0", codes);
    if (root->right != nullptr)
        SetNodeCode(root->right, str + "1", codes);

    if (root->left == nullptr && root->right == nullptr)
    {
        root->SetChCode(str);
        codes[root->GetCh()] = str;
    }
}

void ZipFile(std::string fileName, std::vector<int> frequency, std::string code)
{
    std::string zipFileName = fileName + ".zir";
    std::ofstream ofs(zipFileName, std::ifstream::binary);
    if (!ofs)
        throw "Can't open file";

    int count = count_if(frequency.begin(), frequency.end(), [](int& value)
    {
        return (value != 0);
    });

    ofs.write(reinterpret_cast<char*>(&count), sizeof(count));

    for_each(frequency.begin(), frequency.end(), [index = unsigned char(0), &ofs](int& value) mutable
    {
        if (value != 0)
        {
            ofs.write(reinterpret_cast<char*>(&index), sizeof(index));
            ofs.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
        index++;
    });
    //for (int i = 0; i < frequency.size(); i++)
    //{
    //    if (frequency[i] != 0)
    //    {
    //        ofs.write(reinterpret_cast<char*>(&i), sizeof(i));
    //        ofs.write(reinterpret_cast<char*>(&frequency[i]), sizeof(frequency[i]));
    //    }
    //}
    int num = code.size();
    unsigned long byteCount = 0;
    unsigned long mod = 0;
    while (num > 0)
    {
        num -= ioSize;
        if (num > 0)
        {
            mod = num;
            byteCount++;
        }           
    }


    ofs.write(reinterpret_cast<char*>(&byteCount), sizeof(byteCount));
    ofs.write(reinterpret_cast<char*>(&mod), sizeof(mod));
    int position = 0;
    while (position < byteCount)
    {
        std::bitset<ioSize> b(code.substr(position * ioSize, ioSize));
        ofs.write(reinterpret_cast<char*>(&b), sizeof(b));
        position++;
        std::cout << "\r" << byteCount << "\\" << position << std::flush;
    }
    for (int i = 0; i < mod; i += 64)
    {
        std::bitset<CHAR_BIT*8> b(code.substr(position * ioSize + i, CHAR_BIT*8));
        ofs.write(reinterpret_cast<char*>(&b), sizeof(b));
    }

   
}

std::string UnZipFile(std::string fileName, std::vector<int>& frequency, std::string code)
{
    std::string zipFileName = fileName + ".zir";
    std::ifstream ifs(zipFileName, std::ifstream::binary);
    if (!ifs)
        throw "Can't open file";

    int count = 0;
    ifs.read(reinterpret_cast<char*>(&count), sizeof(count));

    int index = 0;
    while(index < count)
    {
        unsigned char ch;
        ifs.read(reinterpret_cast<char*>(&ch), sizeof(ch));
        int freq = 0;
        ifs.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        frequency[ch] = freq;
        index++;
    }

    unsigned long byteCount = 0;
    unsigned long mod = 0;

    ifs.read(reinterpret_cast<char*>(&byteCount), sizeof(byteCount));
    ifs.read(reinterpret_cast<char*>(&mod), sizeof(mod));

    int position = 0;
    while (position < byteCount)
    {
        std::bitset<ioSize> b("");
        unsigned long value;
        ifs.read(reinterpret_cast<char*>(&b), sizeof(b));
        code += b.to_string();
        position++;
        std::cout << "\r" << byteCount << "\\" << position << std::flush;
    }
    if (mod > 0)
    {
        std::bitset<ioSize> b("");
        unsigned long value;
        ifs.read(reinterpret_cast<char*>(&b), sizeof(b));
        code += b.to_string().substr(ioSize - mod, ioSize);
    }
    return code;
}

std::string GetText(Node::pointer root, std::string str)
{
    Node::pointer node = root;
    std::string text("");

    for (int i = 0; i < str.size(); i++)
    {
        char ch = str[i];
        if (ch == '0')
        {
            if (node->left != nullptr)
            {
                node = node->left;
                if (node->left == nullptr && node->right == nullptr)
                {
                    text += node->GetCh();
                    node = root;
                }
            }        
        }
        else
        {
            if (node->right != nullptr)
            {
                node = node->right;
                if (node->left == nullptr && node->right == nullptr)
                {
                    text += node->GetCh();
                    node = root;
                }
            }
        }
    }
    return text;
}

void WriteFile(std::string fileName, std::string text)
{
    std::ofstream ofs(fileName, std::ifstream::binary);
    if (!ofs)
        throw "Can't open file";
    ofs << text;
}
std::string GetCode(std::string fileName, std::vector<std::string>& codes)
{
    std::ifstream ifs(fileName, std::ifstream::binary);
    std::string code("");
    if (!ifs)
        throw "Can't open file";


    while (!ifs.eof())
    {
        char ch;
        ifs.read(&ch, 1);
        if (ifs.eof()) break;
        code += codes[static_cast<unsigned char>(ch)];
    }
    return code;
}
void Zip(std::string fileName)
{
    std::vector<int> frequency = ReadFile(fileName);

    PriQueue queue;

    for (int i = 0; i < frequency.size(); ++i)
    {
        if (frequency[i] != 0)
        {
            Node::pointer node = std::make_shared<Node>(i, frequency[i]);
            queue.push(node);
        }
    }
    queue = GetTree(queue);

    std::vector<std::string> codes(freqSize, "");
    SetNodeCode(queue.top(), "", codes);
    std::string code = GetCode(fileName, codes);
    ZipFile(fileName, frequency, code);
}
void UnZip(std::string fileName, std::string fileNameSecond)
{
    std::vector<int> frequency(freqSize, 0);

    std::vector<std::string> codes(freqSize, "");
    std::string code("");
    code = UnZipFile(fileName, frequency, code);

    PriQueue queue;
    for (int i = 0; i < frequency.size(); ++i)
    {
        if (frequency[i] != 0)
        {
            Node::pointer node = std::make_shared<Node>(i, frequency[i]);
            queue.push(node);
        }
    }
    queue = GetTree(queue);

    std::string text = GetText(queue.top(), code);
    WriteFile(fileNameSecond, text);
}
int main()
{
    std::string fileName("4.bmp");
    Zip(fileName);

    UnZip(fileName, "42.bmp");

    return 0;
}