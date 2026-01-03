#include<QApplication>
#include<QWidget>
#include<QScreen>
#include<QTimer>
#include<QDebug>
#include <qnamespace.h>
#include<QKeyEvent>
#include<QPushButton>
#include<QLabel>
#include<QVBoxLayout>
#include<QFileDialog>

using namespace std;



int main(int argc, char *argv[]){
    #ifdef __linux__
    qputenv("QT_QPA_PLATFORM", "xcb");
    #endif
    QApplication a(argc, argv);



    QWidget window;
    window.setWindowFlags(Qt::FramelessWindowHint);
    window.setGeometry(0, 100, 300, 300);
    window.setWindowTitle("jumpbox");
    window.setStyleSheet("QWidget {"
    "  border-image: url(':/images/picture.jpeg') 0 0 0 0 stretch stretch;"
    "}");
    window.show();


    QWidget *configWindow = new QWidget();
    configWindow->setWindowTitle("控制面板");
    configWindow->setFixedSize(250, 100);
    
    QVBoxLayout *layout = new QVBoxLayout(configWindow);
    QPushButton *btn = new QPushButton("更换动画图片", configWindow);
    layout->addWidget(btn);
    configWindow->show();

    QObject::connect(btn, &QPushButton::clicked, [&window]() {
        QString fileName = QFileDialog::getOpenFileName(
            nullptr, "选择新图片", "", "Images (*.png *.jpg *.jpeg *.bmp)");
        
        if (!fileName.isEmpty()) {
            // 更新动画窗口的样式
            window.setStyleSheet(QString("QWidget { border-image: url('%1') 0 0 0 0 stretch stretch; }").arg(fileName));
        }
    });


    // 在 QApplication 实例化之后调用
    QScreen *screen = QGuiApplication::primaryScreen();


    //  获取屏幕的可用区域 (自动扣除 Linux 顶栏或 Windows 任务栏)
    QRect availableGeometry = screen->availableGeometry();
    int availWidth = availableGeometry.width();
    int availHeight = availableGeometry.height();


    static int a_y = 1, v_y = 0, v_x = 10; 
    static int ground = availHeight - window.height();
    static int wall_1 = 0, wall_2 = availWidth - window.width();

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&window]() {
        // 1. 物理逻辑计算
    v_y += a_y;
    int next_y = window.y() + v_y;
    int next_x = window.x() + v_x;

        // 2. 碰撞检测
    if (next_y >= ground) {
            next_y = ground;
            v_y = -v_y;
        }
    if (next_x <= wall_1 || next_x >= wall_2){
        if (next_x <= wall_1)
            next_x = wall_1;
        if (next_x >= wall_2)
            next_x = wall_2;

            v_x = -v_x;
        }

        // 3. 执行移动
        window.move(next_x, next_y);
    });

    timer.start(16);
    
    
    return a.exec();

}