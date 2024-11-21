/**
 * @file MY_SETTINGSWINDOW_H
 * @brief Defines the SettingsWindow class, a QDialog derived class for application settings.
 */
#ifndef MY_SETTINGSWINDOW_H
#define MY_SETTINGSWINDOW_H

#include <QDialog>

/**
 * @class SettingsWindow
 * @brief The SettingsWindow class provides a dialog window to adjust application settings.
 */
class SettingsWindow : public QDialog
{
    Q_OBJECT

signals:
    void changeThemeClicked(int theme);
    void changeButtonClicked(int button);


public:
    explicit SettingsWindow(QWidget *parent = nullptr);

private slots:


    void changeThemeButtonClicked(int theme);
    void changeButtonButtonClicked(int button);
};

#endif // MY_SETTINGSWINDOW_H
