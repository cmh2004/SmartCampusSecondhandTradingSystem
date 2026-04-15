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
    connect(m_refreshBtn, &QPushButton::clicked, this, [this]() { loadReports(1); });
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { loadReports(1); });
    connect(m_processBtn, &QPushButton::clicked, this, &ReportsManagePage::onProcessReport);

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    m_prevBtn = new QPushButton("上一页");
    m_prevBtn->setObjectName("secondaryBtn");
    m_prevBtn->setFixedSize(80, 32);
    m_nextBtn = new QPushButton("下一页");
    m_nextBtn->setObjectName("secondaryBtn");
    m_nextBtn->setFixedSize(80, 32);
    m_pageInfoLabel = new QLabel("第 1 页");
    m_pageInfoLabel->setStyleSheet("font-size: 13px; color: #475569; margin: 0 15px;");

    paginationLayout->addWidget(m_prevBtn);
    paginationLayout->addWidget(m_pageInfoLabel);
    paginationLayout->addWidget(m_nextBtn);

    mainLayout->addWidget(paginationWidget);

    // 连接信号
    connect(m_prevBtn, &QPushButton::clicked, this, [this]() {
        if (m_currentPage > 1) {
            loadReports(m_currentPage - 1);
        }
    });
    connect(m_nextBtn, &QPushButton::clicked, this, [this]() {
        loadReports(m_currentPage + 1);
    });
}

void ReportsManagePage::loadReports(int page)
{
    m_currentPage = page;
    m_pageInfoLabel->setText(QString("第 %1 页").arg(page));

    QString status;
    int idx = m_statusFilter->currentIndex();
    if (idx == 1) status = "0";
    else if (idx == 2) status = "1";

    QJsonArray reports = ApiService::instance()->getAllReports(page, m_pageSize, status);
    updateTable(reports);

    bool hasMore = (reports.size() == m_pageSize);
    m_nextBtn->setEnabled(hasMore);
    m_prevBtn->setEnabled(page > 1);
}

void ReportsManagePage::updateTable(const QJsonArray &reports)
{
    m_table->setRowCount(0);
    for (const QJsonValue &val : reports) {
        QJsonObject obj = val.toObject();
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setRowHeight(row, 50);  // 增加行高，防止按钮被截断

        // 提取数据
        int reportId = obj.value("id").toInt();
        int reportedType = obj.value("reported_type").toInt();
        int reportedId = obj.value("reported_id").toInt();
        int reporterId = obj.value("reporter_id").toInt();
        QString reasonType = obj.value("reason_type").toString();
        int status = obj.value("status").toString().toInt(); // 0=待处理,1=已处理
        QString statusStr = (status == 0) ? "待处理" : "已处理";
        QString description = obj.value("description").toString();
        QString evidenceUrls = obj.value("evidence_urls").toString();

        // 设置各列
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(reportId)));
        m_table->setItem(row, 1, new QTableWidgetItem(
                                     (reportedType == 1) ? "商品" : (reportedType == 2) ? "用户" : "订单"));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::number(reportedId)));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(reporterId)));
        m_table->setItem(row, 4, new QTableWidgetItem(reasonType));
        m_table->setItem(row, 5, new QTableWidgetItem(statusStr));

        // ===== 操作列：添加“查看”和“处理”按钮 =====
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 1, 5, 1);
        actionLayout->setSpacing(8);

        // 查看按钮
        QPushButton *viewBtn = new QPushButton("查看");
        viewBtn->setObjectName("secondaryBtn");
        viewBtn->setFixedSize(60, 30);
        connect(viewBtn, &QPushButton::clicked, [this, reportId, reportedType, reportedId, reporterId, reasonType, description, evidenceUrls]() {
            QString detailText = QString(
                                     "📋 举报详情\n"
                                     "━━━━━━━━━━━━━━━━━━━━\n"
                                     "举报ID: %1\n"
                                     "举报类型: %2\n"
                                     "被举报ID: %3\n"
                                     "举报人ID: %4\n"
                                     "原因类型: %5\n"
                                     "详细描述:\n%6\n"
                                     "证据材料:\n%7")
                                     .arg(reportId)
                                     .arg((reportedType == 1) ? "商品" : (reportedType == 2) ? "用户" : "订单")
                                     .arg(reportedId)
                                     .arg(reporterId)
                                     .arg(reasonType)
                                     .arg(description.isEmpty() ? "无" : description)
                                     .arg(evidenceUrls.isEmpty() ? "无" : evidenceUrls);
            QMessageBox::information(this, "举报详情", detailText);
        });

        // 处理按钮
        QPushButton *processBtn = new QPushButton(status == 0 ? "处理" : "已处理");
        processBtn->setObjectName(status == 0 ? "primaryBtn" : "secondaryBtn");
        processBtn->setFixedSize(70, 30);
        if (status != 0) {
            processBtn->setEnabled(false);
        }
        connect(processBtn, &QPushButton::clicked, [this, reportId, status]() {
            if (status != 0) {
                QMessageBox::information(this, "提示", "该举报已处理");
                return;
            }
            bool ok;
            QString result = QInputDialog::getMultiLineText(this, "处理举报",
                                                            "请输入处理结果（将通知举报人）:",
                                                            "", &ok);
            if (!ok || result.trimmed().isEmpty()) return;

            if (ApiService::instance()->processReport(reportId, result)) {
                QMessageBox::information(this, "成功", "举报已处理");
                loadReports(m_currentPage); // 刷新当前页
            } else {
                QMessageBox::warning(this, "失败", "处理失败，请重试");
            }
        });

        actionLayout->addWidget(viewBtn);
        actionLayout->addWidget(processBtn);
        actionLayout->addStretch();
        m_table->setCellWidget(row, 6, actionWidget);
    }

    // 设置操作列宽度，确保两个按钮完整显示
    m_table->setColumnWidth(6, 150);
}

void ReportsManagePage::onProcessReport()
{
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选中一条举报记录");
        return;
    }

    int reportId = m_table->item(row, 0)->text().toInt();
    // 获取当前状态，防止重复处理
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
