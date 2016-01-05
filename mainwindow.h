#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "trainingoverview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QList<QWidget*> tabList;
    static QString workspace;
    static bool outdatedTraining;
    static bool outdatedProtocols;
    static bool outdatedSchedules;
    static void copyPath(QString src, QString dst);
    
private slots:    
    void on_actionNew_Staff_Member_triggered();
    
    void on_actionChange_Workspace_triggered();
    
    void on_actionOpen_Folder_triggered();
    
    void on_actionLegacy_Training_Logs_triggered();
    
    void on_actionLegacy_Protocol_Logs_triggered();
    
    void on_actionBackup_Files_triggered();
    
    void on_actionRun_On_Start_toggled(bool active);
    
    void on_actionAdd_New_Category_triggered();

    void on_actionLegacy_Schedule_Logs_triggered();

signals:
    void staffMemberAdded(QString name);
    void categoryAdded(QString name);
    void workspaceChanged();
    void trainingFileLoadRequest(QString name);    
    void schedulesFileLoadRequest(QString name);
    void protocolFileLoadRequest(QString name);    
    
private:
    Ui::MainWindow *ui;
    TrainingOverview *trainingOverview;
    void writePropertiesFile();
    void loadPropertiesFile();
};

#endif // MAINWINDOW_H
