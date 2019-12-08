#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <Qt>

#include <QCheckBox>
#include <QMessageBox>
#include <QTabBar>
#include <QTimer>

#include <QResizeEvent>

#include <iostream>
#include <optional>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "string_utils.hpp"
#include "ui_mod.hpp"

#include "std_utils.hpp"

MainWindow::MainWindow(std::unique_ptr<ARMA3::Client> client, std::filesystem::path const &config_file,
                       QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    config_file_ = config_file;
    settings_ = nlohmann::json::parse(StdUtils::FileReadAllText(config_file_));

    ui->setupUi(this);

    initialize_table_widget(*ui->table_workshop_mods, {"Enabled", "Name", "Workshop ID"});
    initialize_table_widget(*ui->table_custom_mods, {"Enabled", "Name", "Path"});
    ui->table_custom_mods->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    client_ = std::move(client);
    client_->RefreshMods();
    for (auto const &i : client_->mods_workshop_)
        add_item(*ui->table_workshop_mods, {false, i.GetValueOrReturnDefault("name", "cannot read name"),
                                            i.GetValueOrReturnDefault("publishedid",
                                                    "cannot read id")
                                           });

    for (auto const &i : client_->mods_home_)
        add_item(*ui->table_custom_mods, {false, i.GetValueOrReturnDefault("name", "cannot read name"),
                                          StringUtils::Replace(i.path_, client_->GetPath().string(), "~arma")
                                         });

    connect(&arma_status_checker, &QTimer::timeout, this, QOverload<>::of(&MainWindow::check_if_arma_is_running));
    arma_status_checker.start(2000);
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
    auto find_mod_workshop = [this](std::string const & workshop_id)
    {
        for (auto const &mod : client_->mods_workshop_)
            if (mod.GetValueOrReturnDefault("publishedid", "-1") == workshop_id)
                return std::optional<Mod>(mod);
        return std::optional<Mod>();
    };

    auto find_mod_custom = [this](std::string path)
    {
        for (auto const &mod : client_->mods_custom_)
            if (mod.path_ == StringUtils::Replace(path, "~arma", client_->GetPath()))
                return std::optional<Mod>(mod);
        for (auto const &mod : client_->mods_home_)
            if (mod.path_ == StringUtils::Replace(path, "~arma", client_->GetPath()))
                return std::optional<Mod>(mod);

        return std::optional<Mod>();
    };

    std::vector<Mod> mods;
    for (auto const &mod : get_mods(*ui->table_workshop_mods))
        if (mod.enabled)
        {
            auto mod_2 = find_mod_workshop(mod.path_or_workshop_id);
            if (mod_2.has_value())
                mods.push_back(*mod_2);
        }

    for (auto const &mod : get_mods(*ui->table_custom_mods))
        if (mod.enabled)
        {
            auto mod_2 = find_mod_custom(StringUtils::Replace(mod.path_or_workshop_id, "~arma", client_->GetPath()));
            if (mod_2.has_value())
                mods.push_back(*mod_2);
        }

    client_->CreateArmaCfg(mods, "/home/muttley/mods.cpp");
    return;
    client_->Start("-nolauncher");
}

void MainWindow::add_item(QTableWidget &table_widget, UiMod const &mod)
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

    for (int i = 0; i < table_widget.rowCount(); ++i)
    {
        auto cell_widget = table_widget.cellWidget(i, 0);
        auto checkbox = cell_widget->findChild<QCheckBox *>();

        bool enabled = checkbox->checkState() == Qt::CheckState::Checked;
        std::string name = table_widget.item(i, 1)->text().toStdString();
        std::string path_or_workshop_id = table_widget.item(i, 2)->text().toStdString();
        mods.push_back({enabled, name, path_or_workshop_id});
    }
    return mods;
}

void MainWindow::checkbox_changed(int)
{
    auto workshop_mods = get_mods(*ui->table_workshop_mods);
    auto custom_mods = get_mods(*ui->table_custom_mods);

    int count_workshop = 0;
    int count_custom = 0;

    for (auto const &mod : workshop_mods)
        if (mod.enabled)
            ++count_workshop;

    for (auto const &mod : custom_mods)
        if (mod.enabled)
            ++count_custom;

    auto text = fmt::format("Selected {} mods ({} from workshop, {} custom)", count_workshop + count_custom, count_workshop,
                            count_custom);
    ui->label_selected_mods->setText(QString::fromStdString(text));
}

void MainWindow::on_button_add_custom_mod_clicked()
{
    fmt::print("is running: {}\n", StdUtils::IsProcessRunning("arma3-unix-launcher"));
}

void MainWindow::check_if_arma_is_running()
{
    std::string text = "Status: Arma 3 is not running";
    for (auto const &executable_name : ARMA3::Definitions::executable_names)
        if (auto pid = StdUtils::IsProcessRunning(executable_name); pid != -1)
            text = fmt::format("Status: Arma 3 is running, PID: {}", pid);

    ui->label_arma_status->setText(QString::fromStdString(text));
}
