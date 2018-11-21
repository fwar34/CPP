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
    unsigned int age;
    unsigned int score;
};

bool mycmp(Person p1, Person p2)
{
    return p1.age < p2.age;
}

bool mycmp2(Person p1, Person p2)
{
    return (int)p1.age - (int)p1.score > (int)p2.age - (int)p2.score;
}

int main()
{
    vector<Person> persons;
    Person p1;
    p1.name = "J";
    p1.age = 1;
    p1.score = 13;
    Person p2;
    p2.name = "K";
    p2.age = 8;
    p2.score = 9;
    persons.push_back(p1);
    persons.push_back(p2);
    for (auto it = persons.begin(); it != persons.end(); ++it) {
        cout << (*it).name << endl;
    }

    cout << "sort" << endl;

    sort(persons.begin(), persons.end(), mycmp);
    for (auto it = persons.begin(); it != persons.end(); ++it) {
        cout << (*it).name << endl;
    }

    cout << "sort2" << endl;
    sort(persons.begin(), persons.end(), mycmp2);
    for (auto it = persons.begin(); it != persons.end(); ++it) {
        cout << (*it).name << endl;
    }

    unsigned int a = 10;
    unsigned int b = 20;
    cout << (int)a - (int)b << endl;
    cout << b - a << endl;

    return 0;
}
