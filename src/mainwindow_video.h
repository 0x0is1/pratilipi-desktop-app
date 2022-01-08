#ifndef MAINWINDOW_VIDEO_H
#define MAINWINDOW_VIDEO_H

#include <QMainWindow>
#include <QtMultimediaWidgets>
#include <QMediaPlayer>

namespace Ui {
class mainwindow_video;
}

class mainwindow_video : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainwindow_video(QWidget *parent = nullptr);
    ~mainwindow_video();

private slots:
    void on_actionPrevious_triggered();

    void on_actionRefresh_triggered();

    void on_actionPlay_triggered();

    void on_actionNext_triggered();

    void on_actionPause_triggered();

    void list_playlist(int);

    void list_videos(int);

private:
    Ui::mainwindow_video *ui;
    QComboBox *playlist_dropdown, *video_dropdown;
    QVideoWidget *vw;
    QMediaPlayer *player;
};

#endif // MAINWINDOW_VIDEO_H
