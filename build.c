#define SB_IMPL
#include "sb.h"

int main(int argc, char *argv[]) {
    sb_BUILD(argc, argv) {
        sb_chdir_exe();
        sb_mkdir("build/");
        sb_mkdir("build/tests");
        sb_target_dir("build/");

        if (sb_check_arg("init")) {
            // downloads latest version of
            // cutils
            sb_CMD() {
                sb_cmd_main("curl");
                sb_cmd_arg(
                    "https://raw.githubusercontent.com/hant-hub/Cutils/refs/"
                    "heads/main/include/cutils.h");
                sb_cmd_opt("O");
                sb_cmd_opt("-output-dir");
                sb_cmd_arg("lib/include/");
            }
        }

        // formatting
        sb_FOREACHFILE("./", test) {
            if (sb_cmpext(test, ".c") && sb_cmpext(test, ".h"))
                continue;

            sb_CMD() {
                sb_cmd_main("clang-format");
                sb_cmd_opt("i");
                sb_cmd_arg(test);
            }
        }
        sb_FOREACHFILE("test/", test) {
            if (sb_cmpext(test, ".c") && sb_cmpext(test, ".h"))
                continue;

            sb_CMD() {
                sb_cmd_main("clang-format");
                sb_cmd_opt("i");
                sb_cmd_arg(test);
            }
        }
        sb_FOREACHFILE("include/", test) {
            if (sb_cmpext(test, ".c") && sb_cmpext(test, ".h"))
                continue;

            sb_CMD() {
                sb_cmd_main("clang-format");
                sb_cmd_opt("i");
                sb_cmd_arg(test);
            }
        }
        sb_FOREACHFILE("./", test) {
            if (sb_cmpext(test, ".c") && sb_cmpext(test, ".h"))
                continue;

            sb_CMD() {
                sb_cmd_main("clang-format");
                sb_cmd_opt("i");
                sb_cmd_arg(test);
            }
        }
        sb_fence();

        // Static lib
        sb_EXEC() {
            sb_set_out("strbase.o");
            sb_add_flag("g");

            sb_add_include_path("include/");
            sb_add_include_path("lib/include");

            sb_add_flag("c");

            sb_add_file("src/strbase.c");

            sb_export_command();
        }

        sb_fence();

        sb_CMD() {
            sb_cmd_main("ar");
            sb_cmd_opt("rcs");
            sb_cmd_arg("build/strbase.a");
            sb_cmd_arg("build/strbase.o");
        }

        sb_fence();

        // tests
        sb_FOREACHFILE("test/tests/", test) {
            printf("test: %s\n", test);
            if (sb_cmpext(test, ".c"))
                continue;
            sb_EXEC() {
                sb_add_file(test);
                sb_add_file("build/strbase.a");

                sb_add_include_path("include/");
                sb_add_include_path("lib/include");

                sb_add_flag("g");
                sb_add_flag("fsanitize=address");
                sb_link_library("m");

                char buf[PATH_MAX + 1] = {0};
                char final[PATH_MAX + 1] = {0};
                strncpy(buf, test, PATH_MAX);

                char *name = sb_stripext(sb_basename(buf));
                snprintf(final, PATH_MAX, "tests/%s", name);

                sb_set_out(final);

                sb_export_command();
            }
        }

        sb_EXEC() {
            sb_add_file("test/runner.c");

            sb_add_include_path("include/");
            sb_add_include_path("lib/include");

            sb_add_flag("g");
            sb_link_library("m");

            sb_set_out("runner");

            sb_export_command();
        }
    }

    if (!sb_check_arg("no-test")) {
        sb_build_start(argc, argv);
        sb_target_dir("build/");
        sb_CMD() { sb_cmd_main("clear"); }
        sb_fence();
        sb_CMD() {
            sb_cmd_main("build/runner");
            if (sb_check_arg("v")) {
                sb_cmd_arg("v");
            }
        }
        sb_build_end();
    }
}
