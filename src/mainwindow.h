#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainwindow_video.h"

static QString BASE_URL1 = "https://prod.pratilipicomics.com/";
static QString BASE_URL2 = "https://prod.pratilipi.com";

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionPlay_triggered();

    void on_actionRefresh_triggered();

    void on_actionPrevious_triggered();

    void on_actionNext_triggered();

    void get_accesstoken();

    void get_accesstoken_onfinish(QNetworkReply *rep);

    void get_homepage(QString, int);

    void get_homepage_onfinish(QNetworkReply *rep);

    void comic_typeChanged(int);

    void get_episodes(QString, int);

    void get_episodes_onfinish(QNetworkReply *rep);

    void comics_nameChanged(int);

    void get_episode(QString plid);

    void get_episode_onfinish(QNetworkReply *rep);

    void episode_nameChanged(int);

    void get_image(int idx);

    void get_image_onfinish(QNetworkReply *rep);

    void open_film_window();

private:
    Ui::MainWindow *ui;
    QComboBox* comic_type, * comics_name, * episode_name, * lang_select;
    QLabel* pic_container, * page_no_container;
    QNetworkAccessManager* mgr;
    QPushButton* film_window;
    mainwindow_video* window;
};

#endif // MAINWINDOW_H
