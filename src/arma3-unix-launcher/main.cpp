#ifndef DOCTEST_CONFIG_DISABLE
    #define DOCTEST_CONFIG_IMPLEMENT
    #include "doctest.h"
#endif

#include "arma_path_chooser_dialog.h"
#include "mainwindow.hpp"
#include <QApplication>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "arma3client.hpp"
#include "steam.hpp"
#include "std_utils.hpp"

#include <static_todo.hpp>

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);

        auto config_file = StdUtils::GetConfigFilePath("launcher.conf");
        fmt::print("conf file: {}\n", config_file);
        std::filesystem::create_directories(config_file.parent_path());
        StdUtils::CreateFile(config_file);

        std::string arma_path, workshop_path;
        nlohmann::json settings;

        try
        {
            settings = nlohmann::json::parse(StdUtils::FileReadAllText(config_file));
            arma_path = settings["armaPath"];
            workshop_path = settings["workshopPath"];
        }
        catch (std::exception const &ex)
        {
            fmt::print("cannot read config file: {}\n", ex.what());
        }

        std::unique_ptr<ARMA3::Client> client;

        if (arma_path.empty() || !std::filesystem::exists(arma_path))
        {
            Steam steam;

            for (auto const &path : steam.GetInstallPaths())
            {
                try
                {
                    fmt::print("Install path: {}\n", path);
                    arma_path = steam.GetGamePathFromInstallPath(path, ARMA3::Definitions::app_id);
                    workshop_path = steam.GetWorkshopPath(path, ARMA3::Definitions::app_id);
                }
                catch (std::exception const &e)
                {
                    fmt::print("Didn't find game at {}\nError: {}\n", path, e.what());
                }
            }

            if (arma_path.empty() || workshop_path.empty())
            {
                ArmaPathChooserDialog apcd;
                apcd.exec();

                if (apcd.result() != QDialog::Accepted)
                    exit(0);

                fmt::print("Arma3: {}\nWorkshop: {}\n", apcd.arma_path_, apcd.workshop_path_);
                arma_path = apcd.arma_path_;
                workshop_path = apcd.workshop_path_;
            }
        }

        client = std::make_unique<ARMA3::Client>(arma_path, workshop_path);

        settings["armaPath"] = arma_path;
        settings["workshopPath"] = workshop_path;
        StdUtils::FileWriteAllText(config_file, settings.dump(4));

        MainWindow w(std::move(client), config_file);
        w.show();

        return a.exec();
    }
    catch (std::exception const &ex)
    {
        fmt::print("exception: {}\nshutting down\n", ex.what());
        return 1;
    }
    catch (...)
    {
        fmt::print("unknown exception\n");
        return 1;
    }
}
