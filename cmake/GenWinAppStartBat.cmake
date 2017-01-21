# This script can generate *.bat file to start windows application.
# It can be usuful when QT libraries reside in non-default installation paths.
#
# Expected input variables
# APP_NAME
# EXTRA_PATH
# OUTPUT_DIR

set (bat_file_path "${OUTPUT_DIR}/${APP_NAME}.bat")
file (WRITE ${bat_file_path} "set PATH=${EXTRA_PATH};%PATH%\n")
file (APPEND ${bat_file_path} "set app=%~dp0\\${APP_NAME}.exe\n")
file (APPEND ${bat_file_path} "call %app% %*\n")
