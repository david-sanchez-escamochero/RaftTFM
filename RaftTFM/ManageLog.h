#pragma once



class ManageLog
{
public:
	void write_log();
	void read_log();
	void open_log();
	void close_log();
};

//#include <iostream>
//#include <fstream>
//#include <cstring>
//using namespace std;
//
//struct MyRecord {
//    char name[80];
//    double balance;
//    unsigned long account_num;
//};
//int main()
//{
//    struct MyRecord acc;
//
//    strcpy(acc.name, "R");
//    acc.balance = 1.3;
//    acc.account_num = 34;
//
//    ofstream outbal("balance", ios::out | ios::binary);
//    if (!outbal) {
//        cout << "Cannot open file.\n";
//        return 1;
//    }
//
//    outbal.write((char*)&acc, sizeof(struct MyRecord));
//    outbal.close();
//
//    ifstream inbal("balance", ios::in | ios::binary);
//    if (!inbal) {
//        cout << "Cannot open file.\n";
//        return 1;
//    }
//
//    inbal.read((char*)&acc, sizeof(struct MyRecord));
//
//    cout << acc.name << endl;
//    cout << "Account # " << acc.account_num;
//    cout.precision(2);
//    cout.setf(ios::fixed);
//    cout << endl << "Balance: $" << acc.balance;
//
//    inbal.close();
//    return 0;
//}