#include "mainwindow.h"
#include "dialoglogin.h"
#include <QApplication>
#include "globalsetting.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GlobalSetting::instance().m_app_dir= a.applicationDirPath();
    //DialogLogin dlg;
    //dlg.exec();
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
