/*************************************************************************
    > File Name: sort_test1.cpp
    > Author: Feng
    > Created Time: Wed 21 Nov 2018 02:05:50 PM CST
    > Content: 
 ************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Person
{
public:
    string name;
    int age;
};

bool mycmp(Person p1, Person p2)
{
    return p1.age < p2.age;
}

int main()
{
    vector<Person> persons;
    Person p1;
    p1.name = "J";
    p1.age = 10;
    Person p2;
    p2.name = "K";
    p2.age = 12;
    persons.push_back(p2);
    persons.push_back(p1);
    for (auto it = persons.begin(); it != persons.end(); ++it) {
        cout << (*it).name << endl;
    }

    cout << "sort" << endl;

    sort(persons.begin(), persons.end(), mycmp);
    for (auto it = persons.begin(); it != persons.end(); ++it) {
        cout << (*it).name << endl;
    }

    return 0;
}
