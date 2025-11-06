# scripts/check_architecture.cmake

# Ensure that CURRENT_ARCH_FILE and ALLOWED_ARCH_FILE are passed as arguments
if(NOT DEFINED CURRENT_ARCH_FILE OR NOT DEFINED ALLOWED_ARCH_FILE)
    message(FATAL_ERROR "CURRENT_ARCH_FILE and ALLOWED_ARCH_FILE must be defined.")
endif()

# Read the content of the generated architecture file
file(READ "${CURRENT_ARCH_FILE}" CURRENT_CONTENT)

# Read the content of the allowed architecture file
# If allowed_architecture.dot does not exist, treat it as an architectural change
if(NOT EXISTS "${ALLOWED_ARCH_FILE}")
    message(FATAL_ERROR "Architecture change detected!\n\n    The allowed architecture file (${ALLOWED_ARCH_FILE}) does not exist.\n    Please create this file by copying ${CURRENT_ARCH_FILE} to ${ALLOWED_ARCH_FILE} and commit it.\n    This ensures that future architectural changes are explicitly reviewed.")
endif()

file(READ "${ALLOWED_ARCH_FILE}" ALLOWED_CONTENT)

# Compare the contents
if(NOT CURRENT_CONTENT STREQUAL ALLOWED_CONTENT)
    message(FATAL_ERROR "Architecture change detected!\n\n    current_architecture.dot differs from allowed_architecture.dot.\n    Please review the changes in ${CURRENT_ARCH_FILE} and update ${ALLOWED_ARCH_FILE} if approved.\n    You can use 'diff ${CURRENT_ARCH_FILE} ${ALLOWED_ARCH_FILE}' to see the differences.")
else()
    message(STATUS "Architecture check passed: current_architecture.dot matches allowed_architecture.dot.")
endif()
