#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>
#include "MainWindow.h"
#include "HomePage.h"
#include "PublishPage.h"
#include "UserCenterPage.h"
#include "MessagesPage.h"
#include "OrdersPage.h"
#include "GoodsDetailDialog.h"
#include "ChatDialog.h"
#include "DisputeSubmitDialog.h"
#include "paymentdialog.h"
#include "reviewdialog.h"
#include "profileeditdialog.h"
#include "creditscoredialog.h"
#include "reportsubmitdialog.h"
#include "DisputeSubmitDialog.h"  // 售后纠纷对话框

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),isDragging(false) {
    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle("校园二手商品智能交易系统");
    setMinimumSize(1200, 800);
    setWindowIcon(QIcon(":/icons/img/app.png"));

    setupUI();
}

void MainWindow::setupUI() {
    // 创建主窗口部件
    mainWidget = new QWidget(this);
    mainWidget->setObjectName("mainContainer");
    mainWidget->setStyleSheet(R"(
        #mainContainer {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #d1d5db;
        }
    )");

    // 为整个容器添加阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(30);
    shadowEffect->setColor(QColor(0, 0, 0, 50));
    shadowEffect->setOffset(0, 8);
    mainWidget->setGraphicsEffect(shadowEffect);

    // 创建主布局
    QVBoxLayout *windowLayout = new QVBoxLayout(mainWidget);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    // 添加自定义标题栏
    setupCustomTitleBar();
    windowLayout->addWidget(customTitleBar);

    // 创建主标签页
    mainTabWidget = new QTabWidget(mainWidget);
    mainTabWidget->setTabPosition(QTabWidget::North);
    mainTabWidget->setTabShape(QTabWidget::Rounded);
    mainTabWidget->tabBar()->hide();

    // 创建各个页面
    homePage = new HomePage(this);
    publishPage = new PublishPage(this);
    messagesPage = new MessagesPage(this);
    ordersPage = new OrdersPage(this);
    userCenterPage = new UserCenterPage(this);

    // 连接页面信号
    connect(homePage, &HomePage::goodsDetailRequested, this, [this](int goodsId) {
        // 显示商品详情
        GoodsDetailDialog *detailDialog = new GoodsDetailDialog(this, goodsId);
        detailDialog->setAttribute(Qt::WA_DeleteOnClose);

        // 连接GoodsDetailDialog的所有信号
        connect(detailDialog, &GoodsDetailDialog::contactSellerRequested, this, [this](int goodsId) {
            // 这里应该根据商品ID获取卖家ID，暂时用模拟数据
            QString sellerId = QString("seller_%1").arg(goodsId);
            ChatDialog *chatDialog = new ChatDialog(this, goodsId, sellerId);
            chatDialog->setWindowTitle(QString("与卖家聊天 - 商品ID: %1").arg(goodsId));
            chatDialog->setAttribute(Qt::WA_DeleteOnClose);
            chatDialog->show();
        });

        connect(detailDialog, &GoodsDetailDialog::buyNowRequested, this, [this](int goodsId) {
            // 获取商品信息
            QString goodsName = QString("商品 #%1").arg(goodsId);

            QString message = QString("确认购买商品？\n\n"
                                      "商品: %1\n"
                                      "商品ID: %2\n\n"
                                      "购买后请及时联系卖家完成交易。")
                                  .arg(goodsName)
                                  .arg(goodsId);

            QMessageBox::StandardButton reply = QMessageBox::question(
                this, "确认购买", message,
                QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                QMessageBox::information(this, "购买成功",
                                         "订单已创建！\n\n"
                                         "订单信息:\n"
                                         "• 商品: " + goodsName + "\n"
                                                           "• 状态: 待支付\n\n"
                                                           "请及时联系卖家完成交易。");

                // 跳转到订单页面
                mainTabWidget->setCurrentIndex(3); // 订单页索引
            }
        });

        connect(detailDialog, &GoodsDetailDialog::makeOfferRequested, this, [this](int goodsId) {
            bool ok;
            QString offerPrice = QInputDialog::getText(this, "议价",
                                                       QString("请输入您的出价:"),
                                                       QLineEdit::Normal, "0", &ok);

            if (ok && !offerPrice.isEmpty()) {
                // 验证价格是否为数字
                bool isNumber;
                double price = offerPrice.toDouble(&isNumber);

                if (isNumber && price > 0) {
                    // 打开聊天窗口
                    ChatDialog *chatDialog = new ChatDialog(this, goodsId,
                                                            QString("seller_%1").arg(goodsId));
                    chatDialog->setAttribute(Qt::WA_DeleteOnClose);
                    chatDialog->show();

                    QMessageBox::information(this, "议价成功",
                                             QString("已向卖家发送议价: ¥%1").arg(offerPrice));
                } else {
                    QMessageBox::warning(this, "错误", "请输入有效的价格数字");
                }
            }
        });

        connect(detailDialog, &GoodsDetailDialog::reportGoodsRequested, this, &MainWindow::onReportGoods);

        detailDialog->show();
    });

    // 连接HomePage的其他信号
    connect(homePage, &HomePage::searchRequested, this, [this](const QString &keyword) {
        QMessageBox::information(this, "搜索", QString("搜索关键词: %1").arg(keyword));
        // 实际应该执行搜索逻辑
    });

    connect(homePage, &HomePage::categoryChanged, this, [this](const QString &category) {
        QMessageBox::information(this, "分类切换", QString("切换到分类: %1").arg(category));
        // 实际应该按分类过滤商品
    });

    connect(homePage, &HomePage::reportGoodsRequested, this, &MainWindow::onReportGoods);

    // 连接PublishPage信号
    connect(publishPage, &PublishPage::goodsPublished, this, [this](const QString &name, const QString &category,
                                                                    double price, const QString &description) {
        QMessageBox::information(this, "发布成功",
                                 QString("商品发布成功！\n"
                                         "名称: %1\n"
                                         "分类: %2\n"
                                         "价格: ¥%3\n"
                                         "描述: %4")
                                     .arg(name)
                                     .arg(category)
                                     .arg(price)
                                     .arg(description));

        // 在实际应用中，这里应该：
        // 1. 将商品信息保存到数据库
        // 2. 刷新首页的商品列表
        // 3. 可能跳转到首页
        mainTabWidget->setCurrentIndex(0); // 跳转到首页
        homePage->loadMockData(); // 刷新首页数据
    });

    // 连接MessagesPage信号
    connect(messagesPage, &MessagesPage::sendMessage, this, [this](const QString &message) {
        QMessageBox::information(this, "发送消息", QString("发送消息: %1").arg(message));
        // 实际应该发送消息到服务器
    });

    connect(messagesPage, &MessagesPage::chatSelected, this, [this](int chatId) {
        QMessageBox::information(this, "选择聊天", QString("选择聊天ID: %1").arg(chatId));
        // 实际应该加载聊天记录
    });

    connect(ordersPage, &OrdersPage::paymentRequested, this, &MainWindow::onShowPayment);
    connect(ordersPage, &OrdersPage::reviewRequested, this, &MainWindow::onShowReview);
    connect(ordersPage, &OrdersPage::disputeRequested, this, &MainWindow::onShowDisputeSubmit);
    connect(ordersPage, &OrdersPage::cancelOrderRequested, this, &MainWindow::onCancelOrder);
    connect(ordersPage, &OrdersPage::confirmReceiptRequested, this, &MainWindow::onConfirmReceipt);
    connect(ordersPage, &OrdersPage::exportOrdersRequested, this, [this]() {
        QMessageBox::information(this, "导出订单", "订单导出功能开发中...");
        // 实际应该导出订单数据
    });

    connect(userCenterPage, &UserCenterPage::editProfileRequested,
            this, &MainWindow::onShowProfileEdit);
    connect(userCenterPage, &UserCenterPage::creditScoreRequested,
            this, &MainWindow::onShowCreditScore);
    connect(userCenterPage, &UserCenterPage::menuTabChanged, this, [this](int index) {
        QMessageBox::information(this, "菜单切换", QString("切换到菜单项: %1").arg(index));
        // 实际应该更新界面显示
    });

    mainTabWidget->addTab(homePage, "");
    mainTabWidget->addTab(publishPage, "");
    mainTabWidget->addTab(messagesPage, "");
    mainTabWidget->addTab(ordersPage, "");
    mainTabWidget->addTab(userCenterPage, "");
    homePage->loadMockData();

    windowLayout->addWidget(mainTabWidget, 1);

    // 设置主窗口的中心部件
    setCentralWidget(mainWidget);
    setContentsMargins(1, 0, 1, 1);
    mainWidget->setContentsMargins(0, 0, 0, 0);

    // 设置状态栏
    QStatusBar *statusBar = this->statusBar();
    QLabel *statusLabel = new QLabel("欢迎使用校园二手交易系统");
    statusBar->addWidget(statusLabel);

    // 设置样式
    setStyleSheet(R"(
        QStatusBar {
            background-color: #f8f9fa;
            border-top: 1px solid #e2e8f0;
            border-bottom-left-radius: 12px;
            border-bottom-right-radius: 12px;
            padding: 5px;
        }
        QStatusBar::item {
            border: none;
        }
        /* 窗口控制按钮样式 */
        QPushButton#minimizeBtn, QPushButton#maximizeBtn, QPushButton#closeBtn {
            background-color: rgba(255, 255, 255, 0.2);
            color: white;
            border: none;
            border-radius: 4px;
            font-size: 14px;
            font-weight: bold;
            padding: 0;
            margin: 0;
        }
        QPushButton#minimizeBtn:hover {
            background-color: rgba(255, 255, 255, 0.3);
        }
        QPushButton#maximizeBtn:hover {
            background-color: rgba(255, 255, 255, 0.3);
        }
        QPushButton#closeBtn:hover {
            background-color: #e81123;
            color: white;
        }
        QTabWidget::pane {
            border: none;
            background-color: white;
            border-radius: 0 0 12px 12px;
        }
        QTableWidget {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 6px;
            gridline-color: #eee;
            alternate-background-color: #f9f9f9;
        }
        QTableWidget::item {
            padding: 8px;
        }
        QTableWidget::item:selected {
            background-color: #e3f2fd;
        }
        QListWidget {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 6px;
        }
        QListWidget::item {
            padding: 10px;
            border-bottom: 1px solid #eee;
        }
        QListWidget::item:selected {
            background-color: #e3f2fd;
            color: #1976d2;
        }
        QLineEdit, QTextEdit, QComboBox {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
            border-color: #3498db;
            outline: none;
        }
        QPushButton {
            padding: 8px 16px;
            border-radius: 4px;
            border: none;
            font-weight: bold;
        }
        QPushButton#primaryBtn {
            background-color: #3498db;
            color: white;
        }
        QPushButton#primaryBtn:hover {
            background-color: #2980b9;
        }
        QPushButton#secondaryBtn {
            background-color: #7f8c8d;
            color: white;
        }
        /* 首页样式 */
        #searchBar {
            background-color: white;
            border-radius: 10px;
            padding: 15px;
        }

        #searchEdit {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 0 15px;
            font-size: 14px;
            background-color: white;
        }

        #searchEdit:focus {
            border-color: #3b82f6;
            outline: none;
        }

        #searchEdit::placeholder {
            color: #94a3b8;
        }

        #sortCombo {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 0 10px;
            font-size: 14px;
            background-color: white;
        }

        #sortCombo:hover {
            border-color: #cbd5e1;
        }

        #sortCombo::drop-down {
            border: none;
            width: 20px;
        }

        #welcomeLabel {
            font-size: 22px;
            font-weight: 700;
            color: #1e293b;
            padding: 10px 5px;
            background: linear-gradient(135deg, #3b82f6 0%, #8b5cf6 100%);
        }

        /* 分类区域样式 */
        #categoryWidget {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
            padding: 15px;
        }

        #categoryTitle {
            font-size: 18px;
            font-weight: 600;
            color: #1e293b;
            padding-bottom: 10px;
            border-bottom: 2px solid #f1f5f9;
        }

        #categoryList {
            border: none;
            background-color: transparent;
            font-size: 14px;
            outline: none;
        }

        #categoryList::item {
            padding: 12px 10px;
            border-radius: 8px;
            margin: 2px 0;
            color: #475569;
            border-left: 3px solid transparent;
            outline: none;
        }

        #categoryList::item:hover {
            background-color: #f8fafc;
            color: #3b82f6;
            outline: none;
        }

        #categoryList::item:selected {
            background-color: #eff6ff;
            color: #1d4ed8;
            font-weight: 500;
            border-left: 3px solid #3b82f6;
            outline: none;
        }

        /* 商品区域样式 */
        #goodsWidget {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
            padding: 20px;
        }

        /* 商品表格样式 */
        #goodsTable {
            background-color: white;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            gridline-color: transparent;
            alternate-background-color: #f8fafc;
            font-size: 13px;
            outline: none;
        }

        #goodsTable::item {
            padding: 12px 8px;
            border-bottom: 1px solid #f1f5f9;
            outline: none;
        }

        #goodsTable::item:selected {
            background-color: #eff6ff;
            color: #1e293b;
            border-radius: 4px;
            outline: none;
        }

        QHeaderView::section {
            background-color: #f8fafc;
            padding: 14px 8px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
            font-weight: 600;
            color: #475569;
            font-size: 13px;
        }

        QHeaderView::section:first {
            border-top-left-radius: 8px;
        }

        QHeaderView::section:last {
            border-top-right-radius: 8px;
        }

        /* 滚动条美化 */
        QScrollBar:vertical {
            border: none;
            background: #f1f5f9;
            width: 8px;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical {
            background: #cbd5e1;
            border-radius: 4px;
            min-height: 20px;
        }

        QScrollBar::handle:vertical:hover {
            background: #94a3b8;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
            height: 0px;
        }
        QPushButton#warningBtn {
            background-color: #e74c3c;
            color: white;
            border-radius: 4px;
            padding: 6px 12px;
            font-size: 13px;
            border: none;
        }

        QPushButton#warningBtn:hover {
            background-color: #c0392b;
        }

        QPushButton#warningBtn:pressed {
            background-color: #a93226;
        }
        /* 订单页面样式 */
        QTableWidget QPushButton {
            padding: 5px 10px;
            border-radius: 3px;
            font-size: 12px;
            border: 1px solid transparent;
        }

        QTableWidget QPushButton:hover {
            opacity: 0.9;
        }

        QTableWidget QPushButton:pressed {
            opacity: 0.8;
        }

        QTableWidget {
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            background-color: white;
            gridline-color: #f1f5f9;
            outline: none;
        }

        QTableWidget::item {
            padding: 12px 8px;
            border-bottom: 1px solid #f1f5f9;
            outline: none;
        }

        QTableWidget::item:selected {
            background-color: #e3f2fd;
            color: #1976d2;
            border-radius: 4px;
            outline: none;
        }

        QHeaderView::section {
            background-color: #f8fafc;
            padding: 14px 8px;
            border: none;
            border-bottom: 2px solid #e2e8f0;
            font-weight: 600;
            color: #475569;
            font-size: 13px;
        }

        /* 订单状态标签样式 */
        #orderStatusLabel {
            padding: 3px 8px;
            border-radius: 10px;
            font-size: 12px;
            font-weight: bold;
        }
    )");

    connect(mainTabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::setupCustomTitleBar() {
    // 创建自定义标题栏
    customTitleBar = new QWidget();
    customTitleBar->setFixedHeight(60);  // 标题栏高度
    customTitleBar->setObjectName("titleBar");
    customTitleBar->setStyleSheet(R"(
        #titleBar {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                        stop:0 #1e90ff, stop:1 #00bbcf);
            border-top-left-radius: 0px;
            border-top-right-radius: 0px;
            border-bottom: 1px solid #e2e8f0;
        }
    )");

    QHBoxLayout *titleLayout = new QHBoxLayout(customTitleBar);
    titleLayout->setContentsMargins(20, 0, 6, 0);
    titleLayout->setSpacing(15);

    // 左侧：图标和标题
    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap(":/icons/img/logo.png").scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    titleLabel = new QLabel("校园二手商品智能交易系统");
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            font-size: 16px;
            font-weight: bold;
            padding-left: 10px;
        }
    )");

    // 中间：标签页按钮
    QWidget *tabButtonsWidget = new QWidget();
    QHBoxLayout *tabButtonsLayout = new QHBoxLayout(tabButtonsWidget);
    tabButtonsLayout->setContentsMargins(30, 0, 0, 0);
    tabButtonsLayout->setSpacing(5);

    // 创建标签按钮
    QStringList tabNames = {"首页", "发布商品", "消息", "我的订单", "个人中心"};
    QStringList tabIcons = {":/icons/img/home.png", ":/icons/img/publish.png",
                            ":/icons/img/message.png", ":/icons/img/order.png",
                            ":/icons/img/person.png"};

    QList<QPushButton*> tabButtons;
    for (int i = 0; i < tabNames.size(); ++i) {
        QPushButton *tabBtn = new QPushButton(tabNames[i]);
        tabBtn->setObjectName("tabButton");
        tabBtn->setCheckable(true);
        tabBtn->setIcon(QIcon(tabIcons[i]));
        tabBtn->setIconSize(QSize(20, 20));
        tabBtn->setFixedSize(110, 36);

        tabButtons.append(tabBtn);
        tabButtonsLayout->addWidget(tabBtn);
    }
    // 设置第一个按钮为选中状态
    if (!tabButtons.isEmpty()) {
        tabButtons[0]->setChecked(true);
    }

    // 连接标签页切换信号，更新按钮状态
    for (int i = 0; i < tabButtons.size(); ++i) {
        connect(tabButtons[i], &QPushButton::clicked, this, [this, i, tabButtons]() {
            // 先更新所有按钮状态
            for (int j = 0; j < tabButtons.size(); ++j) {
                tabButtons[j]->setChecked(j == i);
            }
            // 然后切换标签页
            mainTabWidget->setCurrentIndex(i);
        });
    }

    tabButtonsLayout->addStretch();

    // 右侧：窗口控制按钮
    QWidget *windowControls = new QWidget();
    QHBoxLayout *controlsLayout = new QHBoxLayout(windowControls);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(5);

    minimizeBtn = new QPushButton("－");
    minimizeBtn->setObjectName("minimizeBtn");
    minimizeBtn->setFixedSize(40, 40);
    minimizeBtn->setToolTip("最小化");

    maximizeBtn = new QPushButton("□");
    maximizeBtn->setObjectName("maximizeBtn");
    maximizeBtn->setFixedSize(40, 40);
    maximizeBtn->setToolTip("最大化");

    closeBtn = new QPushButton("×");
    closeBtn->setObjectName("closeBtn");
    closeBtn->setFixedSize(40, 40);
    closeBtn->setToolTip("关闭");

    controlsLayout->addWidget(minimizeBtn);
    controlsLayout->addWidget(maximizeBtn);
    controlsLayout->addWidget(closeBtn);

    // 连接窗口控制按钮信号
    connect(minimizeBtn, &QPushButton::clicked, this, &QMainWindow::showMinimized);
    connect(maximizeBtn, &QPushButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
            maximizeBtn->setText("□");
        } else {
            showMaximized();
            maximizeBtn->setText("❐");
        }
    });
    connect(closeBtn, &QPushButton::clicked, this, &QMainWindow::close);

    // 添加所有部件到标题栏布局
    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(tabButtonsWidget, 1);  // 标签按钮占用剩余空间
    titleLayout->addWidget(windowControls);

    // 设置标签按钮样式
    QString tabButtonStyle = R"(
        QPushButton#tabButton {
            background-color: rgba(255, 255, 255, 0.2);
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 14px;
            font-weight: 500;
        }
        QPushButton#tabButton:hover {
            background-color: rgba(255, 255, 255, 0.3);
        }
        QPushButton#tabButton:checked {
            background-color: white;
            color: #1e90ff;
        }
        QPushButton#tabButton:checked:hover {
            background-color: #f0f8ff;
        }
    )";

    QString windowButtonStyle = R"(
        QPushButton#minimizeBtn, QPushButton#maximizeBtn, QPushButton#closeBtn {
            background-color: transparent;
            border: none;
            border-radius: 0;
            padding: 0;
            margin: 0;
            min-width: 32px;
            min-height: 32px;
            color: white;
            font-size: 16px;
            font-weight: bold;
        }

        QPushButton#minimizeBtn:hover {
            background-color: rgba(255, 255, 255, 0.15);
            border-radius: 4px;
        }

        QPushButton#maximizeBtn:hover {
            background-color: rgba(255, 255, 255, 0.15);
            border-radius: 4px;
        }

        QPushButton#closeBtn:hover {
            background-color: #ff3b30;
            color: white;
            border-radius: 4px;
        }

        QPushButton#closeBtn:pressed {
            background-color: #d70015;
        }

        QPushButton#minimizeBtn:pressed, QPushButton#maximizeBtn:pressed {
            background-color: rgba(255, 255, 255, 0.25);
        }
    )";

    customTitleBar->setStyleSheet(customTitleBar->styleSheet() + tabButtonStyle + windowButtonStyle);
}

