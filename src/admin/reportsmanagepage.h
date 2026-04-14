#ifndef REPORTSMANAGEPAGE_H
#define REPORTSMANAGEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class ReportsManagePage : public QWidget
{
    Q_OBJECT
public:
    explicit ReportsManagePage(QWidget *parent = nullptr);

private slots:
    void loadReports(int page = 1);
    void onProcessReport();

private:
    void setupUI();
    void updateTable(const QJsonArray &reports);

    QTableWidget *m_table;
    QComboBox *m_statusFilter;
    QPushButton *m_refreshBtn;
    QPushButton *m_processBtn;

    int m_currentPage;
    int m_pageSize = 20;
    QPushButton *m_prevBtn;
    QPushButton *m_nextBtn;
    QLabel *m_pageInfoLabel;
};

#endif // REPORTSMANAGEPAGE_H
