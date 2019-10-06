#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <Qt>

#include <QCheckBox>
#include <QMessageBox>
#include <QTabBar>

#include <QResizeEvent>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>

#include "string_utils.hpp"

MainWindow::MainWindow(std::unique_ptr<ARMA3::Client> client, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto tableWidget = ui->tableWidget;
    tableWidget->clear();
    tableWidget->setHorizontalHeaderLabels({"Enabled", "Name", "Workshop ID"});
    tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    auto add_item = [&tableWidget](bool enabled, std::string name, std::string workshop_id)
    {
        int id = tableWidget->rowCount();
        tableWidget->insertRow(id);

        QWidget* checkbox_widget = new QWidget();
        QHBoxLayout* checkbox_layout = new QHBoxLayout(checkbox_widget);
        QCheckBox* checkbox = new QCheckBox();

        checkbox_layout->addWidget(checkbox);
        checkbox_layout->setAlignment(Qt::AlignCenter);
        checkbox_layout->setContentsMargins(0, 0, 0, 0);

        checkbox_widget->setLayout(checkbox_layout);

        if (enabled)
            checkbox->setCheckState(Qt::Checked);
        else
            checkbox->setCheckState(Qt::Unchecked);

        tableWidget->setCellWidget(id, 0, checkbox_widget);
        tableWidget->setItem(id, 1, new QTableWidgetItem(name.c_str()));
        tableWidget->setItem(id, 2, new QTableWidgetItem(workshop_id.c_str()));

        for (int i = 1; i <= 2; ++i) {
            auto item = tableWidget->item(id, i);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
        }
    };

    client_ = std::move(client);
    client_->RefreshMods();
    for (auto const &i : client_->mods_workshop_)
        add_item(false, i.GetValueOrReturnDefault("name", "cannot read name"), StringUtils::Replace(i.path_, client_->GetPath(), "~arma"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_client(std::unique_ptr<ARMA3::Client> client)
{
    client_ = std::move(client);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    /*
     * Weird behavior regarding font sizes
    int size = (ui->centralWidget->size().width() / 2) - 16;
    QString stylesheet = QString("QTabBar::tab { min-width: %1px; }").arg(size);
    fmt::print("{}\n", ui->tabWidget->styleSheet().toStdString());
    ui->tabWidget->setStyleSheet(stylesheet);
    */
    QMainWindow::resizeEvent(event);
}

struct MiniMod
{
   bool enabled;
   std::string name;
   std::string workshop_id;
};


void MainWindow::on_pushButton_clicked()
{
    auto table = ui->tableWidget;

    auto get_item = [&table](int index)
    {
        bool enabled;
        std::string name, workshop_id;

        auto cellWidget = table->cellWidget(index, 0);
        auto checkbox = cellWidget->findChild<QCheckBox*>();

        enabled = checkbox->checkState() == Qt::CheckState::Checked;
        name = table->item(index, 1)->text().toStdString();
        workshop_id = table->item(index, 2)->text().toStdString();
        return MiniMod{enabled, name, workshop_id};
    };

    nlohmann::json json;
    for (int i = 0; i < table->rowCount(); ++i)
    {
       nlohmann::json item;
       auto mod = get_item(i);
       item["enabled"] = mod.enabled;
       item["name"] = mod.name;
       item["id"] = mod.workshop_id;
       json.push_back(item);
    }

    fmt::print("{}", json.dump(4));
}
