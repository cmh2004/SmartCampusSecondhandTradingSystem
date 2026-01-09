#ifndef CREDITSCOREDIALOG_H
#define CREDITSCOREDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QPainter>
#include <QMessageBox>

class CreditScoreDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreditScoreDialog(QWidget *parent = nullptr, QString userId = "");

private slots:
    void onScoreDetailClicked();
    void onScoreHistoryClicked();
    void showScoreItemDetail(const QString &itemName);

private:
    void setupUI();
    void loadScoreData(QString userId);

private:
    QString userId;
    QLabel *currentScoreLabel;
    QLabel *scoreLevelLabel;
    QTableWidget *scoreDetailTable;
    QTableWidget *scoreHistoryTable;
    QPushButton *detailBtn;
    QPushButton *historyBtn;
    QPushButton *closeBtn;
};

#endif // CREDITSCOREDIALOG_H
