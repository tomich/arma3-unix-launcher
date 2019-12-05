#ifndef ARMA3Client_HPP_
#define ARMA3Client_HPP_

#include <filesystem>
#include <string>
#include <vector>

#include "mod.hpp"

namespace ARMA3::Definitions
{
    static std::string home_directory = getenv("HOME");

    static constexpr const char *app_id = "107410";

    static const std::array<char const *, 22> exclusions{"!custom", "!workshop", "!DO_NOT_CHANGE_FILES_IN_THESE_FOLDERS", "Addons", "Argo", "BattlEye", "Curator", "Dta", "Expansion", "fontconfig", "Heli", "Jets", "Kart", "Keys", "MPMissions", "Mark", "Missions", "Orange", "Tacops", "Tank", "legal", "steam_shader_cache"};

    static constexpr char const *symlink_workshop_name = "!workshop";
    static constexpr char const *symlink_custom_name = "!custom";
    static constexpr char const *do_not_change_name = "!DO_NOT_CHANGE_FILES_IN_THESE_FOLDERS";

    #ifdef __linux
    static constexpr std::array<char const *, 2> const executable_names {"arma3.x86_64", "arma3_x64.exe"};
    static constexpr char const *local_share_prefix = ".local/share";
    static constexpr char const *bohemia_interactive_prefix = "bohemiainteractive/arma3";
    static constexpr char const *game_config_path = "GameDocuments/Arma 3/Arma3.cfg";
    #else //__APPLE__
    static constexpr std::array<char const *, 1> const executable_names {"ArmA3.app"};
    static constexpr char const *executable_name "ArmA3.app"
    static constexpr char const *local_share_prefix = "Library/Application Support";
    #endif
}

namespace ARMA3
{
    class Client
    {
        public:
            Client(std::filesystem::path const &arma_path, std::filesystem::path const &target_workshop_path,
                   bool skip_initialization = false);

            bool CreateSymlinkToWorkshop();
            void CreateArmaCfg(std::vector<Mod> const &mod_list, std::filesystem::path cfg_path = "");
            bool IsProton();
            bool RefreshMods();
            void Start(std::string const &arguments);
            void AddCustomMod(std::filesystem::path const &path);
            void RemoveCustomMod(std::filesystem::path const &path);

            std::filesystem::path const &GetPath();
            std::filesystem::path const &GetPathExecutable();
            std::filesystem::path const &GetPathWorkshop();

            std::vector<Mod> mods_custom_;
            std::vector<Mod> mods_home_;
            std::vector<Mod> mods_workshop_;

        private:
            void AddModsFromDirectory(std::filesystem::path const &dir, std::vector<Mod> &target);
            std::string PickModName(Mod const &mod, std::vector<std::string> const &names);
            std::filesystem::path GetCfgPath();

            std::filesystem::path path_;
            std::filesystem::path path_custom_;
            std::filesystem::path path_executable_;
            std::filesystem::path path_workshop_local_;
            std::filesystem::path path_workshop_target_;
    };
}

#endif
