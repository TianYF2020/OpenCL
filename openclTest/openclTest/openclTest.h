#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_openclTest.h"

class openclTest : public QMainWindow
{
    Q_OBJECT

public:
    openclTest(QWidget *parent = nullptr);
    ~openclTest();
    int test();
private:
    Ui::openclTestClass ui;
};
