
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
    