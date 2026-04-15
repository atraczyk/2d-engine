import os
import sys
import subprocess

def run(cmd, cwd=None, shell=False):
    print(f"Running: {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    result = subprocess.run(cmd, cwd=cwd, shell=shell)
    if result.returncode != 0:
        print("Command failed:", cmd)
        sys.exit(result.returncode)

def main():
    # Set up vcpkg
    vcpkg_dir = os.environ.get("VCPKG_ROOT", os.path.join(os.getcwd(), "vcpkg"))
    if not os.path.isdir(vcpkg_dir):
        print("VCPKG_ROOT is not set. Installing vcpkg...")
        if not os.path.isdir("vcpkg"):
            run(["git", "clone", "https://github.com/Microsoft/vcpkg.git"])
        run(["git", "pull"], cwd="vcpkg")
        if os.name == "nt":
            run(["cmd", "/c", "bootstrap-vcpkg.bat"], cwd="vcpkg")
        else:
            run(["./bootstrap-vcpkg.sh"], cwd="vcpkg")
        vcpkg_dir = os.path.abspath("vcpkg")
    else:
        print(f"Using vcpkg at {vcpkg_dir}")

    # Create build directory
    build_dir = "build"
    if not os.path.isdir(build_dir):
        os.makedirs(build_dir)
    os.chdir(build_dir)

    # Configure CMake
    print("Configuring project with CMake...")
    cmake_args = [
        "cmake", "..",
        f"-DCMAKE_TOOLCHAIN_FILE={os.path.join(vcpkg_dir, 'scripts', 'buildsystems', 'vcpkg.cmake')}",
        "-DCMAKE_BUILD_TYPE=Release"
    ]
    if os.name == "nt":
        cmake_args.extend(["-G", "Visual Studio 17 2022", "-A", "x64"])
    run(cmake_args)

    # Build
    print("Building project...")
    build_args = ["cmake", "--build", ".", "--config", "Release"]
    run(build_args)

if __name__ == "__main__":
    main()
