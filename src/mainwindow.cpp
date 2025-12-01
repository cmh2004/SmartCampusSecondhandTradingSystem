#include "MainWindow.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("校园二手商品智能交易系统");
    setFixedSize(1000, 700);

    // 1. 菜单栏
    QMenuBar *menuBar = this->menuBar();
    QMenu *userMenu = menuBar->addMenu("个人中心");
    userMenu->addAction("我的发布");
    userMenu->addAction("我的订单");
    userMenu->addAction("退出登录");

    // 2. 工具栏（快捷按钮）
    QToolBar *toolBar = addToolBar("工具");
    toolBar->addAction("发布商品");
    toolBar->addAction("消息通知");

    // 3. 主内容区
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    mainTabWidget = new QTabWidget(centralWidget);

    // 标签页1：首页（整合分类列表 + 商品列表）
    QWidget *homePage = new QWidget();
    QHBoxLayout *homeLayout = new QHBoxLayout(homePage); // 首页用水平布局分左右两部分

    // 首页左侧：商品分类列表
    categoryList = new QListWidget(homePage);
    categoryList->setFixedWidth(150);
    categoryList->addItems({"全部商品", "书籍教材", "电子产品", "生活用品", "体育器材","其他"});
    homeLayout->addWidget(categoryList);

    // 首页右侧：商品表格（4列：图片、名称、价格、状态）
    goodsTable = new QTableWidget(0, 4, homePage);
    goodsTable->setHorizontalHeaderLabels({"商品图片", "名称", "价格", "状态"});
    homeLayout->addWidget(goodsTable, 1); // 商品表格占更大比例

    mainTabWidget->addTab(homePage, "首页");

    // 标签页2：发布商品
    QWidget *publishPage = new QWidget();
    publishPage->setLayout(new QVBoxLayout());
    publishPage->layout()->addWidget(new QLabel("发布商品表单（待实现）", publishPage));
    mainTabWidget->addTab(publishPage, "发布商品");

    // 标签页3：个人中心
    QWidget *userPage = new QWidget();
    userPage->setLayout(new QVBoxLayout());
    userPage->layout()->addWidget(new QLabel("个人信息与订单（待实现）", userPage));
    mainTabWidget->addTab(userPage, "个人中心");

    // 全局样式表
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f7fa;
        }
        QMenuBar {
            background-color: #2c3e50;
            color: white;
            padding: 5px;
        }
        QMenuBar::item {
            padding: 5px 15px;
            margin: 2px;
            border-radius: 3px;
        }
        QMenuBar::item:selected {
            background-color: #34495e;
        }
        QToolBar {
            background-color: #ecf0f1;
            border: none;
            padding: 5px;
        }
        QToolButton {
            background-color: #3498db;
            color: white;
            border: none;
            padding: 6px 15px;
            border-radius: 4px;
            margin: 0 5px;
        }
        QToolButton:hover {
            background-color: #2980b9;
        }
        QListWidget {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 5px;
        }
        QListWidget::item {
            padding: 8px;
            border-radius: 2px;
        }
        QListWidget::item:selected {
            background-color: #3498db;
            color: white;
        }
        QTableWidget {
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 4px;
            gridline-color: #eee;
        }
        QTableWidget::horizontalHeader {
            background-color: #2c3e50;
            color: white;
            padding: 8px;
            border: none;
        }
        QTabWidget::pane {
            border: 1px solid #ddd;
            border-radius: 4px;
            background-color: white;
        }
        QTabBar::tab {
            padding: 8px 20px;
            background-color: #ecf0f1;
            border-radius: 4px 4px 0 0;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-top: 2px solid #3498db;
        }
    )");

    // 主布局只放标签页
    mainLayout->addWidget(mainTabWidget);

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}
