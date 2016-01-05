#ifndef SCHEDULETAB_H
#define SCHEDULETAB_H
#include <QTableWidgetItem>
#include <QWidget>
#include <QComboBox>
namespace Ui {
class ScheduleTab;
}

class ScheduleTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit ScheduleTab(QWidget *parent = 0);
    ~ScheduleTab();
    void loadSavedTraining(QString path);      
    void recalculateRow(int row);
    void importLegacyLog(QString path);
    
public slots:
    void on_addCategory(QString name);
    void on_workspaceChanged();  
    void on_requestLoadTrainingFile(QString name);
    
private slots:
    void on_addTrainingButton_clicked();
        
    void onDataChanged(const QModelIndex& previous, const QModelIndex& current);
    
    void on_deleteTrainingButton_clicked();
    
    void on_saveTrainingButton_clicked();
    
    void on_nameSelectorBox_currentIndexChanged(const QString &arg1);
        
    void on_printButton_clicked();
    
private:
    Ui::ScheduleTab *ui;
};

#endif // SCHEDULETAB_H
