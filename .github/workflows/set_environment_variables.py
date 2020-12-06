import os

repo = os.path.normpath(os.environ['GITHUB_WORKSPACE'])
root = os.path.normpath(os.path.dirname(repo))

cmake_tool_path = os.environ['CMAKE_TOOL_PATH']

with open(os.environ['GITHUB_ENV'], 'a') as env_file:
  env_file.write("REPO_PATH={}\n".format(repo.replace("\\", "\\\\")))
  env_file.write("ROOT_PATH={}\n".format(root.replace("\\", "\\\\")))

if "${{ runner.os }}" == "Windows" or \
   "${{ runner.os }}" == "Linux":
  env_file.write("CMAKE_TOOL_BINARY_PATH={}\n".format(os.path.join(root, cmake_tool_path, "bin").replace("\\", "\\\\")))
elif "${{ runner.os }}" == "macOS":
  env_file.write("CMAKE_TOOL_BINARY_PATH={}\n".format(os.path.join(root, cmake_tool_path, "CMake.app", "Contents", "bin")))
else:
  raise RuntimeError("Unknown OS {}".format("${{ runner.os }}")) 
