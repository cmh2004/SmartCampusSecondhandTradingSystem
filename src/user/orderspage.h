#ifndef ORDERSPAGE_H
#define ORDERSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

class OrdersPage : public QWidget {
    Q_OBJECT

public:
    explicit OrdersPage(QWidget *parent = nullptr);
    void loadOrderExamples();

signals:
    void paymentRequested(int orderId, double amount);
    void reviewRequested(int orderId, const QString &sellerName);
    void disputeRequested(int orderId);
    void cancelOrderRequested(int orderId);
    void confirmReceiptRequested(int orderId);
    void exportOrdersRequested();

private slots:
    void onFilterOrders();
    void onRefreshOrders();
    void onCancelOrder(int orderId);
    void onConfirmReceipt(int orderId);
    void onShowDispute(int orderId);

private:
    void setupUI();
    void createActionButtons(int row, const QString &status, int orderId);
    void filterOrders(const QString &status, const QString &keyword);

    QTableWidget *ordersTable;
    QComboBox *statusCombo;
    QLineEdit *orderSearchEdit;
    QPushButton *filterBtn;
    QPushButton *refreshBtn;
    QPushButton *exportBtn;
};

#endif // ORDERSPAGE_H
