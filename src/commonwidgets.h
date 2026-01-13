#ifndef COMMONWIDGETS_H
#define COMMONWIDGETS_H

#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QMessageBox>
#include <QApplication>
#include <QAbstractButton>

// 自定义圆形头像标签
class AvatarLabel : public QLabel {
public:
    explicit AvatarLabel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

void showMessageBox(QWidget *parent, const QString &title, const QString &text, QMessageBox::Icon icon,const QString &buttonText = QString());

#endif // COMMONWIDGETS_H
