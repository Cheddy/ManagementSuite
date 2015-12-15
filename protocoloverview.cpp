#include "protocoloverview.h"
#include "ui_protocoloverview.h"
#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <iostream>
#include "mainwindow.h"

ProtocolOverview::ProtocolOverview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProtocolOverview)
{
    ui->setupUi(this);
    model = new QFileSystemModel();
    QString path = MainWindow::workspace + "/Protocols/";
    QDir::root().mkpath(path);
    model->setRootPath(path);
    model->setNameFilterDisables(false);
    ui->fileTree->setModel(model);
    QModelIndex idx = model->index(path);
    ui->fileTree->setRootIndex(idx);    
    for (int i = 1; i < model->columnCount(); i++)
        ui->fileTree->hideColumn(i);
    ui->protocolTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->fileTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->protocolTable->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));    
    loadSavedProtocols();
}

ProtocolOverview::~ProtocolOverview()
{
    delete ui;
}


void ProtocolOverview::onSelectionChanged(QItemSelection selected,QItemSelection deselected)
{
    QModelIndexList indexes = selected.indexes();
    ui->protocolTable->clearSelection();
    QDir dir(MainWindow::workspace + "/Protocols");          
    foreach(const QModelIndex &index, indexes) {
        if(index.column() == 0){
            QString path = dir.relativeFilePath(model->fileInfo(index).absoluteFilePath());
            for(int i = 0; i < ui->protocolTable->rowCount(); i++){
                QTableWidgetItem *item = ui->protocolTable->item(i,0);
                if(item != NULL && item->text().startsWith(path, Qt::CaseInsensitive)){
                    ui->protocolTable->setRowHidden(i, false);
                }else{
                    ui->protocolTable->setRowHidden(i, true);                    
                }
            }
        }
    }
}

void ProtocolOverview::on_filterField_textChanged(const QString &text)
{
    ui->protocolTable->clearSelection();
    QModelIndex index = ui->fileTree->selectionModel()->currentIndex();
    if(index.row() >= 0){
        QDir dir(MainWindow::workspace + "/Protocols");                  
        QString path = dir.relativeFilePath(model->fileInfo(index).absoluteFilePath());
        for(int i = 0; i < ui->protocolTable->rowCount(); i++){   
            QTableWidgetItem *item = ui->protocolTable->item(i,0);                
            if(item != NULL && item->text().startsWith(path, Qt::CaseInsensitive) && item->text().contains(text, Qt::CaseInsensitive)){
                ui->protocolTable->setRowHidden(i, false);
            }else{
                ui->protocolTable->setRowHidden(i, true);                    
            }
        }
    }else{
        for(int i = 0; i < ui->protocolTable->rowCount(); i++){   
            QTableWidgetItem *item = ui->protocolTable->item(i,0);
            if(item != NULL && item->text().contains(text, Qt::CaseInsensitive)){
                ui->protocolTable->setRowHidden(i, false);
            }else{
                ui->protocolTable->setRowHidden(i, true);                                        
            }
        }
    }
    
}

void ProtocolOverview::on_addRecordButton_clicked()
{
    QModelIndex index = ui->fileTree->selectionModel()->currentIndex();
    if(index.row() >= 0){
        ui->protocolTable->insertRow(0);
        QModelIndex index = ui->fileTree->selectionModel()->currentIndex();
        QDir dir(MainWindow::workspace + "/Protocols");        
        QString path = dir.relativeFilePath(model->fileInfo(index).absoluteFilePath());
        
        
        QTableWidgetItem *name = new QTableWidgetItem(path,QTableWidgetItem::Type);    
        name->setFlags(name->flags() & ~Qt::ItemIsEditable);
        name->setToolTip(path);
        
        QDate current = QDate::currentDate();
        QTableWidgetItem *lastTaken = new QTableWidgetItem(current.toString("dd/MM/yyyy"),QTableWidgetItem::Type);
        lastTaken->setData(Qt::EditRole, current);
        
        QTableWidgetItem *renewalPeriod = new QTableWidgetItem(0,QTableWidgetItem::Type);
        renewalPeriod->setData(Qt::EditRole, 0);        
        
        QDate currentDate = QDate::currentDate();    
        QTableWidgetItem *item = new QTableWidgetItem(currentDate.toString("dd/MM/yyyy"),QTableWidgetItem::Type);
        item->setData(Qt::EditRole, currentDate);    
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->protocolTable->setSortingEnabled(false);
        ui->protocolTable->setItem(0,0,name);
        ui->protocolTable->setItem(0,1,lastTaken);    
        ui->protocolTable->setItem(0,2,renewalPeriod);        
        ui->protocolTable->setItem(0,3,item);
        recalculateRow(0);        
        ui->protocolTable->setSortingEnabled(true);    
        ui->protocolTable->repaint();
    }
}

