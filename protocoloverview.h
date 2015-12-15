#ifndef PROTOCOLOVERVIEW_H
#define PROTOCOLOVERVIEW_H

#include <QWidget>
#include <QItemSelection>
#include <QFileSystemModel>
namespace Ui {
class ProtocolOverview;
}

class ProtocolOverview : public QWidget
{
    Q_OBJECT
    
public:
    explicit ProtocolOverview(QWidget *parent = 0);
    ~ProtocolOverview();
    void recalculateRow(int row);
    void loadSavedProtocols();
    void copyPath(QString src, QString dst);
    void loadLegacyProtocolFile(QString src, QString dirPath);
public slots:
    void on_workspaceChanged();        
    void on_requestLoadProtocolFile(QString name);
    
private:
    Ui::ProtocolOverview *ui;
    QFileSystemModel *model;

private slots:
    void onSelectionChanged(QItemSelection selected,QItemSelection deselected);
    void onDataChanged(const QModelIndex& previous, const QModelIndex& current);    
    void on_filterField_textChanged(const QString &text);
    void on_addRecordButton_clicked();
    void on_deleteRecordButton_clicked();
    void on_openFileButton_clicked();
    void on_saveButton_clicked();
    void on_printFileButton_clicked();
};

#endif // PROTOCOLOVERVIEW_H
