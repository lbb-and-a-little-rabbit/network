#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>

int main() {
    std::string psScript = R"(
        Add-Type -AssemblyName System.Windows.Forms
        Add-Type -AssemblyName System.Drawing

        $bounds = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
        $bmp = New-Object System.Drawing.Bitmap $bounds.Width, $bounds.Height
        $graphics = [System.Drawing.Graphics]::FromImage($bmp)

        $graphics.CopyFromScreen($bounds.Location, [System.Drawing.Point]::Empty, $bounds.Size)

        $path = Join-Path (Get-Location) "screenshot.png"

        if (Test-Path $path) {
            Remove-Item $path -Force
        }

        $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)

        $graphics.Dispose()
        $bmp.Dispose()

        Write-Host "截图已保存到：$path"
    )";

    std::cout << "C++ 当前工作目录是: "
              << std::filesystem::current_path() << std::endl;

    std::ofstream file("screenshot.ps1", std::ios::out | std::ios::trunc);
    if (!file) {
        std::cerr << "无法创建 screenshot.ps1" << std::endl;
        return 1;
    }

    file << psScript;
    file.close();

    int ret = system("powershell -NoProfile -ExecutionPolicy Bypass -File screenshot.ps1");

    std::cout << "PowerShell 返回值: " << ret << std::endl;

    return 0;
}