void ProtocolOverview::onDataChanged(const QModelIndex& previous, const QModelIndex& current)
{
    if(ui->protocolTable->item(previous.row(), 1) != NULL && ui->protocolTable->item(previous.row(), 2) != NULL && (previous.column() == 2 || previous.column() == 1)){
        recalculateRow(previous.row());       
    }
}


void ProtocolOverview::recalculateRow(int row)
{
    if(ui->protocolTable->item(row, 1) != NULL && ui->protocolTable->item(row, 2) != NULL){
        ui->protocolTable->setSortingEnabled(false);
        if(ui->protocolTable->item(row, 3) == NULL){
            QDate currentDate = QDate::currentDate();    
            QTableWidgetItem *item = new QTableWidgetItem(currentDate.toString("dd/MM/yyyy"),QTableWidgetItem::Type);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->protocolTable->setItem(row, 3, item);
        }
        ui->protocolTable->item(row,3)->setData(Qt::EditRole, ui->protocolTable->item(row, 1)->data(Qt::EditRole).toDate().addYears(ui->protocolTable->item(row, 2)->data(Qt::EditRole).toInt()));
        if(ui->protocolTable->item(row,3)->data(Qt::EditRole).toDate() < QDate::currentDate()){
            ui->protocolTable->item(row,3)->setBackgroundColor(QColor(255,0,0));
        }else if (ui->protocolTable->item(row,3)->data(Qt::EditRole).toDate() < QDate::currentDate().addMonths(1)){
            ui->protocolTable->item(row,3)->setBackgroundColor(QColor(255,255,0));            
        }else{
            ui->protocolTable->item(row,3)->setBackgroundColor(QColor(0,255,0));            
        }
        ui->protocolTable->repaint(); 
        ui->protocolTable->setSortingEnabled(true);        
    }
}

void ProtocolOverview::on_deleteRecordButton_clicked()
{
    while(!ui->protocolTable->selectionModel()->selectedIndexes().isEmpty()){
        ui->protocolTable->removeRow(ui->protocolTable->selectionModel()->selectedIndexes().first().row());
    }
}

void ProtocolOverview::on_openFileButton_clicked()
{
    if(!ui->protocolTable->selectionModel()->selectedIndexes().isEmpty()){
        QDir dir(MainWindow::workspace + "/Protocols");              
        QString path = ui->protocolTable->item(ui->protocolTable->selectionModel()->selectedIndexes().first().row(), 0)->text();
        QDesktopServices desk;
        desk.openUrl(QUrl::fromLocalFile(dir.absoluteFilePath(path)));
    }
}

void ProtocolOverview::on_saveButton_clicked()
{
    QFile f(MainWindow::workspace + "/protocols.csv");
    if (f.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream data( &f );
        for(int row =0; row < ui->protocolTable->rowCount(); row++){
            for(int col =0; col < ui->protocolTable->columnCount() - 1; col++){
                QTableWidgetItem *item = ui->protocolTable->item(row,col);
                if(item != NULL){
                    data << item->text();
                }
                if(col != ui->protocolTable->columnCount() - 2)
                    data << ',';  
            }
            if(row != ui->protocolTable->rowCount() - 1)
                data << '\n';
        }
        f.close();
    }else{
        std::cerr << "Error!";
    }    
}

void ProtocolOverview::loadSavedProtocols()
{
    QFile f(MainWindow::workspace + "/protocols.csv");
    if (f.open(QIODevice::ReadWrite))
    {
        QString fileData = f.readAll();
        if(!fileData.isEmpty()){        
            QStringList lines = fileData.split(QString("\n"));
            for(int row =0; row < lines.size(); row++){
                if(lines.at(row) != NULL){
                    ui->protocolTable->insertRow(row);                    
                    QStringList lineData = lines.at(row).split(QString(","));
                    for(int col =0; col < lineData.size(); col++){
                        if(lineData.at(col) != NULL){
                            QTableWidgetItem *item; 
                            if(col == 1){
                                QDate date = QDate::fromString(lineData.at(col), "yyyy-MM-dd");
                                item = new QTableWidgetItem(date.toString("dd/MM/yyyy"),QTableWidgetItem::Type);
                                item->setData(Qt::DisplayRole, date);                                
                                item->setData(Qt::EditRole, date);    
                            }else if(col == 2){
                                item = new QTableWidgetItem(lineData.at(col),QTableWidgetItem::Type);   
                                item->setData(Qt::DisplayRole, lineData.at(col).toInt());                                
                                item->setData(Qt::EditRole, lineData.at(col).toInt());                                
                            }else{
                                QString path = lineData.at(col);
                                item = new QTableWidgetItem(path,QTableWidgetItem::Type);       
                                item->setToolTip(path);                                                                
                            }
                            if(col == 0 || col == 3){
                                item->setFlags(item->flags() & ~Qt::ItemIsEditable);    
                            }
                            ui->protocolTable->setItem(row,col,item);
                        }
                    }
                }
            }
        }
        f.close();
    }else{
        std::cerr << "Error!";
    }   
    if(QApplication::arguments().length() > 1 && QApplication::arguments().at(1) == "/boot"){
        QDate date = QDate::currentDate().addMonths(1);
        for(int i = 0; i < ui->protocolTable->rowCount(); i++){
            if(ui->protocolTable->item(i, 3)->data(Qt::EditRole).toDate() < date){
                MainWindow::outdatedProtocols = true;
            }
        }
    } 
}

