#include <memory>
#include <string>
class Node
{
public:
	using pointer = std::shared_ptr<Node>;
	pointer left{ nullptr };
	pointer right{ nullptr };
	pointer parent{ nullptr };
	Node() = default;
	Node(unsigned char c, int f) : ch(c), frequency(f) {}

	bool operator<(const Node& node) const
	{
		return frequency < node.frequency;
	}

	int GetFrequency() const
	{
		return frequency;
	}
	std::string GetChCode() const
	{
		return chCode;
	}
	void SetChCode(std::string code)
	{
		chCode = code;
	}
	unsigned char GetCh() const
	{
		return ch;
	}

private:
	unsigned char ch{ 0 };
	int frequency{ 0 };
	std::string chCode{ "" };

};

class  LowestPriority
{
public:
	bool operator()(Node::pointer& a, Node::pointer& b) const
	{
		return a->GetFrequency() > b->GetFrequency();
	}
};
