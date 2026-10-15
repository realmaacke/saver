#include "Project/Project.hpp"
#include "Environment.hpp"
#include "INIStorage.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Codes/InformationCodes.hpp"
#include "Output/Output.hpp"
#include "Project/ProjectImpl.hpp"
#include "Project/ProjectSettings.hpp"
#include "Prompt.hpp"
#include "Sender/DTO/ProjectDTO.hpp"
#include "Service.hpp"
#include "User.hpp"
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>

Project::Project() {
    this->p_impl = ProjectImpl();
}

bool Project::inside_project() {
    if (this->p_impl.find_project_root()) {
        return true;
    }
    return false;
}

void Project::project_environment() {
    std::optional<fs::path> root = this->p_impl.find_project_root();

    if (!root) {
        Output::info(InfoCode::PROJECT_NOT_FOUND);
        return;
    }
    this->set_project_directory(root->string());
}

void Project::system_environment() {
    fs::path config_directory = LocalConfigDir();

    fs::create_directories(config_directory);

    if (fs::exists(config_directory / "global_saver.ini")) {
        return;
    }

    fs::path base_config = ResourceDir("global_config_base.ini");

    try {
        fs::copy_file(base_config, fs::path(config_directory / "global_saver.ini"));
    } catch (const fs::filesystem_error& _) {
        Output::error(ErrorCode::INTERNAL_FILE_ERROR, __FUNCTION__);
        return;
    }
}

int Project::project_help() {
    return this->p_impl.impl_help();
}

int Project::project_status() {
    this->project_environment();
 
    INIStorage storage(fs::path(this->project_directory) / ".saver" / "project.ini");

    storage.loadStorage();

    ProjectSettings settings;

    settings.project_name = storage.getValue("project_name").value_or("undefined");
    settings.project_remote = storage.getValue("project_remote").value_or("undefined");
    settings.default_chapter = storage.getValue("default_chapter").value_or("undefined");
    settings.description = storage.getValue("description").value_or("undefined");
    std::string start = storage.getValue("start_date").value_or("undefined");
    std::string last = storage.getValue("last_sync").value_or("undefined");
    Output::print("[Project settings]");
    Output::print("Project name     : " + settings.project_name);
    Output::print("Project remote   : " + settings.project_remote);
    Output::print("Default chapter  : " + settings.default_chapter);
    Output::print("Private project  : " + storage.getValue("is_private").value_or("undefined"));
    Output::print("Creation date    : " + start);
    Output::print("Last remote sync : " + last);
    Output::print("Description      : " + settings.description.value_or(""));

    return 0;
}

int Project::project_start() {
    // creates .local/share/saver stuff
    this->system_environment(); // THIS NEEDS TO BE AT THE START OF THE METHOD.

    // checks if already inside a project
    std::optional<fs::path> current = this->p_impl.find_project_root();
    if (current) {
        Output::info(InfoCode::ALREADY_IN_PROJECT);
        return 0;
    }

    if (!Service::instance().usr().isConnected()) {
        Output::info(InfoCode::NOT_LOGGED_IN);
        return 0;
    }
    UserInfo user = Service::instance().usr().getUser();
    
    std::string name = prompt("Project name");
    
    ProjectCreation::Response res = Service::instance().send().get<ProjectCreation::Response>
    (
        "proj/init/" +  user.username + "/" + name,
        true
    );
    // (const std::string &path, bool useAuth)

    bool uniqueName = res.success;

    while(!uniqueName) {
        name = prompt("\nYou already have a project with that name \n try again");
        
        ProjectCreation::Response res = Service::instance().send().get<ProjectCreation::Response>
        (
            "proj/init/" +  user.username + "/" + name,
            true
        );

        if (res.success) {
            uniqueName = true;
            break;
        }
    }
    
    
    ProjectSettings settings;
    settings.project_name = name;


    // if name == taken, redo
    if (!polar_prompt("Use default saver remote", true)) {
        settings.project_remote = prompt("Saver remote url");
    } else {
        // take local saver remote.
        INIStorage local(LocalConfigFile());
        local.loadStorage();

        std::string url = local.getValue("API_URL").value_or("undefined");
        std::string port = local.getValue("API_PORT").value_or("undefined");

        if (url == "undefined" || port == "undefined") {
            Output::error(ErrorCode::INTERNAL_RESOURCE_MISSING);
            return 1;
        }

        std::erase(url, '"');
        std::erase(port, '"');

        if (url.ends_with("/")) {
            url = url.substr(0, url.size() - 1);
        }

        settings.project_remote = url + ":" + port + "/";
    }

    settings.default_chapter = prompt("Default chapter", "main");
    settings.is_private = polar_prompt("Make this project public?", true);

    std::string desc = prompt("Description (optional)");

    if (!desc.empty()) settings.description = desc;


    return this->p_impl.impl_start(settings);
}

int Project::project_add(const std::string& path) {
    this->project_environment();
    return this->p_impl.impl_add(path);
}

int Project::project_removal(const std::string& path) {
    this->project_environment();
    return this->p_impl.impl_remove(path);
}

int Project::project_describe(const std::string& message) {
    this->project_environment();

    if (message.empty()) {
        Output::print("Describe cant be empty");
        return 0;
    }

    return this->p_impl.impl_describe(message);
}

int Project::project_upload() {
    this->project_environment();

    // check if user signed in
    // check that user owns repo.
    

    // pass user struct to upload?
    return this->p_impl.impl_upload();
}


// Private methods

void Project::set_project_directory(const std::string& path) {
    this->project_directory = path;
}

