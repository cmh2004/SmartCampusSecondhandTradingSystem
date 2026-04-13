#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonArray>
#include <QLabel>
#include <QInputDialog>
#include <QJsonObject>
#include "reportsmanagepage.h"
#include "..\apiservice.h"

ReportsManagePage::ReportsManagePage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadReports();
}

void ReportsManagePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 筛选栏
    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *statusLabel = new QLabel("状态:");
    m_statusFilter = new QComboBox();
    m_statusFilter->addItems({"全部", "待处理", "已处理"});
    m_refreshBtn = new QPushButton("刷新");
    filterLayout->addWidget(statusLabel);
    filterLayout->addWidget(m_statusFilter);
    filterLayout->addStretch();
    filterLayout->addWidget(m_refreshBtn);

    // 表格
    m_table = new QTableWidget(0, 7);
    m_table->setHorizontalHeaderLabels({"ID", "举报类型", "被举报ID", "举报人", "原因", "状态", "操作"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setEditTriggers(QTableWidget::NoEditTriggers);
    m_table->setSelectionBehavior(QTableWidget::SelectRows);
    m_table->verticalHeader()->setVisible(false);

    m_processBtn = new QPushButton("处理选中");
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_processBtn);

    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(m_table);
    mainLayout->addLayout(btnLayout);

    // 连接信号
    connect(m_refreshBtn, &QPushButton::clicked, this, &ReportsManagePage::loadReports);
    connect(m_processBtn, &QPushButton::clicked, this, &ReportsManagePage::onProcessReport);
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ReportsManagePage::loadReports);
}

void ReportsManagePage::loadReports()
{
    QString status;
    int idx = m_statusFilter->currentIndex();
    if (idx == 1) status = "0";      // 待处理
    else if (idx == 2) status = "1"; // 已处理

    QJsonArray reports = ApiService::instance()->getAllReports(1, 50, status);
    qDebug()<<reports;
    updateTable(reports);
}

void ReportsManagePage::updateTable(const QJsonArray &reports)
{
    m_table->setRowCount(0);
    for (const QJsonValue &val : reports) {
        QJsonObject obj = val.toObject();
        int row = m_table->rowCount();
        m_table->insertRow(row);

        // ID
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(obj.value("id").toInt())));
        // 举报类型 (reported_type: 1=商品,2=用户,3=订单)
        int type = obj.value("reported_type").toInt();
        QString typeStr = (type == 1) ? "商品" : (type == 2) ? "用户" : "订单";
        m_table->setItem(row, 1, new QTableWidgetItem(typeStr));
        // 被举报ID
        m_table->setItem(row, 2, new QTableWidgetItem(QString::number(obj.value("reported_id").toInt())));
        // 举报人ID
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(obj.value("reporter_id").toInt())));
        // 原因
        m_table->setItem(row, 4, new QTableWidgetItem(obj.value("reason_type").toString()));
        // 状态
        int status = obj.value("status").toString().toInt();
        QString statusStr = (status == 0) ? "待处理" : "已处理";
        m_table->setItem(row, 5, new QTableWidgetItem(statusStr));
        // 操作按钮占位
        m_table->setItem(row, 6, new QTableWidgetItem("..."));
    }
}

void ReportsManagePage::onProcessReport()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选中一条举报记录");
        return;
    }

    int reportId = m_table->item(row, 0)->text().toInt();  // 假设第0列是ID
    // 可选：获取当前状态，防止重复处理
    QString status = m_table->item(row, 5)->text();
    if (status == "已处理") {
        QMessageBox::information(this, "提示", "该举报已被处理");
        return;
    }

    // 弹出处理对话框
    bool ok;
    QString result = QInputDialog::getMultiLineText(this, "处理举报",
                                                    "请输入处理结果（将通知举报人）:",
                                                    "", &ok);
    if (!ok || result.trimmed().isEmpty()) {
        return;
    }

    // 调用 API 更新状态
    if (ApiService::instance()->processReport(reportId, result)) {
        QMessageBox::information(this, "成功", "举报已处理");
        loadReports();  // 刷新列表
    } else {
        QMessageBox::warning(this, "失败", "处理失败，请重试");
    }
}
