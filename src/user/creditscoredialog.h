#ifndef CREDITSCOREDIALOG_H
#define CREDITSCOREDIALOG_H

// 1. 先包含所有必要头文件（顺序：基础控件 → Charts控件）
#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QPainter>
#include <QRandomGenerator>
#include <QMessageBox>

// 2. 包含Qt Charts头文件（完整路径）
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

    // 4. 类声明（无需修改）
class CreditScoreDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreditScoreDialog(QWidget *parent = nullptr, QString userId = "");

private slots:
    void onScoreDetailClicked();
    void onScoreHistoryClicked();

private:
    void setupUI();
    void loadScoreData(QString userId);
    void createScoreChart();

private:
    QString userId;
    QLabel *currentScoreLabel;
    QLabel *scoreLevelLabel;
    QLabel *rankingLabel;
    QProgressBar *scoreProgressBar;
    QChartView *scoreChartView; // 现在可以直接用QChartView，无需加QtCharts::
    QTableWidget *scoreDetailTable;
    QTableWidget *scoreHistoryTable;
    QPushButton *detailBtn;
    QPushButton *historyBtn;
    QPushButton *closeBtn;
};

#endif // CREDITSCOREDIALOG_H
