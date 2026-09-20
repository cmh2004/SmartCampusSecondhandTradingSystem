#ifndef IMAGEVIEWERDIALOG_H
#define IMAGEVIEWERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class ImageViewerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ImageViewerDialog(const QPixmap &pixmap, QWidget *parent = nullptr);
    explicit ImageViewerDialog(const QString &imageUrl, QWidget *parent = nullptr);
};

#endif // IMAGEVIEWERDIALOG_H
