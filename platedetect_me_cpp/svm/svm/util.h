
#include <vector>
#include <string>


class Utils {
 public:
  static long getTimestamp();

  /*
   * Get file name from a given path
   * bool postfix: including the postfix
   */
  static std::string getFileName(const std::string& path,
                                 const bool postfix = false);

  /*
   * Split the given string into segements by a delimiter
   */
  static std::vector<std::string> splitString(const std::string& str,
                                              const char delimiter);

  /*
   * returns the smaller of the two numbers
   */
  template <typename T>
  static T min(const T& v1, const T& v2) {
    return (v1 < v2) ? v1 : v2;
  }

  /*
 * Get files from a given folder
 * all: including all sub-folders
 */
  static std::vector<std::string> getFiles(const std::string& folder,
                                           const bool all = true);
};


