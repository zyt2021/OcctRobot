#ifndef FILEVIEWER_H
#define FILEVIEWER_H

#include <QWidget>

class QFileSystemModel;

namespace Ui {
class FileViewer;
}

class FileViewer : public QWidget
{
    Q_OBJECT

public:
    explicit FileViewer(QWidget *parent = nullptr);
    ~FileViewer();

private slots:
    void on_treeView_fileViewer_clicked(const QModelIndex &index);
    void on_comboBox_fileViewerFilter_currentIndexChanged(int index);

private:
    Ui::FileViewer *ui;

    QFileSystemModel* myFileModel;
    const QStringList myCADFileFormat;
    const QStringList myRobotFileFormat;

signals:
    void requestImportCAD(const QString &str);
    void requestImportRobot(const QString &str);
};

#endif // FILEVIEWER_H
