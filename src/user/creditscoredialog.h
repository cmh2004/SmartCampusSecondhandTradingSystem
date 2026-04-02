#ifndef CREDITSCOREDIALOG_H
#define CREDITSCOREDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QScrollArea>

class CreditScoreDialog : public QDialog {
    Q_OBJECT
    Q_PROPERTY(int animationProgress READ animationProgress WRITE setAnimationProgress)

public:
    explicit CreditScoreDialog(QWidget *parent = nullptr, QString userId = "");
    ~CreditScoreDialog();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onScoreDetailClicked();
    void onScoreHistoryClicked();
    void updateScoreAnimation();

private:
    void setupUI();
    void loadScoreData();
    void createScoreMeter();
    void createProgressBars();
    int animationProgress() const { return m_animationProgress; }
    void setAnimationProgress(int progress);

private:
    QString userId;
    int m_animationProgress;
    int m_targetScore;
    QTimer *m_scoreAnimationTimer;

    // UI 组件
    QLabel *currentScoreLabel;
    QLabel *scoreLevelLabel;
    QLabel *scoreTrendLabel;
    QWidget *scoreMeterWidget;
    QTableWidget *scoreHistoryTable;
    QPushButton *detailBtn;
    QPushButton *historyBtn;
    QPushButton *closeBtn;
    QWidget *progressBarsWidget;
    QLabel *lastUpdateLabel;

    // 窗口拖拽相关
    bool m_dragging;
    QPoint m_dragStartPosition;
};

#endif // CREDITSCOREDIALOG_H