// 鼠标按下事件 - 用于窗口拖拽
void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // 将鼠标位置转换为标题栏的局部坐标
        QPoint titleBarPos = customTitleBar->mapFromParent(event->pos());

        // 检查是否在标题栏区域内点击
        if (customTitleBar->rect().contains(titleBarPos)) {
            isDragging = true;
            dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }
}

// 鼠标移动事件 - 实现窗口拖拽
void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint targetPos = event->globalPosition().toPoint() - dragStartPosition;

        // 获取当前屏幕
        QScreen *screen = QApplication::screenAt(targetPos);
        if (!screen) {
            screen = QApplication::primaryScreen();
        }

        QRect screenRect = screen->availableGeometry();
        QSize windowSize = size();

        // 限制窗口在屏幕内
        targetPos.setX(qMax(screenRect.left(),
                            qMin(targetPos.x(),
                                 screenRect.right() - windowSize.width())));
        targetPos.setY(qMax(screenRect.top(),
                            qMin(targetPos.y(),
                                 screenRect.bottom() - windowSize.height())));

        move(targetPos);
        event->accept();
        return;
    }
    QMainWindow::mouseMoveEvent(event);
}

// 鼠标释放事件
void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
}

void MainWindow::onTabChanged(int index) {
    if (index == 0) { // 首页
        homePage->loadMockData();
    }
}

