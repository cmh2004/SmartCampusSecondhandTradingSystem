#ifndef RECOMMENDDIALOG_H
#define RECOMMENDDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

class RecommendDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RecommendDialog(QWidget *parent = nullptr);

signals:
    void goodsDetailRequested(int goodsId);

private:
    void setupUI();
    void loadRecommendations();
    void clearGrid();
    QWidget* createRecommendCard(int goodsId, const QString &name, double price, const QString &imageUrl);

    QWidget *m_gridContainer;
    QGridLayout *m_gridLayout;
    QLabel *m_statusLabel;
    QPushButton *m_refreshBtn;
};

#endif // RECOMMENDDIALOG_H
