import subprocess

def install_vscode_extension(extension_name):
    """
    Installs a Visual Studio Code extension using the given extension name.

    Args:
        extension_name (str): The name of the extension to install.

    Raises:
        subprocess.CalledProcessError: If the installation process fails.

    Returns:
        None
    """
    try:
        subprocess.run(["code", "--install-extension", extension_name], check=True)
        print(f"Successfully installed the extension: {extension_name}")
    except subprocess.CalledProcessError as e:
        print(f"Failed to install the extension: {e}")

def main():
    import json
    with open("CXX.code-profile") as f:
        profile = json.load(f)
        for extension in profile["extensions"]:
            install_vscode_extension(extension)