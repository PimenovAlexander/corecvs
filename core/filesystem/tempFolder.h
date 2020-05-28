#ifndef TEMPFOLDER_H
#define TEMPFOLDER_H

#include <vector>
#include <string>

#include "utils/global.h"

namespace corecvs {

class TempFolder {

public:

    ///
    /// \brief returns path to the project's temp folder
    ///
    /// \param projectEnviromentVariable         - a unique project env var, i.e. "PROJECT_DIR"
    ///
    /// \return project's local temp folder path
    ///
    /// \note   the returned path looks like: "/tmp/<projectName>"
    ///
    static std::string LocalTempPath(const std::string &projectEnviromentVariable);

    ///
    /// \brief returns path to the project's temp folder, not a thread safe
    ///
    /// \param projectEnviromentVariable         - a unique project env var, i.e. "PROJECT_DIR"
    /// \param clear                             - if true, temp folder is cleared when the first time method is called in a process
    //  \param useLocal                          - when true it works in local PC's temp, it's true by default to speed up all tests
    ///
    /// \return project's local temp folder path
    ///
    /// \note   the returned path looks like: "/tmp/<projectName>/master_linux" under Jenkins, otherwise: "/tmp/<projectName>"
    ///
    static std::string TempFolderPath(const std::string &projectEnviromentVariable, bool clear = false, bool useLocal = true);


    ///
    /// \brief returns path to the project's working folder for the current build
    ///
    /// \param projectEnviromentVariable         - a unique project env var, i.e. "PROJECT_DIR". It must not be empty!
    /// \param subfolderRelPathJen               - a subfolder relative path for Jenkins case, which will be added to get the full path to the unique build folder
    ///
    /// \note  the returned path looks like: "<projectDir>/data/test_results/master_linux_555" under Jenkins, otherwise: "<projectDir>/data/test_results"
    ///
    static std::string UniqueBuildPath(const std::string &projectEnviromentVariable, const std::string &subfolderRelPathJen = "data/test_results");

};

} // namespace corecvs

#endif // TEMPFOLDER_H
