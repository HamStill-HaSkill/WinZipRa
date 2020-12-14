#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include "Node.h"
#include <bitset>


using PriQueue = std::priority_queue<Node::pointer, std::vector<Node::pointer>, LowestPriority>;

std::vector<int> ReadFile(std::string fileName)
{
    std::ifstream ifs(fileName, std::ifstream::binary);
    if (!ifs)
        throw "Can't open file";

    std::vector<int> frequency(256, 0);

    while (!ifs.eof())
    {
        char ch;
        ifs.read(&ch, 1);
        if (ifs.eof()) break;
        frequency[static_cast<unsigned char>(ch)]++;
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
        std::cerr << root->GetCh() << str << "\n";
    }
}

void ZipFile(std::string fileName, std::vector<int> frequency, PriQueue queue, std::string code)
{
    std::string zipFileName = fileName + ".zir";
    std::ofstream ofs(zipFileName, std::ifstream::binary);
    if (!ofs)
        throw "Can't open file";

    unsigned char count = count_if(frequency.begin(), frequency.end(), [](int& value)
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

    int byteCount = code.size() / CHAR_BIT;
    unsigned char mod = code.size() % CHAR_BIT;

    ofs.write(reinterpret_cast<char*>(&byteCount), sizeof(byteCount));
    ofs.write(reinterpret_cast<char*>(&mod), sizeof(mod));

    int position = 0;
    while (position < byteCount)
    {
        std::bitset<CHAR_BIT> b(code.substr(position++ * CHAR_BIT, CHAR_BIT));
        unsigned char value = static_cast<unsigned char>(b.to_ulong());
        ofs.write(reinterpret_cast<char*>(&value), sizeof(value));
    }
    if (mod > 0)
    {
        std::bitset<CHAR_BIT> b(code.substr(position * CHAR_BIT, mod));
        unsigned char value = static_cast<unsigned char>(b.to_ulong());
        ofs.write(reinterpret_cast<char*>(&value), sizeof(value));
    }

   
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
int main()
{
    std::string fileName("text.txt");
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

    std::vector<std::string> codes(256, "");
    SetNodeCode(queue.top(), "", codes);
    std::string code = GetCode(fileName, codes);
    ZipFile(fileName, frequency, queue, code);
    while (!queue.empty())
    {
        Node::pointer n = queue.top();
        std::cerr << n->GetFrequency() << "\n";
        queue.pop();
    }
    return 0;
}