// 支付对话框显示
void MainWindow::onShowPayment(int orderId, double amount) {
    PaymentDialog *dialog = new PaymentDialog(this, orderId, amount);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::onShowReview(int orderId, const QString &sellerName) {
    ReviewDialog *dialog = new ReviewDialog(this, orderId, sellerName);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

// 个人资料编辑对话框显示
void MainWindow::onShowProfileEdit() {
    ProfileEditDialog *dialog = new ProfileEditDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::onShowCreditScore() {
    // 如果已经存在对话框，先关闭
    if (creditScoreDialog) {
        creditScoreDialog->close();
        creditScoreDialog->deleteLater();
    }

    // 创建并显示信用分对话框
    // 这里使用当前用户的ID，假设从登录信息中获取
    QString currentUserId = "user_001";  // 实际应该从登录信息获取

    creditScoreDialog = new CreditScoreDialog(this, currentUserId);
    creditScoreDialog->setAttribute(Qt::WA_DeleteOnClose);

    // 连接对话框关闭信号
    connect(creditScoreDialog, &CreditScoreDialog::finished, [this]() {
        creditScoreDialog = nullptr;
    });

    creditScoreDialog->show();
    creditScoreDialog->raise();
    creditScoreDialog->activateWindow();
}

void MainWindow::onReportGoods(int goodsId) {
    if (reportDialog) {
        reportDialog->close();
        reportDialog->deleteLater();
    }

    // 获取商品名称 - 实际应从数据库获取
    QString goodsName = QString("商品 #%1").arg(goodsId);

    reportDialog = new ReportSubmitDialog(this, goodsId, "goods", goodsName);
    reportDialog->setAttribute(Qt::WA_DeleteOnClose);
    reportDialog->setWindowTitle(QString("举报商品 - %1").arg(goodsName));

    connect(reportDialog, &ReportSubmitDialog::reportSubmitted,
            this, &MainWindow::onReportSubmitted);
    connect(reportDialog, &ReportSubmitDialog::finished, [this]() {
        reportDialog = nullptr;
    });

    reportDialog->show();
    reportDialog->raise();
    reportDialog->activateWindow();
}

void MainWindow::onReportUser(const QString &userId) {
    if (reportDialog) {
        reportDialog->close();
        reportDialog->deleteLater();
    }

    // 获取用户名 - 实际应从数据库获取
    QString userName = "未知用户";
    // TODO: 从数据库查询用户名

    reportDialog = new ReportSubmitDialog(this, userId.toInt(), "user", userName);
    reportDialog->setAttribute(Qt::WA_DeleteOnClose);
    reportDialog->setWindowTitle(QString("举报用户 - %1").arg(userName));

    connect(reportDialog, &ReportSubmitDialog::reportSubmitted,
            this, &MainWindow::onReportSubmitted);
    connect(reportDialog, &ReportSubmitDialog::finished, [this]() {
        reportDialog = nullptr;
    });

    reportDialog->show();
    reportDialog->raise();
    reportDialog->activateWindow();
}

void MainWindow::onReportOrder(int orderId) {
    if (reportDialog) {
        reportDialog->close();
        reportDialog->deleteLater();
    }

    // 获取订单信息 - 实际应从数据库获取
    QString orderInfo = QString("订单 %1").arg(orderId);

    reportDialog = new ReportSubmitDialog(this, orderId, "order", orderInfo);
    reportDialog->setAttribute(Qt::WA_DeleteOnClose);
    reportDialog->setWindowTitle(QString("举报订单 - #%1").arg(orderId));

    connect(reportDialog, &ReportSubmitDialog::reportSubmitted,
            this, &MainWindow::onReportSubmitted);
    connect(reportDialog, &ReportSubmitDialog::finished, [this]() {
        reportDialog = nullptr;
    });

    reportDialog->show();
    reportDialog->raise();
    reportDialog->activateWindow();
}

void MainWindow::onReportSubmitted(int targetId, QString targetType) {
    // 在实际项目中，这里可以：
    // 1. 更新数据库中的举报记录
    // 2. 发送通知给管理员
    // 3. 记录举报日志

    // 显示成功消息
    QString typeName;
    if (targetType == "goods") typeName = "商品";
    else if (targetType == "user") typeName = "用户";
    else if (targetType == "order") typeName = "订单";

    QMessageBox::information(this, "举报成功",
                             QString("您的举报已提交成功！\n\n"
                                     "举报对象：%1 #%2\n"
                                     "管理员将在24小时内处理。\n"
                                     "处理结果将通过系统消息通知您。")
                                 .arg(typeName).arg(targetId));
}

// 取消订单函数
void MainWindow::onCancelOrder(int orderId) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认取消",
        QString("确定要取消订单 #%1 吗？").arg(orderId),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 这里应该更新数据库中的订单状态
        QMessageBox::information(this, "取消成功", "订单已取消");
    }
}

// 确认收货函数
void MainWindow::onConfirmReceipt(int orderId) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认收货",
        QString("确认已收到订单 #%1 的商品吗？\n\n"
                "确认后订单将变为【已完成】状态。").arg(orderId),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "确认成功", "订单状态已更新为【已完成】");
    }
}

void MainWindow::onShowDisputeSubmit(int orderId) {
    if (disputeDialog) {
        disputeDialog->close();
        disputeDialog->deleteLater();
    }

    disputeDialog = new DisputeSubmitDialog(this, orderId);
    disputeDialog->setAttribute(Qt::WA_DeleteOnClose);
    disputeDialog->setWindowTitle(QString("提交售后纠纷 - 订单#%1").arg(orderId));

    // 连接提交成功的信号
    connect(disputeDialog, &DisputeSubmitDialog::finished, [this, orderId](int result) {
        if (result == QDialog::Accepted) {
            // 提交成功，更新订单状态
            onDisputeSubmitted(orderId);
        }
        disputeDialog = nullptr;
    });

    disputeDialog->show();
    disputeDialog->raise();
    disputeDialog->activateWindow();
}

void MainWindow::onDisputeSubmitted(int orderId) {
    // 显示成功消息
    QMessageBox::information(this, "提交成功",
                             QString("售后纠纷申请已提交！\n\n"
                                     "订单号: #%1\n"
                                     "管理员将在24小时内处理。\n"
                                     "处理结果将通过系统消息通知您。").arg(orderId));
}

MainWindow::~MainWindow() {}
