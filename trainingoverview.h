#ifndef TRAININGOVERVIEW_H
#define TRAININGOVERVIEW_H
#include <QTableWidgetItem>
#include <QWidget>
#include <QComboBox>
namespace Ui {
class TrainingOverview;
}

class TrainingOverview : public QWidget
{
    Q_OBJECT
    
public:
    explicit TrainingOverview(QWidget *parent = 0);
    ~TrainingOverview();
    void loadSavedTraining(QString path);      
    void recalculateRow(int row);
    void importLegacyLog(QString path);
    
public slots:
    void on_addStaffMember(QString name);
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
    Ui::TrainingOverview *ui;
};

#endif // TRAININGOVERVIEW_H
