#include <QScreen>
#include <QTimer>
#include "commonwidgets.h"

AvatarLabel::AvatarLabel(QWidget *parent) : QLabel(parent) {
    setFixedSize(80, 80);
    setAlignment(Qt::AlignCenter);
}

void AvatarLabel::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制圆形背景
    QPainterPath path;
    path.addEllipse(rect());
    painter.setClipPath(path);

    // 使用渐变色背景
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(0, 150, 255));
    gradient.setColorAt(1, QColor(150, 100, 220));
    painter.fillRect(rect(), gradient);

    // 绘制默认头像图标
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 30, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, "S");

    // 绘制白色边框
    painter.setClipping(false);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(rect().adjusted(1, 1, -1, -1));
}

void showMessageBox(QWidget *parent, const QString &title, const QString &text, QMessageBox::Icon icon,const QString &buttonText) {
    // 1. 确定合适的父窗口
    QWidget *msgParent = parent;

    // 如果传入的父窗口不可用或设置了透明背景，使用顶级窗口
    if (!msgParent ||
        (msgParent->testAttribute(Qt::WA_TranslucentBackground) &&
         msgParent->windowFlags().testFlag(Qt::FramelessWindowHint))) {
        msgParent = QApplication::activeWindow();
    }

    // 2. 创建消息框
    QMessageBox msgBox(msgParent);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(icon);
    // 设置自定义按钮文本或使用默认
    if (!buttonText.isEmpty()) {
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.button(QMessageBox::Ok)->setText(buttonText);
    } else {
        msgBox.setStandardButtons(QMessageBox::Ok);
    }
    // 重置背景为白色，避免透明继承
    msgBox.setStyleSheet(R"(
        QMessageBox {
            background-color: white;
            border-radius: 8px;
        }
        QMessageBox QLabel {
            color: #2d3748;
            font-size: 15px;
        }
        QMessageBox QPushButton {
            background-color: #4299e1;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 8px 20px;
            font-size: 14px;
        }
        QMessageBox QPushButton:hover {
            background-color: #3182ce;
        }
    )");

    // 3. 执行消息框
    msgBox.exec();
}
