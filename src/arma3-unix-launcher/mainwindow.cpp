#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <Qt>

#include <QCheckBox>
#include <QMessageBox>
#include <QTabBar>

#include <QResizeEvent>

#include <iostream>
#include <optional>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>

#include "string_utils.hpp"
#include "ui_mod.hpp"

MainWindow::MainWindow(std::unique_ptr<ARMA3::Client> client, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initialize_table_widget(*ui->table_workshop_mods, {"Enabled", "Name", "Workshop ID"});
    initialize_table_widget(*ui->table_custom_mods, {"Enabled", "Name", "Path"});
    ui->table_custom_mods->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    client_ = std::move(client);
    client_->RefreshMods();
    for (auto const &i : client_->mods_workshop_)
        add_item(*ui->table_workshop_mods, {false, i.GetValueOrReturnDefault("name", "cannot read name"),
                 i.GetValueOrReturnDefault("publishedid",
                                           "cannot read id")});

    for (auto const &i : client_->mods_home_)
        add_item(*ui->table_custom_mods, {false, i.GetValueOrReturnDefault("name", "cannot read name"),
                 StringUtils::Replace(i.path_, client_->GetPath().string(), "~arma")});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_client(std::unique_ptr<ARMA3::Client> client)
{
    client_ = std::move(client);
}

void MainWindow::on_pushButton_clicked()
{
    auto find_mod = [this](std::string workshop_id)
    {
        for (auto const &mod : client_->mods_workshop_)
            if (mod.GetValueOrReturnDefault("publishedid", "-1") == workshop_id)
                return std::optional<Mod>(mod);
        return std::optional<Mod>();
    };

    std::vector<Mod> mods;
    for (auto const &mod : get_mods(*ui->table_workshop_mods))
    {
        if (mod.enabled)
        {
            auto mod_2 = find_mod(mod.path_or_workshop_id);
            if (mod_2.has_value())
                mods.push_back(*mod_2);
        }
    }
    client_->CreateArmaCfg(mods);
    return;
    client_->Start("");
}

void MainWindow::add_item(QTableWidget &table_widget, UiMod const& mod)
{
    int id = table_widget.rowCount();
    table_widget.insertRow(id);

    QWidget *checkbox_widget = new QWidget();
    QHBoxLayout *checkbox_layout = new QHBoxLayout(checkbox_widget);
    QCheckBox *checkbox = new QCheckBox();

    checkbox_layout->addWidget(checkbox);
    checkbox_layout->setAlignment(Qt::AlignCenter);
    checkbox_layout->setContentsMargins(0, 0, 0, 0);

    checkbox_widget->setLayout(checkbox_layout);

    if (mod.enabled)
        checkbox->setCheckState(Qt::Checked);
    else
        checkbox->setCheckState(Qt::Unchecked);

    table_widget.setCellWidget(id, 0, checkbox_widget);
    table_widget.setItem(id, 1, new QTableWidgetItem(mod.name.c_str()));
    table_widget.setItem(id, 2, new QTableWidgetItem(mod.path_or_workshop_id.c_str()));

    QObject::connect(checkbox, &QCheckBox::stateChanged, this, &MainWindow::checkbox_changed);

    for (int i = 1; i <= 2; ++i)
    {
        auto item = table_widget.item(id, i);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWindow::initialize_table_widget(QTableWidget &table_widget, QStringList const &column_names)
{
    table_widget.clear();
    table_widget.setHorizontalHeaderLabels(column_names);
    table_widget.horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

std::vector<UiMod> MainWindow::get_mods(QTableWidget &table_widget)
{
    std::vector<UiMod> mods;

    for (int i = 0; i < table_widget.rowCount(); ++i) {
        auto cell_widget = table_widget.cellWidget(i, 0);
        auto checkbox = cell_widget->findChild<QCheckBox *>();

        bool enabled = checkbox->checkState() == Qt::CheckState::Checked;
        std::string name = table_widget.item(i, 1)->text().toStdString();
        std::string path_or_workshop_id = table_widget.item(i, 2)->text().toStdString();
        mods.push_back({enabled, name, path_or_workshop_id});
    }
    return mods;
}

void MainWindow::checkbox_changed(int state)
{
    fmt::print("tzytze: {}\n", state);
    add_item(*ui->table_custom_mods, {false, "XDDD", "Hello there"});
    std::cout.flush();
}
