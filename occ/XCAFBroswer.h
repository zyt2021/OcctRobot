#ifndef XCAFBROSWER_H
#define XCAFBROSWER_H

#include <TDocStd_Document.hxx>

#include <QTreeWidget>

class XCAFBroswer
{
public:
    explicit XCAFBroswer(const Handle(TDocStd_Document) &h);
    ~XCAFBroswer();

    void CreateTree(QTreeWidget*theTree);

private:
    void load(const TDF_Label& label, QTreeWidgetItem* item, const QString&s);
    std::string toString(const TCollection_ExtendedString& extstr) const
    {
        char* str = new char[extstr.LengthOfCString() + 1];
        extstr.ToUTF8CString(str);
        std::string text(str);
        delete[] str;
        return text;
    }

private:
    Handle(TDocStd_Document) pDoc;
};

#endif // XCAFBROSWER_H
