#include <QHeaderView>
#include <QDateTime>
#include <QTableWidgetItem>
#include "creditscoredialog.h"

CreditScoreDialog::CreditScoreDialog(QWidget *parent, QString userId)
    : QDialog(parent), userId(userId) {
    setWindowTitle("信用分详情");
    this->setMinimumSize(800, 600);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    loadScoreData(userId);
}

void CreditScoreDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *overviewGroup = new QGroupBox("信用分概览");
    QGridLayout *overviewLayout = new QGridLayout();

    QLabel *currentScoreTitle = new QLabel("当前信用分:");
    currentScoreTitle->setStyleSheet("font-size: 14px; color: #666;");

    currentScoreLabel = new QLabel("96");
    currentScoreLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #27ae60;");

    QLabel *scoreLevelTitle = new QLabel("信用等级:");
    scoreLevelTitle->setStyleSheet("font-size: 14px; color: #666;");

    scoreLevelLabel = new QLabel("优秀");
    scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3498db;");

    overviewLayout->addWidget(currentScoreTitle, 0, 0);
    overviewLayout->addWidget(currentScoreLabel, 0, 1);
    overviewLayout->addWidget(scoreLevelTitle, 1, 0);
    overviewLayout->addWidget(scoreLevelLabel, 1, 1);

    overviewGroup->setLayout(overviewLayout);
    mainLayout->addWidget(overviewGroup);

    // 分数明细和历史记录选项卡
    QTabWidget *detailTabs = new QTabWidget();

    // 分数明细表
    QWidget *detailTab = new QWidget();
    QVBoxLayout *detailLayout = new QVBoxLayout(detailTab);

    scoreDetailTable = new QTableWidget(0, 4);
    scoreDetailTable->setHorizontalHeaderLabels({"评分项目", "权重", "得分", "操作"});
    scoreDetailTable->horizontalHeader()->setStretchLastSection(true);
    scoreDetailTable->verticalHeader()->setVisible(false);

    // 示例数据
    QStringList detailItems = {
        "交易完成率", "30%", "30/30",
        "好评率", "25%", "25/25",
        "纠纷率", "20%", "18/20",
        "响应速度", "15%", "15/15",
        "活跃度", "10%", "8/10"
    };

    for (int i = 0; i < detailItems.size() / 3; i++) {
        int row = scoreDetailTable->rowCount();
        scoreDetailTable->insertRow(row);

        scoreDetailTable->setItem(row, 0, new QTableWidgetItem(detailItems[i*3]));
        scoreDetailTable->setItem(row, 1, new QTableWidgetItem(detailItems[i*3+1]));
        scoreDetailTable->setItem(row, 2, new QTableWidgetItem(detailItems[i*3+2]));

        QPushButton *viewBtn = new QPushButton("详情");
        viewBtn->setObjectName("secondaryBtn");
        viewBtn->setFixedHeight(25);
        scoreDetailTable->setCellWidget(row, 3, viewBtn);
        viewBtn->setStyleSheet("QPushButton#secondaryBtn { padding: 0; line-height: 25px; text-align: center; }");
        QString itemName = detailItems[i*3];
        connect(viewBtn, &QPushButton::clicked, this, [this, itemName]() {
            showScoreItemDetail(itemName); // 点击后调用详情展示函数
        });
    }
    scoreDetailTable->setColumnWidth(0, 150);  // 评分项目列
    scoreDetailTable->setColumnWidth(1, 80);   // 权重列
    scoreDetailTable->setColumnWidth(2, 80);   // 得分列
    scoreDetailTable->setColumnWidth(3, 80);   // 操作列（适配按钮宽度）

    detailLayout->addWidget(scoreDetailTable);
    detailTabs->addTab(detailTab, "分数明细");

    // 历史记录表
    QWidget *historyTab = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyTab);

    scoreHistoryTable = new QTableWidget(0, 4);
    scoreHistoryTable->setHorizontalHeaderLabels({"时间", "变更项目", "变更分值", "当前总分"});
    scoreHistoryTable->horizontalHeader()->setStretchLastSection(true);
    scoreHistoryTable->setColumnWidth(0,150);
    scoreHistoryTable->setColumnWidth(1,200);
    scoreHistoryTable->verticalHeader()->setVisible(false);

    // 示例历史数据
    QList<QStringList> historyData = {
        {"2024-03-20 10:30", "完成交易 + 好评", "+10", "96"},
        {"2024-03-18 14:20", "纠纷解决", "+5", "86"},
        {"2024-03-15 09:15", "按时发货", "+3", "81"},
        {"2024-03-10 16:45", "差评扣分", "-8", "78"}
    };

    for (const auto &data : historyData) {
        int row = scoreHistoryTable->rowCount();
        scoreHistoryTable->insertRow(row);

        for (int col = 0; col < data.size(); col++) {
            QTableWidgetItem *item = new QTableWidgetItem(data[col]);
            if (col == 2) {
                if (data[col].startsWith("+")) {
                    item->setForeground(QColor(46, 204, 113)); // 绿色
                } else if (data[col].startsWith("-")) {
                    item->setForeground(QColor(231, 76, 60)); // 红色
                }
            }
            scoreHistoryTable->setItem(row, col, item);
        }
    }

    historyLayout->addWidget(scoreHistoryTable);
    detailTabs->addTab(historyTab, "历史记录");

    mainLayout->addWidget(detailTabs, 1);

    // 底部按钮
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    detailBtn = new QPushButton("评分规则说明");
    historyBtn = new QPushButton("申诉记录");
    closeBtn = new QPushButton("关闭");

    detailBtn->setObjectName("secondaryBtn");
    historyBtn->setObjectName("secondaryBtn");
    closeBtn->setObjectName("primaryBtn");

    buttonLayout->addWidget(detailBtn);
    buttonLayout->addWidget(historyBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeBtn);

    mainLayout->addWidget(buttonWidget);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(detailBtn, &QPushButton::clicked, this, &CreditScoreDialog::onScoreDetailClicked);
    connect(historyBtn, &QPushButton::clicked, this, &CreditScoreDialog::onScoreHistoryClicked);

    // 设置样式
    setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #ddd;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        #primaryBtn {
            background-color: #3498db;
            color: white;
            border-radius: 4px;
            padding: 10px 20px;
        }
        #secondaryBtn {
            background-color: #ecf0f1;
            color: #34495e;
            border-radius: 4px;
            padding: 8px 16px;
        }
    )");
}

