/*
 * main.cpp
 *
 *  Created on: 8 Oct 2016
 *      Author: muttley
 */

#include <iostream>
#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <regex>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "VDF.h"
#include "Filesystem.h"
#include "Settings.h"
#include "Utils.h"

using namespace std;

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	cout << "GTK+ version: " << gtk_major_version << "." << gtk_minor_version << "." << gtk_micro_version << endl
			<< "Glib version: " << glib_major_version << "." << glib_minor_version << "." << glib_micro_version << endl;

	/*Utils us;
	cout << us.Trim("\t\t          \t\t    \t\t          Salam alejkum\n");*/


	Filesystem fs;

	string armaInstall = fs.GetDirectory(DirectoryToFind::ArmaInstall);
	string workshopMods = fs.GetDirectory(DirectoryToFind::WorkshopMods);

	cout << "ArmA 3 install directory: " << armaInstall << "\nWorkshop directory: " << workshopMods << endl;

	vector<string> ModDirs = fs.GetSubDirectories(armaInstall + "/!workshop");
	for (string s: ModDirs)
		cout << s << endl;
	//Mod m("/mnt/nagrania/SteamLibraryLinux/steamapps/workshop/content/107410/743968516");
	//cout << m.ToString();
	vector<Mod> modList = fs.FindMods(workshopMods);
	/*for (Mod m: modList)
		cout << "Mod: " << m.PublishedId << endl;*/

	fs.CheckFileStructure(armaInstall, workshopMods, modList);
	/*int status = mkdir((fs.HomeDirectory + fs.LauncherSettingsDirectory).c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); //d rwx r-x r-x
	cout << "Creating directory " << fs.HomeDirectory + fs.LauncherSettingsDirectory << " result: " << status << endl;
	cout << "Checking if config file " << fs.HomeDirectory + fs.LauncherSettingsDirectory + fs.LauncherSettingsFilename << " exists... ";
	if (!fs.FileExists(fs.HomeDirectory + fs.LauncherSettingsDirectory + fs.LauncherSettingsFilename))
	{
		string DefaultContent = "ArmaPath=\nWorkshopPath=\n";
		fs.WriteAllText(fs.HomeDirectory + fs.LauncherSettingsDirectory + fs.LauncherSettingsFilename, DefaultContent);
	}
	Settings s;
	s.Load(fs.HomeDirectory + fs.LauncherSettingsDirectory + fs.LauncherSettingsFilename);*/
	//cout << "ArmA 3 install directory: " << armaInstallDir() << endl << "Mod workshop directory: " << getWorkshopDir() << endl;

	/*struct stat statinfo;
	if (lstat((HomeDirectory + "/.steampath").c_str(), &statinfo) < 0)
	{
		perror((HomeDirectory + "/.steampath").c_str());
		exit(1);
	}
   if (!S_ISLNK (statinfo.st_mode) && statinfo.st_nlink > 1)
   {
	printf("The file %s IS A HARD LINK to another filename (ln file).\n", (HomeDirectory + "/.steampath").c_str());
   }
   else if (S_ISLNK (statinfo.st_mode))
   {
	  printf("The file %s IS A SYMBOLIC LINK (ln -s file).\n", (HomeDirectory + "/.steampath").c_str());
	  char* buffer = new char[256];
	  size_t hello = readlink((HomeDirectory + "/.steampath").c_str(), buffer, 256);
	  buffer[hello] = '\0';
	  cout << "SYMLINK TARGET: " << buffer << endl;
   }
   else
   {
	  printf("The file %s IS NOT A LINK.\n", (HomeDirectory + "/.steampath").c_str());
	  printf("Check the dp->d_type flag to spesify the filetype (DT_DIR, DT_REG, DT_FIFO, DT_CHR, DT_BLK, DT_SOCK etc...)\n");
   }*/
   return 0;
}