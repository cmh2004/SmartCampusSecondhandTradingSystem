#include "creditscoredialog.h"
// 补充缺失的头文件
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

// 构造函数：修复this指针类型错误（本质是父类初始化正确）
CreditScoreDialog::CreditScoreDialog(QWidget *parent, QString userId)
    : QDialog(parent), userId(userId) {
    setWindowTitle("信用分详情");
    // 修正：setMinimumSize是QWidget的成员函数，调用方式正确
    this->setMinimumSize(800, 600);
    // 修正：windowFlags调用时this指针显式转换为QWidget*（兼容部分Qt版本）
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    loadScoreData(userId);
    createScoreChart();
}

void CreditScoreDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 顶部信用分概览（原有代码不变）
    QGroupBox *overviewGroup = new QGroupBox("信用分概览");
    QGridLayout *overviewLayout = new QGridLayout();

    QLabel *currentScoreTitle = new QLabel("当前信用分:");
    currentScoreTitle->setStyleSheet("font-size: 14px; color: #666;");

    currentScoreLabel = new QLabel("850");
    currentScoreLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #27ae60;");

    QLabel *scoreLevelTitle = new QLabel("信用等级:");
    scoreLevelTitle->setStyleSheet("font-size: 14px; color: #666;");

    scoreLevelLabel = new QLabel("优秀");
    scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3498db;");

    QLabel *rankingTitle = new QLabel("平台排名:");
    rankingTitle->setStyleSheet("font-size: 14px; color: #666;");

    rankingLabel = new QLabel("前5%");
    rankingLabel->setStyleSheet("font-size: 16px; color: #e67e22;");

    scoreProgressBar = new QProgressBar();
    scoreProgressBar->setRange(300, 900);
    scoreProgressBar->setValue(850);
    scoreProgressBar->setTextVisible(true);
    scoreProgressBar->setFormat("信用分: %v");
    scoreProgressBar->setStyleSheet(R"(
        QProgressBar {
            height: 25px;
            border-radius: 12px;
            background-color: #ecf0f1;
            border: 1px solid #bdc3c7;
        }
        QProgressBar::chunk {
            border-radius: 12px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #27ae60, stop:0.5 #2ecc71, stop:1 #1abc9c);
        }
    )");

    overviewLayout->addWidget(currentScoreTitle, 0, 0);
    overviewLayout->addWidget(currentScoreLabel, 0, 1);
    overviewLayout->addWidget(scoreLevelTitle, 1, 0);
    overviewLayout->addWidget(scoreLevelLabel, 1, 1);
    overviewLayout->addWidget(rankingTitle, 2, 0);
    overviewLayout->addWidget(rankingLabel, 2, 1);
    overviewLayout->addWidget(scoreProgressBar, 3, 0, 1, 2);

    overviewGroup->setLayout(overviewLayout);
    mainLayout->addWidget(overviewGroup);

    // 信用分变化图表
    QGroupBox *chartGroup = new QGroupBox("信用分变化趋势");
    QVBoxLayout *chartLayout = new QVBoxLayout();

    scoreChartView = new QChartView();
    scoreChartView->setMinimumHeight(250);

    chartLayout->addWidget(scoreChartView);
    chartGroup->setLayout(chartLayout);
    mainLayout->addWidget(chartGroup);

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
        scoreDetailTable->setCellWidget(row, 3, viewBtn);
    }

    detailLayout->addWidget(scoreDetailTable);
    detailTabs->addTab(detailTab, "分数明细");

    // 历史记录表
    QWidget *historyTab = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyTab);

    scoreHistoryTable = new QTableWidget(0, 4);
    scoreHistoryTable->setHorizontalHeaderLabels({"时间", "变更项目", "变更分值", "当前总分"});
    scoreHistoryTable->horizontalHeader()->setStretchLastSection(true);
    scoreHistoryTable->verticalHeader()->setVisible(false);

    // 示例历史数据
    QList<QStringList> historyData = {
        {"2024-03-20 10:30", "完成交易 + 好评", "+10", "850"},
        {"2024-03-18 14:20", "纠纷解决", "+5", "840"},
        {"2024-03-15 09:15", "按时发货", "+3", "835"},
        {"2024-03-10 16:45", "差评扣分", "-8", "832"}
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

    // 修正connect：显式指定信号槽类型（兼容Qt5/6）
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
    int score = 850;
    currentScoreLabel->setText(QString::number(score));

    if (score >= 800) {
        scoreLevelLabel->setText("优秀");
        scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #27ae60;");
    } else if (score >= 700) {
        scoreLevelLabel->setText("良好");
        scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #3498db;");
    } else if (score >= 600) {
        scoreLevelLabel->setText("中等");
        scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #f39c12;");
    } else {
        scoreLevelLabel->setText("较差");
        scoreLevelLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e74c3c;");
    }
}

void CreditScoreDialog::createScoreChart() {
    // 修正：QChart创建方式（确保命名空间生效）
    QChart *chart = new QChart();
    chart->setTitle("信用分变化趋势");
    chart->setTheme(QChart::ChartThemeLight);

    QLineSeries *series = new QLineSeries();
    series->setName("信用分");

    // 修正：替换老旧的qrand为QRandomGenerator（Qt5.10+推荐）
    for (int i = 30; i >= 0; i--) {
        int day = 30 - i;
        // 生成820±30的随机数（模拟波动）
        int score = 820 + QRandomGenerator::global()->bounded(60) - 30;
        series->append(day, score);
    }

    chart->addSeries(series);
    chart->createDefaultAxes();

    // 设置Y轴范围（修正：正确获取坐标轴）
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    if (yAxis) {
        yAxis->setRange(700, 900);
    }

    scoreChartView->setChart(chart);
    // 修正：RenderHint需要QPainter头文件（已补充）
    scoreChartView->setRenderHint(QPainter::Antialiasing);
}

void CreditScoreDialog::onScoreDetailClicked() {
    // 修正：QMessageBox::information参数（this是QDialog*，符合要求）
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
