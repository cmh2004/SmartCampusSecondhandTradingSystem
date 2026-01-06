// reviewdialog.h
#ifndef REVIEWDIALOG_H
#define REVIEWDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QCheckBox>

class ReviewDialog : public QDialog {
    Q_OBJECT

public:
    explicit ReviewDialog(QWidget *parent = nullptr, int orderId = -1, QString sellerName = "");

signals:
    void reviewSubmitted(int orderId, int rating, QString comment);

private slots:
    void onSubmitReview();

private:
    void setupUI();

    int orderId;
    QString sellerName;

    // 评分
    QList<QRadioButton*> starButtons;

    // 评价内容
    QTextEdit *reviewEdit;

    // 标签
    QLineEdit *tagEdit;

    // 上传图片
    QPushButton *uploadImageBtn;
    QLabel *imagePreview;

    // 按钮
    QPushButton *submitBtn;
    QPushButton *cancelBtn;
};

#endif // REVIEWDIALOG_H
