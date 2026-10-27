Add-Type -AssemblyName System.Runtime.WindowsRuntime
$asDevice = [Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementWatcher, Windows.Devices.Bluetooth, ContentType = WindowsRuntime]
$watcher = [Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementWatcher]::new()
$found = @{}
$action = {
    $name = $EventArgs.Advertisement.LocalName
    $addr = '{0:X}' -f $EventArgs.BluetoothAddress
    if ($name -and -not $found.ContainsKey($addr)) {
        $found[$addr] = $name
        Write-Host "FOUND BLE: $name ($addr)"
    }
}
$reg = Register-ObjectEvent -InputObject $watcher -EventName Received -Action $action
$watcher.Start()
Start-Sleep -Seconds 5
$watcher.Stop()
Unregister-Event -SourceIdentifier $reg.Name
