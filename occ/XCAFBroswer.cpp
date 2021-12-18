#include "XCAFBroswer.h"

#include <TDF_ChildIterator.hxx>
#include <TDataStd_Name.hxx>

XCAFBroswer::XCAFBroswer(const Handle(TDocStd_Document) &h) : pDoc(h)
{
}

XCAFBroswer::~XCAFBroswer()
{
    pDoc.Nullify();
}

void XCAFBroswer::CreateTree(QTreeWidget *theTree)
{
    theTree->clear();

    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(0, "0");
    theTree->addTopLevelItem(root);

    load(pDoc->GetData()->Root(), root, "0");
}

void XCAFBroswer::load(const TDF_Label &label, QTreeWidgetItem *item, const QString &s)
{
    Handle(TDataStd_Name) name;
    if (label.FindAttribute(TDataStd_Name::GetID(), name)) {
        QString text = QString("%1 %2").arg(s).arg(QString::fromStdString(toString(name->Get())));
        item->setText(0, text);
    }

    int i = 1;
    for (TDF_ChildIterator it(label); it.More(); it.Next(), i++) {
        QString text = QString("%1:%2").arg(s).arg(i);
        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setText(0, text);
        item->addChild(child);
        load(it.Value(), child, text);
    }
}
