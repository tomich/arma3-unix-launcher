#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include <QMainWindow>
#include <QTableWidgetItem>

#include <arma3client.hpp>

#include "ui_mod.hpp"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(std::unique_ptr<ARMA3::Client> client, QWidget *parent = nullptr);
        ~MainWindow();

        void set_client(std::unique_ptr<ARMA3::Client> client);

    private slots:

        void on_pushButton_clicked();

    private:
        Ui::MainWindow *ui;

        std::unique_ptr<ARMA3::Client> client_;

        void add_item(QTableWidget &table_widget, UiMod const& mod);
        void initialize_table_widget(QTableWidget &table_widget, QStringList const& column_names);

        std::vector<UiMod> get_mods(QTableWidget &table_widget);

        void checkbox_changed(int state);
};

#endif // MAINWINDOW_HPP_
