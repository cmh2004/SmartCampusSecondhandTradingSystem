#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileInfo>
#include <QJsonArray>
#include <QLabel>
#include <QInputDialog>
#include <QTextEdit>
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

    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(m_table);

    // 连接信号
    connect(m_refreshBtn, &QPushButton::clicked, this, [this]() { loadReports(1); });
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { loadReports(1); });

    // 分页控件
    QWidget *paginationWidget = new QWidget();
    QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
    paginationLayout->setContentsMargins(0, 10, 0, 0);
    paginationLayout->setAlignment(Qt::AlignCenter);

    m_prevBtn = new QPushButton("上一页");
    m_prevBtn->setObjectName("primaryBtn");
    m_prevBtn->setStyleSheet("QPushButton:disabled { background-color: #cbd5e0; color: #a0aec0; }");
    m_prevBtn->setFixedSize(80, 32);
    m_nextBtn = new QPushButton("下一页");
    m_nextBtn->setObjectName("primaryBtn");
    m_nextBtn->setStyleSheet(m_prevBtn->styleSheet());
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

    QJsonObject params;
    if (!status.isEmpty()) params["status"] = status;
    params["page"] = page;
    params["page_size"] = m_pageSize;

    QJsonObject response = HttpClient::instance()->syncRequest("/api/admin/reports", params, "POST");
    if (!response.value("success").toBool()) {
        QMessageBox::warning(this, "错误", response.value("error").toString());
        return;
    }

    QJsonObject data = response.value("data").toObject();
    QJsonArray reports = data.value("list").toArray();
    int total = data.value("total").toInt();
    int totalPages = (total + m_pageSize - 1) / m_pageSize;

    updateTable(reports);
    m_prevBtn->setEnabled(page > 1);
    m_nextBtn->setEnabled(page < totalPages);
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
        // 从返回的 JSON 中获取昵称字段
        QString reporterName = obj.value("reporter_name").toString();
        QString reportedName = obj.value("reported_name").toString();

        // 显示昵称，如果昵称不存在则回退显示 ID（兼容旧数据）
        m_table->setItem(row, 2, new QTableWidgetItem(reportedName.isEmpty() ? QString::number(reportedId) : reportedName));
        m_table->setItem(row, 3, new QTableWidgetItem(reporterName.isEmpty() ? QString::number(reporterId) : reporterName));
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
            showReportDetailDialog(reportId, reportedType, reportedId, reporterId, reasonType, description, evidenceUrls);
        });

        if (status == 0) {
            QPushButton *processBtn = new QPushButton("处理");
            processBtn->setObjectName("primaryBtn");
            processBtn->setFixedSize(70, 30);
            connect(processBtn, &QPushButton::clicked, [this, reportId]() {
                showProcessDialog(reportId);
            });
            actionLayout->addWidget(processBtn);
        }

        actionLayout->addWidget(viewBtn);
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
    ApiService::instance()->processReport(reportId, true ,result);
    loadReports();  // 刷新列表
}

void ReportsManagePage::showProcessDialog(int reportId)
{
    QDialog dialog(this);
    dialog.setWindowTitle("处理举报");
    dialog.setMinimumSize(450, 350);
    dialog.setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // 是否属实
    QLabel *truthLabel = new QLabel("处理结果:");
    QComboBox *truthCombo = new QComboBox();
    truthCombo->addItems({"属实（举报成功）", "不实（驳回）"});
    layout->addWidget(truthLabel);
    layout->addWidget(truthCombo);

    // 处理结果说明
    QLabel *resultLabel = new QLabel("处理说明:");
    QTextEdit *resultEdit = new QTextEdit();
    resultEdit->setPlaceholderText("请填写处理结果说明（将通知举报人）");
    resultEdit->setMaximumHeight(100);
    layout->addWidget(resultLabel);
    layout->addWidget(resultEdit);

    // 按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *okBtn = new QPushButton("确定");
    QPushButton *cancelBtn = new QPushButton("取消");
    okBtn->setObjectName("primaryBtn");
    cancelBtn->setObjectName("secondaryBtn");
    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(okBtn);
    layout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, [&]() {
        QString handleResult = resultEdit->toPlainText().trimmed();
        if (handleResult.isEmpty()) {
            QMessageBox::warning(&dialog, "提示", "请填写处理说明");
            return;
        }
        bool isTrue = (truthCombo->currentIndex() == 0); // 0=属实
        QJsonObject response = ApiService::instance()->processReport(reportId, isTrue, handleResult);
        if (response.value("success").toBool()) {
            QMessageBox::information(&dialog, "成功", "举报已处理");
            dialog.accept();
            loadReports(m_currentPage);
        } else {
            QMessageBox::warning(&dialog, "失败", response.value("error").toString());
        }
    });
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}

void ReportsManagePage::showReportDetailDialog(int reportId, int reportedType, int reportedId,
                                               int reporterId, const QString& reasonType,
                                               const QString& description, const QString& evidenceUrls)
{
    QDialog dialog(this);
    dialog.setWindowTitle("举报详情");
    dialog.setMinimumSize(500, 600);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // 文本信息
    QTextEdit *infoEdit = new QTextEdit();
    infoEdit->setReadOnly(true);
    QString typeStr = (reportedType == 1) ? "商品" : (reportedType == 2) ? "用户" : "订单";
    QString info = QString("举报ID: %1\n举报类型: %2\n被举报ID: %3\n举报人ID: %4\n原因类型: %5\n详细描述:\n%6")
                       .arg(reportId).arg(typeStr).arg(reportedId).arg(reporterId)
                       .arg(reasonType).arg(description.isEmpty() ? "无" : description);
    infoEdit->setPlainText(info);
    layout->addWidget(infoEdit);

    // 证据图片区域
    if (!evidenceUrls.isEmpty()) {
        QLabel *evidenceLabel = new QLabel("证据图片：");
        layout->addWidget(evidenceLabel);

        QTabWidget *tabWidget = new QTabWidget();
        QStringList urls = evidenceUrls.split(',', Qt::SkipEmptyParts);
        for (const QString &url : urls) {
            QLabel *imageLabel = new QLabel();
            imageLabel->setAlignment(Qt::AlignCenter);
            imageLabel->setMinimumSize(200, 200);
            QString fullUrl = url.startsWith("http") ? url : "http://127.0.0.1:8080" + url;
            QNetworkAccessManager *nam = new QNetworkAccessManager(&dialog);
            connect(nam, &QNetworkAccessManager::finished, [imageLabel, nam](QNetworkReply *reply) {
                if (reply->error() == QNetworkReply::NoError) {
                    QPixmap pixmap;
                    pixmap.loadFromData(reply->readAll());
                    if (!pixmap.isNull()) {
                        imageLabel->setPixmap(pixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    } else {
                        imageLabel->setText("图片加载失败");
                    }
                } else {
                    imageLabel->setText("图片加载失败");
                }
                reply->deleteLater();
                nam->deleteLater();
            });
            nam->get(QNetworkRequest(QUrl(fullUrl)));
            tabWidget->addTab(imageLabel, QFileInfo(url).fileName());
        }
        layout->addWidget(tabWidget);
    } else {
        layout->addWidget(new QLabel("无证据图片"));
    }

    QPushButton *closeBtn = new QPushButton("关闭");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignCenter);

    dialog.exec();
}

void ReportsManagePage::refresh() {
    loadReports(m_currentPage);   // 使用当前页码重新加载
}
