#pragma once

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QTimer>

#include <nlohmann/json.hpp>

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
        explicit MainWindow(std::unique_ptr<ARMA3::Client> client, std::filesystem::path const &config_file,
                            QWidget *parent = nullptr);
        ~MainWindow();

        void set_client(std::unique_ptr<ARMA3::Client> client);

    private slots:

        void on_pushButton_clicked();

        void on_button_add_custom_mod_clicked();

        void check_if_arma_is_running();

    private:
        Ui::MainWindow *ui;
        QTimer arma_status_checker;

        std::unique_ptr<ARMA3::Client> client_;

        std::filesystem::path config_file_;
        nlohmann::json settings_;

        void add_item(QTableWidget &table_widget, UiMod const &mod);
        void initialize_table_widget(QTableWidget &table_widget, QStringList const &column_names);

        std::vector<UiMod> get_mods(QTableWidget &table_widget);

        void checkbox_changed(int state);
};
