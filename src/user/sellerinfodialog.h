#ifndef SELLERINFODIALOG_H
#define SELLERINFODIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTabWidget>

class SellerInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SellerInfoDialog(int sellerId, const QString &sellerName, QWidget *parent = nullptr);

private:
    void loadSellerInfo();
    void loadReviews();

    int m_sellerId;
    QString m_sellerName;
    QLabel *m_nameLabel;
    QLabel *m_creditLabel;
    QLabel *m_schoolLabel;
    QLabel *m_phoneLabel;
    QTableWidget *m_reviewTable;
};

#endif // SELLERINFODIALOG_H
