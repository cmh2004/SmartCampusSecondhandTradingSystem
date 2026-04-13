#include <QHeaderView>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QProgressBar>
#include <QJsonObject>
#include "..\apiservice.h"
#include "creditscoredialog.h"

CreditScoreDialog::CreditScoreDialog(QWidget *parent, int userId)
    : QDialog(parent), userId(userId), m_animationProgress(0), m_targetScore(0) {
    setWindowTitle("信用分详情");
    this->setFixedSize(900, 900);  // 增加尺寸
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    // 设置窗口圆角和阴影
    setStyleSheet("QDialog { background-color: #f8fafc; }");

    setupUI();
    loadScoreData();
}

CreditScoreDialog::~CreditScoreDialog() {
    if (m_scoreAnimationTimer) {
        m_scoreAnimationTimer->stop();
        delete m_scoreAnimationTimer;
    }
}

void CreditScoreDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // ========== 顶部卡片：信用分概览 ==========
    QWidget *overviewCard = new QWidget();
    overviewCard->setObjectName("overviewCard");
    overviewCard->setMinimumHeight(175);

    // 添加阴影效果
    QGraphicsDropShadowEffect *cardShadow = new QGraphicsDropShadowEffect();
    cardShadow->setBlurRadius(20);
    cardShadow->setColor(QColor(0, 0, 0, 30));
    cardShadow->setOffset(0, 5);
    overviewCard->setGraphicsEffect(cardShadow);

    QHBoxLayout *overviewLayout = new QHBoxLayout(overviewCard);
    overviewLayout->setContentsMargins(20, 15, 20, 15);

    // 左侧：分数显示区域
    QWidget *scoreDisplayWidget = new QWidget();
    QVBoxLayout *scoreDisplayLayout = new QVBoxLayout(scoreDisplayWidget);
    scoreDisplayLayout->setContentsMargins(0, 0, 40, 0);
    scoreDisplayLayout->setSpacing(10);

    QLabel *scoreTitle = new QLabel("当前信用分");
    scoreTitle->setStyleSheet(R"(
        font-size: 15px;
        color: #64748b;
        font-weight: 500;
    )");

    currentScoreLabel = new QLabel("0");
    currentScoreLabel->setObjectName("currentScoreLabel");
    currentScoreLabel->setStyleSheet(R"(
        #currentScoreLabel {
            font-size: 56px;
            font-weight: bold;
            background: linear-gradient(135deg, #3b82f6, #8b5cf6);
        }
    )");

    QWidget *levelWidget = new QWidget();
    QHBoxLayout *levelLayout = new QHBoxLayout(levelWidget);
    levelLayout->setContentsMargins(0, 0, 0, 0);

    scoreLevelLabel = new QLabel("加载中...");
    scoreLevelLabel->setObjectName("scoreLevelLabel");
    scoreLevelLabel->setStyleSheet(R"(
        #scoreLevelLabel {
            font-size: 16px;
            font-weight: bold;
            color: #475569;
            padding: 5px 15px;
            border-radius: 20px;
            background-color: #f1f5f9;
        }
    )");

    // 分数趋势标签
    scoreTrendLabel = new QLabel("↑ 较上月提升5分");
    scoreTrendLabel->setStyleSheet(R"(
        font-size: 14px;
        color: #10b981;
        font-weight: 500;
    )");

    levelLayout->addWidget(scoreLevelLabel);
    levelLayout->addStretch();
    levelLayout->addWidget(scoreTrendLabel);

    scoreDisplayLayout->addWidget(scoreTitle);
    scoreDisplayLayout->addWidget(currentScoreLabel);
    scoreDisplayLayout->addWidget(levelWidget);

    // 右侧：分数仪表盘
    scoreMeterWidget = new QWidget();
    scoreMeterWidget->setFixedSize(200, 150);

    // 最后更新时间
    lastUpdateLabel = new QLabel("最后更新: 2024-03-20 15:30");
    lastUpdateLabel->setStyleSheet(R"(
        font-size: 12px;
        color: #94a3b8;
        font-style: italic;
        margin-top: 10px;
    )");

    QVBoxLayout *meterLayout = new QVBoxLayout(scoreMeterWidget);
    meterLayout->addWidget(lastUpdateLabel);
    meterLayout->setAlignment(lastUpdateLabel, Qt::AlignHCenter | Qt::AlignBottom);

    overviewLayout->addWidget(scoreDisplayWidget);
    overviewLayout->addStretch();
    overviewLayout->addWidget(scoreMeterWidget);

    mainLayout->addWidget(overviewCard);

    // ========== 中间：分数构成进度条 ==========
    progressBarsWidget = new QWidget();
    progressBarsWidget->setObjectName("progressBarsWidget");
    progressBarsWidget->setMinimumHeight(150);

    QGraphicsDropShadowEffect *barsShadow = new QGraphicsDropShadowEffect();
    barsShadow->setBlurRadius(15);
    barsShadow->setColor(QColor(0, 0, 0, 20));
    barsShadow->setOffset(0, 3);
    progressBarsWidget->setGraphicsEffect(barsShadow);

    QVBoxLayout *barsLayout = new QVBoxLayout(progressBarsWidget);
    barsLayout->setContentsMargins(20, 15, 20, 15);

    QLabel *compositionTitle = new QLabel("分数构成分析");
    compositionTitle->setStyleSheet(R"(
        font-size: 18px;
        font-weight: bold;
        color: #1e293b;
        margin-bottom: 10px;
    )");
    barsLayout->addWidget(compositionTitle);

    // 分数构成进度条将在 loadScoreData 中创建
    barsLayout->addStretch();

    mainLayout->addWidget(progressBarsWidget);

    // ========== 底部：选项卡 ==========
    QTabWidget *detailTabs = new QTabWidget();
    detailTabs->setObjectName("detailTabs");
    detailTabs->tabBar()->setObjectName("customTabBar");

    // 历史记录表
    QWidget *historyTab = new QWidget();
    QVBoxLayout *historyLayout = new QVBoxLayout(historyTab);
    historyLayout->setContentsMargins(15, 15, 15, 15);

    scoreHistoryTable = new QTableWidget(0, 5);  // 增加一列用于图标
    scoreHistoryTable->setObjectName("scoreHistoryTable");
    scoreHistoryTable->setHorizontalHeaderLabels({"", "时间", "变更项目", "变更分值", "当前总分"});
    scoreHistoryTable->horizontalHeader()->setStretchLastSection(true);
    scoreHistoryTable->verticalHeader()->setVisible(false);
    scoreHistoryTable->setAlternatingRowColors(true);
    scoreHistoryTable->setSelectionBehavior(QTableWidget::SelectRows);
    scoreHistoryTable->setEditTriggers(QTableWidget::NoEditTriggers);
    scoreHistoryTable->setColumnWidth(0,100);
    scoreHistoryTable->setColumnWidth(1,200);
    scoreHistoryTable->setColumnWidth(2,200);
    scoreHistoryTable->setColumnWidth(3,120);
    scoreHistoryTable->horizontalHeader()->setStretchLastSection(true);

    historyLayout->addWidget(scoreHistoryTable);

    detailTabs->addTab(historyTab, "📈 历史记录");

    mainLayout->addWidget(detailTabs, 1);

    // ========== 底部按钮 ==========
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 10, 0, 0);

    detailBtn = new QPushButton( "评分规则说明");
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

    // 连接信号
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(detailBtn, &QPushButton::clicked, this, &CreditScoreDialog::onScoreDetailClicked);
    connect(historyBtn, &QPushButton::clicked, this, &CreditScoreDialog::onScoreHistoryClicked);

    // ========== 设置全局样式 ==========
    setStyleSheet(R"(
        /* 卡片样式 */
        #overviewCard, #progressBarsWidget {
            background-color: white;
            border-radius: 16px;
            border: 1px solid #e2e8f0;
        }

        /* 选项卡样式 */
        #detailTabs::pane {
            border: 1px solid #e2e8f0;
            border-radius: 12px;
            background-color: white;
            margin-top: 5px;
        }

        #customTabBar::tab {
            background-color: #f8fafc;
            color: #64748b;
            padding: 12px 24px;
            margin-right: 5px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            font-size: 14px;
            font-weight: 500;
        }

        #customTabBar::tab:selected {
            background-color: white;
            color: #3b82f6;
            border-bottom: 3px solid #3b82f6;
        }

        #customTabBar::tab:hover {
            background-color: #f1f5f9;
            color: #475569;
        }

        /* 表格样式 */
        #scoreHistoryTable {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            background-color: white;
            gridline-color: #f1f5f9;
            outline: none;
        }

        #scoreHistoryTable::item {
            padding: 12px 15px;
            border-bottom: 1px solid #f1f5f9;
            outline: none;
        }

        #scoreHistoryTable::item:selected {
            background-color: #e3f2fd;
            color: #1976d2;
            border-radius: 4px;
            outline: none;
        }

        QTableWidget {
            alternate-background-color: #f9fafb;
        }

        /* 按钮样式 */
        #primaryBtn {
            background-color: #3b82f6;
            color: white;
            border-radius: 8px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: 600;
            border: none;
            min-height: 26px;
        }

        #primaryBtn:hover {
            background-color: #2563eb;
        }

        #primaryBtn:pressed {
            background-color: #1d4ed8;
        }

        #secondaryBtn {
            background-color: #f1f5f9;
            color: #475569;
            border-radius: 8px;
            padding: 6px 14px;
            font-size: 14px;
            font-weight: 500;
            border: 1px solid #e2e8f0;
            min-height: 24px;
        }

        #secondaryBtn:hover {
            background-color: #e2e8f0;
            color: #334155;
        }

        #secondaryBtn:pressed {
            background-color: #cbd5e1;
        }

        /* 进度条样式 */
        QProgressBar {
            border: none;
            border-radius: 6px;
            background-color: #f1f5f9;
            text-align: center;
            color: #475569;
            font-size: 12px;
            height: 24px;
        }

        QProgressBar::chunk {
            border-radius: 6px;
        }
    )");
}

