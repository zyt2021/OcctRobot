#include "FileViewer.h"
#include "ui_FileViewer.h"

#include <QFileSystemModel>
#include <QStyledItemDelegate>

FileViewer::FileViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileViewer),
    myCADFileFormat({"*.step","*.STEP","*.stp","*.STP",
                    "*.iges","*.IGES","*.igs","*.IGS",
                    "*.stl","*.STL",
                    "*.brep","*.brp"}),
    myRobotFileFormat({"*.json"})
{
    ui->setupUi(this);

    myFileModel = new QFileSystemModel(this);
    myFileModel->setNameFilters(myCADFileFormat);
    myFileModel->setRootPath(QDir::currentPath());
    myFileModel->setNameFilterDisables(false);
    ui->treeView_fileViewer->setModel(myFileModel);

    ui->treeView_fileViewer->setColumnWidth(0,this->width()*0.7);

    QStyledItemDelegate* itemDelegate = new QStyledItemDelegate(this);
    ui->comboBox_fileViewerFilter->setItemDelegate(itemDelegate);
}

FileViewer::~FileViewer()
{
    delete ui;
}

void FileViewer::on_treeView_fileViewer_clicked(const QModelIndex &index)
{
    QFileInfo aInfo = myFileModel->fileInfo(index);
    if(aInfo.isDir()) {
        ui->treeView_fileViewer->expand(index);
    }
    else if(aInfo.suffix().toLower() == "json") {
        this->requestImportRobot(aInfo.filePath());
    }
    else {
        this->requestImportCAD(aInfo.filePath());
    }
}

void FileViewer::on_comboBox_fileViewerFilter_currentIndexChanged(int index)
{
    if(0 == index) {
        myFileModel->setNameFilters(myCADFileFormat);
    }
    else {
        myFileModel->setNameFilters(myRobotFileFormat);
    }
}
