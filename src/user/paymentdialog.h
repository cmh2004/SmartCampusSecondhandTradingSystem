#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>

class PaymentDialog : public QDialog {
    Q_OBJECT

public:
    explicit PaymentDialog(QWidget *parent = nullptr, int orderId = -1, double amount = 0.0);

private slots:
    void onConfirmPayment();
    void onPaymentMethodChanged();

private:
    void setupUI();

    int orderId;
    double amount;

    // 支付方式
    QRadioButton *wechatRadio;
    QRadioButton *alipayRadio;

    // 订单信息
    QLabel *orderIdLabel;
    QLabel *amountLabel;
    QLabel *goodsNameLabel;

    // 按钮
    QPushButton *confirmBtn;
    QPushButton *cancelBtn;
};

#endif // PAYMENTDIALOG_H
