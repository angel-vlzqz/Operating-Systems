#include <iostream>
// #include <vector>
using namespace std;

int main()
{
    cout << "Hello, World!" << endl;
    vector<int> g1;

    for (int i = 0; i <= 5; i++)
    {
	    g1.push_back(i);
    }
    cout << g1[2] << endl;
}