void CreditScoreDialog::loadScoreData(QString userId) {
    // 这里应该从数据库加载用户的信用分数据
    this->userId = userId;

    // 模拟数据
    int score = 96;
    currentScoreLabel->setText(QString::number(score));

    if (score >= 90) {
        scoreLevelLabel->setText("优秀");
        scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #27ae60;");
    } else if (score >= 75) {
        scoreLevelLabel->setText("良好");
        scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3498db;");
    } else if (score >= 60) {
        scoreLevelLabel->setText("中等");
        scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #f39c12;");
    } else {
        scoreLevelLabel->setText("较差");
        scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e74c3c;");
    }
}

void CreditScoreDialog::onScoreDetailClicked() {
    QMessageBox::information(this,
                             "评分规则说明",
                             "信用分计算规则：\n"
                             "1. 交易完成率（30%）：成功完成的交易比例\n"
                             "2. 好评率（25%）：收到的好评比例\n"
                             "3. 纠纷率（20%）：涉及纠纷的交易比例\n"
                             "4. 响应速度（15%）：回复消息的平均时间\n"
                             "5. 活跃度（10%）：近期活跃程度\n\n"
                             "信用等级划分：\n"
                             "• 优秀（800-900分）\n"
                             "• 良好（700-799分）\n"
                             "• 中等（600-699分）\n"
                             "• 较差（300-599分）");
}

void CreditScoreDialog::onScoreHistoryClicked() {
    QMessageBox::information(this, "申诉记录", "暂无申诉记录");
}

void CreditScoreDialog::showScoreItemDetail(const QString &itemName) {
    // 按评分项目展示对应的详细规则和得分依据
    QString detailContent;
    if (itemName == "交易完成率") {
        detailContent = QString("【%1】详细规则：\n").arg(itemName) +
                        "1. 权重占比：30%\n"
                        "2. 计算方式：已完成交易数 / 总发起交易数 × 30\n"
                        "3. 本次得分：30/30（完成率100%）\n"
                        "4. 扣分场景：交易发起后取消/未完成";
    } else if (itemName == "好评率") {
        detailContent = QString("【%1】详细规则：\n").arg(itemName) +
                        "1. 权重占比：25%\n"
                        "2. 计算方式：好评数 / 总评价数 × 25\n"
                        "3. 本次得分：25/25（好评率100%）\n"
                        "4. 扣分场景：收到中评/差评";
    } else if (itemName == "纠纷率") {
        detailContent = QString("【%1】详细规则：\n").arg(itemName) +
                        "1. 权重占比：20%\n"
                        "2. 计算方式：(1 - 纠纷数/交易数) × 20\n"
                        "3. 本次得分：18/20（纠纷率10%）\n"
                        "4. 扣分场景：被投诉";
    } else if (itemName == "响应速度") {
        detailContent = QString("【%1】详细规则：\n").arg(itemName) +
                        "1. 权重占比：15%\n"
                        "2. 计算方式：平均响应时间＜5小时得满分\n"
                        "3. 本次得分：15/15（响应及时）\n"
                        "4. 扣分场景：响应时间＞24小时";
    } else if (itemName == "活跃度") {
        detailContent = QString("【%1】详细规则：\n").arg(itemName) +
                        "1. 权重占比：10%\n"
                        "2. 计算方式：近30天登录/交易次数\n"
                        "3. 本次得分：8/10（活跃度一般）\n"
                        "4. 加分场景：每日登录/发布商品";
    }

    // 弹出详情窗口
    QMessageBox::information(this, QString("%1详情").arg(itemName), detailContent);
}