void ProtocolOverview::on_printFileButton_clicked()
{
    if(!ui->protocolTable->selectionModel()->selectedIndexes().isEmpty()){
        QDir dir(MainWindow::workspace + "/Protocols");      
        QString path = ui->protocolTable->item(ui->protocolTable->selectionModel()->selectedIndexes().first().row(), 0)->text();
        ShellExecute(0, reinterpret_cast<const WCHAR*>(QString("print").utf16()), reinterpret_cast<const WCHAR*>(dir.absoluteFilePath(path).utf16()), 0, 0, SW_HIDE);        
    }    
}

void ProtocolOverview::on_workspaceChanged()
{
    ui->protocolTable->clearContents();
    ui->protocolTable->setRowCount(0);
    QString path = MainWindow::workspace + "/Protocols/";
    QDir::root().mkpath(path);
    model->setRootPath(path);
    QModelIndex idx = model->index(path);
    ui->fileTree->setRootIndex(idx);    
    loadSavedProtocols();
}  

void ProtocolOverview::on_requestLoadProtocolFile(QString name)
{
    copyPath(name, MainWindow::workspace + "/Protocols");    
}

void ProtocolOverview::copyPath(QString src, QString dst)
{
    QDir dir(src);
    if (!dir.exists())
        return;
    
    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + d;
        dir.mkpath(dst_path);
        copyPath(src+ QDir::separator() + d, dst_path);
    }
    
    foreach (QString f, dir.entryList(QDir::Files)) {
        if(f != "ProtocolData.ptl"){
            QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
        }else{
            loadLegacyProtocolFile(src, dst);
        }
    }
}

void ProtocolOverview::loadLegacyProtocolFile(QString src, QString dirPath)
{
    QFile f(src + "/ProtocolData.ptl");
    if (f.open(QIODevice::ReadOnly))
    {
        QString fileData = f.readAll();
        if(!fileData.isEmpty()){        
            QStringList lines = fileData.remove("\r").split(QString("\n"));
            for(int row =0; row < lines.size(); row++){
                if(lines.at(row) != NULL && !lines.at(row).isEmpty()){
                    ui->protocolTable->insertRow(row);                    
                    QStringList lineData = lines.at(row).split(QString(","));
                    for(int col =0; col < lineData.size(); col++){
                        if(lineData.at(col) != NULL){
                            QTableWidgetItem *item; 
                            if(col == 1){
                                QDate date = QDate::fromString(lineData.at(col), "dd-MM-yyyy");
                                if(date.isNull() || !date.isValid())
                                    date = QDate::fromString("01-01-2000", "dd-MM-yyyy");
                                item = new QTableWidgetItem(QTableWidgetItem::Type);
                                item->setData(Qt::DisplayRole, date);                                
                                item->setData(Qt::EditRole, date);    
                            }else if(col == 2){
                                QString tmp = lineData.at(col);
                                tmp.remove(QRegExp("[^0-9]"));
                                if(tmp.isEmpty())
                                    tmp = QString("0");
                                
                                item = new QTableWidgetItem(QTableWidgetItem::Type);   
                                item->setData(Qt::DisplayRole, tmp.toInt());                                
                                item->setData(Qt::EditRole, tmp.toInt());                                
                            }else{
                                QString path = dirPath + "/" + lineData.at(col);
                                QDir dir(MainWindow::workspace + "/Protocols");                                        
                                path = dir.relativeFilePath(QFileInfo(path).absoluteFilePath());
                                item = new QTableWidgetItem(path,QTableWidgetItem::Type);      
                                item->setToolTip(path);                                
                            }
                            if(col == 0 || col == 3){
                                item->setFlags(item->flags() & ~Qt::ItemIsEditable);               
                            }
                            ui->protocolTable->setItem(row,col,item);
                        }
                    }
                }
            }
        }
        f.close();
    }else{
        std::cerr << "Error!";
    }   
}
