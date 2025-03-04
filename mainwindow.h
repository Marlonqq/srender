#pragma once

#include "SRenderWidget.h"
#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "SRender.h"
#include <qfiledialog.h>
#include <qdebug.h>
#include <qpainter.h>
#include <qcolordialog.h>

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = nullptr);
    ~mainwindow();

    void SetLightColor(LIGHT_TYPE light_type, QColor color);
    void SetLightDir(); 


private slots:
    void on_action_open_file_triggered();

    void on_action_save_image_triggered();

    void on_action_multithread_triggered();

    void on_AmbitientColorSet_clicked();

    void on_DiffuseColorSet_clicked();

    void on_SpecularColorSet_clicked();

    void on_PitchSlider_valueChanged(int value);

    void on_YawDial_valueChanged(int value);

    void on_FovSilder_valueChanged(int value);

    void on_NearSilder_valueChanged(int value);

private:
    Ui::mainwindowClass* ui;
    QColor ambitient_color;
    QColor diffuse_color;
    QColor specular_color;
};
