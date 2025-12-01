#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QMenuBar>
#include <QToolBar>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTabWidget *mainTabWidget; // 主内容区标签页（首页、发布、我的）
    QListWidget *categoryList; // 商品分类列表
    QTableWidget *goodsTable;  // 商品列表展示表格
};

#endif // MAINWINDOW_H
