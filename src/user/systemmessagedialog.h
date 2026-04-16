#ifndef SYSTEMMESSAGEDIALOG_H
#define SYSTEMMESSAGEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

class SystemMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SystemMessageDialog(QWidget *parent = nullptr);

private slots:
    void loadMessages();          // 加载消息列表
    void onMarkRead();            // 标记选中消息为已读
    void onMarkAllRead();         // 标记所有消息为已读
    void onRefresh();             // 刷新列表

private:
    void setupUI();
    void updateUnreadCount();     // 更新未读消息数量显示

    QTableWidget *m_table;
    QPushButton *m_refreshBtn;
    QPushButton *m_markReadBtn;
    QPushButton *m_markAllReadBtn;
    QLabel *m_unreadLabel;
};

#endif // SYSTEMMESSAGEDIALOG_H
