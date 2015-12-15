#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "trainingoverview.h"
#include "protocoloverview.h"
#include <QtCore>
#include <QInputDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QDesktopServices>

QString MainWindow::workspace("");
bool MainWindow::outdatedTraining = false;
bool MainWindow::outdatedProtocols = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadPropertiesFile();
    if(MainWindow::workspace == ""){
        on_actionChange_Workspace_triggered();
        if(MainWindow::workspace == ""){
            exit(0);
        }
    }
    this->setWindowTitle("Management Suite [" + MainWindow::workspace + "]");
    TrainingOverview *overview = new TrainingOverview();
    ProtocolOverview *protocols = new ProtocolOverview();
    tabList << overview;
    tabList << protocols;
    ui->tabWidget-> addTab(tabList.first(), "Training Manager");  
    ui->tabWidget-> addTab(tabList.back(), "Protocol Manager");   
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if(settings.contains("Management Suite")){
        ui->actionRun_On_Start->setChecked(true);
    }    
    writePropertiesFile();
    connect(this, SIGNAL(staffMemberAdded(QString)), overview, SLOT(on_addStaffMember(QString)));        
    connect(this, SIGNAL(workspaceChanged()), overview, SLOT(on_workspaceChanged()));        
    connect(this, SIGNAL(workspaceChanged()), protocols, SLOT(on_workspaceChanged()));        
    connect(this, SIGNAL(trainingFileLoadRequest(QString)), overview, SLOT(on_requestLoadTrainingFile(QString)));        
    connect(this, SIGNAL(protocolFileLoadRequest(QString)), protocols, SLOT(on_requestLoadProtocolFile(QString))); 
    if(QApplication::arguments().length() > 1 && QApplication::arguments().at(1) == "/boot"){    
        if(outdatedTraining){
            ui->tabWidget->setCurrentWidget(overview);
        }else if(outdatedProtocols){
            ui->tabWidget->setCurrentWidget(protocols);            
        }else{
            exit(0);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_Staff_Member_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this, QString("Add Staff"),QString("Staff Name:"), QLineEdit::Normal, QString(""), &ok);
    if (ok && !text.isEmpty())
        emit staffMemberAdded(text);
}

void MainWindow::on_actionChange_Workspace_triggered()
{
    QString res = QFileDialog::getExistingDirectory(this, QString("Choose Workspace"),QString("/home"), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!res.isEmpty()){
        MainWindow::workspace = res + "/Management Suite";
        writePropertiesFile();
        this->setWindowTitle("Management Suite [" + MainWindow::workspace + "]");
        emit workspaceChanged();
    }
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QDesktopServices desk;
    desk.openUrl(QUrl::fromLocalFile(MainWindow::workspace + "/Protocols/"));
}

void MainWindow::writePropertiesFile()
{
    QSettings settings(QString(QApplication::applicationDirPath() + "/properties.ini"), QSettings::IniFormat);
    settings.setValue("workspace", MainWindow::workspace);
}

void MainWindow::loadPropertiesFile(){
    QSettings settings(QString(QApplication::applicationDirPath() + "/properties.ini"), QSettings::IniFormat);
    MainWindow::workspace = settings.value("workspace", "").toString();
}


void MainWindow::on_actionLegacy_Training_Logs_triggered()
{
    QStringList list = QFileDialog::getOpenFileNames(this, QString("Select Training Logs To Import"),QString("/home"));
    foreach(QString s, list){
        emit trainingFileLoadRequest(s);
    }
}

void MainWindow::on_actionLegacy_Protocol_Logs_triggered()
{
    QString res = QFileDialog::getExistingDirectory(this, QString("Choose Protocol Folder"),QString("/home"), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!res.isEmpty()){
        emit protocolFileLoadRequest(res);
    }
}

void MainWindow::on_actionBackup_Files_triggered()
{
    QString res = QFileDialog::getExistingDirectory(this, QString("Choose Backup Location"),QString("/home"), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!res.isEmpty()){
        MainWindow::copyPath(workspace, res + "/Management Suite");
    }
}

void MainWindow::copyPath(QString src, QString dst)
{
    QDir dir(src);
    if (!dir.exists())
        return;
    
    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + d;
        dir.mkpath(dst_path);
        MainWindow::copyPath(src+ QDir::separator() + d, dst_path);
    }
    
    foreach (QString f, dir.entryList(QDir::Files)) {
        QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
    }
}


void MainWindow::on_actionRun_On_Start_toggled(bool active)
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if(active){
        settings.setValue("Management Suite", "\"" + QApplication::applicationFilePath().replace("/","\\") +  "\" /boot");
    }else{
        settings.remove("Management Suite");
    }
}
