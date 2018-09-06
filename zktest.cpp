/*************************************************************************
    > File Name: zktest.cpp
    > Author: Feng
    > Created Time: Wed 05 Sep 2018 11:23:44 AM CST
    > Content: 
 ************************************************************************/
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include "ZooClient.h"

using namespace std;

int main(int argc, char *argv[])
{
    string host = "192.168.32.107:2181";
    ZooClient *zk = new ZooClient;
    bool ret = zk->start(host);
    if (!ret) {
        cout << "ZooClient start host(" << host << ") failed!" << endl;
        return 0;
    }

    string root = "/feng";
    if (!zk->nodeExist(root)) {
        ret = zk->createNode(root, ZNODE_TYPE_PERSIST);
        if (!ret) {
            cout << "ZooClient create node(" << root << ") failed!" << endl;
            return 0;
        }
    }

    string child = "/feng/child";
    for (int i = 0; i < 3; ++i) {
        ret = zk->createNode(child, ZNODE_TYPE_PERSIST_SEQUENCE);
        if (!ret) {
            cout << "ZooClient create node(" << child << ") failed!" << endl;
        }
    }

    vector<string> children;
    ret = zk->getNodeChildren(root, children);
    if (!ret) {
        cout << "ZooClient get node(" << root << ") children failed!" << endl;
        return 0;
    }

    vector<string>::iterator it = children.begin();
    for (; it != children.end(); ++it) {
        cout << *it << endl;
        if (argc > 1 && strcmp(argv[1], "delete") == 0) {
            ret = zk->deleteNode(*it);
            if (!ret) {
                cout << "ZooClient delete node(" << *it << ") failed!" << endl;
            }
        }
    }


    if (argc > 1 && strcmp(argv[1], "delete") == 0) {
        ret = zk->deleteNode(root);
        if (!ret) {
            cout << "ZooClient delete node(" << root << ") failed!" << endl;
        }
    }

    return 0;
}

