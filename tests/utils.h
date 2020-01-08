/* See LICENSE file for license and copyright information */

#define _STR(x) #x
#define STR(x) _STR(x)
#define TEST_PLUGIN_DIR_PATH STR(_TEST_PLUGIN_DIR_PATH)
#define TEST_PLUGIN_FILE_PATH STR(_TEST_PLUGIN_FILE_PATH)
#define TEST_FILE_PATH STR(_TEST_FILE_PATH)

const char* get_plugin_path(void);
const char* get_plugin_dir_path(void);
const char* get_plugin_test_file_path(void);
