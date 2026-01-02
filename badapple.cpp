#include<QApplication>
#include<QWidget>
#include<QTimer>
#include<vector>
#include <qnamespace.h>
#include<iostream>
#include<chrono>
#include<thread>
using namespace std;

int main(int argc, char *argv[]){
    qputenv("QT_QPA_PLATFORM", "xcb");
    QApplication a(argc, argv);

    
    
    return a.exec();

}