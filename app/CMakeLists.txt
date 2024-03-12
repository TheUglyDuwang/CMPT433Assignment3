# Build the app which supports ALSA, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(a3 ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(a3 LINK_PRIVATE hal)

# ---------------- NEW! ----------------
# ALSA support
find_package(ALSA REQUIRED)
target_link_libraries(a3 LINK_PRIVATE asound)

# PTHREAD support
find_package(Threads REQUIRED)
target_link_libraries(a3 LINK_PRIVATE Threads::Threads)

# Copy executable to final location (change `a3` to project name as needed)
add_custom_command(TARGET a3 POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:a3>"
     "~/cmpt433/public/myApps/a3" 
  COMMENT "Copying ARM executable to public NFS directory")


# ---------------- NEW! ----------------
# Copy the WAV folder to NFS
add_custom_command(TARGET a3 POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy_directory
     "${CMAKE_SOURCE_DIR}/audio"
     "~/cmpt433/public/myApps/audio" 
  COMMENT "Copying WAVE files to public NFS directory")
