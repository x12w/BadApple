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
#include<QMouseEvent>
#include <QElapsedTimer>

using namespace std;

static QPoint dragPosition; // 用于存储鼠标按下时的偏移量
static int a_y = 1, v_y = 0, v_x = 10; 
// 创建一个简单的事件过滤器类
class DragFilter : public QObject {
    QElapsedTimer timer;
    QPoint lastPos;
    double currentVx = 0;
    double currentVy = 0;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        QWidget *window = qobject_cast<QWidget*>(obj);
        if (!window) return false;

        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                dragPosition = mouseEvent->globalPos() - window->frameGeometry().topLeft();
                
                // 初始化计时和位置
                lastPos = mouseEvent->globalPos();
                timer.start(); 
                
                // 建议：按下时停止物理引擎定时器，防止冲突
                // globalTimer->stop(); 
                return true;
            }
        } 
        else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->buttons() & Qt::LeftButton) {
                QPoint currentPos = mouseEvent->globalPos();
                qint64 elapsed = timer.restart(); // 获取距离上次移动的时间(ms)

                if (elapsed > 0) {
                    // 计算瞬时速度 (像素/毫秒)
                    // 为了匹配你物理引擎的 16ms 步进，这里可以换算成 像素/帧
                    currentVx = (double)(currentPos.x() - lastPos.x()) / elapsed * 16.0;
                    currentVy = (double)(currentPos.y() - lastPos.y()) / elapsed * 16.0;
                }

                window->move(currentPos - dragPosition);
                lastPos = currentPos;
                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            // 当鼠标松开时，将计算出的瞬时速度同步回你的物理引擎变量
            v_x = (int)currentVx;
            v_y = (int)currentVy;
            //globalTimer->start();
            qDebug() << "松开速度:" << currentVx << "," << currentVy;
        }
        return QObject::eventFilter(obj, event);
    }
};


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
    DragFilter *filter = new DragFilter();
    window.installEventFilter(filter);
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



    static int ground = availHeight - window.height(), ceil = 0;
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
            v_y = -v_y * 0.9;
            v_x = v_x * 0.9;
        }
    if(next_y <= ceil){
        next_y = ceil;
        v_y = -v_y * 0.9;
        v_x = v_x * 0.9;
    }
    if (next_x <= wall_1 || next_x >= wall_2){
        if (next_x <= wall_1)
            next_x = wall_1;
        if (next_x >= wall_2)
            next_x = wall_2;

            v_x = -v_x * 0.9;
        }

        // 3. 执行移动
        window.move(next_x, next_y);
    });

    timer.start(16);
    
    
    return a.exec();

}