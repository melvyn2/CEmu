#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QLabel>

#include <romselection.h>
#include <emuthread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *p = 0);
    ~MainWindow();

public slots:
    // Misc.
    void closeEvent(QCloseEvent *) override;
    void showStatusMsg(QString str);

    // Actions
    void runSetup(void);
    void showAbout(void);
    void actionExit(void);

    // Console
    void clearConsole(void);
    void consoleStr(QString str);

    // Debugger
    void raiseDebugger();
    void currentChangedSlot(int);
    void populateDebugWindow();

private slots:

private:
    Ui::MainWindow *ui = nullptr;
    EmuThread emu;

    QLabel status_label;
};

// Used as global instance by EmuThread and friends
extern MainWindow *main_window;

#endif // MAINWINDOW_H