void CreditScoreDialog::createScoreMeter() {
    // 创建圆形分数仪表盘
    // 这个函数会在 loadScoreData 中调用
}

void CreditScoreDialog::createProgressBars() {
    QVBoxLayout *barsLayout = qobject_cast<QVBoxLayout*>(progressBarsWidget->layout());
    if (!barsLayout) return;

    // 清空现有内容（除了标题）
    while (barsLayout->count() > 1) {
        QLayoutItem* item = barsLayout->takeAt(1);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    barsLayout->setSpacing(0);

    // 分数构成数据
    QList<QPair<QString, QPair<int, int>>> scoreComponents = {
        {"交易完成率", {30, 30}},
        {"好评率", {25, 25}},
        {"纠纷率", {18, 20}},
        {"响应速度", {15, 15}},
        {"活跃度", {8, 10}}
    };

    QColor colors[] = {
        QColor("#3b82f6"),  // 蓝色
        QColor("#10b981"),  // 绿色
        QColor("#f59e0b"),  // 橙色
        QColor("#8b5cf6"),  // 紫色
        QColor("#ef4444")   // 红色
    };

    for (int i = 0; i < scoreComponents.size(); ++i) {
        const auto &component = scoreComponents[i];
        QString name = component.first;
        int current = component.second.first;
        int total = component.second.second;

        QWidget *progressWidget = new QWidget();
        QHBoxLayout *progressLayout = new QHBoxLayout(progressWidget);
        progressLayout->setContentsMargins(0, 8, 0, 8);

        // 左侧：项目名称和权重
        QWidget *infoWidget = new QWidget();
        QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoLayout->setSpacing(2);

        QLabel *nameLabel = new QLabel(name);
        nameLabel->setStyleSheet(R"(
            font-size: 14px;
            font-weight: 600;
            color: #1e293b;
        )");

        QLabel *weightLabel = new QLabel(QString("权重: %1%").arg(total));
        weightLabel->setStyleSheet(R"(
            font-size: 12px;
            color: #64748b;
        )");

        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(weightLabel);

        // 中间：进度条
        QProgressBar *progressBar = new QProgressBar();
        progressBar->setRange(0, total);
        progressBar->setValue(current);
        progressBar->setFormat(QString("%1/%2").arg(current).arg(total));
        progressBar->setStyleSheet(QString(R"(
            QProgressBar {
                border: none;
                border-radius: 6px;
                background-color: #f1f5f9;
                text-align: center;
                color: #475569;
                font-size: 12px;
                height: 24px;
            }
            QProgressBar::chunk {
                border-radius: 6px;
                background-color: %1;
            }
        )").arg(colors[i].name()));

        // 右侧：得分和百分比
        QWidget *scoreWidget = new QWidget();
        QVBoxLayout *scoreLayout = new QVBoxLayout(scoreWidget);
        scoreLayout->setContentsMargins(0, 0, 0, 0);
        scoreLayout->setSpacing(2);

        QLabel *scoreLabel = new QLabel(QString("%1分").arg(current));
        scoreLabel->setStyleSheet(QString(R"(
            font-size: 16px;
            font-weight: bold;
            color: %1;
        )").arg(colors[i].name()));

        int percentage = total > 0 ? (current * 100 / total) : 0;
        QLabel *percentageLabel = new QLabel(QString("%1%").arg(percentage));
        percentageLabel->setStyleSheet(R"(
            font-size: 12px;
            color: #64748b;
        )");

        scoreLayout->addWidget(scoreLabel);
        scoreLayout->addWidget(percentageLabel);

        // 组装所有部件
        progressLayout->addWidget(infoWidget, 1);
        progressLayout->addWidget(progressBar, 3);
        progressLayout->addWidget(scoreWidget, 1);

        barsLayout->addWidget(progressWidget);
    }
}

void CreditScoreDialog::loadScoreData() {
    // 获取信用分
    QJsonObject scoreResult = ApiService::instance()->getCreditScore(userId);
    if (!scoreResult.value("success").toBool()) {
        QMessageBox::warning(this, "加载失败", "获取信用分失败：" + scoreResult.value("error").toString());
        return;
    }
    int score = scoreResult.value("data").toObject().value("credit_score").toInt();
    m_targetScore = score;

    // 创建分数动画
    m_scoreAnimationTimer = new QTimer(this);
    connect(m_scoreAnimationTimer, &QTimer::timeout, this, &CreditScoreDialog::updateScoreAnimation);
    m_scoreAnimationTimer->start(30);

    // 设置信用等级
    QString level, levelColor, levelBg;
    if (score >= 90) {
        level = "优秀";
        levelColor = "#10b981";
        levelBg = "#d1fae5";
    } else if (score >= 75) {
        level = "良好";
        levelColor = "#3b82f6";
        levelBg = "#dbeafe";
    } else if (score >= 60) {
        level = "中等";
        levelColor = "#f59e0b";
        levelBg = "#fef3c7";
    } else {
        level = "待提升";
        levelColor = "#ef4444";
        levelBg = "#fee2e2";
    }

    scoreLevelLabel->setText(level);
    scoreLevelLabel->setStyleSheet(QString(R"(
        #scoreLevelLabel {
            font-size: 17px;
            font-weight: bold;
            color: %1;
            padding: 7px 20px;
            border-radius: 20px;
            background-color: %2;
            border: 1.5px solid %1;
        }
    )").arg(levelColor).arg(levelBg));

    // 创建进度条展示
    createProgressBars();

    // 填充历史记录表格
    // 获取历史记录
    QJsonArray history = ApiService::instance()->getCreditHistory(userId, 1, 20);
    scoreHistoryTable->setRowCount(0);
    scoreHistoryTable->setRowHeight(0, 50);

    for (const QJsonValue &val : history) {
        QJsonObject record = val.toObject();
        int row = scoreHistoryTable->rowCount();
        scoreHistoryTable->insertRow(row);
        scoreHistoryTable->setRowHeight(row, 50);

        // 第0列：图标（根据变更值正负显示）
        int changeValue = record.value("change_value").toInt();
        QLabel *iconLabel = new QLabel();
        if (changeValue > 0) {
            iconLabel->setPixmap(QPixmap(":/icons/img/up.png").scaled(24, 24));
        } else if (changeValue < 0) {
            iconLabel->setPixmap(QPixmap(":/icons/img/down.png").scaled(24, 24));
        } else {
            iconLabel->setText(""); // 无变化
        }
        iconLabel->setAlignment(Qt::AlignCenter);
        scoreHistoryTable->setCellWidget(row, 0, iconLabel);

        // 第1列：时间
        QString timeStr = record.value("create_time").toString();
        QTableWidgetItem *timeItem = new QTableWidgetItem(timeStr);
        scoreHistoryTable->setItem(row, 1, timeItem);

        // 第2列：变更项目（原因）
        QString reason = record.value("reason").toString();
        QTableWidgetItem *reasonItem = new QTableWidgetItem(reason);
        scoreHistoryTable->setItem(row, 2, reasonItem);

        // 第3列：变更分值
        QString changeStr = (changeValue > 0 ? "+" : "") + QString::number(changeValue);
        QTableWidgetItem *changeItem = new QTableWidgetItem(changeStr);
        changeItem->setTextAlignment(Qt::AlignCenter);
        if (changeValue > 0) {
            changeItem->setForeground(QColor("#10b981"));
            changeItem->setFont(QFont("", 12, QFont::Bold));
        } else if (changeValue < 0) {
            changeItem->setForeground(QColor("#ef4444"));
            changeItem->setFont(QFont("", 12, QFont::Bold));
        }
        scoreHistoryTable->setItem(row, 3, changeItem);

        // 第4列：当前总分
        int currentScore = record.value("current_score").toInt();
        QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(currentScore));
        totalItem->setTextAlignment(Qt::AlignCenter);
        totalItem->setFont(QFont("", 12, QFont::Bold));
        totalItem->setForeground(QColor("#3b82f6"));
        scoreHistoryTable->setItem(row, 4, totalItem);
    }
}

void CreditScoreDialog::updateScoreAnimation() {
    static int currentScore = 0;
    if (currentScore < m_targetScore) {
        currentScore += (m_targetScore - currentScore) / 10 + 1;
        if (currentScore > m_targetScore) currentScore = m_targetScore;
        currentScoreLabel->setText(QString::number(currentScore));
    } else {
        m_scoreAnimationTimer->stop();
    }
}

void CreditScoreDialog::setAnimationProgress(int progress) {
    m_animationProgress = progress;
    update();
}

void CreditScoreDialog::onScoreDetailClicked() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("📚 评分规则说明");

    QString details =
        "✨ 信用分计算规则 ✨\n\n"
        "信用分 = 各项指标加权总分（满分100分）\n\n"
        "📊 各项指标说明：\n"
        "• 交易完成率（30分）：成功完成的交易比例\n"
        "• 好评率（25分）：收到的好评占总评价的比例\n"
        "• 纠纷率（20分）：涉及纠纷的交易比例\n"
        "• 响应速度（15分）：回复消息的平均时间\n"
        "• 活跃度（10分）：近期登录和交易频率\n\n"
        "🏆 信用等级划分：\n"
        "• 优秀（90-100分）💎\n"
        "• 良好（75-89分）⭐\n"
        "• 中等（60-74分）📈\n"
        "• 待提升（0-59分）📉\n\n"
        "🔔 温馨提示：\n"
        "保持良好交易习惯，信用分会逐步提升！";

    msgBox.setText(details);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setStyleSheet(R"(
        QMessageBox {
            background-color: white;
            border-radius: 12px;
        }
        QLabel {
            font-size: 14px;
            line-height: 1.6;
        }
    )");
    msgBox.exec();
}

void CreditScoreDialog::onScoreHistoryClicked() {
    QMessageBox::information(this, "📝 申诉记录",
                             "📋 您的申诉记录\n\n"
                             "暂无进行中的申诉\n\n"
                             "✅ 已处理申诉：2条\n"
                             "• 2024-02-15 申诉成功（误扣信用分已恢复）\n"
                             "• 2024-01-20 申诉成功（交易纠纷已解决）");
}

void CreditScoreDialog::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // 检查是否在顶部拖拽区域内点击
        QPoint dragPos = this->mapFromParent(event->pos());
        if (this->rect().contains(dragPos)) {
            m_dragging = true;
            m_dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
            return;
        }
    }
    QDialog::mousePressEvent(event);
}

void CreditScoreDialog::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragStartPosition);
        event->accept();
        return;
    }
    QDialog::mouseMoveEvent(event);
}

void CreditScoreDialog::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
    QDialog::mouseReleaseEvent(event);
}

void CreditScoreDialog::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制半透明背景
    painter.fillRect(rect(), QColor(115, 205, 235, 70));

    QDialog::paintEvent(event);
}
