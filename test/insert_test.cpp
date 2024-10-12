#include <iostream>
#include <map>
#include <string>

using namespace std;

int main()
{
    map<int, string> mp;
    auto ret = mp.insert({3, "sldkf"s});
    cout << ret.first->first << endl;

    auto ret2 = mp.insert(mp.begin(), {4, "333333333333slkf"s});
    cout << ret2->second << endl;
}
