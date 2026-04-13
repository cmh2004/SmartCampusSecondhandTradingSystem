#ifndef REPORTSMANAGEPAGE_H
#define REPORTSMANAGEPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>

class ReportsManagePage : public QWidget
{
    Q_OBJECT
public:
    explicit ReportsManagePage(QWidget *parent = nullptr);

private slots:
    void loadReports();
    void onProcessReport();

private:
    void setupUI();
    void updateTable(const QJsonArray &reports);

    QTableWidget *m_table;
    QComboBox *m_statusFilter;
    QPushButton *m_refreshBtn;
    QPushButton *m_processBtn;
};

#endif // REPORTSMANAGEPAGE_H
